#ifndef DBGSERVER_H
#define DBGSERVER_H

#include "dbgserverproto.h"
#include "lf/bp_data.h"
#include "server/dbgserverbase.h"
#include <QObject>
#include <QCache>

class CDebug;
class QProcess;
class QTextCodec;
class DbgServer : public DbgServerBase
{
    Q_OBJECT
public:
    DbgServer(QObject *parent = nullptr);
    virtual ~DbgServer();

    CDebug *m_curdbg;

    void UpdateDbgInfo(TBpData *data);

    void setIsNewDebug(const bool &v);

public slots:
    void sendEventBreakPoint(Qt::HANDLE BpData);

protected:
    virtual bool startApp() Q_DECL_OVERRIDE;
    virtual bool serverActionEvent(ServerActionEvent *e) Q_DECL_OVERRIDE;
    virtual bool serverEvent(QEvent *e) Q_DECL_OVERRIDE;
    virtual void clientDisconnected() Q_DECL_OVERRIDE;

signals:
    void finished();
    void started();
    void execContinue();

private:
    QCache<Qt::HANDLE, QByteArray> m_ProcNamespace;

    QTextCodec *oem866;
    int RslGetModuleLine(Qt::HANDLE module, int offs, int len);

    QString ReadTextFileContent(const QString &filename, const QString &encode);
    void UpdateText(const int &index);
    void UpdateStack();
    void UpdateWatch();
    void UpdateDbgInfo(const int &index);
    void UpdateVariables(const int &index);
    void ShowVariables(const int &index);
    void ShowVariableValue(const qint64 &val, const qint64 &info);
    QString m_LastError;
    RSLSTACK prevStack;
    bool newDbg;

    Qt::HANDLE m_curModuleInView;

    // Кэш текста модуля: UPDATETEXT идёт на каждом шаге, а файл .mac
    // во время отладки не меняется - не перечитываем его с диска.
    QString m_TextCacheFile;
    QString m_TextCacheContent;

    QScopedPointer<QProcess> m_pProc;
};

#endif // DBGSERVER_H
