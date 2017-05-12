#include "common.h"
#include "remotedirwidget.h"
#include "localdirwidget.h"
#include "tinyftp.h"
#include "dirtreemodel.h"

RemoteDirWidget::RemoteDirWidget(QWidget *parent)
	: QWidget(parent)
{
	parentTinyFtp = reinterpret_cast<TinyFTP*>(parent);
	currentCommand = CMD_NONE;

	/*remoteDirTreeModel = 0;*/

	remoteDirTreeView = new QTreeView(this);
    /*remoteDirTreeView->setModel(remoteDirTreeModel);*/
    remoteDirTreeView->header()->setStretchLastSection(true);   
    remoteDirTreeView->resizeColumnToContents(0);
	remoteDirTreeView->setAlternatingRowColors(true);
	remoteDirTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
	remoteDirTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	remoteDirTreeView->setSortingEnabled(true);
	remoteDirTreeView->sortByColumn(0, Qt::AscendingOrder);
    remoteDirTreeView->setItemsExpandable(false);
    remoteDirTreeView->setRootIsDecorated(false);

	remoteDirFileSystemModel = new QFileSystemModel(this);
	remoteDirFileSystemModel->setFilter(QDir::AllDirs | QDir::Drives | 
		QDir::NoDot | QDir::NoDotDot | QDir::NoDotAndDotDot);

	remoteDirComboTreeView = new QTreeView;
	remoteDirComboTreeView->setModel(remoteDirFileSystemModel);
	remoteDirComboTreeView->resizeColumnToContents(0);
	remoteDirComboTreeView->hideColumn(1);
	remoteDirComboTreeView->hideColumn(2);
	remoteDirComboTreeView->hideColumn(3);
	remoteDirComboTreeView->setHeaderHidden(true);
	remoteDirComboTreeView->setItemsExpandable(true);

	remoteDirComboBox = new QComboBox(this);
	remoteDirComboBox->setEnabled(false);
// 	remoteDirComboBox->setModel(remoteDirFileSystemModel);
// 	remoteDirComboBox->setView(remoteDirComboTreeView);

	connectButton = new QToolButton(this);
	connectButton->setText(tr("断开"));
	connectButton->setEnabled(false);
	dotdotDirToolButton = new QToolButton(this);
	dotdotDirToolButton->setText(tr("上级目录"));
	dotdotDirToolButton->setEnabled(false);
	refreshDirToolButton = new QToolButton(this);
	refreshDirToolButton->setText(tr("刷新"));
	refreshDirToolButton->setEnabled(false);

	logTextEdit = new QTextEdit(this);
	logTextEdit->setFixedHeight(100);
	logTextEdit->setReadOnly(true);
	remoteDirStatusBar = new QStatusBar(this);

//     username = tr("");
//     password = tr("");
//     port = 21;
//     address = tr("");
    ftpClient = new QFtp(this);

	QHBoxLayout *topHBoxLayout = new QHBoxLayout;
	topHBoxLayout->addWidget(connectButton);
	topHBoxLayout->addWidget(dotdotDirToolButton);
	topHBoxLayout->addWidget(refreshDirToolButton);
	topHBoxLayout->addWidget(remoteDirComboBox);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(topHBoxLayout);
	mainLayout->addWidget(remoteDirTreeView);
	mainLayout->addWidget(logTextEdit);
	mainLayout->addWidget(remoteDirStatusBar);
	mainLayout->setStretch(1,1);
	setLayout(mainLayout);

	//*******************************
	// treeview context menu
	contextMenu = new QMenu(this);
	dotdotAction = new QAction(tr("上级目录"), this);
	downloadAction = new QAction(tr("下载"), this);
	queueAction = new QAction(tr("队列"), this);
	refreshAction = new QAction(tr("刷新"), this);
	sendToAction = contextMenu->addMenu(new QMenu(tr("发送到"), this));
	editAction = new QAction(tr("编辑"), this);
	readAction = new QAction(tr("查看"), this);
	changePermissionAction = new QAction(tr("更改文件权限"), this);
	delAction = new QAction(tr("删除"), this);
	renameAction = new QAction(tr("重命名"), this);
	newDirAction = new QAction(tr("新建文件夹"), this);
	propertyAction = new QAction(tr("属性"), this);
	contextMenu->addAction(dotdotAction);
	contextMenu->addSeparator();
	contextMenu->addAction(downloadAction);
	contextMenu->addAction(queueAction);
	contextMenu->addSeparator();
	contextMenu->addAction(refreshAction);
	contextMenu->addAction(sendToAction);
	contextMenu->addAction(editAction);
	contextMenu->addAction(readAction);
	contextMenu->addAction(changePermissionAction);
	contextMenu->addSeparator();
	contextMenu->addAction(delAction);
	contextMenu->addAction(renameAction);
	contextMenu->addAction(newDirAction);
	contextMenu->addSeparator();
	contextMenu->addAction(propertyAction);
	isListing = false;

/*	connect(remoteDirTableView, SIGNAL(doubleClicked(const QModelIndex &)), remoteDirTableModel, SLOT(setRootIndex(const QModelIndex &)));*/
	connect(dotdotDirToolButton, SIGNAL(clicked()), this, SLOT(dotdot()));
    connect(remoteDirTreeView, SIGNAL(doubleClicked(const QModelIndex &)), 
		this, SLOT(setRootIndex(const QModelIndex &)));
    connect(ftpClient, SIGNAL(listInfo(const QUrlInfo &)), 
		this, SLOT(ftpListInfo(const QUrlInfo &)));
    connect(ftpClient, SIGNAL(done(bool)), 
		this, SLOT(ftpDone(bool)));
    connect(ftpClient, SIGNAL(commandFinished(int,bool)), 
		this, SLOT(ftpCommandFinished(int,bool)));
    connect(ftpClient, SIGNAL(commandStarted(int)), 
		this, SLOT(ftpCommandStarted(int)));
	connect(ftpClient, SIGNAL(stateChanged(int)), 
		this, SLOT(ftpStateChanged(int)));
	connect(remoteDirTreeView, SIGNAL(pressed(const QModelIndex &)), 
		this, SLOT(showContextMenu(const QModelIndex &)));
	connect(this, SIGNAL(ftpCommandDone(QFtp::Command, bool)), 
		parentTinyFtp, SLOT(ftpCommandDone(QFtp::Command, bool)));
	connect(refreshDirToolButton, SIGNAL(clicked()), this, SLOT(refresh()));

	//*******************************
	// context menu slots & signals
	connect(dotdotAction, SIGNAL(triggered()), this, SLOT(dotdot()));
	connect(downloadAction, SIGNAL(triggered()), this, SLOT(download()));
	connect(queueAction, SIGNAL(triggered()), this, SLOT(queue()));
	connect(refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));
	connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));
	connect(readAction, SIGNAL(triggered()), this, SLOT(read()));
	connect(changePermissionAction, SIGNAL(triggered()), this, SLOT(changePermission()));
	connect(delAction, SIGNAL(triggered()), this, SLOT(del()));
	connect(renameAction, SIGNAL(triggered()), this, SLOT(rename()));
	connect(newDirAction, SIGNAL(triggered()), this, SLOT(newDir()));
	connect(propertyAction, SIGNAL(triggered()), this, SLOT(property()));
}

