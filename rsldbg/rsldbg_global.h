#ifndef RSLDBG_GLOBAL_H
#define RSLDBG_GLOBAL_H

#if defined(RSLDBG_LIBRARY)
#define RSLDBG_EXPORT __declspec(dllexport)
#else
#define RSLDBG_EXPORT __declspec(dllimport)
#endif

#endif // RSLDBG_GLOBAL_H
