#ifndef DBGSERVERBASE_H
#define DBGSERVERBASE_H

#include "dbgserverproto.h"
#include "lf/bp_data.h"
#include <QObject>
#include <QRunnable>
#include <QMutex>
#include <QLoggingCategory>
#include <QQueue>
#include <QAtomicInt>
#include <QScopedPointer>

Q_DECLARE_LOGGING_CATEGORY(dbgServer)

class CDebug;
class QProcess;
class QTextCodec;
class ServerActionEvent;
class DbgAnnouncer;
class DbgServerBase : public QObject, public QRunnable
{
    Q_OBJECT
public:
    DbgServerBase(QObject *parent = nullptr);
    virtual ~DbgServerBase();

    bool isConnected();
    int port();
    virtual void run() Q_DECL_FINAL;

    void sendServerAction(QEvent *e);

    // Флаги для ожидания без сигналов. Объект живёт в потоке m_Thread,
    // который никогда не запускается, поэтому queued-доставка сигналов
    // из рабочего потока пула ненадёжна и Rsldbg::process() опрашивает
    // эти флаги напрямую. Пишутся из рабочего потока сервера.
    QAtomicInt m_finishedFlag;
    QAtomicInt m_continueFlag;

signals:
    void finished();
    void started();

protected:
    virtual bool startApp();
    virtual bool serverEvent(QEvent *e);
    virtual bool serverActionEvent(ServerActionEvent *e);

    // Вызывается из run() после обрыва соединения с клиентом. Потомок
    // должен снять программу с паузы (abort), если отладка ещё активна,
    // иначе интерпретатор останется подвешенным в ожидании ExecContinue.
    virtual void clientDisconnected();

    void write(qint16 action, const QByteArray &data);

protected:
    QMutex m_Mutex;
    QTextCodec *oem866;
    CDebug *m_curdbg;

    int m_Port;
    QString m_LastError;
    //RSLSTACK prevStack;
    //bool newDbg;
    //Qt::HANDLE m_curModuleInView;

    //QScopedPointer<QProcess> m_pProc;

private:
    QEvent* peekServerEvent();

    // Фаза авторизации на принятом (пока блокирующем) сокете: handshake ->
    // ожидание DBG_REQUEST_AUTH {"client", "token"?} -> ответ {"ok":bool}.
    bool waitAuth();
    int recvAll(char *dst, int size);

    QQueue<QEvent*> m_Messages;
    unsigned int ClientSocket;
    QString m_Token; // пусто = авторизация не требуется (только 127.0.0.1)
    QScopedPointer<DbgAnnouncer> m_Announcer;
};

#endif // DBGSERVERBASE_H
