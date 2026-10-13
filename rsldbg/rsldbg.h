#ifndef RSLDBG_H
#define RSLDBG_H

#define __RSLTYPE_H
#include "lf/bp_data.h"
#include "rsldbg_global.h"
#include "rsl/dbgintf.h"
#include <loki/SmartPtr.h>
#include <QObject>
#include <QStack>
#include <QEventLoop>
#include <QThread>
#include <QMap>

class CDebug;
typedef Loki::SmartPtr<CDebug, Loki::RefLinked> SpDebugPtrType;

typedef char *(*tRslGetModuleFile)(Qt::HANDLE,int*);
typedef Qt::HANDLE(*tRslGetModuleFromStack)(Qt::HANDLE);
typedef char *(*tRslGetProcNameFromStack)(Qt::HANDLE);
typedef Qt::HANDLE(*tRslGetStatementFromStack)(Qt::HANDLE);
typedef Qt::HANDLE(*tRslModuleForStatement)(Qt::HANDLE);
typedef void (*tRslGetStatementPos)(Qt::HANDLE, int *offs, int *len);
//typedef void(*tRslGetStatementPosEx)(Qt::HANDLE, int *offs, int *len, int *line);

class MainWindow;
class QApplication;
class QLibrary;
class CDebug;
class DbgServer;
class RSLDBG_EXPORT Rsldbg
{
public:
    Rsldbg();
    virtual ~Rsldbg();

    bool init_ui();
    void done_ui();

    Qt::HANDLE activeBeforeDbg();

    int size();
    HDBG init(HRD inst, TDbgIntf * _dbg_ftable);
    void done(HDBG hDBG);
    int DbgBreak(HDBG hDBG, uint32 data);

    void trace(HDBG hinst,const char *str);
    void remModule(HDBG hinst, RSLMODULE hmod);

    CDebug *currentDebug();
    void stoploop();

    tRslGetModuleFile RslGetModuleFile;
    tRslGetModuleFromStack RslGetModuleFromStack;
    tRslGetProcNameFromStack RslGetProcNameFromStack;
    tRslGetStatementFromStack RslGetStatementFromStack;
    tRslGetStatementPos RslGetStatementPos;
    tRslModuleForStatement RslModuleForStatement;
    //tRslGetStatementPosEx RslGetStatementPosEx;

private:
    int process(HDBG hDBG, TBpData *data);
    QApplication *m_pApp;
    char **argv;

    bool is_inited, is_ui;
    Qt::HANDLE m_ActiveBeforeDbg;
    QVector<SpDebugPtrType> m_Dbg;
    QMap<CDebug*, DbgServer*> m_Servers;

    QEventLoop m_EventLoop;

    QScopedPointer<MainWindow> m_pWndMain;
    QScopedPointer<QLibrary> m_RsLeng;
    QScopedPointer<QThread> m_Thread;
};

Qt::HANDLE ActiveBeforeDbg();
Rsldbg *rslDbgObj();

char *RslGetModuleFile(Qt::HANDLE mod, int*isBtrStream);
Qt::HANDLE RslGetModuleFromStack(Qt::HANDLE stack);
char *RslGetProcNameFromStack(Qt::HANDLE stack);
Qt::HANDLE RslGetStatementFromStack(Qt::HANDLE stack);
void RslGetStatementPos(Qt::HANDLE hst, int *offs, int *len);
Qt::HANDLE RslModuleForStatement(Qt::HANDLE hst);
//int RslGetModuleLine(Qt::HANDLE module, int offs, int len);

#endif // RSLDBG_H
