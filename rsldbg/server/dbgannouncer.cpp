#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "dbgannouncer.h"
#include "../dbgserverproto.h"
#include "../dbgsettings.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QHostInfo>
#include <QDateTime>
#include <QDebug>

namespace
{
const int HEARTBEAT_MS = 2000;

QString processExeName()
{
    wchar_t path[MAX_PATH] = {};
    if (GetModuleFileNameW(NULL, path, MAX_PATH))
        return QString::fromWCharArray(path);

    return QString();
}

// Заголовок первого видимого окна текущего процесса (хост-приложение).
struct EnumWndCtx
{
    DWORD pid;
    wchar_t title[256];
};

BOOL CALLBACK enumWndProc(HWND hwnd, LPARAM lParam)
{
    EnumWndCtx *ctx = reinterpret_cast<EnumWndCtx*>(lParam);

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != ctx->pid || !IsWindowVisible(hwnd))
        return TRUE;

    if (GetWindowTextW(hwnd, ctx->title, 255) > 0)
        return FALSE; // нашли - хватит

    return TRUE;
}

QString processWindowTitle()
{
    EnumWndCtx ctx = { GetCurrentProcessId(), {} };
    EnumWindows(enumWndProc, reinterpret_cast<LPARAM>(&ctx));

    return QString::fromWCharArray(ctx.title);
}

QString currentUserName()
{
    wchar_t name[256] = {};
    DWORD size = 255;
    if (GetUserNameW(name, &size))
        return QString::fromWCharArray(name);

    return QString();
}
} // namespace

DbgAnnouncer::DbgAnnouncer() = default;

DbgAnnouncer::~DbgAnnouncer()
{
    stop();
}

