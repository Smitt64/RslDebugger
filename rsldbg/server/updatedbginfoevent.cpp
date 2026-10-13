#include "updatedbginfoevent.h"
#include "dbgserverproto.h"

UpdateDbgInfoEvent::UpdateDbgInfoEvent(Qt::HANDLE data) :
    QEvent((QEvent::Type)MSG_UPDATEDBGINFO_EVENT)
{
    pData = data;
}

Qt::HANDLE UpdateDbgInfoEvent::data()
{
    return pData;
}