RemoteDirWidget::~RemoteDirWidget()
{

}

bool RemoteDirWidget::listing() const
{
	return isListing;
}

void RemoteDirWidget::setListing(bool isDoing)
{
	isListing = isDoing;
}

// void RemoteDirWidget::setLoginInfo(const QString &port, const QString &address, 
//     const QString &usrname/* = QString()*/, const QString &pwd/* = QString()*/)
// {
//     this->username = usrname;
//     this->password = pwd;
//     this->port = port;
//     this->address = address;
// }

void RemoteDirWidget::writeLog(const QString &logData)
{
    logTextEdit->append("[" + QDateTime::currentDateTime().toString("hh:mm:ss")  + "] " + logData);
}

void RemoteDirWidget::connectToHost(const QString &address, const QString &port, 
    const QString &username/* = QString()*/, const QString &password/* = QString()*/)
{
    QString strAddress = address;
//     if (!strAddress.startsWith(tr("ftp://"), Qt::CaseInsensitive)) {
//         strAddress = tr("ftp://") + strAddress;
//     }
    urlAddress.setHost(strAddress);
	urlAddress.setPort(port.toInt());
	urlAddress.setUserName(username);
	urlAddress.setPassword(password);

	connectButton->setText(tr("连接"));
	connectButton->setEnabled(false);
	dotdotDirToolButton->setEnabled(false);
	refreshDirToolButton->setEnabled(false);
	remoteDirComboBox->setEnabled(false);
//     if (!urlAddress.isValid()) {
//         writeLog(tr("Error: Invalid URL"));
//         return false;
//     }
//     if (url.scheme() != "ftp") {
//          LOGSTREAM << DataPair(this, tr("Error: URL must start with 'ftp:'"));
//          return false;
//     }
	if (ftpClient->state() != QFtp::Unconnected) {
		ftpClient->close();
        DirTreeModel *dirTreeModel = static_cast<DirTreeModel*>(remoteDirTreeView->model());
        delete dirTreeModel;
	}
    ftpClient->connectToHost(urlAddress.host(), urlAddress.port());
    

//     QString path = url.path();
//     if (path.isEmpty())
//         path = QDir::separator();
// 
// /*    pendingDirs.append(path);*/
// 
//     processDirectory(path);

/*    return true;*/
}

