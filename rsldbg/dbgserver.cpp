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
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "rsldbg.h"
#include "dbgsettings.h"

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
    // В режимах agent/gui_remote клиент подключается сам, GUI не запускаем
    // (проверка режима также есть в DbgServerBase::run(), дублируем для ясности).
    if (dbgSettings().clientMode != DbgClientGui)
        return true;

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

    if (port() <= 0)
    {
        qCCritical(dbgServer()) << "Invalid server port (0), refusing to start client";
        return false;
    }

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

void DbgServer::sendEventBreakPoint(Qt::HANDLE BpData)
{
    TBpData *data = (TBpData*)BpData;

    QJsonObject root;
    root["event"] = QStringLiteral("breakpoint");
    root["set"] = (data != nullptr);

    if (data)
    {
        QJsonObject bp;
        bp["type"] = (int)data->bp_type;
        bp["offs"] = data->offs;
        bp["len"] = data->len;
        bp["line"] = data->line;
        bp["key"] = QString::number((qint64)data->key);
        bp["modname"] = data->modname;
        bp["mod"] = QString::number(reinterpret_cast<qint64>(data->mod));
        bp["stmt"] = QString::number(reinterpret_cast<qint64>(data->stmt));
        root["bp"] = bp;
    }

    write(DBG_REQUEST_EVENT_JSON, QJsonDocument(root).toJson(QJsonDocument::Compact));
}

