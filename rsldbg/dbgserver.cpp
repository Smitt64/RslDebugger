#include <winsock2.h>
#include <ws2tcpip.h>
#include "dbgserver.h"
#include "clocals.h"
#include "dbgserverproto.h"
#include "server/breakpointevent.h"
#include "server/serveractionevent.h"
#include "server/updatedbginfoevent.h"
#include "cdebug.h"
#include "rsldbg.h"
#include "dbgsettings.h"
#include "dbglog.h"
#include "strconv.h"
#include "vendor/json.hpp"

#include <vector>
#include <cstdio>
#include <cstdlib> // atoll

using nlohmann::json;

// Locate rsldbgclnt.exe: next to rsldbg.dll (the client is deployed next to
// the debugger dll), with fallbacks to the host exe dir and plain PATH lookup.
static std::string toolFindDebugClient()
{
    std::vector<std::string> candidates;

    const std::string dllDir = dbgDllDir();
    if (!dllDir.empty())
        candidates.push_back(dllDir + "\\rsldbgclnt.exe");

    wchar_t exePath[MAX_PATH] = {};
    if (GetModuleFileNameW(NULL, exePath, MAX_PATH))
    {
        std::string exeDir = dbgstr::wide_to_utf8(exePath);
        const std::string::size_type pos = exeDir.find_last_of("\\/");
        if (pos != std::string::npos)
            exeDir.resize(pos);

        candidates.push_back(exeDir + "\\rsldbgclnt.exe");
        candidates.push_back(exeDir + "\\..\\bin\\rsldbgclnt.exe");
        candidates.push_back(exeDir + "\\..\\..\\bin\\rsldbgclnt.exe");
    }

    for (const std::string &candidate : candidates)
    {
        const std::wstring w = dbgstr::utf8_to_wide(candidate);
        if (GetFileAttributesW(w.c_str()) != INVALID_FILE_ATTRIBUTES)
            return candidate;
    }

    return std::string("rsldbgclnt.exe");
}

DbgServer::DbgServer()
    : DbgServerBase()
{
    prevStack = 0;
    newDbg = true;
    m_curdbg = nullptr;
    m_curModuleInView = nullptr;
    m_ProcHandle = nullptr;
    m_SkipCmd = 0;
    m_SkipLeft = 0;
    m_SkipMod = nullptr;
    m_SkipLine = -1;
    m_SkipSwallowBp = false;
}

DbgServer::~DbgServer()
{
    DBG_LOGI("~DbgServer()");
}

void DbgServer::setIsNewDebug(const bool &v)
{
    newDbg = v;
}

std::string DbgServer::ReadTextFileContent(const std::string &filename, const std::string &encode)
{
    std::string content;

    FILE *f = _wfopen(dbgstr::utf8_to_wide(filename).c_str(), L"rb");
    if (f)
    {
        char buf[8192];
        size_t n;
        std::string raw;
        while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
            raw.append(buf, n);
        fclose(f);

        // Текст модуля в IBM 866; пустой encode - файл уже UTF-8.
        if (encode.empty())
            content = raw;
        else
            content = dbgstr::oem_to_utf8(raw);
    }
    return content;
}

