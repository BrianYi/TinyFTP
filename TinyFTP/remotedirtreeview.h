#ifndef REMOTEDIRTREEVIEW_H
#define REMOTEDIRTREEVIEW_H

#include <QTreeView>

class RemoteDirWidget;
struct Node;
class RemoteDirTreeView : public QTreeView
{
    Q_OBJECT

public:
    RemoteDirTreeView(QWidget *parent);
    ~RemoteDirTreeView();
    int rowCount();
    int columnCount();
    void setRootPath(const QString path);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    QString currentDirPath() const;
    Node* item(int row, int column) const;
protected:
    void contextMenuEvent(QContextMenuEvent *event);
private:
    RemoteDirWidget *parentRemoteDirWidget;
};

#endif // REMOTEDIRTREEVIEW_H
