#ifndef BP_DATA_H
#define BP_DATA_H

#include <windows.h>
#include <string>
#include "rsl/dbgintf.h"

enum BP_TYPE
{
    BP_ENABLED,
    BP_DISABLED,
    BP_INVISIBLE,
    BP_DELETED
};

// modname - имя модуля в кодировке рантайма (ANSI/OEM байты, не UTF-8).
typedef struct tagTBpData
{
    BP_TYPE	      bp_type;
    int            offs;
    int            len;
    RSLMODULE      mod;
    std::string    modname;
    int            line;
    RSLSTMT        stmt;
    unsigned long  key;
} TBpData;

#endif // BP_DATA_H
