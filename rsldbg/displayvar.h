#ifndef DISPLAYVAR_H
#define DISPLAYVAR_H

#include "cdebugroot.h"
#include <string>

#define MAX_TYPENAME 100
#define MAX_VALUE    100
#define MAX_NAME     100

// str_* - сырые строки рантайма (ANSI/OEM байты, не UTF-8).
class DisplayVar
{
public:
    DisplayVar(CDebugRoot* pDebug);
    virtual ~DisplayVar();

    std::string str_name;
    std::string str_type;
    std::string str_value;
    std::string str_proc;
    unsigned char  depth;
    bool           is_object;
    bool           is_expanded;
    bool           is_lvalue;
    RSLVALUE       val;
    RSLEXPCTX      exp;
    RSLSTACK       st;
    RSLPROC        proc;
    RSLVINFO       info;
    CDebugRoot*    debug;
    RSLMODULE      mod;

    bool isFakeChildrensItem;

    void Free(void);
    bool IsError () const;
    bool IsStringLikeEditable () const;

private:
    std::string str_error;
};

#endif // DISPLAYVAR_H
