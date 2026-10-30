#ifndef DBGSERVERBASE_H
#define DBGSERVERBASE_H

#include "dbgserverproto.h"
#include "../lf/bp_data.h"
#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <memory>

class CDebug;
class DbgServerEvent;
class ServerActionEvent;
class DbgAnnouncer;

// Отладочный сервер без Qt: run() вызывается в std::thread (создаётся
// в Rsldbg::init), события - собственная очередь под мьютексом, сигналы
// finished/started заменены атомарными флагами (queued-доставке в Qt
// здесь и раньше не доверяли, см. m_finishedFlag).
class DbgServerBase
{
public:
    DbgServerBase();
    virtual ~DbgServerBase();

    bool isConnected();
    int port();

    // Точка входа потока сервера.
    void run();

    void sendServerAction(DbgServerEvent *e);

    // Флаги для ожидания без сигналов. Rsldbg::process() опрашивает их
    // напрямую. Пишутся из рабочего потока сервера.
    std::atomic<int> m_finishedFlag;
    std::atomic<int> m_continueFlag;

protected:
    virtual bool startApp();
    virtual bool serverEvent(DbgServerEvent *e);
    virtual bool serverActionEvent(ServerActionEvent *e);

    // Вызывается из run() после обрыва соединения с клиентом. Потомок
    // должен снять программу с паузы (abort), если отладка ещё активна,
    // иначе интерпретатор останется подвешенным в ожидании ExecContinue.
    virtual void clientDisconnected();

    void write(qint16 action, const std::string &data);

protected:
    std::mutex m_Mutex;
    CDebug *m_curdbg;

    int m_Port;
    std::string m_LastError; // UTF-8

private:
    DbgServerEvent* peekServerEvent();

    // Фаза авторизации на принятом (пока блокирующем) сокете: handshake ->
    // ожидание DBG_REQUEST_AUTH {"client", "token"?} -> ответ {"ok":bool}.
    bool waitAuth();
    int recvAll(char *dst, int size);

    std::queue<DbgServerEvent*> m_Messages;
    unsigned int ClientSocket;
    std::string m_Token; // UTF-8; пусто = авторизация не требуется (только 127.0.0.1)
    std::unique_ptr<DbgAnnouncer> m_Announcer;
};

#endif // DBGSERVERBASE_H
