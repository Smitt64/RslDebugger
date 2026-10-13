#include <windows.h>

#include <cstdio>

#include <QApplication>

#include "mainwindow.h"
#include "macrorunner.h"

// Headless mode: rsledit --run <file.mac> [--debug]
// Runs the macro without showing the editor window (used for testing and
// from scripts). Output goes to the parent console.
static int runHeadless(const QString &fileName, bool debug)
{
    // Attach to the parent console so log output is visible.
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    MacroRunner runner(fileName, debug);

    QObject::connect(&runner, &MacroRunner::logMessage,
                     [](const QString &text) {
                         fprintf(stderr, "%s\n", text.toUtf8().constData());
                         fflush(stderr);
                     });

    return runner.exec();
}

int main(int argc, char *argv[])
{
    // QApplication (QtWidgets) initializes COM/OLE itself via OleInitialize,
    // so no explicit CoInitialize is needed here.
    QApplication app(argc, argv);

    app.setApplicationName("rsledit");
    app.setOrganizationName("RslDebugger");

    // Headless run mode: rsledit --run <file.mac> [--debug]
    QStringList args = app.arguments();
    int runIdx = args.indexOf(QStringLiteral("--run"));
    if (runIdx >= 0)
    {
        bool debug = args.contains(QStringLiteral("--debug"));
        QString fileName;
        for (int i = runIdx + 1; i < args.count(); ++i)
        {
            if (!args.at(i).startsWith(QLatin1Char('-')))
            {
                fileName = args.at(i);
                break;
            }
        }

        if (fileName.isEmpty())
        {
            fprintf(stderr, "Usage: rsledit --run <file.mac> [--debug]\n");
            return 2;
        }

        return runHeadless(fileName, debug);
    }

    MainWindow window;
    window.show();

    return app.exec();
}
