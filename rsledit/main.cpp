#include <windows.h>

#include <cstdio>

#include <QApplication>
#include <QDir>
#include <QFileInfo>

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

    // Файл для автооткрытия: позиционный аргумент либо hello.mac из репозитория.
    // exe лежит в rsledit/debug (сборка) или в bin (установка), поэтому
    // проверяем несколько относительных путей.
    QString startFile;
    for (int i = 1; i < args.count(); ++i)
    {
        if (!args.at(i).startsWith(QLatin1Char('-')))
        {
            startFile = args.at(i);
            break;
        }
    }

    if (startFile.isEmpty())
    {
        const QString appDir = app.applicationDirPath();
        const QStringList candidates = {
            appDir + QStringLiteral("/test/hello.mac"),          // bin/test/hello.mac
            appDir + QStringLiteral("/../bin/test/hello.mac"),
            appDir + QStringLiteral("/../../bin/test/hello.mac") // rsledit/debug -> repo/bin
        };

        for (const QString &path : candidates)
        {
            if (QFileInfo::exists(path))
            {
                startFile = QDir(path).canonicalPath();
                break;
            }
        }
    }

    MainWindow window(startFile);
    window.show();

    return app.exec();
}
