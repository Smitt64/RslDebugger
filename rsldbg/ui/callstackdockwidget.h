#ifndef CALLSTACKDOCKWIDGET_H
#define CALLSTACKDOCKWIDGET_H

#include <QDockWidget>
#include <QWidget>

class QTreeView;
class CallStackModel;
class CallStackDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    CallStackDockWidget(QWidget *parent = nullptr);
    virtual ~CallStackDockWidget();

    CallStackModel *model();

private:
    QScopedPointer<QTreeView> m_pView;
    QScopedPointer<CallStackModel> m_pModel;
};

#endif // CALLSTACKDOCKWIDGET_H
