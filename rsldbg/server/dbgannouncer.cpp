#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "dbgannouncer.h"
#include "../dbgserverproto.h"
#include "../dbgsettings.h"
#include "../strconv.h"
#include "../dbglog.h"
#include "../vendor/json.hpp"

#include <ctime>
#include <algorithm>

using nlohmann::json;

namespace
{
const int HEARTBEAT_MS = 2000;

std::string processExeName()
{
    wchar_t path[MAX_PATH] = {};
    if (GetModuleFileNameW(NULL, path, MAX_PATH))
        return dbgstr::wide_to_utf8(path);

    return std::string();
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

    // НЕ GetWindowTextW: он шлёт WM_GETTEXT и ждёт ответа UI-потока, который
    // во время debugbreak заблокирован в ожидании клиента - анонсер бы
    // завис навсегда. SendMessageTimeoutW с SMTO_ABORTIFHUNG не ждёт.
    DWORD_PTR res = 0;
    if (SendMessageTimeoutW(hwnd, WM_GETTEXT, 255,
                            reinterpret_cast<LPARAM>(ctx->title),
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 300, &res) && res > 0)
        return FALSE; // нашли - хватит

    return TRUE;
}

std::string processWindowTitle()
{
    EnumWndCtx ctx = { GetCurrentProcessId(), {} };
    ctx.title[0] = 0;
    EnumWindows(enumWndProc, reinterpret_cast<LPARAM>(&ctx));

    return dbgstr::wide_to_utf8(ctx.title);
}

std::string currentUserName()
{
    wchar_t name[256] = {};
    DWORD size = 255;
    if (GetUserNameW(name, &size))
        return dbgstr::wide_to_utf8(name);

    return std::string();
}

// Локальное время в ISO-виде (был QDateTime::toString(Qt::ISODate)).
std::string currentIsoTime()
{
    const std::time_t t = std::time(nullptr);
    std::tm tmv;
    localtime_s(&tmv, &t);

    char buf[32];
    std::snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d",
                  tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
                  tmv.tm_hour, tmv.tm_min, tmv.tm_sec);

    return std::string(buf);
}

std::string localHostName()
{
    char hn[256] = {};
    if (gethostname(hn, sizeof(hn) - 1) == 0)
        return dbgstr::ansi_to_utf8(hn);

    return std::string();
}
} // namespace

DbgAnnouncer::DbgAnnouncer() = default;

DbgAnnouncer::~DbgAnnouncer()
{
    stop();
}

bool DbgAnnouncer::start(unsigned short dbgPort)
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

std::string DbgAnnouncer::buildPayload() const
{
    json obj;
    obj["proto"] = 1;
    obj["pid"] = (long long)GetCurrentProcessId();
    obj["port"] = (long long)m_DbgPort;
    obj["exe"] = processExeName();
    obj["user"] = currentUserName();
    obj["host"] = localHostName();
    obj["dbgver"] = RSLDBG_PROTOCOL_VERSION;
    obj["ts"] = currentIsoTime();

    if (m_WithTitle)
        obj["title"] = processWindowTitle();

    // NB: токен в анонс НЕ включаем - клиент получает его из rsldbg://-URI
    // (лог + буфер обмена) или из rsldbg.ini.
    return obj.dump();
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
    m_BcastAddrs.push_back(0xFFFFFFFFul); // 255.255.255.255

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
                    if (bcast && bcast != 0xFFFFFFFFul &&
                        std::find(m_BcastAddrs.begin(), m_BcastAddrs.end(), bcast) == m_BcastAddrs.end())
                        m_BcastAddrs.push_back(bcast);
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
    const std::string payload = buildPayload();

    if (m_Broadcast && m_BcastSock != INVALID_SOCKET)
    {
        for (ULONG bcast : m_BcastAddrs)
        {
            sockaddr_in to = {};
            to.sin_family = AF_INET;
            to.sin_addr.s_addr = bcast;
            to.sin_port = htons(RSLDBG_ANNOUNCE_PORT);

            sendto(static_cast<SOCKET>(m_BcastSock), payload.data(), (int)payload.size(),
                   0, reinterpret_cast<sockaddr*>(&to), sizeof(to));
        }
    }

    for (const std::string &addr : m_UnicastTo)
    {
        sockaddr_in to = {};
        to.sin_family = AF_INET;
        to.sin_port = htons(RSLDBG_ANNOUNCE_PORT);

        if (InetPtonA(AF_INET, addr.c_str(), &to.sin_addr) == 1)
            sendto(static_cast<SOCKET>(m_BcastSock), payload.data(), (int)payload.size(),
                   0, reinterpret_cast<sockaddr*>(&to), sizeof(to));
    }
}

void DbgAnnouncer::handleProbe()
{
    SOCKET sock = static_cast<SOCKET>(m_ProbeSock);

    // select с таймаутом HEARTBEAT_MS: за это время отвечаем на все
    // накопившиеся discover-запросы.
    int deadline = HEARTBEAT_MS;
    while (!m_Stop.load() && deadline > 0)
    {
        const int slice = deadline < 200 ? deadline : 200;

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
        const json req = json::parse(buf, buf + n, nullptr, false);
        if (req.is_discarded() || !req.is_object() ||
            req.value("cmd", std::string()) != "discover")
            continue;

        const std::string payload = buildPayload();
        sendto(sock, payload.data(), (int)payload.size(), 0,
               reinterpret_cast<sockaddr*>(&from), fromLen);
    }
}