bool RemoteDirWidget::isConnected() const
{
	return ftpClient->state() != QFtp::Unconnected;
}

QString RemoteDirWidget::currentDirPathUrl() const
{
	return url(currentDirPath());
}

QString RemoteDirWidget::currentFilePathUrl() const
{
// 	Node *node = static_cast<Node*>(remoteDirTreeView->currentIndex().internalPointer());
// 	if (!node) {
// 		return QString();
// 	}
	return url(currentFilePath());
}

QString RemoteDirWidget::currentDirPath() const
{
	return ((DirTreeModel*)remoteDirTreeView->model())->currentDirPath();
}

QString RemoteDirWidget::currentFilePath() const
{
	Node *node = static_cast<Node*>(remoteDirTreeView->currentIndex().internalPointer());
	if (!node) {
		return QString();
	}
	return node->filePath;
}

void RemoteDirWidget::reconnect()
{
	connectToHost(urlAddress.host(), QString::number(urlAddress.port()), 
		urlAddress.userName(), urlAddress.password());
}

void RemoteDirWidget::reset()
{
    //*******************************
    // 这里需要仔细思考，有bug
// 	QString curDirPathUrl = currentDirPathUrl();
//     listDirectoryFiles(curDirPathUrl);
// 	d->setRootPath(curDirPath);
// 	remoteDirTreeView->resizeColumnToContents(0);
	refresh();
}

void RemoteDirWidget::closeEvent(QCloseEvent *event)
{
	if (ftpClient->state() != QFtp::Unconnected)
		ftpClient->close();
}

void RemoteDirWidget::download()
{
	currentCommand = CMD_DOWNLOAD;
	Node *node = static_cast<Node*>(remoteDirTreeView->currentIndex().internalPointer());
	QString path = currentFilePathUrl()/*url(node->filePath)*/;
	int lstIdx = path.lastIndexOf(QDir::separator());
	currentDownloadBaseDir = path.left(lstIdx);
	currentDownloadRelativeDir = path.mid(lstIdx);
/*	currentDownloadLocalDir = "";*/
	/*	path = path.mid(lstIdx);*/
	/*download(path);*/

	// 1. 文件直接下载
	// 2. 目录递归下载
	if (node->isDir) {
		/*ftpClient->cd(baseDir);*/
		pendingDownloadRelativeDirs.append(currentDownloadRelativeDir);
		processDirectory();
	} else {
		LocalDirWidget *l = parentTinyFtp->localCurrentWidget();
		// 		QString dir = node->dirPath;
		// 		dir = dir.mid(dir.indexOf(urlAddress.host()) + urlAddress.host().length());

		/*currentDownloadRelativeDir = */
		currentDownloadLocalDir = l->currentDirPath();

		QFile *file = new QFile(currentDownloadLocalDir + QDir::separator()
			+ node->fileName);		

		if (!file->open(QIODevice::WriteOnly)) {
			writeLog(tr("Warning: Cannot write file %1: %2").arg(
				l->currentDirPath() + QDir::separator() + 
				file->fileName()).arg(file->errorString()));
//			currentCommand = CMD_NONE;
//			return;
		} else {
			ftpClient->get(encoded(path), file);
		}
		openedDownloadingFiles.append(file);
	}
}

