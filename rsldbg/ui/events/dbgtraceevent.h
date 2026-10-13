#ifndef DBGTRACEEVENT_H
#define DBGTRACEEVENT_H

#include <QEvent>

class CDebug;
class DbgTraceEvent : public QEvent
{
public:
    DbgTraceEvent(CDebug *dbg);

    CDebug *dbg();
private:
    CDebug *_dbg;
};

#endif // DBGTRACEEVENT_H
