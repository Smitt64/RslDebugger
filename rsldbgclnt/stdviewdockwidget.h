#ifndef STDVIEWDOCKWIDGET_H
#define STDVIEWDOCKWIDGET_H

#include <QDockWidget>

class QTreeView;
class QAbstractItemModel;
class StdViewDockWidget : public QDockWidget
{
public:
    StdViewDockWidget();
    virtual ~StdViewDockWidget();

    virtual void setModel(QAbstractItemModel *model);
    void setAutoScrollToBottom(bool value);

    QTreeView *view();
    void setDrawBranches(const bool &value);

public slots:
    void rowsInserted(const QModelIndex &parent, int first, int last);

private:
    QScopedPointer<QTreeView> m_View;

    bool m_AutoScrollToBottom;
};

#endif // STDVIEWDOCKWIDGET_H
