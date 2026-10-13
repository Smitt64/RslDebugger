#include <winsock2.h>
#include <ws2tcpip.h>
#include "dbgserver.h"
#include "clocals.h"
#include "dbgserverproto.h"
#include "server/breakpointevent.h"
#include "server/serveractionevent.h"
#include "server/updatedbginfoevent.h"
#include <QThread>
#include <QProcess>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QApplication>
#include "cdebug.h"
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDataStream>
#include <QTextCodec>
#include <QBuffer>
#include "rsldbg.h"

QString toolProcessStateText(qint16 State);
QString toolProcessExitStatusText(qint16 State);
QString toolGetProcessErrorText(const QProcess::ProcessError &error);

// Locate rsldbgclnt.exe: next to rsldbg.dll (the client is deployed next to
// the debugger dll), with fallbacks to the host exe dir and plain PATH lookup.
static QString toolFindDebugClient()
{
    QStringList candidates;

    HMODULE hd = GetModuleHandleW(L"rsldbg.dll");
    if (hd)
    {
        wchar_t path[MAX_PATH] = {};
        if (GetModuleFileNameW(hd, path, MAX_PATH))
            candidates << QFileInfo(QString::fromWCharArray(path)).absolutePath() + "/rsldbgclnt.exe";
    }

    candidates << QCoreApplication::applicationDirPath() + "/rsldbgclnt.exe";
    candidates << QCoreApplication::applicationDirPath() + "/../bin/rsldbgclnt.exe";
    candidates << QCoreApplication::applicationDirPath() + "/../../bin/rsldbgclnt.exe";

    for (const QString &candidate : candidates)
    {
        QString clean = QDir::cleanPath(candidate);
        if (QFileInfo::exists(clean))
            return clean;
    }

    return QStringLiteral("rsldbgclnt.exe");
}

DbgServer::DbgServer(QObject *parent)
    : DbgServerBase{parent}
{
    prevStack = 0;
    newDbg = true;
    m_curModuleInView = nullptr;

    // DirectConnection: выставляем флаг прямо в рабочем потоке сервера,
    // Rsldbg::process() опрашивает его без участия сигналов.
    QObject::connect(this, &DbgServer::execContinue, this, [this]
    {
        m_continueFlag.storeRelease(1);
    }, Qt::DirectConnection);
}

DbgServer::~DbgServer()
{
    //closesocket(ClientSocket);
    qDebug() << "~DbgServer()";
}

void DbgServer::setIsNewDebug(const bool &v)
{
    newDbg = v;
}

QString DbgServer::ReadTextFileContent(const QString &filename, const QString &encode)
{
    QString content;
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (encode.isEmpty())
            content = f.readAll();
        else
        {
            QTextStream stream(&f);
            stream.setCodec(encode.toLocal8Bit().data());

            content = stream.readAll();
        }

        f.close();
    }
    return content;
}

bool DbgServer::startApp()
{
    QNetworkAddressEntry adress;
    foreach(const QNetworkInterface &qNetInterface, QNetworkInterface::allInterfaces())
    {
        foreach(const QNetworkAddressEntry &qNetInterfaceAddress, qNetInterface.addressEntries())
        {
            if (qNetInterfaceAddress.ip().protocol() == QAbstractSocket::IPv4Protocol
                && qNetInterfaceAddress.ip() != QHostAddress(QHostAddress::LocalHost)
                && qNetInterfaceAddress.netmask().toString() != "")
            {
                adress = qNetInterfaceAddress;
                break;
                //qDebug() << qNetInterfaceAddress.ip().toString();
                //qDebug() << qNetInterfaceAddress.netmask().toString();
            }
        }
    }

    QString program = toolFindDebugClient();
    qCInfo(dbgServer()) << QString("Starting debug client: %1").arg(program);
    m_pProc.reset(new QProcess());
    m_pProc->setProgram(program);
    m_pProc->setArguments(
    {
        "--host", "127.0.0.1",//adress.ip().toString(),
        "--port", QString::number(port())
    });

    bool hr = true;
    QObject::connect(m_pProc.data(), &QProcess::stateChanged, [](QProcess::ProcessState newState)
                     {
                        qCInfo(dbgServer()) << QString("Process state changed to: %1 (%2)")
                        .arg(newState).arg(toolProcessStateText(newState));
                     });
    QObject::connect(m_pProc.data(), &QProcess::errorOccurred, [&hr](QProcess::ProcessError error)
                     {
                        qCInfo(dbgServer()) << QString("Process error occurred: %1 (%2)")
                        .arg(error).arg(toolGetProcessErrorText(error));

                        hr = false;
                     });

    m_pProc->start();
    m_pProc->waitForStarted();

    if (m_pProc->state() == QProcess::NotRunning)
        hr = false;

    //qDebug() << m_pProc->state() << m_pProc->exitStatus() << m_pProc->exitCode() << m_pProc->error();

    return hr;
}

