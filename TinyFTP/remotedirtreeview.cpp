#include "remotedirtreeview.h"
#include "remotedirwidget.h"
#include "dirtreemodel.h"

RemoteDirTreeView::RemoteDirTreeView(QWidget *parent)
    : QTreeView(parent)
{
    parentRemoteDirWidget = static_cast<RemoteDirWidget*>(parent);
}

RemoteDirTreeView::~RemoteDirTreeView()
{

}

void RemoteDirTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    RemoteDirWidget *p = parentRemoteDirWidget;
    //*******************************
    // 默认 使能 所有菜单项
    QList<QAction*> actions = p->contextMenu->actions();
    foreach (QAction* action, actions)
        action->setEnabled(false);

    //*******************************
    // 当前未连接上或是有命令未执行完，则禁用所有选项
    if (!p->isConnected() || p->ftpClient->hasPendingCommands()) {
        goto menuexec;
    }

    //*******************************
    // 处理上下文菜单
    if (p->currentDirPathUrl() == QDir::separator()) {
        p->dotdotAction->setEnabled(false);
    } else {
        DirTreeModel *r = static_cast<DirTreeModel*>(p->remoteDirTreeView->model());
        Node *dotdotNode = static_cast<Node*>(r->index(0, 0).internalPointer());
        p->dotdotAction->setEnabled(dotdotNode->fileName == tr(".."));
    }
    p->refreshAction->setEnabled(true);
    p->newDirAction->setEnabled(true);
menuexec:
    p->contextMenu->exec(QCursor::pos());
}