bool DbgServer::startApp()
{
    // В режимах agent/gui_remote клиент подключается сам, GUI не запускаем
    // (проверка режима также есть в DbgServerBase::run(), дублируем для ясности).
    if (dbgSettings().clientMode != DbgClientGui)
        return true;

    const std::string program = toolFindDebugClient();
    DBG_LOGI("Starting debug client: %s", program.c_str());

    if (port() <= 0)
    {
        DBG_LOGE("Invalid server port (0), refusing to start client");
        return false;
    }

    // CreateProcessW требует изменяемую командную строку.
    std::wstring cmd = L"\"" + dbgstr::utf8_to_wide(program) + L"\" --host 127.0.0.1 --port "
                       + std::to_wstring(port());

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};

    if (!CreateProcessW(NULL, &cmd[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        DBG_LOGE("CreateProcessW failed, error %lu", GetLastError());
        return false;
    }

    CloseHandle(pi.hThread);
    m_ProcHandle = pi.hProcess;

    // Процесс может упасть сразу после старта: ждём чуть-чуть и проверяем.
    if (WaitForSingleObject(pi.hProcess, 2000) == WAIT_OBJECT_0)
    {
        DWORD code = 0;
        GetExitCodeProcess(pi.hProcess, &code);
        DBG_LOGE("Debug client exited immediately, code %lu", code);
        CloseHandle(pi.hProcess);
        m_ProcHandle = nullptr;
        return false;
    }

    return true;
}

void DbgServer::sendEventBreakPoint(Qt::HANDLE BpData)
{
    TBpData *data = (TBpData*)BpData;

    json root;
    root["event"] = "breakpoint";
    root["set"] = (data != nullptr);

    if (data)
    {
        json bp;
        bp["type"] = (int)data->bp_type;
        bp["offs"] = data->offs;
        bp["len"] = data->len;
        bp["line"] = data->line;
        bp["key"] = std::to_string((long long)data->key);
        // modname - сырые байты рантайма (ANSI), отдаём в UTF-8.
        bp["modname"] = dbgstr::ansi_to_utf8(data->modname);
        bp["mod"] = std::to_string(reinterpret_cast<uintptr_t>(data->mod));
        bp["stmt"] = std::to_string(reinterpret_cast<uintptr_t>(data->stmt));
        root["bp"] = bp;
    }

    write(DBG_REQUEST_EVENT_JSON, root.dump());
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

    // Пакетный шаг (skip_same_line): если позиция не сменилась, клиенту
    // ничего не шлём - сразу продолжаем выполнение той же командой.
    // Но срабатывание настоящего брейкпоинта (data != nullptr) режим
    // прерывает - его нельзя проглатывать.
    if (m_SkipCmd && data)
        m_SkipCmd = 0; // останов по брейкпоинту - режим прерываем

    if (m_SkipCmd)
    {
        RSLMODULE mod = m_curdbg->GetCurModule(0);
        int offs2 = 0, len2 = 0;
        m_curdbg->GetCurStatement(&offs2, &len2, 0);
        const int line = m_curdbg->GetStatementLine(offs2, len2, mod);

        if (m_SkipLeft > 0 && mod == (RSLMODULE)m_SkipMod && line == m_SkipLine)
        {
            m_SkipLeft--;
            m_SkipSwallowBp = true; // проглотить идущий следом BreakPointEvent
            m_curdbg->SetDebugState(false);
            m_curdbg->do_ExecContinue(m_SkipCmd);
            m_continueFlag.store(1);
            return;
        }

        // Позиция сменилась или лимит исчерпан - останавливаемся и
        // отдаём состояние клиенту обычным образом.
        DBG_LOGI("skip_same_line: stop (line %d, auto steps left %d)", line, m_SkipLeft);
        m_SkipCmd = 0;
    }

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
        const std::string &name = (*i)->str_name;
        const std::string &value = (*i)->str_value;
        const std::string &type = (*i)->str_type;

        if (name == "THIS" && value.find("Object") != std::string::npos &&
            m_ProcNamespace.find(st) == m_ProcNamespace.end())
            m_ProcNamespace[st] = type;
    }

    ShowVariables(index);
}

void DbgServer::ShowVariables(const int &index)
{
    // Имена/значения приходят из рантайма в ANSI; исторически клиент
    // декодировал их как Windows-1251 - сохраняем то же отображение.
    RSLSTACK st = m_curdbg->GetStackAt(index);
    CLocals* pLocals = m_curdbg->GetLocals();

    json vars = json::array();

    CLocals::iterator i;
    int idx = 0;

    for (i = pLocals->begin (); i != pLocals->end (); ++i, ++idx)
    {
        json obj;
        obj["index"] = idx;
        obj["name"] = dbgstr::ansi_to_utf8((*i)->str_name);
        obj["type"] = dbgstr::ansi_to_utf8((*i)->str_type);
        obj["proc"] = dbgstr::ansi_to_utf8((*i)->str_proc);
        obj["depth"] = (int)(*i)->depth;
        obj["object"] = (bool)(*i)->is_object;
        obj["expanded"] = (bool)(*i)->is_expanded;
        obj["lvalue"] = (bool)(*i)->is_lvalue;
        obj["fakeChildren"] = (bool)(*i)->isFakeChildrensItem;

        // 64-битные хендлы строками: JSON-числа в клиентах хранятся как
        // double, а double точно представляет целые только до 2^53.
        obj["val"] = std::to_string(reinterpret_cast<uintptr_t>((*i)->val));
        obj["exp"] = std::to_string(reinterpret_cast<uintptr_t>((*i)->exp));
        obj["st"] = std::to_string(reinterpret_cast<uintptr_t>((*i)->st));
        obj["procHandle"] = std::to_string(reinterpret_cast<uintptr_t>((*i)->proc));
        obj["info"] = std::to_string(reinterpret_cast<uintptr_t>((*i)->info));
        obj["mod"] = std::to_string(reinterpret_cast<uintptr_t>((*i)->mod));

        if (!(*i)->str_value.empty())
            obj["value"] = dbgstr::ansi_to_utf8((*i)->str_value);

        vars.push_back(obj);
    }

    json root;
    root["vars"] = vars;

    write(DBG_REQUEST_UPDATELOCALS_JSON, root.dump());
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
    json frames = json::array();
    int rv = m_curdbg->GetStackCount();
    for (int i = 0; i < rv; i++)
    {
        Qt::HANDLE stack = m_curdbg->GetStackAt(i);
        Qt::HANDLE module = RslGetModuleFromStack(stack);
        Qt::HANDLE hst = RslGetStatementFromStack(stack);

        int offs = 0, len = 0;
        int isBtrStream = 0;
        RslGetStatementPos(hst, &offs, &len);

        json frame;
        frame["offs"] = offs;
        frame["len"] = len;
        frame["line"] = RslGetModuleLine(module, offs, len);
        frame["module"] = std::to_string(reinterpret_cast<uintptr_t>(module));
        frame["file"] = dbgstr::oem_to_utf8(RslGetModuleFile(module, &isBtrStream));
        frame["func"] = dbgstr::oem_to_utf8(RslGetProcNameFromStack(stack));

        std::map<void*, std::string>::const_iterator ns = m_ProcNamespace.find(stack);
        if (ns != m_ProcNamespace.end())
            frame["namespace"] = dbgstr::ansi_to_utf8(ns->second);

        frames.push_back(frame);
    }

    json root;
    root["frames"] = frames;

    write(DBG_REQUEST_UPDATESTACK_JSON, root.dump());
}

