#ifndef DBGSETTINGS_H
#define DBGSETTINGS_H

#include <string>
#include <vector>

// Режим клиента, заданный ведущим приложением (rsledit - GUI, debugbreak - agent).
// Имеет приоритет над ini до первой перезагрузки настроек.
enum DbgClientMode
{
    DbgClientGui,       // слушать 127.0.0.1:0, запустить rsldbgclnt.exe
    DbgClientAgent,     // слушать 0.0.0.0:0, ничего не запускать, анонсировать по UDP
    DbgClientGuiRemote  // слушать 0.0.0.0:0, запустить rsldbgclnt.exe (удалённый GUI)
};

struct DbgSettings
{
    DbgClientMode clientMode = DbgClientGui;
    bool announceBroadcast = false;
    bool announceTitle = true;
    std::vector<std::string> announceTo; // IPv4-адреса (UTF-8, фактически ASCII)
    std::string authToken;               // UTF-8
};

// Настройки rsldbg.ini лежат рядом с rsldbg.dll. Значения ini читаются как UTF-8
// (как и раньше при QSettings IniFormat). Результаты кэшируются; dbgReloadSettings
// сбрасывает кэш. dbgSetClientMode переопределяет clientMode до следующего reload.
DbgSettings dbgSettings();
void dbgReloadSettings();
void dbgSetClientMode(DbgClientMode mode);

// Путь к каталогу rsldbg.dll (UTF-8, без завершающего слеша) - нужен и
// для поиска rsldbgclnt.exe, и для расположения rsldbg.ini.
std::string dbgDllDir();

#endif // DBGSETTINGS_H