void RemoteDirWidget::upload(const QString &filePath)
{
    currentCommand = CMD_UPLOAD;
    QFileInfo fileInfo(filePath);
    currentUploadBaseDir = currentDirPathUrl();
    currentUploadRelativeDir = QDir::separator() + fileInfo.fileName();
    if (fileInfo.isDir()) {
        pendingUploadRelativeDirs.append(currentUploadRelativeDir);
        processDirectory();
    } else {
        /*LocalDirWidget *l = parentTinyFtp->localCurrentWidget();*/
        // 		QString dir = node->dirPath;
        // 		dir = dir.mid(dir.indexOf(urlAddress.host()) + urlAddress.host().length());

        /*currentDownloadRelativeDir = */
        /*currentDownloadLocalDir = l->currentDirPath();*/
        QFile *file = new QFile(filePath);		
        if (!file->open(QIODevice::ReadOnly)) {
            writeLog(tr("Warning: Cannot read file %1: %2").arg(
                filePath).arg(file->errorString()));
// 			currentCommand = CMD_NONE;
//             return;
        } else {
			ftpClient->cd(encoded(currentUploadBaseDir));
			ftpClient->put(file, encoded(fileInfo.fileName()));
		}
		openedUploadingFiles.append(file);
    }
}

void RemoteDirWidget::processDirectory()
{
	if (currentCommand == CMD_DOWNLOAD) {
		if (pendingDownloadRelativeDirs.isEmpty()) {
			//*******************************
			// 下载完成
			writeLog(tr("所有文件已下载完成"));

			//*******************************
			// 让本地窗口进行重置，显示文件下载后的目录树
			LocalDirWidget *l = parentTinyFtp->localCurrentWidget();
			l->reset();
			currentCommand = CMD_NONE;
			return ;
		}
		LocalDirWidget *l = parentTinyFtp->localCurrentWidget();
		/*QString dir = pendingDownloadRelativeDirs.takeFirst();*/
		currentDownloadRelativeDir = pendingDownloadRelativeDirs.takeFirst();
		currentDownloadLocalDir = l->currentDirPath() + currentDownloadRelativeDir;
		if (QFileInfo(currentDownloadLocalDir).exists()) {
			//*******************************
			// 是否进行覆盖文件
		}
		/*QString dirName = currentDownloadDir.mid(currentDownloadDir.lastIndexOf(QDir::separator())+1);*/
		QDir().mkdir(currentDownloadLocalDir);
		ftpClient->cd(encoded(currentDownloadBaseDir + currentDownloadRelativeDir));
		ftpClient->list();
	} else if (currentCommand == CMD_UPLOAD) {
		if (pendingUploadRelativeDirs.isEmpty()) {
			//*******************************
			// 下载完成
			writeLog(tr("所有文件已上传完成"));

			//*******************************
			// 让远程窗口进行重置，显示文件下载后的目录树
			RemoteDirWidget *r = parentTinyFtp->remoteCurrentWidget();
			r->reset();
			currentCommand = CMD_NONE;
			return ;
		}
		LocalDirWidget *l = parentTinyFtp->localCurrentWidget();
		currentUploadRelativeDir = pendingUploadRelativeDirs.takeFirst();
		currentUploadLocalDir = l->currentDirPath() + currentUploadRelativeDir;
		QString dirPath = currentUploadBaseDir + currentUploadRelativeDir;
		QString cacheFilePath = currentDirPath() + currentUploadRelativeDir;
		if (!QDir(cacheFilePath).exists()) {
			QDir().mkdir(cacheFilePath);
		}
		ftpClient->mkdir(encoded(dirPath));
		ftpClient->cd(encoded(dirPath));
		foreach (QFileInfo fileInfo, QDir(currentUploadLocalDir).entryInfoList(
			QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot)) {
				if (fileInfo.isDir()) {
					pendingUploadRelativeDirs.append(currentUploadRelativeDir + QDir::separator() + fileInfo.fileName());
					/*processDirectory();*/
				} else {
					QFile *file = new QFile(fileInfo.absoluteFilePath());

					if (!file->open(QIODevice::ReadOnly)) {
						writeLog(tr("Warning: Cannot read file %1: %2").arg(
							QDir::toNativeSeparators(
							file->fileName())).arg(file->errorString()));
// 						currentCommand = CMD_NONE;
// 						return;
					} else {
						ftpClient->put(file, encoded(fileInfo.fileName()));
					}
					openedUploadingFiles.append(file);
				}
		}
	}
}

