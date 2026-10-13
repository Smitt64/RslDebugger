#ifndef MACRORUNNER_H
#define MACRORUNNER_H

#include <QObject>

// Runs an RSL macro (.mac, CP866) synchronously on the caller thread via
// RslExecutor::playRep (ported from RsWorkTools/ToolsRuntime), which calls
// RunRSLEx internally - the call is intentionally blocking, exactly like
// RslTest does it. A play-rep proc pumps Qt events while the macro runs
// so the editor UI stays responsive. In debug mode the runtime itself
// loads rsldbg.dll and spawns rsldbgclnt; debugbreak / breakpoints pause
// execution and control goes to the debug client.
class MacroRunner : public QObject
{
    Q_OBJECT

public:
    MacroRunner(const QString &fileName, bool debug, QObject *parent = nullptr);

    // Blocking: runs the macro and returns its exit code (0 = success).
    int exec();

signals:
    void logMessage(const QString &text);

private:
    QString m_fileName;
    bool m_debug;
};

#endif // MACRORUNNER_H
