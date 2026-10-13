#ifndef DBGBREAKPOINTEVENT_H
#define DBGBREAKPOINTEVENT_H

#include <QEvent>

class CDebug;
class DbgBreakpointEvent : public QEvent
{
public:
    DbgBreakpointEvent(CDebug* dbg, Qt::HANDLE data);

    CDebug *dbg();
    Qt::HANDLE data();

private:
    CDebug* _dbg;
    Qt::HANDLE _data;
};

#endif // DBGBREAKPOINTEVENT_H
