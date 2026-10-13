#include "breakpointevent.h"
#include "dbgserverproto.h"

BreakPointEvent::BreakPointEvent(Qt::HANDLE data) :
    QEvent((QEvent::Type)MSG_BREAKPOINT)
{
    pData = data;
}

Qt::HANDLE BreakPointEvent::data()
{
    return pData;
}
