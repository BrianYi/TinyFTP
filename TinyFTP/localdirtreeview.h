#ifndef LOCALDIRTREEVIEW_H
#define LOCALDIRTREEVIEW_H

#include <QTreeView>

class LocalDirWidget;
class LocalDirTreeView : public QTreeView
{
    Q_OBJECT

public:
    LocalDirTreeView(QWidget *parent);
    ~LocalDirTreeView();
    void resizeColumnsToContents();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
private:
    LocalDirWidget *parentLocalDirWidget;
};

#endif // LOCALDIRTREEVIEW_H
