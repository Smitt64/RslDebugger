#include <QCoreApplication>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <QDir>
#include <cstdio>
#include "mcpserver.h"

// MCP-клиент (Claude Desktop) stderr перехватывает или теряет, а при
// нескольких экземплярах exe логи путаются - поэтому Qt-лог дублируем в файл
// %TEMP%\rsldbgmcp-<pid>.log (append, одна строка на сообщение) и в stderr.
static void mcpMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    static QFile logFile;
    static QMutex logMutex;

    const QString line = QStringLiteral("%1 [%2] %3: %4")
        .arg(QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz")))
        .arg(type == QtDebugMsg ? "D" : type == QtInfoMsg ? "I" :
             type == QtWarningMsg ? "W" : type == QtCriticalMsg ? "C" : "F")
        .arg(QLatin1String(ctx.category))
        .arg(msg);

    QMutexLocker lock(&logMutex);
    if (!logFile.isOpen())
    {
        logFile.setFileName(QDir::temp().filePath(
            QStringLiteral("rsldbgmcp-%1.log").arg(QCoreApplication::applicationPid())));
        logFile.open(QIODevice::Append | QIODevice::Text);
    }
    if (logFile.isOpen())
    {
        logFile.write(line.toUtf8());
        logFile.write("\n");
        logFile.flush();
    }

    fprintf(stderr, "%s\n", qPrintable(line));
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("rsldbgmcp"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1.0"));

    qInstallMessageHandler(mcpMessageHandler);

    // ВНИМАНИЕ: в stdout пишется ТОЛЬКО JSON-RPC (McpServer). Весь лог - stderr.
    McpServer server;
    return app.exec();
}