void DbgServer::UpdateDbgInfo(TBpData *data)
{
    // Событие могло дождаться обработки уже после Rsldbg::done() -
    // CDebug уничтожен, указатель обнулён в done().
    if (!m_curdbg)
        return;

    m_curdbg->GetRemoteVersion();
    m_curdbg->UpdateDbgInfo();
    m_curdbg->SetDebugState(true);
    m_curdbg->SetCurModule((RSLMODULE)-1);

    int offs = 0, len = 0;
    if (data && data->bp_type == BP_INVISIBLE)
    {
        RSLSTMT stmt = m_curdbg->GetCurStatement(&offs, &len);
        if (stmt)
            m_curdbg->do_ClrBreakPoint(stmt);
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
    // Имена/значения приходят из рантайма в ANSI; исторически клиент
    // декодировал их как Windows-1251 - сохраняем то же отображение.
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");

    RSLSTACK st = m_curdbg->GetStackAt(index);
    CLocals* pLocals = m_curdbg->GetLocals();

    QJsonArray vars;

    CLocals::iterator i;

    for (i = pLocals->begin (); i != pLocals->end (); ++i)
    {
        QJsonObject obj;
        obj["index"] = (int)std::distance(pLocals->begin(), i);
        obj["name"] = codec->toUnicode((*i)->str_name);
        obj["type"] = codec->toUnicode((*i)->str_type);
        obj["proc"] = codec->toUnicode((*i)->str_proc);
        obj["depth"] = (int)(*i)->depth;
        obj["object"] = (*i)->is_object;
        obj["expanded"] = (*i)->is_expanded;
        obj["lvalue"] = (*i)->is_lvalue;
        obj["fakeChildren"] = (*i)->isFakeChildrensItem;

        // 64-битные хендлы строками: QJsonValue хранит числа как double,
        // а double точно представляет целые только до 2^53.
        obj["val"] = QString::number(reinterpret_cast<qint64>((*i)->val));
        obj["exp"] = QString::number(reinterpret_cast<qint64>((*i)->exp));
        obj["st"] = QString::number(reinterpret_cast<qint64>((*i)->st));
        obj["procHandle"] = QString::number(reinterpret_cast<qint64>((*i)->proc));
        obj["info"] = QString::number(reinterpret_cast<qint64>((*i)->info));
        obj["mod"] = QString::number(reinterpret_cast<qint64>((*i)->mod));

        if (!(*i)->str_value.isEmpty())
            obj["value"] = codec->toUnicode((*i)->str_value);

        vars.append(obj);
    }

    QJsonObject root;
    root["vars"] = vars;

    write(DBG_REQUEST_UPDATELOCALS_JSON, QJsonDocument(root).toJson(QJsonDocument::Compact));
}

void DbgServer::UpdateDbgInfo(const int &index)
{
    // При завершении программы стек может оказаться пустым - обновлять нечего.
    if (!m_curdbg || m_curdbg->GetStackCount() == 0)
        return;

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
    // Клиент исторически декодировал имя файла/функции как IBM 866,
    // а namespace как Windows-1251 - сохраняем то же отображение.
    QTextCodec *oem866 = QTextCodec::codecForName("IBM 866");
    QTextCodec *cp1251 = QTextCodec::codecForName("Windows-1251");

    QJsonArray frames;
    int rv = m_curdbg->GetStackCount();
    for (int i = 0; i < rv; i++)
    {
        Qt::HANDLE stack = m_curdbg->GetStackAt(i);
        Qt::HANDLE module = RslGetModuleFromStack(stack);
        Qt::HANDLE hst = RslGetStatementFromStack(stack);

        int offs = 0, len = 0;
        int isBtrStream = 0;
        RslGetStatementPos(hst, &offs, &len);

        QJsonObject frame;
        frame["offs"] = offs;
        frame["len"] = len;
        frame["line"] = RslGetModuleLine(module, offs, len);
        frame["module"] = QString::number(reinterpret_cast<qint64>(module));
        frame["file"] = oem866->toUnicode(RslGetModuleFile(module, &isBtrStream));
        frame["func"] = oem866->toUnicode(RslGetProcNameFromStack(stack));

        if (m_ProcNamespace.contains(stack))
            frame["namespace"] = cp1251->toUnicode(m_ProcNamespace[stack]->data());

        frames.append(frame);
    }

    QJsonObject root;
    root["frames"] = frames;

    write(DBG_REQUEST_UPDATESTACK_JSON, QJsonDocument(root).toJson(QJsonDocument::Compact));
}

void DbgServer::UpdateText(const int &index)
{
    // Текст модуля читается из файла в IBM 866, имя файла - тоже OEM,
    // поэтому оба декодируем этим кодеком (клиент больше не трогает кодировки).
    QTextCodec *oem866 = QTextCodec::codecForName("IBM 866");

    RSLMODULE mod = m_curdbg->GetCurModule(index);

    int offs = 0;
    int len = 0;

    RSLSTMT statement = m_curdbg->GetCurStatement(&offs, &len, index);
    int line = m_curdbg->GetStatementLine(offs, len, mod);

    // NB: UPDATETEXT отправляется на КАЖДОМ шаге - клиент по нему обновляет
    // позицию курсора. Не "оптимизировать" условием m_curModuleInView != mod:
    // иначе при шагах внутри одного модуля клиент не узнает о смене строки.
    QJsonObject root;
    root["offs"] = offs;
    root["len"] = len;
    root["line"] = line;
    root["curModuleInView"] = QString::number(reinterpret_cast<qint64>(mod));

    int isBtrStream = 0;
    char *filename = RslGetModuleFile((Qt::HANDLE)mod, &isBtrStream);

    const QString fileNameStr = oem866->toUnicode(filename);
    if (m_TextCacheFile != fileNameStr)
    {
        m_TextCacheFile = fileNameStr;
        m_TextCacheContent = ReadTextFileContent(filename, "IBM 866");
    }

    root["filename"] = fileNameStr;
    if (!m_TextCacheContent.isEmpty())
        root["text"] = m_TextCacheContent;

    write(DBG_REQUEST_UPDATETEXT_JSON, QJsonDocument(root).toJson(QJsonDocument::Compact));

    m_curModuleInView = mod;

    // Брейкпоинты текущего модуля
    QJsonArray bps;

    int nBP = m_curdbg->GetBPCount();
    for (int i = 0; i < nBP; ++i)
    {
        TBpData *bp = m_curdbg->GetBP(i);
        if (mod == bp->mod)
        {
            QJsonObject item;
            item["type"] = (int)bp->bp_type;
            item["offs"] = bp->offs;
            item["len"] = bp->len;
            item["line"] = bp->line;
            item["key"] = QString::number((qint64)bp->key);
            item["modname"] = bp->modname;
            item["mod"] = QString::number(reinterpret_cast<qint64>(bp->mod));
            item["stmt"] = QString::number(reinterpret_cast<qint64>(bp->stmt));
            bps.append(item);
        }
    }

    if (!bps.isEmpty())
    {
        QJsonObject bproot;
        bproot["bps"] = bps;
        write(DBG_REQUEST_UPDATEBP_JSON, QJsonDocument(bproot).toJson(QJsonDocument::Compact));
    }
}

void DbgServer::ShowVariableValue(const qint64 &val, const qint64 &info)
{
    // Имя/тип/значение исторически декодировались клиентом как Windows-1251.
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");

    int size = 0;
    char str_name[MAX_NAME] = {0};
    char str_type[MAX_TYPENAME] = {0};
    char str_value[MAX_VALUE] = {0};
    RSLVALUE v  = NULL;
    m_curdbg->do_GetValueSize(reinterpret_cast<RSLVINFO>(info), &size);

    QByteArray value;
    if (size)
    {
        char *buf = new char[size + 1];
        memset(buf, 0, size + 1);
        m_curdbg->do_GetValueData(reinterpret_cast<RSLVINFO>(info), buf, size + 1, 0, &size);
        value = QByteArray(buf, size);
        delete[] buf;
    }

    m_curdbg->do_RefreshInfo(reinterpret_cast<RSLVINFO>(info), &v, NULL,
                             str_name, MAX_NAME, str_type, MAX_TYPENAME,
                             str_value, MAX_VALUE);

    // Отвечаем всегда, иначе клиент ждёт кадр до таймаута: для объектов
    // do_GetValueSize возвращает 0 и сырых данных нет, но строковое
    // представление (str_value) RefreshInfo всё равно даёт.
    QJsonObject root;
    root["name"] = codec->toUnicode(str_name);
    root["type"] = codec->toUnicode(str_type);
    root["value"] = size ? codec->toUnicode(value)
                         : codec->toUnicode(str_value);

    write(DBG_REQUEST_GETVALUENFO_JSON, QJsonDocument(root).toJson(QJsonDocument::Compact));
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
    // Клиент мог прислать команду после завершения отладки (done()):
    // CDebug уничтожен, все действия ниже требуют живого m_curdbg.
    if (!m_curdbg)
        return false;

    QByteArray &data = e->buffer();
    if (e->action() == DBG_REQUEST_EXECCONTNUE_JSON)
    {
        QJsonObject obj = QJsonDocument::fromJson(data).object();
        m_curdbg->SetDebugState(false);
        m_curdbg->do_ExecContinue(obj.value("traceLog").toInt());
        emit execContinue();
    }
    else if (e->action() == DBG_REQUEST_EXPANDVARIABLE_JSON)
    {
        QJsonObject obj = QJsonDocument::fromJson(data).object();

        CLocals* pLocals = m_curdbg->GetLocals();
        pLocals->ExpandV(obj.value("index").toInt());
        ShowVariables(0);
    }
    else if (e->action() == DBG_REQUEST_GETVALUENFO_JSON)
    {
        QJsonObject obj = QJsonDocument::fromJson(data).object();

        ShowVariableValue(obj.value("val").toString().toLongLong(),
                          obj.value("info").toString().toLongLong());
    }

    return DbgServerBase::serverActionEvent(e);
}

void DbgServer::clientDisconnected()
{
    // Клиент ушёл, не сняв программу с паузы: интерпретатор ждёт
    // ExecContinue и без него ведущее приложение зависнет навсегда.
    // Режим abort - то же, что клиент шлёт из DbgClient::stop().
    if (m_curdbg && m_curdbg->GetDebugState())
    {
        qInfo(dbgServer()) << "Client disconnected while paused, aborting execution";
        m_curdbg->SetDebugState(false);
        m_curdbg->do_ExecContinue(EXECCONTNUE_DBG_ABORT);
    }
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
