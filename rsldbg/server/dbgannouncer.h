#ifndef DBGANNOUNCER_H
#define DBGANNOUNCER_H

#include <QByteArray>
#include <QList>
#include <QStringList>
#include <atomic>
#include <thread>

// Рассылает UDP-анонсы отладочной сессии (heartbeat) и отвечает на
// discover-запросы слушателей (GUI-диалог выбора сессии, MCP-адаптер).
//
// Живёт, пока сервер ждёт подключения клиента: после успешной авторизации
// клиента вызывается stop(). Работает в собственном потоке, Qt не использует
// для сетевой части (сырой winsock, как и DbgServerBase).
class DbgAnnouncer
{
public:
    DbgAnnouncer();
    ~DbgAnnouncer();

    // dbgPort - реальный порт debug-сервера (после bind на порт 0).
    // Возвращает false, если поток/сокеты создать не удалось.
    bool start(quint16 dbgPort);
    void stop();

    bool isRunning() const { return m_Running.load(); }

private:
    void run();
    void sendAnnounces();
    void handleProbe();
    QByteArray buildPayload() const;

    quint16 m_DbgPort = 0;
    QStringList m_UnicastTo;   // announce_to из rsldbg.ini
    bool m_WithTitle = true;   // announce_title
    bool m_Broadcast = true;   // announce != off

    std::atomic<bool> m_Stop{false};
    std::atomic<bool> m_Running{false};
    std::thread m_Thread;

    unsigned long long m_BcastSock = ~0ull; // SOCKET
    unsigned long long m_ProbeSock = ~0ull; // SOCKET
    QList<unsigned long> m_BcastAddrs;      // адреса directed broadcast
};

#endif // DBGANNOUNCER_H