void DbgServer::UpdateText(const int &index)
{
    // Текст модуля читается из файла в IBM 866, имя файла - тоже OEM,
    // поэтому оба декодируем этой кодировкой (клиент кодировки не трогает).
    RSLMODULE mod = m_curdbg->GetCurModule(index);

    int offs = 0;
    int len = 0;

    RSLSTMT statement = m_curdbg->GetCurStatement(&offs, &len, index);
    (void)statement;
    int line = m_curdbg->GetStatementLine(offs, len, mod);

    // NB: UPDATETEXT отправляется на КАЖДОМ шаге - клиент по нему обновляет
    // позицию курсора. Но поле "text" (полный исходник) шлём только при смене
    // модуля: файл .mac во время отладки не меняется, клиент держит свой кэш.
    json root;
    root["offs"] = offs;
    root["len"] = len;
    root["line"] = line;
    root["curModuleInView"] = std::to_string(reinterpret_cast<uintptr_t>(mod));

    int isBtrStream = 0;
    char *filename = RslGetModuleFile((Qt::HANDLE)mod, &isBtrStream);

    const std::string fileNameStr = dbgstr::oem_to_utf8(filename ? filename : "");
    const bool textChanged = (m_TextCacheFile != fileNameStr);
    if (textChanged)
    {
        m_TextCacheFile = fileNameStr;
        m_TextCacheContent = ReadTextFileContent(fileNameStr, "IBM 866");
    }

    root["filename"] = fileNameStr;
    root["textChanged"] = textChanged;
    if (textChanged && !m_TextCacheContent.empty())
        root["text"] = m_TextCacheContent;

    write(DBG_REQUEST_UPDATETEXT_JSON, root.dump());

    m_curModuleInView = mod;

    // Брейкпоинты текущего модуля
    json bps = json::array();

    int nBP = m_curdbg->GetBPCount();
    for (int i = 0; i < nBP; ++i)
    {
        TBpData *bp = m_curdbg->GetBP(i);
        if (mod == bp->mod)
        {
            json item;
            item["type"] = (int)bp->bp_type;
            item["offs"] = bp->offs;
            item["len"] = bp->len;
            item["line"] = bp->line;
            item["key"] = std::to_string((long long)bp->key);
            item["modname"] = dbgstr::ansi_to_utf8(bp->modname);
            item["mod"] = std::to_string(reinterpret_cast<uintptr_t>(bp->mod));
            item["stmt"] = std::to_string(reinterpret_cast<uintptr_t>(bp->stmt));
            bps.push_back(item);
        }
    }

    if (!bps.empty())
    {
        json bproot;
        bproot["bps"] = bps;
        write(DBG_REQUEST_UPDATEBP_JSON, bproot.dump());
    }
}