void DbgServer::process(const QByteArray &data, const qint16 &action)
{
    /*if (action == DBG_REQUEST_EXECCONTNUE)
    {
        DBG_EXECCONTNUE *exec = (DBG_EXECCONTNUE*)data.data();
        m_curdbg->SetDebugState(false);
        m_curdbg->do_ExecContinue(exec->trace_log);
        qDebug() << "DBG_REQUEST_EXECCONTNUE" << exec->trace_log;
    }*/
}

void DbgServer::fillBpData(DBGBPDATA *body, Qt::HANDLE BpData)
{
    TBpData *data = (TBpData*)BpData;
    body->bp_type = data->bp_type;
    body->offs = data->offs;
    body->len = data->len;
    body->line = data->line;
    body->key = data->key;
    body->mod = reinterpret_cast<qint64>(data->mod);
    body->stmt = reinterpret_cast<qint64>(data->stmt);
    qstrcpy(body->modname, data->modname.toLocal8Bit().data());
}

void DbgServer::sendEventBreakPoint(Qt::HANDLE BpData)
{
    TBpData *data = (TBpData*)BpData;

    DBGEVENT event;
    event.event = MSG_BREAKPOINT;
    event.data.bp_event.stackindex = 0;

    if (data)
    {
        event.data.bp_event.BpSetted = 1;
        fillBpData(&event.data.bp_event, data);
    }
    else
    {
        event.data.bp_event.BpSetted = 0;
        memset(&event.data.bp_event, 0, sizeof(DBGBPDATA));
    }

    DBGHEADER handshake;
    DbgMakeHeader(&handshake, 0, DBG_REQUEST_EVENT);
    write(&handshake, &event, sizeof(DBGEVENT));
}

void DbgServer::UpdateDbgInfo(TBpData *data)
{
    m_curdbg->GetRemoteVersion();
    m_curdbg->UpdateDbgInfo();
    m_curdbg->SetDebugState(true);
    m_curdbg->SetCurModule((RSLMODULE)-1);

    int offs, len;
    if (data && data->bp_type == BP_INVISIBLE)
    {
        m_curdbg->do_ClrBreakPoint (m_curdbg->GetCurStatement (&offs, &len));
        m_curdbg->DelBp(data);
    }

    m_curdbg->SetIndex(0);
    UpdateDbgInfo(0);
}

void DbgServer::UpdateVariables(const int &index)
{
    CLocals* pLocals = m_curdbg->GetLocals ();
    RSLSTACK st = m_curdbg->GetStackAt (index);
    pLocals->CollectLocals (st, &prevStack, newDbg);

    CLocals::iterator i;
    for (i = pLocals->begin (); i != pLocals->end (); ++i)
    {
        QString name = (*i)->str_name;
        QString value = (*i)->str_value;
        QByteArray type = (*i)->str_type;

        if (name == "THIS" && value.contains("Object") && !m_ProcNamespace.contains(st))
            m_ProcNamespace.insert(st, new QByteArray(type));
    }

    ShowVariables(index);
}

void DbgServer::ShowVariables(const int &index)
{
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");

    RSLSTACK st = m_curdbg->GetStackAt(index);
    CLocals* pLocals = m_curdbg->GetLocals();

    QByteArray buffer;
    QBuffer stream(&buffer);
    stream.open(QIODevice::ReadWrite);

    DBG_LOCALS locals;
    memset(&locals, 0, sizeof(DBG_LOCALS));
    stream.write((const char*)&locals, sizeof(DBG_LOCALS));

    CLocals::iterator i;

    for (i = pLocals->begin (); i != pLocals->end (); ++i)
    {
        DBG_VARIABLEDATA var;
        var.index = std::distance(pLocals->begin(), i);
        (*i)->toDbgVariable(&var);

        if (!(*i)->str_value.isEmpty())
            var.value_size = (*i)->str_value.size() + 1;
        else
            var.value_size = 0;

        stream.write((const char*)&var, sizeof(DBG_VARIABLEDATA));

        if (var.value_size)
        {
            char *value = new char[var.value_size];
            memset(value, 0, var.value_size);
            qstrcpy(value, (*i)->str_value.data());
            stream.write(value, var.value_size);
            delete[] value;
        }

        locals.var_count ++;
    }

    stream.seek(0);
    stream.write((const char*)&locals, sizeof(DBG_LOCALS));
    stream.close();

    write(DBG_REQUEST_UPDATELOCALS, buffer);
}

