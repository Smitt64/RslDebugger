#ifndef BP_DATA_H
#define BP_DATA_H

#include <QObject>
#include <qt_windows.h>
#include "rsl/dbgintf.h"

enum BP_TYPE
{
    BP_ENABLED,
    BP_DISABLED,
    BP_INVISIBLE,
    BP_DELETED
};

typedef struct tagTBpData
{
    BP_TYPE	      bp_type;
    int            offs;
    int            len;
    RSLMODULE      mod;
    QString        modname;
    int            line;
    RSLSTMT        stmt;
    unsigned long  key;
} TBpData;

#endif // BP_DATA_H