void DbgServer::ShowVariableValue(const long long &val, const long long &info)
{
    (void)val;
    // Имя/тип/значение исторически декодировались клиентом как Windows-1251.
    int size = 0;
    char str_name[MAX_NAME] = {0};
    char str_type[MAX_TYPENAME] = {0};
    char str_value[MAX_VALUE] = {0};
    RSLVALUE v  = NULL;
    m_curdbg->do_GetValueSize(reinterpret_cast<RSLVINFO>(info), &size);

    std::string value;
    if (size)
    {
        char *buf = new char[size + 1];
        memset(buf, 0, size + 1);
        m_curdbg->do_GetValueData(reinterpret_cast<RSLVINFO>(info), buf, size + 1, 0, &size);
        value.assign(buf, size);
        delete[] buf;
    }

    m_curdbg->do_RefreshInfo(reinterpret_cast<RSLVINFO>(info), &v, NULL,
                             str_name, MAX_NAME, str_type, MAX_TYPENAME,
                             str_value, MAX_VALUE);

    // Отвечаем всегда, иначе клиент ждёт кадр до таймаута: для объектов
    // do_GetValueSize возвращает 0 и сырых данных нет, но строковое
    // представление (str_value) RefreshInfo всё равно даёт.
    json root;
    root["name"] = dbgstr::ansi_to_utf8(str_name);
    root["type"] = dbgstr::ansi_to_utf8(str_type);
    root["value"] = size ? dbgstr::ansi_to_utf8(value)
                         : dbgstr::ansi_to_utf8(str_value);

    write(DBG_REQUEST_GETVALUENFO_JSON, root.dump());
}

int DbgServer::RslGetModuleLine(Qt::HANDLE module, int offs, int len)
{
    int      line = -1;
    int      realoffs, reallen;
    RSLSTMT  stmt;
    m_curdbg->do_GetStatementOfPos((RSLMODULE)module, offs, len, &realoffs, &reallen, &stmt, &line);

    return line;
}

bool DbgServer::serverActionEvent(ServerActionEvent *e)
{
    // Клиент мог прислать команду после завершения отладки (done()):
    // CDebug уничтожен, все действия ниже требуют живого m_curdbg.
    if (!m_curdbg)
        return false;

    const std::string &data = e->buffer();
    if (e->action() == DBG_REQUEST_EXECCONTNUE_JSON)
    {
        const json obj = json::parse(data, nullptr, false);
        const int cmd = obj.is_object() ? obj.value("traceLog", 0) : 0;

        // Пакетный шаг: "skip_same_line" (только step_over/step_into) -
        // сервер сам повторяет команду, пока не сменится (модуль, строка)
        // текущей точки, не более "max_steps" раз (защита от зацикливания).
        m_SkipCmd = 0;
        if ((cmd == EXECCONTNUE_STEP_OVER || cmd == EXECCONTNUE_STEP_INTO) &&
            obj.is_object() && obj.value("skip_same_line", false))
        {
            int maxSteps = obj.value("max_steps", 500);
            if (maxSteps < 1)
                maxSteps = 1;
            if (maxSteps > 100000)
                maxSteps = 100000;

            RSLMODULE mod = m_curdbg->GetCurModule(0);
            int offs = 0, len = 0;
            m_curdbg->GetCurStatement(&offs, &len, 0);

            m_SkipCmd = cmd;
            m_SkipLeft = maxSteps;
            m_SkipMod = (void*)mod;
            m_SkipLine = m_curdbg->GetStatementLine(offs, len, mod);
            DBG_LOGI("skip_same_line: start (cmd %d, line %d, max %d)", cmd, m_SkipLine, maxSteps);
        }

        m_curdbg->SetDebugState(false);
        m_curdbg->do_ExecContinue(cmd);
        m_continueFlag.store(1);
    }
    else if (e->action() == DBG_REQUEST_EXPANDVARIABLE_JSON)
    {
        const json obj = json::parse(data, nullptr, false);

        CLocals* pLocals = m_curdbg->GetLocals();
        pLocals->ExpandV(obj.is_object() ? obj.value("index", 0) : 0);
        ShowVariables(0);
    }
    else if (e->action() == DBG_REQUEST_GETVALUENFO_JSON)
    {
        const json obj = json::parse(data, nullptr, false);
        const long long v = obj.is_object() ? atoll(obj.value("val", "0").c_str()) : 0;
        const long long inf = obj.is_object() ? atoll(obj.value("info", "0").c_str()) : 0;

        ShowVariableValue(v, inf);
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
        DBG_LOGW("Client disconnected while paused, aborting execution");
        m_curdbg->SetDebugState(false);
        m_curdbg->do_ExecContinue(EXECCONTNUE_DBG_ABORT);
    }
}

bool DbgServer::serverEvent(DbgServerEvent *event)
{
    if (event->type() == MSG_UPDATEDBGINFO_EVENT)
    {
        UpdateDbgInfoEvent *e = static_cast<UpdateDbgInfoEvent*>(event);
        UpdateDbgInfo((TBpData*)e->data());
    }
    else if (event->type() == MSG_BREAKPOINT)
    {
        // Событие от авто-шага skip_same_line - клиенту не отправляем.
        if (m_SkipSwallowBp)
            m_SkipSwallowBp = false;
        else
        {
            BreakPointEvent *e = static_cast<BreakPointEvent*>(event);
            sendEventBreakPoint(e->data());
        }
    }

    return DbgServerBase::serverEvent(event);
}
