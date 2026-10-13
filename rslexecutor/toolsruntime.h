// Минимальный аналог RsWorkTools/ToolsRuntime/toolsruntime.h.
// Содержит только то, что реально используют rslexecutor.cpp и rsscript/*:
// категорию логирования logRsl и поиск файла рядом с exe.
#ifndef TOOLSRUNTIME_H
#define TOOLSRUNTIME_H

#include <QLoggingCategory>
#include "ToolsRuntime_global.h"

TOOLSRUNTIME_EXPORT Q_DECLARE_LOGGING_CATEGORY(logRsl)

TOOLSRUNTIME_EXPORT QString toolFullFileNameFromDir(const QString &file);

#endif // TOOLSRUNTIME_H
