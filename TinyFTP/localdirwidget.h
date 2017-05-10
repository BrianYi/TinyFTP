#ifndef LOCALDIRWIDGET_H
#define LOCALDIRWIDGET_H

#include <QtGui>
#include "dirtreemodel.h"

class TinyFTP;
class LocalDirWidget : public QWidget
{
	Q_OBJECT

public:
	LocalDirWidget(QWidget *parent);
	~LocalDirWidget();
	//void contextMenuEvent(QContextMenuEvent *event);
	QDir currentDir(bool *ok = 0) const;
	QString currentDirPath() const;
    void reset();
	private slots:
		void setRootIndex(const QModelIndex &index);
        void currentIndexChanged(const QString &text);
		void showContextMenu(const QModelIndex &index);
        void upload();
        void queue();
        void refresh();
        void edit();
        void read();
        void exec();
        void del();
        void rename();
        void property();
private:
	DirTreeModel *localDirTreeModel;
	QTreeView *localDirTreeView;
	QFileSystemModel *localDirFileSystemModel;
	QTreeView *localDirComboTreeView;
	QComboBox *localDirComboBox;
	QToolButton *dotdotDirToolButton;
	QToolButton *refreshDirToolButton;
	QStatusBar *localDirStatusBar;
	QMenu *contextMenu;
	QAction *uploadAction;
	QAction *queueAction;
    QAction *refreshAction;
	QAction *sendToAction;
	QAction *editAction;
	QAction *readAction;
	QAction *execAction;
	QAction *delAction;
	QAction *renameAction;
	QAction *propertyAction;
	TinyFTP *parentTinyFtp;
};

#endif // LOCALDIRWIDGET_H
