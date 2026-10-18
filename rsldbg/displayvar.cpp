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