void RemoteDirWidget::listDirectoryFiles(const QString &dirUrl)
{
	/*currentCommand = CMD_LIST;*/
	if (!isConnected()) {
		reconnect();
	}
	setListing(true);
	currentListDir = (dirUrl == "" ? QDir::separator() : dirUrl);
	currentListLocalDir = cacheDir + dirUrl;
	QDir(".").mkpath(currentListLocalDir);
	//ftpClient->cd(currentListDir);
	ftpClient->list(encoded(currentListDir));
}

void RemoteDirWidget::ftpListInfo(const QUrlInfo &urlInfo)
{
	if (listing()) {
		if (urlInfo.isFile()) {
			if (urlInfo.isReadable()) {
				/*filesSize.push_back(urlInfo.size());*/
				/*filesModifyDate.append(urlInfo.lastModified().toString("yyyy/MM/dd hh:mm"));*/
				QString path = currentListLocalDir + QDir::separator()
					+ decoded(urlInfo.name());
				QFile file(path);
				if (!file.exists()) {
                    if (!file.open(QIODevice::WriteOnly)) {
                        writeLog(tr("Warning: Cannot create the cache file ") +
                            QDir::toNativeSeparators(
                            file.fileName()) +
                            ": " + file.errorString());
/*                        return ;*/
                    }
				}
			}
		} else if (urlInfo.isDir() && !urlInfo.isSymLink()) {
			//pendingDirs.append(currentDir + "/" + urlInfo.name());
			/*filesModifyDate.append(urlInfo.lastModified().toString("yyyy/MM/dd hh:mm"));*/
			QString localDir = currentListLocalDir + QDir::separator() + decoded(urlInfo.name());
			QDir(".").mkpath(localDir);
		}
        if (filesInfoMap.count(decoded(urlInfo.name()))) {
            writeLog(tr("Error: filesInfoMap has the same key: %1").arg(decoded(urlInfo.name())));
            return ;
        }
        filesInfoMap[decoded(urlInfo.name())] = urlInfo;
	} 
	
	if (currentCommand == CMD_DOWNLOAD) {
		if (urlInfo.isFile()) {
			if (urlInfo.isReadable()) {
				QFile *file = new QFile(currentDownloadLocalDir + QDir::separator()
					+ decoded(urlInfo.name()));

				if (!file->open(QIODevice::WriteOnly)) {
					writeLog(tr("Warning: Cannot write file %1: %2").arg(
						QDir::toNativeSeparators(
						file->fileName())).arg(file->errorString()));
					//return;
				} else {
					ftpClient->get(urlInfo.name(), file);
				}
				openedDownloadingFiles.append(file);
			}
		} else if (urlInfo.isDir() && !urlInfo.isSymLink()) {
			pendingDownloadRelativeDirs.append(currentDownloadRelativeDir + QDir::separator() + decoded(urlInfo.name()));
		}
	}
}

