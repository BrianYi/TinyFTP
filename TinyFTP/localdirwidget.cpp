#include "common.h"
#include "tinyftp.h"
#include "localdirwidget.h"
#include "remotedirwidget.h"
#include "dirtreemodel.h"
#include "tabwidget.h"
#include "localdirtreeview.h"

LocalDirWidget::LocalDirWidget(QWidget *parent)
	: QWidget(parent)
{
	parentTinyFtp = reinterpret_cast<TinyFTP*>(parent);

	localDirTreeModel = new DirTreeModel(this);
	localDirTreeModel->setRootPath(QDir::currentPath());

	localDirTreeView = new LocalDirTreeView(this);
    localDirTreeView->setModel(localDirTreeModel);
	localDirTreeView->header()->setStretchLastSection(true);
	localDirTreeView->resizeColumnsToContents();
    localDirTreeView->setAlternatingRowColors(true);
	localDirTreeView->setSelectionMode(QAbstractItemView::ContiguousSelection);
	localDirTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    localDirTreeView->setSortingEnabled(true);
    localDirTreeView->sortByColumn(0, Qt::AscendingOrder);
    localDirTreeView->setItemsExpandable(false);
    localDirTreeView->setRootIsDecorated(false);
    localDirTreeView->setExpandsOnDoubleClick(false);
    localDirTreeView->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);

	localDirFileSystemModel = new QFileSystemModel(this);
	localDirFileSystemModel->setFilter(QDir::AllDirs | QDir::Drives | 
		QDir::NoDot | QDir::NoDotDot | QDir::NoDotAndDotDot);
	QModelIndex index = localDirFileSystemModel->setRootPath(QDir::currentPath());
	
	localDirComboTreeView = new QTreeView(this);
	localDirComboTreeView->setModel(localDirFileSystemModel);
    localDirComboTreeView->resizeColumnToContents(0);
    localDirComboTreeView->hideColumn(1);
    localDirComboTreeView->hideColumn(2);
    localDirComboTreeView->hideColumn(3);
    localDirComboTreeView->setHeaderHidden(true);
    localDirComboTreeView->expand(index);
    localDirComboTreeView->scrollTo(index);
    localDirComboTreeView->setCurrentIndex(index);
    localDirComboTreeView->setItemsExpandable(true);
	
	localDirComboBox = new QComboBox(this);
	localDirComboBox->setModel(localDirFileSystemModel);
	localDirComboBox->setView(localDirComboTreeView);

	dotdotDirToolButton = new QToolButton(this);
	dotdotDirToolButton->setText(tr("上级目录"));
	refreshDirToolButton = new QToolButton(this);
	refreshDirToolButton->setText(tr("刷新"));

	localDirStatusBar = new QStatusBar(this);

	QHBoxLayout *topHBoxLayout = new QHBoxLayout;
	topHBoxLayout->addWidget(dotdotDirToolButton);
	topHBoxLayout->addWidget(refreshDirToolButton);
	topHBoxLayout->addWidget(localDirComboBox);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(topHBoxLayout);
	mainLayout->addWidget(localDirTreeView);
	mainLayout->addWidget(localDirStatusBar);
	setLayout(mainLayout);

	//*******************************
	// context menu
	contextMenu = new QMenu(this);
	dotdotAction = new QAction(tr("上级目录"), this);
	uploadAction = new QAction(tr("上传"), this);
	queueAction = new QAction(tr("队列"), this);
    refreshAction = new QAction(tr("刷新"), this);
	editAction = new QAction(tr("编辑"), this);
	readAction = new QAction(tr("查看"), this);
	execAction = new QAction(tr("执行"), this);
	delAction = new QAction(tr("删除"), this);
	renameAction = new QAction(tr("重命名"), this);
    newDirAction = new QAction(tr("新建文件夹"), this);
	propertyAction = new QAction(tr("属性"), this);
	contextMenu->addAction(dotdotAction);
	contextMenu->addSeparator();
	contextMenu->addAction(uploadAction);
	contextMenu->addAction(queueAction);
	contextMenu->addSeparator();
    contextMenu->addAction(refreshAction);
	sendToAction = contextMenu->addMenu(new QMenu(tr("发送到"), this));
	contextMenu->addAction(editAction);
	contextMenu->addAction(readAction);
	contextMenu->addAction(execAction);
	contextMenu->addSeparator();
	contextMenu->addAction(delAction);
	contextMenu->addAction(renameAction);
    contextMenu->addAction(newDirAction);
	contextMenu->addSeparator();
	contextMenu->addAction(propertyAction);

    //*******************************
    // 控件 信号 & 槽
	connect(dotdotDirToolButton, SIGNAL(clicked()), this, SLOT(dotdot()));
	connect(localDirTreeView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(setRootIndex(const QModelIndex &)));
	/*connect(localDirComboTreeView, SIGNAL())*/
    connect(localDirComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(currentIndexChanged(const QString &)));
	connect(localDirTreeView, SIGNAL(pressed(const QModelIndex &)), this, SLOT(showContextMenu(const QModelIndex &)));
    connect(refreshDirToolButton, SIGNAL(clicked()), this, SLOT(refresh()));
    connect(localDirTreeModel, SIGNAL(editingFinished(const QModelIndex &)), this, SLOT(editingFinished(const QModelIndex &)));

	//*******************************
	// context menu slots & signals
	connect(dotdotAction, SIGNAL(triggered()), this, SLOT(dotdot()));
	connect(uploadAction, SIGNAL(triggered()), this, SLOT(upload()));
	connect(queueAction, SIGNAL(triggered()), this, SLOT(queue()));
    connect(refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));
	connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));
	connect(readAction, SIGNAL(triggered()), this, SLOT(read()));
	connect(execAction, SIGNAL(triggered()), this, SLOT(exec()));
	connect(delAction, SIGNAL(triggered()), this, SLOT(del()));
	connect(renameAction, SIGNAL(triggered()), this, SLOT(rename()));
    connect(newDirAction, SIGNAL(triggered()), this, SLOT(newDir()));
	connect(propertyAction, SIGNAL(triggered()), this, SLOT(property()));
}

