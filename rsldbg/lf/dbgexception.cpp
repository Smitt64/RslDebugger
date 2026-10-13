#include "dbgexception.h"
#include <sstream>

dbgexception::~dbgexception ()
{

}

const char* dbgexception::what () const
{
    return "RSL debugger error";
}

// ----------------------------------------------------------

const char* bdgnotsupported::what () const
{
    static const char* pStr = "operation not supported";
    return pStr;
}

// ----------------------------------------------------------

dbgverexception::dbgverexception (int reqMJver_, int reqMIver_) :
    reqMJver (reqMJver_), reqMIver (reqMIver_)
{
    std::ostringstream ostr;
    ostr << reqMJver << "." << reqMIver;

    errStr = "RSL version required ";
    errStr += ostr.str ();
}

const char* dbgverexception::what () const
{
    return errStr.c_str ();
}

int dbgverexception::GetReqMJVer () const
{
    return reqMJver;
}

int dbgverexception::GetReqMIVer () const
{
    return reqMIver;
}

// ----------------------------------------------------------

dbglogicerror::dbglogicerror (const std::string& err) : errString (err)
{

}

const char* dbglogicerror::what () const
{
    return errString.c_str ();
}
