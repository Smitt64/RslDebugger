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

Q_DECLARE_LOGGING_CATEGORY(dbgServer)

class CDebug;
class QProcess;
class QTextCodec;
class ServerActionEvent;
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

    void write(DBGHEADER *hdr, void *data, int len, const QByteArray &adddata = QByteArray());
    void write(DBGHEADER *hdr, const QByteArray &adddata);
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
    QQueue<QEvent*> m_Messages;
    unsigned int ClientSocket;
};

#endif // DBGSERVERBASE_H
