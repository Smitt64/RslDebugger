#include "dbgserver.h"
#include "server/updatedbginfoevent.h"
#include "dbgsettings.h"
#include "server/breakpointevent.h"
#include "cdebug.h"
#include "rsl/dbgintf.h"
#include "saver.h"
#include "cqsurvey.h"
#include "csurvey.h"
#include "clocals.h"
#include "cwatchv.h"
#include "rsldbg.h"
#include "dbglog.h"
#include <windows.h>
#include <thread>

Rsldbg *rslDbgObj()
{
    // Статическая инициализация потокобезопасна начиная с C++11 (magic statics).
    static Rsldbg *inst = new Rsldbg();
    return inst;
}

Qt::HANDLE ActiveBeforeDbg()
{
    return rslDbgObj()->activeBeforeDbg();
}

// ------------------------------------------------------------------------------

Rsldbg::Rsldbg() :
    is_inited(false),
    is_ui(false),
    m_ActiveBeforeDbg(nullptr),
    m_CurDbg(nullptr),
    m_RsLeng(nullptr)
{
    HMODULE hLeng = LoadLibraryW(L"rsscript.dll");
    m_RsLeng = hLeng;
    if (hLeng)
    {
        RslGetModuleFile = (tRslGetModuleFile)GetProcAddress(hLeng, "RslGetModuleFile");
        RslGetModuleFromStack = (tRslGetModuleFromStack)GetProcAddress(hLeng, "RslGetModule");
        RslGetProcNameFromStack = (tRslGetProcNameFromStack)GetProcAddress(hLeng, "RslGetProcName");
        RslGetStatementFromStack = (tRslGetStatementFromStack)GetProcAddress(hLeng, "RslGetStatement");
        RslGetStatementPos = (tRslGetStatementPos)GetProcAddress(hLeng, "RslGetStatementPos");
        RslModuleForStatement = (tRslModuleForStatement)GetProcAddress(hLeng, "RslModuleForStatement");
    }
}

Rsldbg::~Rsldbg()
{
    if (m_RsLeng)
        FreeLibrary((HMODULE)m_RsLeng);
}

Qt::HANDLE Rsldbg::activeBeforeDbg()
{
    return m_ActiveBeforeDbg;
}

int Rsldbg::size()
{
    return (int)m_Dbg.size();
}

HDBG Rsldbg::init(HRD inst,TDbgIntf * _dbg_ftable)
{
    if(!is_ui)
    {
        if(!init_ui())
            return NULL;

        is_ui = true;
    }

    for (std::vector<SpDebugPtrType>::iterator i = m_Dbg.begin(); i != m_Dbg.end(); ++i)
    {
        if ((*i)->GetInst() == inst)
            return (HDBG)GetImpl (*i);
    }

    CDebug* new_dbg = new CDebug(inst, _dbg_ftable);
    SpDebugPtrType new_dbg_aptr (new_dbg);
    m_Dbg.push_back (new_dbg_aptr);

    DbgServer *srv = new DbgServer();
    m_Servers[new_dbg] = srv;
    new_dbg->SetChildsLimit(100);
    //srv->m_curdbg = new_dbg;

    // Сервер работает в собственном потоке (раньше - QThreadPool).
    // Поток отсоединяем: живёт до завершения отладочной сессии (run()).
    std::thread([srv] { srv->run(); }).detach();

    return (HDBG)new_dbg;
}

