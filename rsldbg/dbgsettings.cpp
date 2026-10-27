#include "dbgsettings.h"
#include <windows.h>
#include <QSettings>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>

static DbgSettings g_Settings;
static bool g_Loaded = false;
static bool g_ModeOverridden = false;
static DbgClientMode g_OverriddenMode = DbgClientGui;
static QMutex g_Mutex;

static QString dbgIniPath()
{
    HMODULE hd = GetModuleHandleW(L"rsldbg.dll");
    if (hd)
    {
        wchar_t path[MAX_PATH] = {};
        if (GetModuleFileNameW(hd, path, MAX_PATH))
            return QFileInfo(QString::fromWCharArray(path)).absolutePath() + "/rsldbg.ini";
    }

    return QStringLiteral("rsldbg.ini");
}

static DbgClientMode dbgParseMode(const QString &s)
{
    const QString v = s.trimmed().toLower();

    if (v == QLatin1String("agent"))
        return DbgClientAgent;
    if (v == QLatin1String("gui_remote") || v == QLatin1String("remote"))
        return DbgClientGuiRemote;

    return DbgClientGui;
}

static void dbgLoadLocked()
{
    QSettings ini(dbgIniPath(), QSettings::IniFormat);
    ini.beginGroup(QStringLiteral("debug"));

    DbgSettings s;
    s.clientMode = dbgParseMode(ini.value(QStringLiteral("client_mode"),
                                          QStringLiteral("gui")).toString());
    s.announceBroadcast = ini.value(QStringLiteral("announce"),
                                    QStringLiteral("broadcast")).toString().trimmed().toLower()
                          != QLatin1String("off");
    s.announceTitle = ini.value(QStringLiteral("announce_title"), true).toBool();
    s.announceTo = ini.value(QStringLiteral("announce_to")).toString()
                       .split(QLatin1Char(';'), Qt::SkipEmptyParts);
    for (QString &addr : s.announceTo)
        addr = addr.trimmed();
    s.announceTo.removeAll(QString());
    s.authToken = ini.value(QStringLiteral("auth_token")).toString().trimmed();

    ini.endGroup();

    if (g_ModeOverridden)
        s.clientMode = g_OverriddenMode;

    g_Settings = s;
    g_Loaded = true;
}

DbgSettings dbgSettings()
{
    QMutexLocker locker(&g_Mutex);

    if (!g_Loaded)
        dbgLoadLocked();

    return g_Settings;
}

void dbgReloadSettings()
{
    QMutexLocker locker(&g_Mutex);
    dbgLoadLocked();
}

void dbgSetClientMode(DbgClientMode mode)
{
    QMutexLocker locker(&g_Mutex);
    g_ModeOverridden = true;
    g_OverriddenMode = mode;

    if (g_Loaded)
        g_Settings.clientMode = mode;
}
