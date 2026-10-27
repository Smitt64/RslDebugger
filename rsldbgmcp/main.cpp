#include <QCoreApplication>
#include "mcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("rsldbgmcp"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1.0"));

    // ВНИМАНИЕ: в stdout пишется ТОЛЬКО JSON-RPC (McpServer). Весь лог - stderr.
    McpServer server;
    return app.exec();
}
