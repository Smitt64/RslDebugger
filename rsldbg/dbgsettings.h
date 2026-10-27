#ifndef DBGSETTINGS_H
#define DBGSETTINGS_H

#include <QString>
#include <QStringList>

// Режимы запуска/подключения клиента отладки (rsldbg.ini, секция [debug],
// ключ client_mode = gui | agent | gui_remote).
enum DbgClientMode
{
    DbgClientGui = 0,   // сервер сам запускает локальный GUI (rsldbgclnt.exe)
    DbgClientAgent,     // клиент не запускается, только UDP-анонсы; bind 127.0.0.1
    DbgClientGuiRemote  // клиент не запускается, UDP-анонсы; bind 0.0.0.0 (удалённый GUI)
};

struct DbgSettings
{
    DbgClientMode clientMode = DbgClientGui;
    bool announceBroadcast = true; // announce = broadcast | off
    bool announceTitle = true;     // включать ли заголовок окна хоста в анонс
    QStringList announceTo;        // announce_to = ip1; ip2 - юникаст-получатели анонсов
    QString authToken;             // auth_token: фиксированный токен (для gui_remote),
                                   // пусто - случайный GUID на каждую сессию
};

// Настройки читаются из rsldbg.ini рядом с rsldbg.dll при первом обращении.
// Возвращается копия - безопасно для вызовов из рабочих потоков сервера.
DbgSettings dbgSettings();

// Перечитать ini (например, после правки файла вживую).
void dbgReloadSettings();

// Программное перекрытие режима (RslDbgSetClientMode из экспорта dll);
// переживает dbgReloadSettings().
void dbgSetClientMode(DbgClientMode mode);

#endif // DBGSETTINGS_H
