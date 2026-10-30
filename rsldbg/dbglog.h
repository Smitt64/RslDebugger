#ifndef DBGLOG_H
#define DBGLOG_H

// Логирование отладочного сервера (замена qInfo/qWarning без Qt).
// Пишет построчно в %TEMP%\rsldbg-<pid>.log и зеркалит в OutputDebugString.
// Потокобезопасно (внутренний мьютекс), сброс на диск после каждой строки.

void dbgLogImpl(char level, const char *fmt, ...);

#define DBG_LOGI(...) dbgLogImpl('I', __VA_ARGS__)
#define DBG_LOGW(...) dbgLogImpl('W', __VA_ARGS__)
#define DBG_LOGE(...) dbgLogImpl('C', __VA_ARGS__)

#endif // DBGLOG_H
