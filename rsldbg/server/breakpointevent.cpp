#include "breakpointevent.h"

BreakPointEvent::BreakPointEvent(const TBpData *data) :
    DbgServerEvent(MSG_BREAKPOINT),
    pData(data ? new TBpData(*data) : nullptr)
{
}

BreakPointEvent::~BreakPointEvent()
{
    delete pData;
}

TBpData *BreakPointEvent::data()
{
    return pData;
}
