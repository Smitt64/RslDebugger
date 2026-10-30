#ifndef DBGANNOUNCER_H
#define DBGANNOUNCER_H

#include <string>
#include <vector>
#include <atomic>
#include <thread>

// Рассылает UDP-анонсы отладочной сессии (heartbeat) и отвечает на
// discover-запросы слушателей (GUI-диалог выбора сессии, MCP-адаптер).
//
// Живёт, пока сервер ждёт подключения клиента: после успешной авторизации
// клиента вызывается stop(). Работает в собственном потоке, сеть - сырой
// winsock.
class DbgAnnouncer
{
public:
    DbgAnnouncer();
    ~DbgAnnouncer();

    // dbgPort - реальный порт debug-сервера (после bind на порт 0).
    // Возвращает false, если поток/сокеты создать не удалось.
    bool start(unsigned short dbgPort);
    void stop();

    bool isRunning() const { return m_Running.load(); }

private:
    void run();
    void sendAnnounces();
    void handleProbe();
    std::string buildPayload() const;

    unsigned short m_DbgPort = 0;
    std::vector<std::string> m_UnicastTo; // announce_to из rsldbg.ini
    bool m_WithTitle = true;              // announce_title
    bool m_Broadcast = true;              // announce != off

    std::atomic<bool> m_Stop{false};
    std::atomic<bool> m_Running{false};
    std::thread m_Thread;

    unsigned long long m_BcastSock = ~0ull; // SOCKET
    unsigned long long m_ProbeSock = ~0ull; // SOCKET
    std::vector<unsigned long> m_BcastAddrs; // адреса directed broadcast
};

#endif // DBGANNOUNCER_H
