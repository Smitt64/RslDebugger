#include "callstackdockwidget.h"
#include "models/callstackmodel.h"
#include <QTreeView>
#include <QHeaderView>

CallStackDockWidget::CallStackDockWidget(QWidget *parent) :
    QDockWidget(parent),
    m_pView(new QTreeView()),
    m_pModel(new CallStackModel())
{
    setWindowTitle(tr("Call stack"));
    setWidget(m_pView.data());

    m_pView->setRootIsDecorated(false);
    m_pView->setModel(m_pModel.data());
}

CallStackDockWidget::~CallStackDockWidget()
{

}

CallStackModel *CallStackDockWidget::model()
{
    return m_pModel.data();
}
