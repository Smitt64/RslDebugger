#include "stdviewdockwidget.h"
#include "models/logeventmodel.h"
#include <QTreeView>
#include <QHeaderView>
#include <QFile>
#include <QDebug>

StdViewDockWidget::StdViewDockWidget() :
    QDockWidget()
{
    m_View.reset(new QTreeView());
    m_View->setIndentation(0);
    m_View->setRootIsDecorated(false);
    m_View->setIconSize(QSize(16, 16)); // квадратный запрос: иначе SVG растягивается при загрузке
    setWidget(m_View.data());

    //setWindowTitle(tr("Debugger log"));
}

StdViewDockWidget::~StdViewDockWidget()
{

}

void StdViewDockWidget::setModel(QAbstractItemModel *model)
{
    m_View->setModel(model);

    connect(model, &QAbstractItemModel::rowsInserted, this, &StdViewDockWidget::rowsInserted);
}

void StdViewDockWidget::rowsInserted(const QModelIndex &parent, int first, int last)
{
    if (m_AutoScrollToBottom)
        m_View->scrollToBottom();
}

void StdViewDockWidget::setAutoScrollToBottom(bool value)
{
    m_AutoScrollToBottom = value;
}

QTreeView *StdViewDockWidget::view()
{
    return m_View.data();
}

void StdViewDockWidget::setDrawBranches(const bool &value)
{
    if (!value)
        m_View->setStyleSheet("");
    else
    {
        QFile f(":/img/tree/treebranch.qss");
        f.open(QIODevice::ReadOnly);
        m_View->setStyleSheet(f.readAll());
    }
}