LocalDirWidget::~LocalDirWidget()
{
}

QDir LocalDirWidget::currentDir(bool *ok/* = 0*/) const
{
	return localDirTreeModel->currentDir(ok);
}

QString LocalDirWidget::currentDirPath() const
{
	return localDirTreeModel->currentDirPath();
}

QString LocalDirWidget::currentFilePath() const
{
	Node *node = static_cast<Node*>(
		localDirTreeView->currentIndex().internalPointer());
	if (!node) {
		return QString();
	}
	return node->filePath;
}

void LocalDirWidget::setRootIndex(const QModelIndex &index)
{
	if (!index.isValid()) {
		return ;
	}
	Node *node = static_cast<Node*>(index.internalPointer());
	if (node->fileName == tr("..")) {
		dotdot();
	} else if (node->isDir) {
		dotdotDirToolButton->setEnabled(true);
        QString dir = node->filePath;
		localDirTreeModel->setRootIndex(index);
		localDirTreeView->resizeColumnsToContents();
		
        //*******************************
        // 这里的代码没有效果，不知为何
        QModelIndex curIndex = localDirFileSystemModel->index(dir);
        localDirComboTreeView->collapseAll();
        localDirComboTreeView->expand(curIndex);
        localDirComboTreeView->reset();
	}
}

void LocalDirWidget::currentIndexChanged(const QString &text)
{
    QModelIndex curIndex = localDirComboTreeView->currentIndex();
    localDirComboTreeView->collapseAll();
    localDirComboTreeView->expand(curIndex);
    localDirComboTreeView->reset();

    localDirTreeModel->setRootPath(localDirFileSystemModel->filePath(curIndex));
    localDirTreeView->resizeColumnsToContents();
}

