#include <winsock2.h>
#include <ws2tcpip.h>
#include "dbgserverbase.h"
#include "dbgannouncer.h"
#include "serveractionevent.h"
#include "../dbgsettings.h"
#include <QThread>
#include <QProcess>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QApplication>
#include <QTextCodec>
#include <QStack>
#include <QMutexLocker>
#include <QDataStream>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonObject>

Q_LOGGING_CATEGORY(dbgServer, "rsldbg.server")

// Копирование строки в буфер обмена через Win32: run() выполняется в рабочем
// потоке, а QClipboard доступен только из GUI-потока.
static void copyToClipboard(const QString &text)
{
    if (!OpenClipboard(NULL))
        return;

    EmptyClipboard();

    const QString wide = text;
    const int bytes = (wide.length() + 1) * int(sizeof(wchar_t));
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hMem)
    {
        void *dst = GlobalLock(hMem);
        if (dst)
        {
            memcpy(dst, wide.utf16(), bytes);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        }
        else
            GlobalFree(hMem);
    }

    CloseClipboard();
}

DbgServerBase::DbgServerBase(QObject *parent)
    : QObject{parent},
    QRunnable()
{
    //prevStack = 0;
    //newDbg = true;
    ClientSocket = INVALID_SOCKET;
    m_Port = 0;
    oem866 = QTextCodec::codecForName("IBM 866");

    m_finishedFlag.storeRelease(0);
    m_continueFlag.storeRelease(0);

    // DirectConnection: лямбда исполняется прямо в рабочем потоке сервера
    // в момент emit finished() - queued-доставке мы не доверяем (см. .h).
    QObject::connect(this, &DbgServerBase::finished, this, [this]
    {
        m_finishedFlag.storeRelease(1);
    }, Qt::DirectConnection);
}

DbgServerBase::~DbgServerBase()
{

}

bool cstartApp()
{
    return false;
}

bool DbgServerBase::isConnected()
{
    return ClientSocket != INVALID_SOCKET;
}

int DbgServerBase::port()
{
    return m_Port;
}

