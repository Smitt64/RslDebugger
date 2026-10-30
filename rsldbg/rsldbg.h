#ifndef RSLDBG_H
#define RSLDBG_H

#define __RSLTYPE_H
#include "lf/bp_data.h"
#include "rsldbg_global.h"
#include "dbgserverproto.h"
#include "rsl/dbgintf.h"
#include <loki/SmartPtr.h>
#include <vector>
#include <map>

class CDebug;
typedef Loki::SmartPtr<CDebug, Loki::RefLinked> SpDebugPtrType;

typedef char *(*tRslGetModuleFile)(Qt::HANDLE,int*);
typedef Qt::HANDLE(*tRslGetModuleFromStack)(Qt::HANDLE);
typedef char *(*tRslGetProcNameFromStack)(Qt::HANDLE);
typedef Qt::HANDLE(*tRslGetStatementFromStack)(Qt::HANDLE);
typedef Qt::HANDLE(*tRslModuleForStatement)(Qt::HANDLE);
typedef void (*tRslGetStatementPos)(Qt::HANDLE, int *offs, int *len);

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

private:
    int process(HDBG hDBG, TBpData *data);

    bool is_inited, is_ui;
    Qt::HANDLE m_ActiveBeforeDbg;
    std::vector<SpDebugPtrType> m_Dbg;
    std::map<CDebug*, DbgServer*> m_Servers;

    CDebug *m_CurDbg;
    void *m_RsLeng; // HMODULE rsscript.dll
};

Qt::HANDLE ActiveBeforeDbg();
Rsldbg *rslDbgObj();

char *RslGetModuleFile(Qt::HANDLE mod, int*isBtrStream);
Qt::HANDLE RslGetModuleFromStack(Qt::HANDLE stack);
char *RslGetProcNameFromStack(Qt::HANDLE stack);
Qt::HANDLE RslGetStatementFromStack(Qt::HANDLE stack);
void RslGetStatementPos(Qt::HANDLE hst, int *offs, int *len);
Qt::HANDLE RslModuleForStatement(Qt::HANDLE hst);

#endif // RSLDBG_H
