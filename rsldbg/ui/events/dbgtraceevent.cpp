#include "dbgtraceevent.h"
#include "cdebug.h"

DbgTraceEvent::DbgTraceEvent(CDebug *dbg) :
    QEvent((QEvent::Type)MSG_TRACE)
{
    _dbg = dbg;
}

CDebug *DbgTraceEvent::dbg()
{
    return _dbg;
}