void DbgServerBase::write(qint16 action, const QByteArray &data)
{
    DBGHEADER hdr;
    // NB: размер вычисляем выражением, а не запоминаем в hdr.size заранее:
    // DbgMakeHeader сперва обнуляет структуру memset'ом (раньше параметр
    // передавался по ссылке и алиасинг с hdr.size давал size == 0 на проводе).
    DbgMakeHeader(&hdr, int(sizeof(DBGHEADER)) + data.size(), action);

    QByteArray tmp;
    tmp.append((char*)&hdr, sizeof(DBGHEADER));
    tmp.append(data);

    // Сокет неблокирующий: send может отослать часть данных или
    // вернуть WSAEWOULDBLOCK - досылаем в цикле.
    int sent = 0;
    while (sent < tmp.size())
    {
        int r = send(ClientSocket, tmp.data() + sent, tmp.size() - sent, 0);

        if (r > 0)
            sent += r;
        else if (r == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
            QThread::usleep(50);
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
        m_LastError = QString("WSAStartup failed with error: %1").arg(iResult);
        qCritical(dbgServer()) << m_LastError;
        emit finished();
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
        m_LastError = QString("getaddrinfo failed with error: %1").arg(iResult);
        qCritical(dbgServer()) << m_LastError;

        WSACleanup();
        emit finished();
        return;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        m_LastError = QString("socket failed with error: %1").arg(WSAGetLastError());
        qCritical(dbgServer()) << m_LastError;

        freeaddrinfo(result);
        WSACleanup();
        emit finished();
        return;
    }

    char ipAddress[INET_ADDRSTRLEN] = { '\0' };
    inet_ntop(AF_INET, &((struct sockaddr_in *)result->ai_addr)->sin_addr, ipAddress, INET_ADDRSTRLEN);

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        m_LastError = QString("bind failed with error: %1").arg(WSAGetLastError());
        qCritical(dbgServer()) << m_LastError;

        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        emit finished();
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
    qInfo(dbgServer()) << "Server binds to" << ipAddress << "port" << m_Port;

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        m_LastError = QString("listen failed with error: %1").arg(WSAGetLastError());
        qCritical(dbgServer()) << m_LastError;

        closesocket(ListenSocket);
        WSACleanup();
        emit finished();
        return;
    }

    // Токен опционален: без auth_token в ini локальным клиентам (127.0.0.1)
    // достаточно самого кадра DBG_REQUEST_AUTH - он и есть "привет, я клиент".
    // Для gui_remote (0.0.0.0) без явного токена генерируем случайный -
    // слушать всю сеть совсем без защиты нельзя.
    if (!settings.authToken.isEmpty())
        m_Token = settings.authToken;
    else if (settings.clientMode == DbgClientGuiRemote)
        m_Token = QUuid::createUuid().toString(QUuid::WithoutBraces);

    // URI подключения (как токен бота в Telegram): показываем пользователю
    // в логе и кладём в буфер обмена - вставил в конфиг клиента/MCP и готово.
    if (settings.clientMode != DbgClientGui)
    {
        QString uri = QStringLiteral("rsldbg://%1:%2")
                          .arg(settings.clientMode == DbgClientGuiRemote
                                   ? QHostInfo::localHostName()
                                   : QStringLiteral("127.0.0.1"))
                          .arg(m_Port);
        if (!m_Token.isEmpty())
            uri += QStringLiteral("?token=") + m_Token;

        qWarning(dbgServer()) << "Debug session URI:" << uri;
        copyToClipboard(uri);
    }

    // В локальном gui-режиме сервер сам запускает клиента. В agent/gui_remote
    // клиент подключается сам (по анонсам) - рассылаем UDP heartbeat.
    if (settings.clientMode == DbgClientGui)
    {
        if (!startApp())
        {
            m_LastError = QString("Can't start debug app");
            qCritical(dbgServer()) << m_LastError;

            closesocket(ListenSocket);
            WSACleanup();
            emit finished();
            return;
        }
    }
    else if (settings.announceBroadcast || !settings.announceTo.isEmpty())
    {
        m_Announcer.reset(new DbgAnnouncer());
        m_Announcer->start((quint16)m_Port);
    }

    // accept с авторизацией: при неверном токене закрываем соединение и
    // возвращаемся к accept - один неверный клиент не должен убивать сессию.
    for (;;)
    {
        ClientSocket = accept(ListenSocket, NULL, NULL);

        if (ClientSocket == INVALID_SOCKET)
        {
            m_LastError = QString("accept failed with error: %1").arg(WSAGetLastError());
            qCritical(dbgServer()) << m_LastError;

            closesocket(ListenSocket);
            WSACleanup();
            emit finished();
            return;
        }

        // Логируем пира: помогает опознать "молчаливые" подключения
        // (сканеры портов, антивирусы, старые клиенты), которые занимают
        // accept-цикл и не дают подключиться настоящему клиенту.
        QString peerStr = QStringLiteral("?");
        {
            sockaddr_in peer = {};
            int peerLen = sizeof(peer);
            if (getpeername(ClientSocket, (sockaddr*)&peer, &peerLen) == 0)
            {
                char ipbuf[INET_ADDRSTRLEN] = {};
                if (inet_ntop(AF_INET, &peer.sin_addr, ipbuf, sizeof(ipbuf)))
                    peerStr = QStringLiteral("%1:%2").arg(QLatin1String(ipbuf))
                                                     .arg(ntohs(peer.sin_port));
            }
        }
        qInfo(dbgServer()) << "Accepted connection from" << peerStr;

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

        qInfo(dbgServer()) << "Client auth failed or timed out" << peerStr
                           << "- back to accept";
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
    qInfo(dbgServer()) << "Client connected (accepted), port" << m_Port;

    // Неблокирующий режим, чтобы можно было отличить закрытие соединения
    // клиентом (recv == 0) от простого отсутствия данных (WSAEWOULDBLOCK).
    u_long nonBlock = 1;
    ioctlsocket(ClientSocket, FIONBIO, &nonBlock);

    emit started();

    QByteArray buffer;
    QStack<DBGHEADER> headers;
    bool stop = false;

    do {
        QEvent *event = peekServerEvent();
        if (event)
        {
            serverEvent(event);
            delete event;
        }

        QThread::usleep(50);

        if (headers.empty())
        {
            unsigned long l = 0;
            int ioRes = ioctlsocket(ClientSocket, FIONREAD, &l);

            if (ioRes == SOCKET_ERROR)
            {
                int wsa = WSAGetLastError();
                qInfo(dbgServer()) << "FIONREAD(header) failed, WSA =" << wsa;
                stop = true;
                continue;
            }

            if (l >= sizeof(DBGHEADER))
            {
                DBGHEADER hdr;
                iResult = recv(ClientSocket, (char*)&hdr, sizeof(DBGHEADER), 0);

                if (iResult <= 0)
                {
                    qInfo(dbgServer()) << "Connection closed while reading header, recv =" << iResult;
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
                    qInfo(dbgServer()) << "Client disconnected (peek), recv =" << iResult
                                       << "WSA =" << WSAGetLastError();
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
                int wsa = WSAGetLastError();
                qInfo(dbgServer()) << "FIONREAD(body) failed, WSA =" << wsa;
                stop = true;
                continue;
            }

            if (l >= hdr.size)
            {
                char *tmp = new char[hdr.size];
                iResult = recv(ClientSocket, tmp, hdr.size - buffer.size(), 0);

                if (iResult <= 0)
                    stop = true;
                else
                    buffer.append(tmp, iResult);

                if (buffer.size() == hdr.size)
                {
                    QEvent *event = new ServerActionEvent(hdr.action, buffer);
                    sendServerAction(event);

                    buffer.clear();
                    headers.pop();
                }

                delete[] tmp;
            }
        }
    } while (!stop);

    // Клиент отключился, не прислав ExecContinue: интерпретатор мог
    // остаться на паузе после DbgBreak - снимаем его (см. DbgServer).
    clientDisconnected();

    iResult = shutdown(ClientSocket, SD_SEND);
    closesocket(ClientSocket);
    WSACleanup();
    emit finished();
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

    QByteArray packet;
    packet.append((char*)&handshake, sizeof(DBGHEADER));
    packet.append((char*)&hs, sizeof(DBG_HANDSHAKE));
    if (send(ClientSocket, packet.data(), packet.size(), 0) == SOCKET_ERROR)
    {
        qInfo(dbgServer()) << "Auth: handshake send failed, WSA error"
                           << WSAGetLastError();
        return false;
    }

    // Ожидаем DBG_REQUEST_AUTH {"token": "..."} (SO_RCVTIMEO=10с выставлен
    // вызывающим кодом).
    DBGHEADER hdr;
    const int gotHdr = recvAll((char*)&hdr, sizeof(hdr));
    if (gotHdr <= 0)
    {
        qInfo(dbgServer()) << "Auth: no auth frame received (recv" << gotHdr
                           << ", WSA error" << WSAGetLastError() << ")";
        return false;
    }

    // NB: асимметрия протокола - клиент в hdr.size кладёт размер ТОЛЬКО
    // payload (см. DbgClient::write), главный цикл ниже тоже ждёт payload-only.
    const int payload = hdr.size;
    if (!DbgCheckHeader(&hdr) || hdr.action != DBG_REQUEST_AUTH ||
        payload <= 0 || payload > 4096)
        return false;

    QByteArray body(payload, 0);
    if (recvAll(body.data(), payload) <= 0)
        return false;

    const QJsonObject obj = QJsonDocument::fromJson(body).object();

    // Токен не настроен -> авторизуем любого, кто прислал корректный
    // кадр DBG_REQUEST_AUTH (локальный listen на 127.0.0.1).
    const bool ok = m_Token.isEmpty() ||
                    obj.value(QStringLiteral("token")).toString() == m_Token;

    qInfo(dbgServer()) << "Auth request from client"
                       << obj.value(QStringLiteral("client")).toString()
                       << "token ok:" << ok;

    QJsonObject reply;
    reply.insert(QStringLiteral("ok"), ok);
    if (!ok)
        reply.insert(QStringLiteral("error"), QStringLiteral("Invalid auth token"));

    const QByteArray replyJson = QJsonDocument(reply).toJson(QJsonDocument::Compact);

    DBGHEADER rhdr;
    DbgMakeHeader(&rhdr, int(sizeof(DBGHEADER)) + replyJson.size(), DBG_REQUEST_AUTH);

    QByteArray rpacket;
    rpacket.append((char*)&rhdr, sizeof(DBGHEADER));
    rpacket.append(replyJson);
    send(ClientSocket, rpacket.data(), rpacket.size(), 0);

    return ok;
}

bool DbgServerBase::startApp()
{
    return false;
}

QEvent* DbgServerBase::peekServerEvent()
{
    QMutexLocker locker(&m_Mutex);

    if (m_Messages.empty())
        return nullptr;

    return m_Messages.dequeue();
}

void DbgServerBase::sendServerAction(QEvent *e)
{
    QMutexLocker locker(&m_Mutex);
    m_Messages.enqueue(e);
}

bool DbgServerBase::serverEvent(QEvent *e)
{
    if (e->type() == MSG_SERVER_EVENT)
        return serverActionEvent(dynamic_cast<ServerActionEvent*>(e));

    return false;
}

bool DbgServerBase::serverActionEvent(ServerActionEvent *e)
{
    qDebug() << "DbgServerBase::serverActionEvent";
    return false;
}

void DbgServerBase::clientDisconnected()
{
}
