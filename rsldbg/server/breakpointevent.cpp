#include "breakpointevent.h"
#include "dbgserverproto.h"

BreakPointEvent::BreakPointEvent(const TBpData *data) :
    QEvent((QEvent::Type)MSG_BREAKPOINT),
    pData(data ? new TBpData(*data) : nullptr)
{
}

BreakPointEvent::~BreakPointEvent()
{
    delete pData;
}

Qt::HANDLE BreakPointEvent::data()
{
    return pData;
}