void RemoteDirWidget::ftpDone(bool error)
{
	
// 	else {
// 		writeLog(tr("Downloaded ") + currentListDir + tr(" to ") +
// 			QDir::toNativeSeparators(QDir(currentListLocalDir).canonicalPath()));
// 	}
	if (/*currentCommand == CMD_LIST*/listing()) {
		if (currentListDir.isEmpty() || error) {
			writeLog(tr("Error: ") + ftpClient->errorString());
			filesInfoMap.clear();
			setListing(false);
			return ;
		} else {
			DirTreeModel *dirTreeModel = static_cast<DirTreeModel*>(remoteDirTreeView->model());
			if (dirTreeModel) {
				delete dirTreeModel;
			}
			dirTreeModel = new DirTreeModel(this);

			dirTreeModel->setRootPath(currentListLocalDir);
			remoteDirTreeView->setModel(dirTreeModel);
			remoteDirTreeView->resizeColumnToContents(0);
			if (dirTreeModel->rowCount()) {
				for (int row = 0; row < dirTreeModel->rowCount(); row++) {
					QModelIndex index = dirTreeModel->index(row, 1);
					QModelIndex index2 = dirTreeModel->index(row, 3);
					Node *node = static_cast<Node*>(index.internalPointer());
					QUrlInfo urlInfo = filesInfoMap[node->fileName];
					if (node->isFile) {
						dirTreeModel->setData(index, urlInfo.size()/*filesSize.takeFirst()*/);
					}
					dirTreeModel->setData(index2, urlInfo.lastModified().toString("yyyy/MM/dd hh:mm"));

	// 				if (node->isDir && node->fileName == tr("..")) {
	// 					dirTreeModel->setData(index2, "");
	// 				} else if (!node->isSystemLink)
	// 					dirTreeModel->setData(index2, /*filesModifyDate.takeFirst()*/);
				}
			}
			filesInfoMap.clear();
			setListing(false);
		}
	} 
	
	if (currentCommand == CMD_DOWNLOAD) {
		if (error) {
			writeLog(tr("Error: ") + ftpClient->errorString());
			qDeleteAll(openedDownloadingFiles);
			openedDownloadingFiles.clear();
			currentCommand = CMD_NONE;
			return ;
		} else {
            QString dirPath = currentDownloadBaseDir + currentDownloadRelativeDir;
			if (openedDownloadingFiles.count() == 1) {
                QFileInfo fileInfo(*openedDownloadingFiles.first());
				writeLog(tr("文件下载 %1 到 %2 完成").arg(
					dirPath + QDir::separator() + fileInfo.fileName()).arg(
                    currentDownloadLocalDir + QDir::separator() + fileInfo.fileName()));
			} else {
				writeLog(tr("文件夹下载 %1 到 %2 完成").arg(
					dirPath).arg(currentDownloadLocalDir));
			}

			qDeleteAll(openedDownloadingFiles);
			openedDownloadingFiles.clear();

			processDirectory();
		}
	} else if (currentCommand == CMD_UPLOAD) {
		if (error) {
			writeLog(tr("Error: ") + ftpClient->errorString());
			qDeleteAll(openedUploadingFiles);
			openedUploadingFiles.clear();
			currentCommand = CMD_NONE;
			return ;
		} else {
            QString dirPath = currentUploadBaseDir + currentUploadRelativeDir;
			if (openedUploadingFiles.count() == 1) {
				QFileInfo fileInfo(*openedUploadingFiles.first());
				writeLog(tr("文件上传 %1 到 %2 完成").arg(
					currentUploadLocalDir + QDir::separator() + fileInfo.fileName()).arg(
					dirPath + QDir::separator() + fileInfo.fileName())
					);
			} else {
				writeLog(tr("文件夹上传 %1 到 %2 完成").arg(
					currentUploadLocalDir).arg(dirPath));
			}

			qDeleteAll(openedUploadingFiles);
			openedUploadingFiles.clear();

			processDirectory();
		}
	} else if (currentCommand == CMD_MKDIR) {
		if (error) {
			writeLog(tr("Error: ") + ftpClient->errorString());
			currentCommand = CMD_NONE;
			return ;
		} else {
			refresh();
			currentCommand = CMD_NONE;
		}
	}
}

