// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef TYPEINFO_P_H
#define TYPEINFO_P_H

#include "statvars.h"
#include "rsl/isymbol.h"
#include "playrep.h"
#include "statvars.h"
#include "registerinfobase.h"

typedef struct tagQObjectRsl
{
    tagQObjectRsl()
    {
        instance = nullptr;
        init();
    }

    void init()
    {
        object = nullptr;
        refCount = 0;
        className[0] = 0;
        generation = 0;
        marknumber = 0;
        owner = RegisterInfoBase::RslOwner;
        instance = RslGetCurrentInst();

        memset(&derived, 0, sizeof (TGenObject));
    }

    TGenObject derived;
    quint8 refCount;
    HRSLINST instance;

    char className[256];

#ifndef NO_GC
    unsigned short generation;  // Used to collect cyclic objects
    unsigned short marknumber;  // Used to report references
#endif

    QObject *object;
    RegisterInfoBase::QObjectRslOwner owner;
}QObjectRsl;

#endif // TYPEINFO_P_H
