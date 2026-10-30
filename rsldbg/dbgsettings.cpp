#include "dbgsettings.h"
#include "strconv.h"

#include <windows.h>
#include <mutex>
#include <algorithm>
#include <cctype>

static DbgSettings g_Settings;
static bool g_Loaded = false;
static bool g_ModeOverridden = false;
static DbgClientMode g_OverriddenMode = DbgClientGui;
static std::mutex g_Mutex;

std::string dbgDllDir()
{
    HMODULE hd = GetModuleHandleW(L"rsldbg.dll");
    if (hd)
    {
        wchar_t path[MAX_PATH] = {};
        if (GetModuleFileNameW(hd, path, MAX_PATH))
        {
            std::string dir = dbgstr::wide_to_utf8(path);
            const std::string::size_type pos = dir.find_last_of("\\/");
            if (pos != std::string::npos)
                dir.resize(pos);

            return dir;
        }
    }

    return std::string(".");
}

static std::string dbgIniPath()
{
    return dbgDllDir() + "\\rsldbg.ini";
}

static std::string trimLower(std::string s)
{
    const char *ws = " \t\r\n";
    const std::string::size_type b = s.find_first_not_of(ws);
    const std::string::size_type e = s.find_last_not_of(ws);

    if (b == std::string::npos)
        return std::string();

    s = s.substr(b, e - b + 1);
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });

    return s;
}

static std::string trim(const std::string &s)
{
    const char *ws = " \t\r\n";
    const std::string::size_type b = s.find_first_not_of(ws);
    const std::string::size_type e = s.find_last_not_of(ws);

    return b == std::string::npos ? std::string() : s.substr(b, e - b + 1);
}

static DbgClientMode dbgParseMode(const std::string &s)
{
    const std::string v = trimLower(s);

    if (v == "agent")
        return DbgClientAgent;
    if (v == "gui_remote" || v == "remote")
        return DbgClientGuiRemote;

    return DbgClientGui;
}

// GetPrivateProfileStringA возвращает значение как есть, в байтах файла;
// трактуем их как UTF-8 (то же, что делал QSettings IniFormat).
static std::string iniValue(const std::string &path, const char *key, const char *defValue)
{
    char buf[2048];
    const DWORD n = GetPrivateProfileStringA("debug", key, defValue, buf, sizeof(buf), path.c_str());
    return std::string(buf, n);
}

static bool iniBool(const std::string &path, const char *key, bool defValue)
{
    const std::string v = trimLower(iniValue(path, key, defValue ? "true" : "false"));
    return v == "true" || v == "1" || v == "yes" || v == "on";
}

static void dbgLoadLocked()
{
    const std::string path = dbgIniPath();

    DbgSettings s;
    s.clientMode = dbgParseMode(iniValue(path, "client_mode", "gui"));
    s.announceBroadcast = trimLower(iniValue(path, "announce", "broadcast")) != "off";
    s.announceTitle = iniBool(path, "announce_title", true);

    const std::string to = iniValue(path, "announce_to", "");
    std::string::size_type start = 0;
    while (start <= to.size())
    {
        const std::string::size_type semi = to.find(';', start);
        const std::string item = trim(to.substr(start, semi == std::string::npos ? std::string::npos : semi - start));

        if (!item.empty())
            s.announceTo.push_back(item);

        if (semi == std::string::npos)
            break;

        start = semi + 1;
    }

    s.authToken = trim(iniValue(path, "auth_token", ""));

    if (g_ModeOverridden)
        s.clientMode = g_OverriddenMode;

    g_Settings = s;
    g_Loaded = true;
}

DbgSettings dbgSettings()
{
    std::lock_guard<std::mutex> locker(g_Mutex);

    if (!g_Loaded)
        dbgLoadLocked();

    return g_Settings;
}

void dbgReloadSettings()
{
    std::lock_guard<std::mutex> locker(g_Mutex);
    dbgLoadLocked();
}

void dbgSetClientMode(DbgClientMode mode)
{
    std::lock_guard<std::mutex> locker(g_Mutex);
    g_ModeOverridden = true;
    g_OverriddenMode = mode;

    if (g_Loaded)
        g_Settings.clientMode = mode;
}