void Rsldbg::done(HDBG hDBG)
{
    // CDebug ниже удаляется из m_Dbg (SmartPtr уничтожает объект), а сервер
    // продолжает жить в своём потоке. Обнуляем m_curdbg, чтобы поздние события
    // из очереди сервера не упали на уже освобождённом объекте.
    CDebug *finishedDbg = (CDebug*)hDBG;
    std::map<CDebug*, DbgServer*>::iterator si = m_Servers.find(finishedDbg);
    if (si != m_Servers.end())
    {
        si->second->m_curdbg = nullptr;
        m_Servers.erase(si);
    }

    if (m_Dbg.size() == 1)
    {
        is_ui = false;

        CDebug* aDBG = (CDebug*)hDBG;
        for (std::vector<SpDebugPtrType>::iterator i = m_Dbg.begin(); i != m_Dbg.end(); ++i)
        {
            if ((*i)->GetInst() == aDBG->GetInst())
            {
                m_Dbg.erase(i);
                break;
            }
        }

        done_ui();
    }
    else if (m_Dbg.size() > 1)
    {
        CDebug* aDBG = (CDebug* )hDBG;

        int ctr = 0;
        int cur_ctr = 0;

        for (std::vector<SpDebugPtrType>::iterator i = m_Dbg.begin(); i != m_Dbg.end(); ++i)
        {
            if ((*i)->GetInst() == aDBG->GetInst())
            {
                if(m_CurDbg == GetImpl(m_Dbg[0]))
                {
                    if(cur_ctr != 1){
                        ctr = 1;
                    } else {
                        ctr = cur_ctr - 1;
                    }
                }
                m_CurDbg = GetImpl(m_Dbg[ctr]);
                m_Dbg.erase(i);
                break;
            }
            ++cur_ctr;
        }
    }
}

int Rsldbg::DbgBreak(HDBG hDBG, uint32 data)
{
    CDebug* aDBG = (CDebug* )hDBG;

    TBpData *bp = nullptr;
    if (data)
    {
        bp = aDBG->FindBPbyKey (data);
        if (bp && bp->bp_type == BP_DISABLED)
            return FALSE;
    }

    return process(hDBG, bp);
}

int Rsldbg::process(HDBG hDBG, TBpData *data)
{
    CDebug* pDebug = (CDebug* )hDBG;

    int result = 1;
    DbgServer *srv = m_Servers[pDebug];
    srv->setIsNewDebug(pDebug != srv->m_curdbg);
    srv->m_curdbg = pDebug;

    if (!srv->isConnected())
    {
        // Ждём подключения клиента опросом атомарного флага, который
        // выставляется из run() сервера (сигналов Qt больше нет; GUI-помп
        // сообщений хоста здесь удалён - во время ожидания клиента хост
        // "заморожен", так же, как и на паузе отладки).
        while (!srv->isConnected() && !srv->m_finishedFlag.load())
            Sleep(10);

        if (!srv->isConnected())
            result = 0;
    }

    // ВАЖНО: DbgBreak - колбэк рантайма, и пока мы внутри него, вызывать
    // API рантайма (do_GetVersion, EnumStack и т.п.) из серверного потока
    // НЕЛЬЗЯ - рантайм держит внутреннюю блокировку, вызовы повисают
    // (проверено: серверный поток встаёт в CDebug::GetRemoteVersion).
    // Паузу после останова делает сам рантайм: DbgBreak только публикует
    // события и возвращается, а рантайм ждёт ExecContinue, который придёт
    // из серверного потока по команде клиента (EXECCONTNUE).
    srv->sendServerAction(new UpdateDbgInfoEvent(data));
    srv->sendServerAction(new BreakPointEvent(data));

    return result;
}

void Rsldbg::trace(HDBG hinst,const char *str)
{
    // Трассировочные сообщения в UI больше не выводятся (встроенный
    // интерфейс удалён), колбэк оставлен для совместимости с dbgintf.
    (void)hinst;
    (void)str;
}

CDebug *Rsldbg::currentDebug()
{
    return m_CurDbg;
}

void Rsldbg::stoploop()
{
}

void Rsldbg::remModule(HDBG hinst, RSLMODULE hmod)
{
    CDebug* pDebug = (CDebug* )hinst;
    std::vector<SpDebugPtrType>::iterator i;
    for(i = m_Dbg.begin(); i != m_Dbg.end(); ++i)
    {
        if ((*i)->GetInst() == pDebug->GetInst())
            break;
    }

    if(i != m_Dbg.end ())
    {
        std::vector <elem_bpdata>::iterator j;
        CBPData* pBPData = (*i)->GetBP ();
        for(j = pBPData->begin (); j!=pBPData->end (); )
        {
            if( hmod == (*j)->mod)
                j = pBPData->erase (j);
            else
                ++j;
        }

        CWatchV::iterator k;
        CWatchV* watch[3];
        watch[0] = (*i)->GetLocals ();
        watch[1] = (*i)->GetSurvey ();
        watch[2] = (*i)->GetQSurvey ();
        int l;
        for ( l = 0; l < 3; l++ )
        {
            if ( watch[l] )
            {
                for (k = watch[l]->begin(); k != watch[l]->end (); ++k)
                {
                    if ( (*k)->mod == hmod )
                        (*k)->Free();
                }
            }
        }

    }
}

