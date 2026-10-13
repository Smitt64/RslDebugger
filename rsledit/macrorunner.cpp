#include "macrorunner.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>

#include "rslexecutor.h"
#include "toolsruntime.h"
#include "playrep.h"
#include "rsldirs.h"

// ---------------------------------------------------------------------------
// Debugger integration (same approach as RslTest).
//
// The macro runs through RslExecutor::playRep (ported from RsWorkTools),
// which internally calls RunRSLEx with a full message proc (IM_GETHOSTINFO,
// IM_OUTPUT, IM_ERROR, dynamic/static module registration, ...). The debug
// switch uses the original mechanism: the "debugbreak" key in rsl.ini
// ([applicationName] group), which playRep reads right before RunRSLEx and
// turns into SetDebugMacroFlag. With the flag set the runtime loads
// rsldbg.dll (searched in the executable directory) when the executor
// instance is created; rsldbg.dll starts the debug server and launches
// rsldbgclnt.exe automatically.
//
// This requires the newer RSL runtime (rsrtlwm.dll/RSScript.dll from the
// 19.14 toolset, deployed to bin/); the old rstools runtime defaulted to
// remote (FS-based) debugging and never fired DbgBreak in a standalone host.
//
// playRep is called on the caller (GUI) thread and blocks, exactly like
// RslTest does; the play-rep proc below keeps the UI alive.
// ---------------------------------------------------------------------------

// The runner instance whose executor signals are active (set for the
// duration of exec()).
static MacroRunner *g_runner = nullptr;

static void emitLog(const QString &text)
{
    if (g_runner)
        emit g_runner->logMessage(text);
}

// ---------------------------------------------------------------------------

MacroRunner::MacroRunner(const QString &fileName, bool debug, QObject *parent)
    : QObject(parent),
      m_fileName(fileName),
      m_debug(debug)
{
}

int MacroRunner::exec()
{
    g_runner = this;

    SetDefInstName("rsledit");

    // Let the runtime find the macro: search the macro's own directory.
    QFileInfo fi(m_fileName);
    ToolsSetIncDir(fi.absolutePath().toLocal8Bit().data());

    // Debug switch - exactly the RslTest mechanism: the "debugbreak" key in
    // rsl.ini ([applicationName] group), read by RslExecutor::playRep.
    {
        /*QSettings settings(toolFullFileNameFromDir(QStringLiteral("rsl.ini")),
                           QSettings::IniFormat);
        settings.beginGroup(QCoreApplication::applicationName());
        settings.setValue(QStringLiteral("debugbreak"), m_debug);
        settings.endGroup();*/
    }

    emitLog(QString::fromUtf8("[rsledit] выполняется: %1 (отладка=%2)")
                .arg(m_fileName)
                .arg((int)m_debug));

    RslExecutor executor;
    connect(&executor, &RslExecutor::WriteOut, this,
            [](const QString &msg) { emitLog(msg.trimmed()); });
    connect(&executor, &RslExecutor::ErrorMessage, this,
            [](int, const QString &msg) { emitLog(msg); });

    // Blocking call on the caller thread, like RslTest does. The play-rep
    // proc pumps Qt events so the editor UI stays responsive while the
    // macro runs (analog of RslPlayRepActionProc's PlayRepProc callback).
    executor.setDebugMacroFlag(true);
    executor.playRep(fi.absoluteFilePath(), QString(),
                     []() { QCoreApplication::processEvents(); });

    const QStringList errs = executor.errors();
    for (const QString &err : errs)
        emitLog(err);

    emitLog(QString::fromUtf8("[rsledit] выполнение завершено, ошибок: %1")
                .arg(errs.size()));

    g_runner = nullptr;
    return errs.isEmpty() ? 0 : 1;
}