void RemoteDirWidget::setRootIndex(const QModelIndex &index)
{
	if (!index.isValid()) {
		return ;
	}
	Node *node = static_cast<Node*>(index.internalPointer());
	if (node->fileName == tr("..")) {
		if (currentDirPathUrl() == QDir::separator()) {
			return ;
		}
		dotdot();
	} else if (node->isDir) {
		dotdotDirToolButton->setEnabled(true);
//        if (node->fileName == tr("..")) {
//             if (!currentListDir.endsWith(QDir::separator()) || currentListDir == QDir::separator())
//                 return ;
//             currentListDir.resize(currentListDir.length() - 1);
//             int lastSeparatorIndex = currentListDir.lastIndexOf(QDir::separator());
//             if (lastSeparatorIndex != -1)
//                 currentListDir = currentListDir.mid(0, lastSeparatorIndex + 1);
// 			currentListDir = url(node->dirPath);
//         } else
//		currentListDir = url(node->filePath);
        listDirectoryFiles(url(node->filePath));
	}
	/*remoteDirTreeModel->set(index);*/
// 	remoteDirTreeView->resizeColumnToContents(0);
}

void RemoteDirWidget::showContextMenu(const QModelIndex &index)
{
	if (QApplication::mouseButtons() == Qt::RightButton) {
		//*******************************
		// 默认 使能 所有菜单项
		QList<QAction*> actions = contextMenu->actions();
		foreach (QAction* action, actions)
			action->setEnabled(true);

		//*******************************
		// 处理 发送到 菜单


		//*******************************
		// 处理 所有上下文菜单的 使能 状态
		Node *node = static_cast<Node*>(index.internalPointer());
		QFileInfo fileInfo(node->filePath);
		if (fileInfo.isDir() && node->fileName == tr("..")) {
			foreach (QAction* action, actions)
				action->setEnabled(false);
		} else if (true/*fileInfo.isFile()*/) {
			if (!fileInfo.isWritable()) {
				editAction->setEnabled(false);
				delAction->setEnabled(false);
				renameAction->setEnabled(false);
			}
			if (!fileInfo.isReadable()) {
				readAction->setEnabled(false);
				downloadAction->setEnabled(false);
				queueAction->setEnabled(false);
			}
			if (currentDirPathUrl() == QDir::separator()) {
				dotdotAction->setEnabled(false);
			} else {
				DirTreeModel *r = static_cast<DirTreeModel*>(remoteDirTreeView->model());
				Node *dotdotNode = static_cast<Node*>(r->index(0, 0).internalPointer());
				dotdotAction->setEnabled(dotdotNode->fileName == tr(".."));
			}
			if (!isConnected() || ftpClient->hasPendingCommands()) {
				foreach (QAction* action, actions)
					action->setEnabled(false);
			}
		}

		contextMenu->exec(QCursor::pos());
	}
}

void RemoteDirWidget::dotdot()
{
	DirTreeModel *d = static_cast<DirTreeModel*>(remoteDirTreeView->model());
	d->setRootPath(currentDirPath() + QDir::separator() + tr(".."));
	remoteDirTreeView->reset();
	remoteDirTreeView->resizeColumnToContents(0);
	
	if (currentDirPathUrl() == QDir::separator())
		dotdotDirToolButton->setEnabled(false);
	else {
		Node *dotdotNode = static_cast<Node*>(d->index(0, 0).internalPointer());
		dotdotDirToolButton->setEnabled(dotdotNode->fileName == tr(".."));
	}
}

void RemoteDirWidget::queue()
{

}

void RemoteDirWidget::refresh()
{
	QString curDirPathUrl = currentDirPathUrl();
	listDirectoryFiles(curDirPathUrl);
}

void RemoteDirWidget::edit()
{

}

void RemoteDirWidget::read()
{

}

void RemoteDirWidget::changePermission()
{

}

void RemoteDirWidget::del()
{

}

void RemoteDirWidget::rename()
{

}

void RemoteDirWidget::newDir()
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
	currentCommand = CMD_MKDIR;
	dir.mkdir(dirName);
	ftpClient->mkdir(dirName);
	//refresh();
	return ;
}

void RemoteDirWidget::property()
{

}

