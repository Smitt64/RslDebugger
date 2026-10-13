#include "dbgbreakpointevent.h"
#include "cdebug.h"

DbgBreakpointEvent::DbgBreakpointEvent(CDebug* dbg, Qt::HANDLE data) :
    QEvent((QEvent::Type)MSG_BREAKPOINT)
{
    _dbg = dbg;
    _data = data;
}

CDebug *DbgBreakpointEvent::dbg()
{
    return _dbg;
}

Qt::HANDLE DbgBreakpointEvent::data()
{
    return _data;
}
