#ifndef UPDATEDBGINFOEVENT_H
#define UPDATEDBGINFOEVENT_H

#include <QEvent>

class UpdateDbgInfoEvent : public QEvent
{
public:
    UpdateDbgInfoEvent(Qt::HANDLE data);

    Qt::HANDLE data();

private:
    Qt::HANDLE pData;
};

#endif // UPDATEDBGINFOEVENT_H
