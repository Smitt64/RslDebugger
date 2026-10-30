#include <winsock2.h>
#include <ws2tcpip.h>
#include <wincrypt.h>
#include "dbgserverbase.h"
#include "dbgannouncer.h"
#include "serveractionevent.h"
#include "../dbgsettings.h"
#include "../dbglog.h"
#include "../strconv.h"
#include "../vendor/json.hpp"

#include <thread>
#include <chrono>
#include <stack>
#include <vector>
#include <cstdio>

using nlohmann::json;

// Копирование строки в буфер обмена через Win32: run() выполняется в рабочем
// потоке, GUI-API буфера обмена недоступен.
static void copyToClipboard(const std::string &utf8)
{
    if (!OpenClipboard(NULL))
        return;

    EmptyClipboard();

    const std::wstring wide = dbgstr::utf8_to_wide(utf8);
    const int bytes = int((wide.size() + 1) * sizeof(wchar_t));
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hMem)
    {
        void *dst = GlobalLock(hMem);
        if (dst)
        {
            memcpy(dst, wide.c_str(), bytes);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        }
        else
            GlobalFree(hMem);
    }

    CloseClipboard();
}

static void sleepUs(unsigned long us)
{
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

// Случайный токен сессии для gui_remote без явного auth_token в ini
// (был QUuid::createUuid): 16 байт CryptGenRandom в hex-виде.
static std::string makeRandomToken()
{
    HCRYPTPROV prov = 0;
    unsigned char rnd[16] = {};

    if (CryptAcquireContextW(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    {
        CryptGenRandom(prov, sizeof(rnd), rnd);
        CryptReleaseContext(prov, 0);
    }
    else
    {
        // Крайне маловероятный fallback: не криптостойко, но лучше, чем ничего.
        DWORD t = GetTickCount();
        for (size_t i = 0; i < sizeof(rnd); ++i)
        {
            t = t * 1664525u + 1013904223u;
            rnd[i] = (unsigned char)(t >> 24);
        }
    }

    char hex[33];
    static const char digits[] = "0123456789abcdef";
    for (size_t i = 0; i < sizeof(rnd); ++i)
    {
        hex[i * 2] = digits[rnd[i] >> 4];
        hex[i * 2 + 1] = digits[rnd[i] & 0xF];
    }
    hex[32] = 0;

    return std::string(hex, 32);
}

DbgServerBase::DbgServerBase()
{
    ClientSocket = INVALID_SOCKET;
    m_Port = 0;
    m_curdbg = nullptr;

    m_finishedFlag.store(0);
    m_continueFlag.store(0);
}

DbgServerBase::~DbgServerBase()
{
}

bool DbgServerBase::isConnected()
{
    return ClientSocket != INVALID_SOCKET;
}

int DbgServerBase::port()
{
    return m_Port;
}

void DbgServerBase::write(qint16 action, const std::string &data)
{
    DBGHEADER hdr;
    // NB: размер вычисляем выражением, а не запоминаем в hdr.size заранее:
    // DbgMakeHeader сперва обнуляет структуру memset'ом (раньше параметр
    // передавался по ссылке и алиасинг с hdr.size давал size == 0 на проводе).
    DbgMakeHeader(&hdr, int(sizeof(DBGHEADER)) + data.size(), action);

    std::string tmp;
    tmp.append((char*)&hdr, sizeof(DBGHEADER));
    tmp.append(data);

    // Сокет неблокирующий: send может отослать часть данных или
    // вернуть WSAEWOULDBLOCK - досылаем в цикле.
    size_t sent = 0;
    while (sent < tmp.size())
    {
        int r = send(ClientSocket, tmp.data() + sent, int(tmp.size() - sent), 0);

        if (r > 0)
            sent += r;
        else if (r == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
            sleepUs(50);
        else
            break;
    }
}

void DbgServerBase::run()
{
    WSADATA wsaData = { 0 };
    int iResult = 0;

    SOCKET ListenSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
    {
        m_LastError = std::string("WSAStartup failed with error: ") + std::to_string(iResult);
        DBG_LOGE("%s", m_LastError.c_str());
        m_finishedFlag.store(1);
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    const DbgSettings settings = dbgSettings();

    // gui_remote слушает на всех интерфейсах, локальные режимы - только loopback.
    const char *bindNode = nullptr;
    if (settings.clientMode == DbgClientGuiRemote)
        hints.ai_flags = AI_PASSIVE;
    else
        bindNode = "127.0.0.1";

    // Порт 0 - свободный порт выделяет ОС. Фикс коллизии: раньше все процессы
    // начинали с 1492 (++max_port статичен только внутри одного процесса).
    iResult = getaddrinfo(bindNode, "0", &hints, &result);

    if ( iResult != 0 )
    {
        m_LastError = std::string("getaddrinfo failed with error: ") + std::to_string(iResult);
        DBG_LOGE("%s", m_LastError.c_str());

        WSACleanup();
        m_finishedFlag.store(1);
        return;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        m_LastError = std::string("socket failed with error: ") + std::to_string(WSAGetLastError());
        DBG_LOGE("%s", m_LastError.c_str());

        freeaddrinfo(result);
        WSACleanup();
        m_finishedFlag.store(1);
        return;
    }

    char ipAddress[INET_ADDRSTRLEN] = { '\0' };
    inet_ntop(AF_INET, &((struct sockaddr_in *)result->ai_addr)->sin_addr, ipAddress, INET_ADDRSTRLEN);

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        m_LastError = std::string("bind failed with error: ") + std::to_string(WSAGetLastError());
        DBG_LOGE("%s", m_LastError.c_str());

        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        m_finishedFlag.store(1);
        return;
    }

    freeaddrinfo(result);

    // Реальный порт, выданный ОС.
    {
        sockaddr_in bound = {};
        int boundLen = sizeof(bound);
        if (getsockname(ListenSocket, (sockaddr*)&bound, &boundLen) == 0)
            m_Port = ntohs(bound.sin_port);
    }
    DBG_LOGI("Server binds to %s port %d", ipAddress, m_Port);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        m_LastError = std::string("listen failed with error: ") + std::to_string(WSAGetLastError());
        DBG_LOGE("%s", m_LastError.c_str());

        closesocket(ListenSocket);
        WSACleanup();
        m_finishedFlag.store(1);
        return;
    }

    // Токен опционален: без auth_token в ini локальным клиентам (127.0.0.1)
    // достаточно самого кадра DBG_REQUEST_AUTH - он и есть "привет, я клиент".
    // Для gui_remote (0.0.0.0) без явного токена генерируем случайный -
    // слушать всю сеть совсем без защиты нельзя.
    if (!settings.authToken.empty())
        m_Token = settings.authToken;
    else if (settings.clientMode == DbgClientGuiRemote)
        m_Token = makeRandomToken();

    // URI подключения (как токен бота в Telegram): показываем пользователю
    // в логе и кладём в буфер обмена - вставил в конфиг клиента/MCP и готово.
    if (settings.clientMode != DbgClientGui)
    {
        std::string host;
        if (settings.clientMode == DbgClientGuiRemote)
        {
            char hn[256] = {};
            if (gethostname(hn, sizeof(hn) - 1) == 0)
                host = dbgstr::ansi_to_utf8(hn);
            else
                host = "127.0.0.1";
        }
        else
            host = "127.0.0.1";

        std::string uri = "rsldbg://" + host + ":" + std::to_string(m_Port);
        if (!m_Token.empty())
            uri += "?token=" + m_Token;

        DBG_LOGW("Debug session URI: %s", uri.c_str());
        copyToClipboard(uri);
    }

    // В локальном gui-режиме сервер сам запускает клиента. В agent/gui_remote
    // клиент подключается сам (по анонсам) - рассылаем UDP heartbeat.
    if (settings.clientMode == DbgClientGui)
    {
        if (!startApp())
        {
            m_LastError = "Can't start debug app";
            DBG_LOGE("%s", m_LastError.c_str());

            closesocket(ListenSocket);
            WSACleanup();
            m_finishedFlag.store(1);
            return;
        }
    }
    else if (settings.announceBroadcast || !settings.announceTo.empty())
    {
        m_Announcer.reset(new DbgAnnouncer());
        m_Announcer->start((unsigned short)m_Port);
    }

    // accept с авторизацией: при неверном токене закрываем соединение и
    // возвращаемся к accept - один неверный клиент не должен убивать сессию.
    for (;;)
    {
        ClientSocket = accept(ListenSocket, NULL, NULL);

        if (ClientSocket == INVALID_SOCKET)
        {
            m_LastError = std::string("accept failed with error: ") + std::to_string(WSAGetLastError());
            DBG_LOGE("%s", m_LastError.c_str());

            closesocket(ListenSocket);
            WSACleanup();
            m_finishedFlag.store(1);
            return;
        }

        // Логируем пира: помогает опознать "молчаливые" подключения
        // (сканеры портов, антивирусы, старые клиенты), которые занимают
        // accept-цикл и не дают подключиться настоящему клиенту.
        std::string peerStr = "?";
        {
            sockaddr_in peer = {};
            int peerLen = sizeof(peer);
            if (getpeername(ClientSocket, (sockaddr*)&peer, &peerLen) == 0)
            {
                char ipbuf[INET_ADDRSTRLEN] = {};
                if (inet_ntop(AF_INET, &peer.sin_addr, ipbuf, sizeof(ipbuf)))
                {
                    char pbuf[64];
                    _snprintf(pbuf, sizeof(pbuf), "%s:%u", ipbuf, (unsigned)ntohs(peer.sin_port));
                    pbuf[sizeof(pbuf) - 1] = 0;
                    peerStr = pbuf;
                }
            }
        }
        DBG_LOGI("Accepted connection from %s", peerStr.c_str());

        // Таймаут на авторизацию: случайный скан порта не вешает сессию.
        DWORD authTimeout = 10000;
        setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO,
                   (const char*)&authTimeout, sizeof(authTimeout));

        // Мелкий request/response пинг-понг с клиентом: отключаем Nagle,
        // чтобы кадры уходили без задержки на склейку.
        BOOL noDelay = TRUE;
        setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY,
                   (const char*)&noDelay, sizeof(noDelay));

        if (waitAuth())
            break;

        DBG_LOGI("Client auth failed or timed out %s - back to accept", peerStr.c_str());
        shutdown(ClientSocket, SD_BOTH);
        closesocket(ClientSocket);
        ClientSocket = INVALID_SOCKET;
    }

    // Авторизованный клиент подключён: анонсы больше не нужны.
    if (m_Announcer)
        m_Announcer->stop();

    // Снимаем таймаут авторизации.
    DWORD noTimeout = 0;
    setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO,
               (const char*)&noTimeout, sizeof(noTimeout));

    closesocket(ListenSocket);
    DBG_LOGI("Client connected (accepted), port %d", m_Port);

    // Неблокирующий режим, чтобы можно было отличить закрытие соединения
    // клиентом (recv == 0) от простого отсутствия данных (WSAEWOULDBLOCK).
    u_long nonBlock = 1;
    ioctlsocket(ClientSocket, FIONBIO, &nonBlock);

    std::string buffer;
    std::stack<DBGHEADER> headers;
    bool stop = false;

    do {
        DbgServerEvent *event = peekServerEvent();
        if (event)
        {
            serverEvent(event);
            delete event;
        }

        sleepUs(50);

        if (headers.empty())
        {
            unsigned long l = 0;
            int ioRes = ioctlsocket(ClientSocket, FIONREAD, &l);

            if (ioRes == SOCKET_ERROR)
            {
                DBG_LOGI("FIONREAD(header) failed, WSA = %d", WSAGetLastError());
                stop = true;
                continue;
            }

            if (l >= sizeof(DBGHEADER))
            {
                DBGHEADER hdr;
                iResult = recv(ClientSocket, (char*)&hdr, sizeof(DBGHEADER), 0);

                if (iResult <= 0)
                {
                    DBG_LOGI("Connection closed while reading header, recv = %d", iResult);
                    stop = true;
                }
                else
                    headers.push(hdr);
            }
            else if (l == 0)
            {
                // Клиент закрыл соединение: recv вернёт 0. При живом
                // соединении без данных будет WSAEWOULDBLOCK.
                char peek;
                iResult = recv(ClientSocket, &peek, 1, MSG_PEEK);

                if (iResult == 0 ||
                    (iResult == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK))
                {
                    DBG_LOGI("Client disconnected (peek), recv = %d WSA = %d",
                             iResult, WSAGetLastError());
                    stop = true;
                }
            }
        }
        else
        {
            const DBGHEADER &hdr = headers.top();

            unsigned long l = 0;
            int ioRes = ioctlsocket(ClientSocket, FIONREAD, &l);

            if (ioRes == SOCKET_ERROR)
            {
                DBG_LOGI("FIONREAD(body) failed, WSA = %d", WSAGetLastError());
                stop = true;
                continue;
            }

            if (l >= (unsigned long)hdr.size)
            {
                std::vector<char> tmp(hdr.size);
                iResult = recv(ClientSocket, tmp.data(), hdr.size - (int)buffer.size(), 0);

                if (iResult <= 0)
                    stop = true;
                else
                    buffer.append(tmp.data(), iResult);

                if (buffer.size() == (size_t)hdr.size)
                {
                    DbgServerEvent *event = new ServerActionEvent(hdr.action, buffer);
                    sendServerAction(event);

                    buffer.clear();
                    headers.pop();
                }
            }
        }
    } while (!stop);

    // Клиент отключился, не прислав ExecContinue: интерпретатор мог
    // остаться на паузе после DbgBreak - снимаем его (см. DbgServer).
    clientDisconnected();

    iResult = shutdown(ClientSocket, SD_SEND);
    (void)iResult;
    closesocket(ClientSocket);
    WSACleanup();
    m_finishedFlag.store(1);
}

