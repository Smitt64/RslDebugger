#ifndef VARWATCHDOCKWIDGET_H
#define VARWATCHDOCKWIDGET_H

#include <QVariant>
#include <QStyledItemDelegate>
#include "stdviewdockwidget.h"

class TreeButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TreeButtonDelegate();
    virtual ~TreeButtonDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) Q_DECL_OVERRIDE;

signals:
    void clicked(const QModelIndex &index);

private:
    QStyleOptionToolButton getOption(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QPoint m_MousePos;
    bool m_pressed;
};

class VarWatchDockWidget : public StdViewDockWidget
{
    Q_OBJECT
public:
    VarWatchDockWidget();
    virtual ~VarWatchDockWidget();

    virtual void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

signals:
    void expandVariable(int, qint64);
    void showVarValue(qint64 val, qint64 info);

private slots:
    void customContextMenu(const QPoint &pos);

private:
    QVariant m_LastScroll;
    QScopedPointer<TreeButtonDelegate> m_pDelegate;
};

#endif // VARWATCHDOCKWIDGET_H
