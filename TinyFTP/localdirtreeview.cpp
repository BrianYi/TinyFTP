#include "localdirtreeview.h"
#include "localdirwidget.h"
#include "dirtreemodel.h"

LocalDirTreeView::LocalDirTreeView(QWidget *parent)
    : QTreeView(parent)
{
    parentLocalDirWidget = static_cast<LocalDirWidget*>(parent);
}

LocalDirTreeView::~LocalDirTreeView()
{

}

void LocalDirTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    LocalDirWidget *p = parentLocalDirWidget;
    //*******************************
    // 默认 使能 所有菜单项
    QList<QAction*> actions = p->contextMenu->actions();
    foreach (QAction* action, actions)
        action->setEnabled(false);

    //*******************************
    // 处理上下文菜单
    QModelIndex dotdotIndex = p->localDirTreeModel->index(0, 0);
    Node *dotdotNode = static_cast<Node*>(dotdotIndex.internalPointer());
    p->dotdotAction->setEnabled(dotdotNode->fileName == tr(".."));
    p->refreshAction->setEnabled(true);
    p->newDirAction->setEnabled(true);
    p->contextMenu->exec(QCursor::pos());
}
