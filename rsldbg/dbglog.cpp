#include "dbglog.h"

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <mutex>

static std::mutex g_LogMutex;
static FILE *g_LogFile = nullptr;
static bool g_LogFileTried = false;

// ------------------------------------------------------------------------------
static FILE *dbgLogFile()
{
    if (!g_LogFileTried)
    {
        g_LogFileTried = true;

        char tmp[MAX_PATH];
        DWORD n = GetTempPathA(MAX_PATH, tmp);
        if (n > 0 && n < MAX_PATH)
        {
            char path[MAX_PATH * 2];
            _snprintf(path, sizeof(path) - 1, "%srsldbg-%u.log", tmp, (unsigned)GetCurrentProcessId());
            path[sizeof(path) - 1] = '\0';
            g_LogFile = fopen(path, "ab");
        }
    }

    return g_LogFile;
}

// ------------------------------------------------------------------------------
void dbgLogImpl(char level, const char *fmt, ...)
{
    char msg[2048];

    va_list ap;
    va_start(ap, fmt);
    _vsnprintf(msg, sizeof(msg) - 1, fmt, ap);
    va_end(ap);
    msg[sizeof(msg) - 1] = '\0';

    SYSTEMTIME st;
    GetLocalTime(&st);

    char line[2304];
    _snprintf(line, sizeof(line) - 1, "%02d:%02d:%02d.%03d [%c] %s\n",
              st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, level, msg);
    line[sizeof(line) - 1] = '\0';

    std::lock_guard<std::mutex> lock(g_LogMutex);

    FILE *f = dbgLogFile();
    if (f)
    {
        fwrite(line, 1, strlen(line), f);
        fflush(f);
    }

    OutputDebugStringA(line);
}
