#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <QJsonArray>

// Ищет живые отладочные сессии rsldbg: слушает UDP-порт анонсов
// (RSLDBG_ANNOUNCE_PORT) и шлёт discover-пробы (localhost + limited
// broadcast). Возвращает уникальные JSON-анонсы сессий
// (pid, port, exe, title, user, host, dbgver).
QJsonArray discoverDebugSessions(int timeoutMs);

#endif // DISCOVERY_H