bool DbgAnnouncer::start(quint16 dbgPort)
{
    if (m_Running.load())
        return false;

    m_DbgPort = dbgPort;

    const DbgSettings s = dbgSettings();
    m_UnicastTo = s.announceTo;
    m_WithTitle = s.announceTitle;
    m_Broadcast = s.announceBroadcast;

    m_Stop.store(false);

    try
    {
        m_Thread = std::thread(&DbgAnnouncer::run, this);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

void DbgAnnouncer::stop()
{
    m_Stop.store(true);

    if (m_Thread.joinable())
        m_Thread.join();
}

QByteArray DbgAnnouncer::buildPayload() const
{
    QJsonObject obj;
    obj.insert(QStringLiteral("proto"), 1);
    obj.insert(QStringLiteral("pid"), static_cast<qint64>(GetCurrentProcessId()));
    obj.insert(QStringLiteral("port"), static_cast<qint64>(m_DbgPort));
    obj.insert(QStringLiteral("exe"), processExeName());
    obj.insert(QStringLiteral("user"), currentUserName());
    obj.insert(QStringLiteral("host"), QHostInfo::localHostName());
    obj.insert(QStringLiteral("dbgver"), RSLDBG_PROTOCOL_VERSION);
    obj.insert(QStringLiteral("ts"),
               QDateTime::currentDateTime().toString(Qt::ISODate));

    if (m_WithTitle)
        obj.insert(QStringLiteral("title"), processWindowTitle());

    // NB: токен в анонс НЕ включаем - клиент получает его из rsldbg://-URI
    // (лог + буфер обмена) или из rsldbg.ini.
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

void DbgAnnouncer::run()
{
    WSADATA wsaData = {};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return;

    m_Running.store(true);

    // --- broadcast-сокет ---
    m_BcastSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_BcastSock != INVALID_SOCKET)
    {
        BOOL bcast = TRUE;
        setsockopt(static_cast<SOCKET>(m_BcastSock), SOL_SOCKET,
                   SO_BROADCAST, reinterpret_cast<char*>(&bcast), sizeof(bcast));
    }

    // --- список directed broadcast адресов по всем интерфейсам ---
    m_BcastAddrs.clear();
    m_BcastAddrs.append(0xFFFFFFFFul); // 255.255.255.255

    ULONG bufLen = 16 * 1024;
    IP_ADAPTER_ADDRESSES *adapters = static_cast<IP_ADAPTER_ADDRESSES*>(malloc(bufLen));
    if (adapters &&
        GetAdaptersAddresses(AF_INET, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST,
                             NULL, adapters, &bufLen) == NO_ERROR)
    {
        for (IP_ADAPTER_ADDRESSES *a = adapters; a; a = a->Next)
        {
            if (a->OperStatus != IfOperStatusUp || a->IfType == IF_TYPE_SOFTWARE_LOOPBACK)
                continue;

            for (IP_ADAPTER_UNICAST_ADDRESS *u = a->FirstUnicastAddress; u; u = u->Next)
            {
                if (u->Address.lpSockaddr->sa_family != AF_INET)
                    continue;

                sockaddr_in *sin = reinterpret_cast<sockaddr_in*>(u->Address.lpSockaddr);
                ULONG mask = 0;
                if (ConvertLengthToIpv4Mask(u->OnLinkPrefixLength, &mask) == NO_ERROR && mask)
                {
                    ULONG bcast = (sin->sin_addr.s_addr & mask) | ~mask;
                    if (bcast && bcast != 0xFFFFFFFFul && !m_BcastAddrs.contains(bcast))
                        m_BcastAddrs.append(bcast);
                }
            }
        }
    }
    free(adapters);

    // --- probe-сокет: ответы на discover-запросы слушателей ---
    m_ProbeSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_ProbeSock != INVALID_SOCKET)
    {
        BOOL reuse = TRUE;
        setsockopt(static_cast<SOCKET>(m_ProbeSock), SOL_SOCKET,
                   SO_REUSEADDR, reinterpret_cast<char*>(&reuse), sizeof(reuse));

        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(RSLDBG_ANNOUNCE_PORT);

        if (bind(static_cast<SOCKET>(m_ProbeSock),
                 reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
        {
            closesocket(static_cast<SOCKET>(m_ProbeSock));
            m_ProbeSock = INVALID_SOCKET;
        }
    }

    while (!m_Stop.load())
    {
        sendAnnounces();

        // Ждём discover-запросы HEARTBEAT_MS (или до остановки).
        if (m_ProbeSock != INVALID_SOCKET)
            handleProbe();
        else
        {
            for (int i = 0; i < HEARTBEAT_MS / 50 && !m_Stop.load(); ++i)
                Sleep(50);
        }
    }

    if (m_ProbeSock != INVALID_SOCKET)
        closesocket(static_cast<SOCKET>(m_ProbeSock));
    if (m_BcastSock != INVALID_SOCKET)
        closesocket(static_cast<SOCKET>(m_BcastSock));

    m_Running.store(false);
    WSACleanup();
}

void DbgAnnouncer::sendAnnounces()
{
    const QByteArray payload = buildPayload();

    if (m_Broadcast && m_BcastSock != INVALID_SOCKET)
    {
        for (ULONG bcast : m_BcastAddrs)
        {
            sockaddr_in to = {};
            to.sin_family = AF_INET;
            to.sin_addr.s_addr = bcast;
            to.sin_port = htons(RSLDBG_ANNOUNCE_PORT);

            sendto(static_cast<SOCKET>(m_BcastSock), payload.constData(), payload.size(),
                   0, reinterpret_cast<sockaddr*>(&to), sizeof(to));
        }
    }

    for (const QString &addr : m_UnicastTo)
    {
        sockaddr_in to = {};
        to.sin_family = AF_INET;
        to.sin_port = htons(RSLDBG_ANNOUNCE_PORT);

        if (InetPtonA(AF_INET, addr.toLatin1().constData(), &to.sin_addr) == 1)
            sendto(static_cast<SOCKET>(m_BcastSock), payload.constData(), payload.size(),
                   0, reinterpret_cast<sockaddr*>(&to), sizeof(to));
    }
}

void DbgAnnouncer::handleProbe()
{
    SOCKET sock = static_cast<SOCKET>(m_ProbeSock);

    // select с таймаутом HEARTBEAT_MS: за это время отвечаем на все
    // накопившиеся discover-запросы.
    qint64 deadline = HEARTBEAT_MS;
    while (!m_Stop.load() && deadline > 0)
    {
        const int slice = qMin<qint64>(deadline, 200);

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);

        timeval tv = {};
        tv.tv_sec = slice / 1000;
        tv.tv_usec = (slice % 1000) * 1000;

        int sel = select(0, &rfds, NULL, NULL, &tv);
        deadline -= slice;

        if (sel <= 0)
            continue;

        char buf[1024];
        sockaddr_in from = {};
        int fromLen = sizeof(from);
        int n = recvfrom(sock, buf, sizeof(buf) - 1, 0,
                         reinterpret_cast<sockaddr*>(&from), &fromLen);
        if (n <= 0)
            continue;

        buf[n] = 0;
        QJsonObject req = QJsonDocument::fromJson(QByteArray(buf, n)).object();
        if (req.value(QStringLiteral("cmd")).toString() != QLatin1String("discover"))
            continue;

        const QByteArray payload = buildPayload();
        sendto(sock, payload.constData(), payload.size(), 0,
               reinterpret_cast<sockaddr*>(&from), fromLen);
    }
}
