// rslrun - minimal console host for executing RSL macro files (.mac).
//
// Usage:
//   rslrun <file.mac> [--debug]
//
// With --debug the runtime debug mode is enabled (SetDebugMacroFlag) and the
// RSL runtime loads rsldbg.dll itself (searched in the executable directory),
// which starts the debug server and launches rsldbgclnt.exe automatically
// (same mechanism as in RslTest). This requires the newer RSL runtime
// (rsrtlwm.dll/RSScript.dll from the 19.14 toolset, deployed to bin/); the
// old rstools runtime defaulted to remote (FS-based) debugging and never
// fired DbgBreak in a standalone host.
//
// The host goes through RslStartApp() so the RTL performs its full
// framework initialization, then runs the macro from the RsMain callback.

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <QApplication>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTextCodec>
#include <QUuid>

#include "playrep.h"
#include "rsl/msg.h"
#include "rsl/ierror.h"
#include "rsldirs.h"
#include "panel/rsmain.h"
#include "toolver.h"

static void logf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    fflush(stderr);
}

// Convert a CP866 (DOS) string coming from the RSL runtime to UTF-8.
static QByteArray from866(const char *str)
{
    if (!str)
        return QByteArray();

    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    return codec->toUnicode(str).toUtf8();
}

// Action proc: the runtime calls this periodically while the macro runs
// (same role as RslPlayRepActionProc in RslExecutor).
static bool actionProc(void * /*userData*/)
{
    QCoreApplication::processEvents();
    return true;
}

static int msgProc(IMESSAGE mes, void *ptr, void *userData)
{
    switch (mes)
    {
    case IM_GETHOSTINFO:
        // Host handshake - the runtime asks about the host application.
        // Same values as RslExecutor (Std_MsgProc is a no-op stub in the
        // 19.14 runtime, so this must be answered explicitly).
        if (ptr)
        {
            THostInfoData *data = (THostInfoData *)ptr;
            data->useFdecimal  = true;
            data->isGUI        = false;
            data->toolsVersion = RSL_TOOLS_VERSION;
        }
        break;

    case IM_OUTPUT:
        if (ptr)
            logf("%s", from866((const char *)ptr).data());
        break;

    case IM_ERROR:
    case IM_RUN_ERROR:
    case IM_RUN_ERROR_EX:
        if (ptr)
        {
            ERRINFO *err = (ERRINFO *)ptr;
            logf("[error] file=%s line=%d pos=%d code=%d mes=%s text=%s",
                 from866(err->file ? err->file : "").data(),
                 err->line, err->pos, (int)err->code,
                 from866(err->mes ? err->mes : "").data(),
                 from866(GetErrMesEx(err->code, 0)).data());
        }
        break;

    default:
        break;
    }

    // Default runtime handling for everything else (host info, executor
    // init, etc.) - same as RslExecutor does.
    return Std_MsgProc(mes, ptr, userData);
}

static int g_argc = 0;
static char **g_argv = nullptr;

static int runMacro(const char *macFile, bool debug)
{
    QApplication app(g_argc, g_argv);
    app.setApplicationName("rslrun");
    app.setOrganizationName("RslDebugger");

    SetDefInstName("rslrun");

    // Let the runtime find the macro: search the macro's own directory.
    QFileInfo fi(QString::fromLocal8Bit(macFile));
    QString macDir = fi.absolutePath();
    ToolsSetIncDir(macDir.toLocal8Bit().data());

    // Output is always a valid file name (RunRSL expects a real buffer).
    QTemporaryFile outputFile;
    outputFile.open();
    QByteArray output = outputFile.fileName().toLocal8Bit();

    // Execution context passed to the runtime (must not be null!).
    STD_USERDATA userData = {};
    QByteArray nameSpace = QStringLiteral("rslrun-%1")
                               .arg(QUuid::createUuid().toString(QUuid::WithoutBraces).left(8))
                               .toLocal8Bit();
    qstrncpy(userData.nameSpace, nameSpace.data(), sizeof(userData.nameSpace) - 1);

    if (debug)
        SetDebugMacroFlag(1);

    logf("[rslrun] executing: %s (debug=%d)", macFile, (int)debug);

    int rc = RunRSL(macFile,
                    output.data(),
                    msgProc,
                    actionProc,     // action proc (event pump)
                    &userData,
                    false);         // addExec

    logf("[rslrun] RunRSL returned %d, stat=%d", rc, (int)userData.stat);

    if (debug)
        SetDebugMacroFlag(0);

    // RunRSL returns nonzero on success (the macro ran), userData.stat is set.
    return (rc && userData.stat) ? 0 : 1;
}

extern "C" int RsMain(int argc, char **argv)
{
    const char *macFile = nullptr;
    bool debug = false;

    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--debug") == 0)
            debug = true;
        else if (argv[i][0] != '-' && !macFile)
            macFile = argv[i];
        else
        {
            logf("Usage: %s <file.mac> [--debug]", argv[0]);
            return 2;
        }
    }

    if (!macFile)
    {
        logf("Usage: %s <file.mac> [--debug]", argv[0]);
        return 2;
    }

    g_argc = argc;
    g_argv = argv;

    int rc = runMacro(macFile, debug);

    RslStopApp(1, rc);
    return rc;
}

int main(int argc, char **argv)
{
    return RslStartApp(argc, argv, RsMain, 1);
}
