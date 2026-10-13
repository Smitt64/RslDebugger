#include "callstackdockwidget.h"
#include "models/callstackmodel.h"
#include <QTreeView>
#include <QMenu>
#include <QGuiApplication>

CallStackDockWidget::CallStackDockWidget() :
    StdViewDockWidget()
{
    QTreeView *tree = view();
    tree->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tree, &QTreeView::customContextMenuRequested, this, &CallStackDockWidget::customContextMenu);
}

CallStackDockWidget::~CallStackDockWidget()
{

}

void CallStackDockWidget::customContextMenu(const QPoint &pos)
{
    QTreeView *tree = view();
    QClipboard *clipboard = QGuiApplication::clipboard();
    QItemSelectionModel *selection = tree->selectionModel();

    if (!selection->hasSelection() || !selection->currentIndex().isValid())
        return;

    CallStackModel *model = qobject_cast<CallStackModel*>(tree->model());

    if (!model)
        return;

    QMenu menu;
    QAction *pCopyFunction = menu.addAction(QIcon::fromTheme("Copy"), tr("Copy function name"));
    menu.addSeparator();
    QAction *pCopyMacro = menu.addAction(tr("Copy macro name"));
    QAction *pCopyFullMacro = menu.addAction(tr("Copy macro full name(with path)"));

    menu.setDefaultAction(pCopyFunction);
    QAction *result = menu.exec(tree->mapToGlobal(pos));
}