void DbgServer::UpdateDbgInfo(const int &index)
{
    UpdateText(index);
    UpdateVariables(index);
    UpdateStack();
    UpdateWatch();
}

void DbgServer::UpdateWatch()
{
    m_curdbg->UpdateSurvey();
    //ShowSurvey (/*index*/);
}

void DbgServer::UpdateStack()
{
    QVector<DBG_UPDATSTACK> packet;
    int rv = m_curdbg->GetStackCount();
    for (int i = 0; i < rv; i++)
    {
        DBG_UPDATSTACK item;
        memset(&item, 0, sizeof(DBG_UPDATSTACK));

        Qt::HANDLE stack = m_curdbg->GetStackAt(i);
        Qt::HANDLE module = RslGetModuleFromStack(stack);
        Qt::HANDLE hst = RslGetStatementFromStack(stack);

        int isBtrStream = 0;
        RslGetStatementPos(hst, &item.offs, &item.len);
        item.line = RslGetModuleLine(module, item.offs, item.len);
        item.module = reinterpret_cast<qint64>(module);

        qstrcpy(item.fullfilename, RslGetModuleFile(module, &isBtrStream));
        qstrcpy(item.func, RslGetProcNameFromStack(stack));

        if (m_ProcNamespace.contains(stack))
            qstrcpy(item.fnamespace, m_ProcNamespace[stack]->data());

        packet.append(item);
    }

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    if (!packet.isEmpty())
        stream.writeRawData((const char*)packet.constData(), sizeof(DBG_UPDATSTACK) * packet.size());

    DBGHEADER hdr;
    DbgMakeHeader(&hdr, sizeof(DBG_UPDATEBP), DBG_REQUEST_UPDATESTACK);
    write(&hdr, nullptr, 0, data);
}

void DbgServer::UpdateText(const int &index)
{
    RSLMODULE mod = m_curdbg->GetCurModule(index);

    int offs = 0;
    int len = 0;
    int line = 0;

    RSLSTMT statement = m_curdbg->GetCurStatement(&offs, &len, index);
    line = m_curdbg->GetStatementLine(offs, len, mod);

    DBGHEADER hdr;
    DBG_UPDATETEXT updtext;
    memset(&updtext, 0, sizeof(DBG_UPDATETEXT));

    updtext.offs = offs;
    updtext.len = len;
    updtext.line = line;
    updtext.curModuleInView = reinterpret_cast<qint64>(mod);

    // NB: в оригинале m_curModuleInView в DbgServer никогда не инициализировался
    // и не присваивался, поэтому пакет UPDATETEXT отправлялся на КАЖДОМ шаге -
    // клиент по нему обновляет позицию курсора. Не "оптимизировать" условием
    // m_curModuleInView != mod: иначе при шагах внутри одного модуля клиент
    // не узнает о смене строки.
    {
        int isBtrStream = 0;
        char *filename = RslGetModuleFile((Qt::HANDLE)mod, &isBtrStream);

        QString content = ReadTextFileContent(filename, "IBM 866");
        QByteArray text = content.toLocal8Bit();

        qstrncpy(updtext.filename, filename, sizeof(updtext.filename));
        updtext.size = text.size();

        DbgMakeHeader(&hdr, sizeof(DBG_UPDATETEXT) + updtext.size, DBG_REQUEST_UPDATETEXT);
        write(&hdr, &updtext, sizeof(DBG_UPDATETEXT), text);

        m_curModuleInView = mod;
    }

    TBpData* bp;
    QVector<DBGBPDATA> tmpbp;

    int nBP = m_curdbg->GetBPCount();
    for (int i = 0; i < nBP; ++i)
    {
        bp = m_curdbg->GetBP(i);
        if (mod == bp->mod)
        {
            DBGBPDATA dbgbp;
            fillBpData(&dbgbp, bp);
            tmpbp.push_back(dbgbp);
            /*if (bp->bp_type == BP_DISABLED)
                color = RGB (0, 0, 255);
            else if (bp->bp_type == BP_ENABLED)
                color = RGB(0, 255, 0);
            else
                color = GetSysColor (COLOR_WINDOW);

            SetTextColor (bp->offs, bp->len, color, bp->line);*/
        }
    }

    if (!tmpbp.empty())
    {
        QByteArray bpdata;
        QDataStream stream(&bpdata, QIODevice::WriteOnly);
        stream.writeRawData((char*)&tmpbp[0], sizeof(DBGBPDATA) * tmpbp.size());

        DBG_UPDATEBP upd;
        upd.count = tmpbp.size();

        DBGHEADER hdr;
        DbgMakeHeader(&hdr, sizeof(DBG_UPDATEBP), DBG_REQUEST_UPDATEBP);
        write(&hdr, &upd, sizeof(DBG_UPDATETEXT), bpdata);
    }
}