int DbgServerBase::recvAll(char *dst, int size)
{
    // Блокирующее чтение ровно size байт (используется только в фазе
    // авторизации, пока сокет ещё блокирующий, с выставленным SO_RCVTIMEO).
    int got = 0;
    while (got < size)
    {
        int r = recv(ClientSocket, dst + got, size - got, 0);
        if (r <= 0)
            return r;
        got += r;
    }

    return got;
}

bool DbgServerBase::waitAuth()
{
    // handshake
    DBGHEADER handshake;
    DbgMakeHeader(&handshake, sizeof(DBGHEADER) + sizeof(DBG_HANDSHAKE), DBG_REQUEST_HANDSHAKE);

    DBG_HANDSHAKE hs;
    hs.version = RSLDBG_PROTOCOL_VERSION;

    std::string packet;
    packet.append((char*)&handshake, sizeof(DBGHEADER));
    packet.append((char*)&hs, sizeof(DBG_HANDSHAKE));
    if (send(ClientSocket, packet.data(), (int)packet.size(), 0) == SOCKET_ERROR)
    {
        DBG_LOGI("Auth: handshake send failed, WSA error %d", WSAGetLastError());
        return false;
    }

    // Ожидаем DBG_REQUEST_AUTH {"token": "..."} (SO_RCVTIMEO=10с выставлен
    // вызывающим кодом).
    DBGHEADER hdr;
    const int gotHdr = recvAll((char*)&hdr, sizeof(hdr));
    if (gotHdr <= 0)
    {
        DBG_LOGI("Auth: no auth frame received (recv %d, WSA error %d)",
                 gotHdr, WSAGetLastError());
        return false;
    }

    // NB: асимметрия протокола - клиент в hdr.size кладёт размер ТОЛЬКО
    // payload (см. DbgClient::write), главный цикл ниже тоже ждёт payload-only.
    const int payload = hdr.size;
    if (!DbgCheckHeader(&hdr) || hdr.action != DBG_REQUEST_AUTH ||
        payload <= 0 || payload > 4096)
        return false;

    std::string body(payload, 0);
    if (recvAll(&body[0], payload) <= 0)
        return false;

    const json obj = json::parse(body, nullptr, false);
    if (obj.is_discarded() || !obj.is_object())
        return false;

    // Токен не настроен -> авторизуем любого, кто прислал корректный
    // кадр DBG_REQUEST_AUTH (локальный listen на 127.0.0.1).
    const std::string token = obj.value("token", std::string());
    const bool ok = m_Token.empty() || token == m_Token;

    DBG_LOGI("Auth request from client %s token ok: %d",
             obj.value("client", std::string()).c_str(), (int)ok);

    json reply;
    reply["ok"] = ok;
    if (!ok)
        reply["error"] = "Invalid auth token";

    const std::string replyJson = reply.dump();

    DBGHEADER rhdr;
    DbgMakeHeader(&rhdr, int(sizeof(DBGHEADER)) + replyJson.size(), DBG_REQUEST_AUTH);

    std::string rpacket;
    rpacket.append((char*)&rhdr, sizeof(DBGHEADER));
    rpacket.append(replyJson);
    send(ClientSocket, rpacket.data(), (int)rpacket.size(), 0);

    return ok;
}

bool DbgServerBase::startApp()
{
    return false;
}

DbgServerEvent* DbgServerBase::peekServerEvent()
{
    std::lock_guard<std::mutex> locker(m_Mutex);

    if (m_Messages.empty())
        return nullptr;

    DbgServerEvent *e = m_Messages.front();
    m_Messages.pop();

    return e;
}

void DbgServerBase::sendServerAction(DbgServerEvent *e)
{
    std::lock_guard<std::mutex> locker(m_Mutex);
    m_Messages.push(e);
}

bool DbgServerBase::serverEvent(DbgServerEvent *e)
{
    if (e->type() == MSG_SERVER_EVENT)
        return serverActionEvent(static_cast<ServerActionEvent*>(e));

    return false;
}

bool DbgServerBase::serverActionEvent(ServerActionEvent *e)
{
    (void)e;
    DBG_LOGI("DbgServerBase::serverActionEvent");
    return false;
}

void DbgServerBase::clientDisconnected()
{
}
