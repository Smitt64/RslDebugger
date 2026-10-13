#ifndef CALLSTACKDOCKWIDGET_H
#define CALLSTACKDOCKWIDGET_H

#include "stdviewdockwidget.h"

class CallStackDockWidget : public StdViewDockWidget
{
public:
    CallStackDockWidget();
    virtual ~CallStackDockWidget();

private slots:
    void customContextMenu(const QPoint &pos);
};

#endif // CALLSTACKDOCKWIDGET_H
