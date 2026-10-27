#include "dbgserver.h"
#include "qdebug.h"
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
#include <windows.h>
#include <QApplication>
#include <QLibrary>
#include "rsldbg.h"
#include <QThreadPool>
#include <QCoreApplication>
#include <QThread>

// Прокачка событий во время ожидания в Rsldbg::process(). Сигналы из рабочего
// потока сервера до QEventLoop не доходят (объект сервера живёт в QThread,
// который никогда не запускается), поэтому ждём опросом атомарных флагов.
static void dbgPumpEvents()
{
    QCoreApplication *app = QCoreApplication::instance();

    if (app && QThread::currentThread() == app->thread())
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    else
        QThread::msleep(10);
}

Q_GLOBAL_STATIC(Rsldbg, rsldbg)

Qt::HANDLE ActiveBeforeDbg()
{
    return rsldbg->activeBeforeDbg();
}

Rsldbg *rslDbgObj()
{
    return rsldbg;
}

// ------------------------------------------------------------------------------

Rsldbg::Rsldbg() :
    m_pApp(nullptr),
    argv(nullptr),
    is_inited(false),
    is_ui(false),
    m_CurDbg(nullptr)
{
    QStringList arguments = QApplication::arguments();

    int argc = arguments.count();

    argv = new char*[argc];
    for (int i = 0; i < argc; i++)
    {
        QString element = arguments.at(i);
        argv[i] = new char[element.size() + 1];
        strcpy(argv[i], element.toStdString().c_str());
    }

    m_RsLeng.reset(new QLibrary("rsscript.dll"));
    if (m_RsLeng->load())
    {
        RslGetModuleFile = (tRslGetModuleFile)m_RsLeng->resolve("RslGetModuleFile");
        RslGetModuleFromStack = (tRslGetModuleFromStack)m_RsLeng->resolve("RslGetModule");
        RslGetProcNameFromStack = (tRslGetProcNameFromStack)m_RsLeng->resolve("RslGetProcName");
        RslGetStatementFromStack = (tRslGetStatementFromStack)m_RsLeng->resolve("RslGetStatement");
        RslGetStatementPos = (tRslGetStatementPos)m_RsLeng->resolve("RslGetStatementPos");
        RslModuleForStatement = (tRslModuleForStatement)m_RsLeng->resolve("RslModuleForStatement");
        //RslGetStatementPosEx = (tRslGetStatementPosEx)m_RsLeng->resolve("RslGetStatementPosEx");
    }

    if (qApp)
        m_pApp = qApp;
    else
        m_pApp = new QApplication(argc, argv);

    m_Thread.reset(new QThread());
    //m_pApp.reset();
}

Rsldbg::~Rsldbg()
{
    //if (m_pApp)
        //delete m_pApp;

    for (int i = 0; i < m_pApp->arguments().count(); i++)
        delete[] argv[i];

    delete[] argv;
}

Qt::HANDLE Rsldbg::activeBeforeDbg()
{
    return m_ActiveBeforeDbg;
}

int Rsldbg::size()
{
    return m_Dbg.size();
}

HDBG Rsldbg::init(HRD inst,TDbgIntf * _dbg_ftable)
{
    if(!is_ui)
    {
        if(!init_ui())
            return NULL;

        is_ui = true;
    }

    QVector<SpDebugPtrType>::iterator i;
    for(i = m_Dbg.begin(); i != m_Dbg.end(); ++i)
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

    srv->moveToThread(m_Thread.data());
    QThreadPool::globalInstance()->start(srv);

    return (HDBG)new_dbg;
}

void Rsldbg::done(HDBG hDBG)
{
    // CDebug ниже удаляется из m_Dbg (SmartPtr уничтожает объект), а сервер
    // продолжает жить в пуле потоков. Обнуляем m_curdbg, чтобы поздние события
    // из очереди сервера не упали на уже освобождённом объекте.
    CDebug *finishedDbg = (CDebug*)hDBG;
    DbgServer *finishedSrv = m_Servers.value(finishedDbg, nullptr);
    if (finishedSrv)
    {
        finishedSrv->m_curdbg = nullptr;
        m_Servers.remove(finishedDbg);
    }

    if (m_Dbg.size() == 1)
    {
        is_ui = false;

        CDebug* aDBG = (CDebug*)hDBG;
        QVector<SpDebugPtrType>::iterator i;
        for(i = m_Dbg.begin(); i != m_Dbg.end(); ++i)
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
        QVector<SpDebugPtrType>::iterator i;

        int ctr = 0;
        int cur_ctr = 0;

        for(i = m_Dbg.begin(); i != m_Dbg.end(); ++i)
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

    //ReplyMessage((LRESULT)TRUE);
    if (!srv->isConnected())
    {
        // QEventLoop + сигналы started/finished здесь не работают: сигнал
        // started эмитится из потока пула, а объект сервера живёт в QThread,
        // который никогда не запускается, и queued-доставка не происходит
        // (проверено: loop.exec() висит после "Handshake sent").
        // Ждём опросом атомарного флага, который выставляется из run()
        // сервера через DirectConnection.
        while (!srv->isConnected() && !srv->m_finishedFlag.loadAcquire())
            dbgPumpEvents();

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
    Q_UNUSED(hinst);
    Q_UNUSED(str);
}

CDebug *Rsldbg::currentDebug()
{
    return m_CurDbg;
}

void Rsldbg::stoploop()
{
    //m_EventLoop.quit();
}

void Rsldbg::remModule(HDBG hinst, RSLMODULE hmod)
{
    CDebug* pDebug = (CDebug* )hinst;
    QVector<SpDebugPtrType>::iterator i;
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
    return rsldbg->size();
}

static HDBG RSDBG DbgInit(HRD inst, TDbgIntf *_dbg_ftable)
{
    return rsldbg->init(inst, _dbg_ftable);
}

static void RSDBG DbgDone(HDBG hDBG)
{
    rsldbg->done(hDBG);
    qDebug() << "DbgDone";
}

static int RSDBG DbgBreak (HDBG hDBG, uint32 data)
{
    return rsldbg->DbgBreak(hDBG, data);
}

static void RSDBG DbgTrace (HDBG hinst,const char *str)
{
    CDebug* pDebug = (CDebug* )hinst;

    if (pDebug && pDebug->NeedConvert())
        ; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!

    rsldbg->trace(hinst, str);
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
    rsldbg->remModule(hinst, hmod);
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
    return rsldbg->RslGetModuleFile(mod, isBtrStream);
}

Qt::HANDLE RslGetModuleFromStack(Qt::HANDLE stack)
{
    return rsldbg->RslGetModuleFromStack(stack);
}

char *RslGetProcNameFromStack(Qt::HANDLE stack)
{
    return rsldbg->RslGetProcNameFromStack(stack);
}

Qt::HANDLE RslGetStatementFromStack(Qt::HANDLE stack)
{
    return rsldbg->RslGetStatementFromStack(stack);
}

Qt::HANDLE RslModuleForStatement(Qt::HANDLE hst)
{
    return rsldbg->RslModuleForStatement(hst);
}

void RslGetStatementPos(Qt::HANDLE hst, int *offs, int *len)
{
    rsldbg->RslGetStatementPos(hst, offs, len);
}

/*int RslGetModuleLine(Qt::HANDLE module, int offs, int len)
{
    int      line = -1;
    int      realoffs, reallen;
    RSLSTMT  stmt;
    //RslGetStatementPos(hst, offs, len);
    rsldbg->currentDebug()->do_GetStatementOfPos((RSLMODULE)module, offs, len, &realoffs, &reallen, &stmt, &line);

    return line;
}*/

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
