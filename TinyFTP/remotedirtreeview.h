#ifndef REMOTEDIRTREEVIEW_H
#define REMOTEDIRTREEVIEW_H

#include <QTreeView>

class RemoteDirWidget;
class RemoteDirTreeView : public QTreeView
{
    Q_OBJECT

public:
    RemoteDirTreeView(QWidget *parent);
    ~RemoteDirTreeView();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
private:
    RemoteDirWidget *parentRemoteDirWidget;
};

#endif // REMOTEDIRTREEVIEW_H
