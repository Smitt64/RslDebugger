#include "discovery.h"
#include "dbgserverproto.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QElapsedTimer>
#include <QThread>
#include <cstdio>

#include <winsock2.h>
#include <ws2tcpip.h>

// NB: QUdpSocket в этом окружении не получал ответные датаграммы
// (bind/отправка ок, readyRead молчал), поэтому discovery работает
// напрямую через winsock - по образцу серверного анонсера.

namespace
{
void sendProbe(SOCKET sock, const char *probe, int probeLen, quint32 addrBE)
{
    sockaddr_in to = {};
    to.sin_family = AF_INET;
    to.sin_port = htons(RSLDBG_ANNOUNCE_PORT);
    to.sin_addr.s_addr = addrBE;
    sendto(sock, probe, probeLen, 0,
           reinterpret_cast<sockaddr*>(&to), sizeof(to));
}
} // namespace

QJsonArray discoverDebugSessions(int timeoutMs)
{
    QJsonArray sessions;

    WSADATA wsa = {};
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return sessions;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        WSACleanup();
        return sessions;
    }

    BOOL bcast = TRUE;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
               reinterpret_cast<char*>(&bcast), sizeof(bcast));

    // Эфемерный порт: мы единственный слушатель, ответы анонсера
    // (юникаст на наш порт) не перехватывает никто.
    sockaddr_in local = {};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = 0;
    if (bind(sock, reinterpret_cast<sockaddr*>(&local), sizeof(local)) != 0)
    {
        closesocket(sock);
        WSACleanup();
        return sessions;
    }

    const char probe[] = "{\"cmd\":\"discover\"}";
    const int probeLen = int(sizeof(probe) - 1);

    QSet<QString> seen;
    QElapsedTimer timer;
    timer.start();

    int nextProbeAt = 0;
    sendProbe(sock, probe, probeLen, htonl(INADDR_LOOPBACK));
    sendProbe(sock, probe, probeLen, INADDR_BROADCAST);
    nextProbeAt = 400;

    while (timer.elapsed() < timeoutMs)
    {
        // Повторные пробы на случай потери датаграммы.
        if (timer.elapsed() >= nextProbeAt)
        {
            sendProbe(sock, probe, probeLen, htonl(INADDR_LOOPBACK));
            sendProbe(sock, probe, probeLen, INADDR_BROADCAST);
            nextProbeAt = int(timer.elapsed()) + 400;
        }

        const qint64 remain = timeoutMs - timer.elapsed();
        int slice = nextProbeAt > 0 ? int(nextProbeAt - timer.elapsed()) : int(remain);
        if (slice <= 0)
            slice = 1;
        if (slice > remain)
            slice = int(remain);

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        timeval tv = {};
        tv.tv_sec = slice / 1000;
        tv.tv_usec = (slice % 1000) * 1000;

        if (select(0, &rfds, NULL, NULL, &tv) <= 0)
            continue;

        char buf[1024];
        sockaddr_in from = {};
        int fromLen = sizeof(from);
        const int n = recvfrom(sock, buf, sizeof(buf) - 1, 0,
                               reinterpret_cast<sockaddr*>(&from), &fromLen);
        if (n <= 0)
            continue;

        const QJsonObject obj = QJsonDocument::fromJson(QByteArray(buf, n)).object();
        if (obj.value(QStringLiteral("proto")).toInt() != 1)
            continue;

        const QString key = QStringLiteral("%1:%2:%3")
                                .arg(obj.value(QStringLiteral("host")).toString())
                                .arg(obj.value(QStringLiteral("pid")).toInt())
                                .arg(obj.value(QStringLiteral("port")).toInt());
        if (seen.contains(key))
            continue;

        seen.insert(key);
        sessions.append(obj);
    }

    closesocket(sock);
    WSACleanup();
    return sessions;
}