void DbgServer::ShowVariableValue(const DBG_GETVALUENFO &getvalue)
{
    int size = 0;
    char str_name[MAX_NAME] = {0};
    char str_type[MAX_TYPENAME] = {0};
    char str_value[MAX_VALUE] = {0};
    RSLVALUE val  = NULL;
    m_curdbg->do_GetValueSize(reinterpret_cast<RSLVINFO>(getvalue.info), &size);

    if (size)
    {
        QByteArray result;
        DBG_GETVALUENFO_RESULT resfata;
        resfata.size = size + 1;

        char *value = new char[resfata.size];
        memset(value, 0, resfata.size);
        m_curdbg->do_GetValueData(reinterpret_cast<RSLVINFO>(getvalue.info), value, resfata.size, 0, &size);
        m_curdbg->do_RefreshInfo(reinterpret_cast<RSLVINFO>(getvalue.info), &val, NULL, str_name, MAX_NAME, str_type, MAX_TYPENAME, str_value, MAX_VALUE);

        qstrcpy(resfata.str_name, str_name);
        qstrcpy(resfata.str_type, str_type);

        result.append((char*)&resfata, sizeof(DBG_GETVALUENFO_RESULT));
        result.append(value, resfata.size);

        write(DBG_REQUEST_GETVALUENFO, result);
        delete[] value;
    }
}

int DbgServer::RslGetModuleLine(Qt::HANDLE module, int offs, int len)
{
    int      line = -1;
    int      realoffs, reallen;
    RSLSTMT  stmt;
    //RslGetStatementPos(hst, offs, len);
    m_curdbg->do_GetStatementOfPos((RSLMODULE)module, offs, len, &realoffs, &reallen, &stmt, &line);

    return line;
}

bool DbgServer::serverActionEvent(ServerActionEvent *e)
{
    QByteArray &data = e->buffer();
    if (e->action() == DBG_REQUEST_EXECCONTNUE)
    {
        DBG_EXECCONTNUE *exec = (DBG_EXECCONTNUE*)data.data();
        m_curdbg->SetDebugState(false);
        m_curdbg->do_ExecContinue(exec->trace_log);
        emit execContinue();
    }
    else if (e->action() == DBG_REQUEST_EXPANDVARIABLE)
    {
        DBG_EXPANDVARIABLE *expand = (DBG_EXPANDVARIABLE*)data.data();

        CLocals* pLocals = m_curdbg->GetLocals();
        RSLSTACK st = m_curdbg->GetStackAt (expand->stackindex);
        pLocals->ExpandV(expand->index);
        //pLocals->CollectLocals (st, &prevStack, newDbg);
        //pLocals->ExpandV(expand->index);//reinterpret_cast<RSLSTACK>(expand->st)
        ShowVariables(0);
    }
    else if (e->action() == DBG_REQUEST_GETVALUENFO)
    {
        DBG_GETVALUENFO *getvalue = (DBG_GETVALUENFO*)data.data();
        ShowVariableValue(*getvalue);
    }

    return DbgServerBase::serverActionEvent(e);
}

bool DbgServer::serverEvent(QEvent *event)
{
    if (event->type() == MSG_UPDATEDBGINFO_EVENT)
    {
        UpdateDbgInfoEvent *e = dynamic_cast<UpdateDbgInfoEvent*>(event);
        UpdateDbgInfo((TBpData*)e->data());
    }
    else if (event->type() == MSG_BREAKPOINT)
    {
        BreakPointEvent *e = dynamic_cast<BreakPointEvent*>(event);
        sendEventBreakPoint(e->data());
    }

    return DbgServerBase::serverEvent(event);
}
