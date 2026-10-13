// Минимальный аналог RsWorkTools/ToolsRuntime/toolsruntime.cpp.
#include "toolsruntime.h"

#include <QCoreApplication>
#include <QDir>

Q_LOGGING_CATEGORY(logRsl, "rsl.runtime")

QString toolFullFileNameFromDir(const QString &file)
{
    return QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(file);
}