void LocalDirWidget::showContextMenu(const QModelIndex &index)
{
	if (QApplication::mouseButtons() == Qt::RightButton) {
		//*******************************
		// 默认 使能 所有菜单项
		QList<QAction*> actions = contextMenu->actions();
		foreach (QAction* action, actions)
			action->setEnabled(true);

		//*******************************
		// 处理 发送到 菜单
		sendToAction->menu()->clear();
		TabWidget *remoteDirTabWidget = parentTinyFtp->remoteDirTabWidget;
		int count = remoteDirTabWidget->count();
		for (int i = 0; i < count; i++) {
			RemoteDirWidget *w = static_cast<RemoteDirWidget*>(remoteDirTabWidget->widget(i));
			if (w->isConnected()) {
				QAction *action = new QAction(remoteDirTabWidget->tabText(i), this);
				sendToAction->menu()->addAction(action);
			}
		}

		//*******************************
		// 处理 所有上下文菜单的 使能 状态
		if (!sendToAction->menu()->actions().count()) {
			sendToAction->setEnabled(false);
		}
		Node *node = static_cast<Node*>(index.internalPointer());
		QFileInfo fileInfo(node->filePath);
		if (fileInfo.isDir() && node->fileName == tr("..")) {
			foreach (QAction* action, actions)
				action->setEnabled(false);
		} else if (true/*fileInfo.isFile()*/) {
			if (!fileInfo.isWritable()) {
				editAction->setEnabled(false);
				execAction->setEnabled(false);
				delAction->setEnabled(false);
				renameAction->setEnabled(false);
			}
			if (!fileInfo.isReadable()) {
				readAction->setEnabled(false);
				uploadAction->setEnabled(false);
				queueAction->setEnabled(false);
			}
			if (!fileInfo.isExecutable()) {
				execAction->setEnabled(false);
			}
			if (!static_cast<RemoteDirWidget*>(
				remoteDirTabWidget->currentWidget())->isConnected()) {
                    uploadAction->setEnabled(false);
                    sendToAction->setEnabled(false);
			}
			QModelIndex dotdotIndex = localDirTreeModel->index(0, 0);
			Node *dotdotNode = static_cast<Node*>(dotdotIndex.internalPointer());
			dotdotAction->setEnabled(dotdotNode->fileName == tr(".."));
		}

		contextMenu->exec(QCursor::pos());
	}
}

void LocalDirWidget::dotdot()
{
	localDirTreeModel->setRootPath(currentDirPath() + tr("/") + tr(".."));
	localDirTreeView->reset();
	localDirTreeView->resizeColumnsToContents();
	Node *dotdotNode = static_cast<Node*>(localDirTreeModel->index(0, 0).internalPointer());
	dotdotDirToolButton->setEnabled(dotdotNode->fileName == tr(".."));
}

void LocalDirWidget::upload()
{
	RemoteDirWidget *r = parentTinyFtp->remoteCurrentWidget();
	r->upload(currentFilePath());
}

void LocalDirWidget::queue()
{

}

void LocalDirWidget::edit()
{

}

void LocalDirWidget::read()
{

}

void LocalDirWidget::exec()
{
	
}

void LocalDirWidget::del()
{
	QString filePath = currentFilePath();
	QFileInfo fileInfo(filePath);
	if (fileInfo.isDir()) {
		delDir(filePath);
	} else {
		QFile(filePath).remove();
	}
	refresh();
}

void LocalDirWidget::rename()
{

}

void LocalDirWidget::property()
{

}

void LocalDirWidget::reset()
{
    refresh();
}

void LocalDirWidget::refresh()
{
    localDirTreeModel->setRootPath(currentDirPath());
    localDirTreeView->resizeColumnsToContents();
}

void LocalDirWidget::newDir()
{
    QString dirName = tr("新建文件夹");
    QDir dir(currentDirPath());
    if (!dir.exists(dirName)) {
        goto succeed;
        return ;
    }

    int i = 2;
    while (dir.exists(dirName)) {
        dirName = tr("新建文件夹(%1)").arg(i);
        i++;
    }
succeed:
    dir.mkdir(dirName);
    refresh();
    return ;
}

void LocalDirWidget::editingFinished(const QModelIndex &index)
{
    Node *n = static_cast<Node*>(index.internalPointer());
    QFile file(n->filePath);
    if (QFileInfo(n->filePath).fileName() != n->fileName) {
        file.rename(n->fileName);
        n->filePath = n->dirPath + tr("/") + n->fileName;
    }
}
