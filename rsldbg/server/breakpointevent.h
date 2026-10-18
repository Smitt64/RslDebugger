#ifndef BREAKPOINTEVENT_H
#define BREAKPOINTEVENT_H

#include <QEvent>
#include "lf/bp_data.h"

// Событие владеет копией TBpData: оригинал хранится в CDebug::m_bpdata и может
// быть удалён (DelBp при BP_INVISIBLE) раньше, чем серверный поток обработает
// это событие из очереди. Чтение сырого указателя было use-after-free.
class BreakPointEvent : public QEvent
{
public:
    BreakPointEvent(const TBpData *data);
    ~BreakPointEvent();

    Qt::HANDLE data();

private:
    TBpData *pData;
};

#endif // BREAKPOINTEVENT_H
