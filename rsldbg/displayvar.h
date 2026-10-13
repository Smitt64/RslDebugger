#ifndef DISPLAYVAR_H
#define DISPLAYVAR_H

#include <QObject>
#include "cdebugroot.h"

#define MAX_TYPENAME 100
#define MAX_VALUE    100
#define MAX_NAME     100

class DisplayVar
{
public:
    DisplayVar(CDebugRoot* pDebug);
    virtual ~DisplayVar();

    QByteArray str_name;
    QByteArray str_type;
    QByteArray str_value;
    QByteArray str_proc;
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

    void toDbgVariable(Qt::HANDLE obj);

private:
    QByteArray str_error;
};

#endif // DISPLAYVAR_H