void RemoteDirWidget::ftpCommandStarted(int)
{
	QFtp::Command command = ftpClient->currentCommand();
    if (command == QFtp::ConnectToHost) {
        writeLog(tr("正在解析地址 %1, 端口 %2").arg(
			urlAddress.host()).arg(urlAddress.port()));
    } else if (command == QFtp::Login) {
		if (urlAddress.userName().isEmpty()) {
			writeLog(tr("正在连接到服务器...(%1)").arg(tr("匿名")));
		} else {
			writeLog(tr("正在连接到服务器...(USER %1, PASS (%2))").arg(
			urlAddress.userName()).arg(tr("隐藏")));
		}
    } else if (command == QFtp::Close) {
    } else if (command == QFtp::List) {
    } else if (command == QFtp::Cd) {
    } else if (command == QFtp::Get) {
    } else if (command == QFtp::Put) {
    } else if (command == QFtp::Remove) {
    } else if (command == QFtp::Mkdir) {
    } else if (command == QFtp::Rmdir) {
    } else if (command == QFtp::Rename) {
    }
}

void RemoteDirWidget::ftpStateChanged(int state)
{
	QFtp::Command cmd = ftpClient->currentCommand();
	//*******************************
	// 超时已被自动断开连接
	if (/*state == QFtp::Unconnected && */cmd == QFtp::None) {
		writeLog(tr("已从服务器断开连接"));
		delDir(cacheDir);
	}
}

void RemoteDirWidget::ftpCommandFinished(int,bool error)
{
	QFtp::Command command = ftpClient->currentCommand();
    if (command == QFtp::ConnectToHost) {
        if (!error) {
            writeLog(tr("已连接到服务器, 正在等待响应..."));
			ftpClient->login(urlAddress.userName(), urlAddress.password());
        } else {
            writeLog(tr("不能连接到服务器(主机未找到: %1)").arg(urlAddress.host()));
			if (ftpClient->state() != QFtp::Unconnected) {
				ftpClient->close();
			}
        }
    } else if (command == QFtp::Login) {
        if (!error) {
            writeLog(tr("连接成功(登录成功)"));
			QString path = urlAddress.path();
			if (path.isEmpty()) {
				path = QDir::separator();
			}
            cacheDir = tr("cache") + QDir::separator() + urlAddress.host();
			listDirectoryFiles(path);

			connectButton->setText(tr("断开"));
			connectButton->setEnabled(true);
			/*dotdotDirToolButton->setEnabled(false);*/
			refreshDirToolButton->setEnabled(true);
			remoteDirComboBox->setEnabled(true);

			//*******************************
			// 设置ComboBox目录树显示
// 			remoteDirFileSystemModel->setRootPath()
// 			remoteDirComboBox->setModel(remoteDirFileSystemModel);
// 			remoteDirComboBox->setView(remoteDirComboTreeView);
        } else {
            writeLog(tr("连接失败(登录失败), 用户名或密码错误!")/* + ftpClient->errorString()*/);
			if (ftpClient->state() != QFtp::Unconnected) {
				ftpClient->close();
			}
        }
    } else if (command == QFtp::Close) {
        if (!error) {
            writeLog(tr("已从服务器断开连接"));
            delDir(cacheDir);
        } else {
            writeLog(tr("无法断开连接, ") + ftpClient->errorString());
        }
    } else if (command == QFtp::List) {
    } else if (command == QFtp::Cd) {
    } else if (command == QFtp::Get) {
    } else if (command == QFtp::Put) {
    } else if (command == QFtp::Remove) {
    } else if (command == QFtp::Mkdir) {
    } else if (command == QFtp::Rmdir) {
    } else if (command == QFtp::Rename) {
    }

	emit ftpCommandDone(command, error);
}

// bool RemoteDirWidget::delDir(const QString &dirPath)
// {
//     if (dirPath.isEmpty()) {
//         return false;
//     }
//     QDir dir(dirPath);
//     if (!dir.exists()) {
//         return true;
//     }
//     foreach (QFileInfo fileInfo, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
//         QString fileName = fileInfo.fileName();
//         if (fileInfo.isDir()) {
//             delDir(fileInfo.absoluteFilePath());
//         } else {
//             fileInfo.dir().remove(fileInfo.fileName());
//         }
//     }
//     return QDir().rmdir(dirPath);
// }

QString RemoteDirWidget::url(const QString &str) const
{
	QString s = str.mid(str.indexOf(urlAddress.host()) + urlAddress.host().length());
	return s == "" ? QDir::separator() : s;
}