void Rsldbg::done_ui()
{
}

bool Rsldbg::init_ui()
{
    done_ui();
    m_ActiveBeforeDbg = GetForegroundWindow();

    return true;
}

// ------------------------------------------------------------------------------

static int RSDBG DbgCount(void)
{
    return rslDbgObj()->size();
}

static HDBG RSDBG DbgInit(HRD inst, TDbgIntf *_dbg_ftable)
{
    return rslDbgObj()->init(inst, _dbg_ftable);
}

static void RSDBG DbgDone(HDBG hDBG)
{
    rslDbgObj()->done(hDBG);
    DBG_LOGI("DbgDone");
}

static int RSDBG DbgBreak (HDBG hDBG, uint32 data)
{
    return rslDbgObj()->DbgBreak(hDBG, data);
}

static void RSDBG DbgTrace (HDBG hinst,const char *str)
{
    CDebug* pDebug = (CDebug* )hinst;

    if (pDebug && pDebug->NeedConvert())
        ; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!

    rslDbgObj()->trace(hinst, str);
}

static int RSDBG DbgVersion(void)
{
    return 2; //version 2, now supports decoding  dos<->win cp
}

static void RSDBG DbgAddModule(HDBG hinst, RSLMODULE hmod)
{
    CDebug* dbg = reinterpret_cast<CDebug*> (hinst);
    dbg->GetSaver()->Command (CMD_MODULE_LOAD, reinterpret_cast<void*>(hmod));
}

static void RSDBG DbgRemModule(HDBG hinst, RSLMODULE hmod)
{
    CDebug* pDebug = (CDebug* )hinst;

    if(!pDebug)
        return;

    pDebug->GetSaver()->Command (CMD_MODULE_UNLOAD, reinterpret_cast<void*>(hmod));
    rslDbgObj()->remModule(hinst, hmod);
}

static void RSDBG DbgSetMode(HDBG hInst, int mode)
{
    CDebug* pDebug = (CDebug* )hInst;

    if(0!=mode)
        pDebug->SetExternalMode(FALSE);
}

// ------------------------------------------------------------------------------

char *RslGetModuleFile(Qt::HANDLE mod, int *isBtrStream)
{
    return rslDbgObj()->RslGetModuleFile(mod, isBtrStream);
}

Qt::HANDLE RslGetModuleFromStack(Qt::HANDLE stack)
{
    return rslDbgObj()->RslGetModuleFromStack(stack);
}

char *RslGetProcNameFromStack(Qt::HANDLE stack)
{
    return rslDbgObj()->RslGetProcNameFromStack(stack);
}

Qt::HANDLE RslGetStatementFromStack(Qt::HANDLE stack)
{
    return rslDbgObj()->RslGetStatementFromStack(stack);
}

Qt::HANDLE RslModuleForStatement(Qt::HANDLE hst)
{
    return rslDbgObj()->RslModuleForStatement(hst);
}

void RslGetStatementPos(Qt::HANDLE hst, int *offs, int *len)
{
    rslDbgObj()->RslGetStatementPos(hst, offs, len);
}

// ------------------------------------------------------------------------------

static TRslDbg dbgInterface =
{
        DbgVersion,
        DbgCount,
        DbgInit,
        DbgDone,
        DbgBreak,
        DbgTrace,
        DbgAddModule,
        DbgRemModule,
        DbgSetMode
};

extern "C" __declspec(dllexport)
TRslDbg *RSDBG RslGetInterface (void)
{
    return &dbgInterface;
}

// Программное переключение режима клиента (перекрывает rsldbg.ini):
// 0 = gui (локальный GUI), 1 = agent, 2 = gui_remote.
// Можно вызывать из хост-приложения/скрипта до срабатывания debugbreak.
extern "C" __declspec(dllexport)
void RSDBG RslDbgSetClientMode (int mode)
{
    dbgSetClientMode(DbgClientMode(mode));
}
