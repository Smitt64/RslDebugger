#ifndef BREAKPOINTEVENT_H
#define BREAKPOINTEVENT_H

#include <QEvent>

class BreakPointEvent : public QEvent
{
public:
    BreakPointEvent(Qt::HANDLE data);
    Qt::HANDLE data();

private:
    Qt::HANDLE pData;
};

#endif // BREAKPOINTEVENT_H
