#ifndef UPDATEDBGINFOEVENT_H
#define UPDATEDBGINFOEVENT_H

#include "serverevent.h"
#include "../dbgserverproto.h"

class UpdateDbgInfoEvent : public DbgServerEvent
{
public:
    explicit UpdateDbgInfoEvent(Qt::HANDLE data)
        : DbgServerEvent(MSG_UPDATEDBGINFO_EVENT), pData(data) {}

    Qt::HANDLE data() { return pData; }

private:
    Qt::HANDLE pData;
};

#endif // UPDATEDBGINFOEVENT_H
