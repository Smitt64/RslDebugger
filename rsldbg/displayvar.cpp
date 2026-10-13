#include "displayvar.h"
#include "dbgserverproto.h"

DisplayVar::DisplayVar(CDebugRoot* pDebug)
{
    depth = 1;
    isFakeChildrensItem = false;
    is_object = false;
    is_expanded = false;
    is_lvalue = true;
    val = NULL;
    exp = NULL;
    info = NULL;
    debug = pDebug;

    st = NULL;
    proc = NULL;

    //if (!str_error.LoadString (IDR_STR_ERROR))
    str_error = "error";
}

DisplayVar::~DisplayVar()
{
    Free();
}

void DisplayVar::Free(void)
{
    if(debug && info)
    {
        debug->do_DoneInfo(info);
        info = NULL;
    }

    if(debug && exp)
    {
        debug->  do_RemExp(exp);
        exp = NULL;
    }

    depth = 1;
    is_object = false;
    is_expanded = false;
    is_lvalue = true;
    val = NULL;
    exp = NULL;
    info = NULL;

    st = NULL;
    proc = NULL;
}

bool DisplayVar::IsError () const
{
    return str_type == str_error;
}

bool DisplayVar::IsStringLikeEditable () const
{
    return (str_type == "String") || (str_type == "STRING") || IsError ();
}

void DisplayVar::toDbgVariable(Qt::HANDLE obj)
{
    DBG_VARIABLEDATA *v = (DBG_VARIABLEDATA*)obj;
    qstrcpy(v->str_name, str_name.data());
    qstrcpy(v->str_type, str_type.data());
    qstrcpy(v->str_proc, str_proc.data());

    v->depth = depth;
    v->is_object = is_object;
    v->is_expanded = is_expanded;
    v->is_lvalue = is_lvalue;
    v->isFakeChildrensItem = isFakeChildrensItem;

    v->val = reinterpret_cast<qint64>(val);
    v->exp = reinterpret_cast<qint64>(exp);
    v->st = reinterpret_cast<qint64>(st);
    v->proc = reinterpret_cast<qint64>(proc);
    v->info = reinterpret_cast<qint64>(info);
    v->mod = reinterpret_cast<qint64>(mod);
}
