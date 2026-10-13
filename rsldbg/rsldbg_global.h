#ifndef RSLDBG_GLOBAL_H
#define RSLDBG_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RSLDBG_LIBRARY)
#define RSLDBG_EXPORT Q_DECL_EXPORT
#else
#define RSLDBG_EXPORT Q_DECL_IMPORT
#endif

#endif // RSLDBG_GLOBAL_H
