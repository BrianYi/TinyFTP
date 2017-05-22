#include "queuewidget.h"
#include "ftpclient.h"
#include "tinyftp.h"
#include "remotedirwidget.h"
#include "localdirwidget.h"
#include <QTreeWidgetItemIterator>

qint64 Task::sId = 0;
QMutex Task::sMutex;

QueueWidget::QueueWidget(const QString &title, QWidget * parent/* = 0*/)
	: QDockWidget(title, parent)
{
	tabWidget = new QTabWidget(parent);
	queueTreeWidget = new QTreeWidget(parent);
	queueDelegate = new QueueDelegate(queueTreeWidget);
	taskThread = new TaskThread(parent);
    taskThread->setObject(this);
	queueTreeWidget->setItemDelegateForColumn(7, queueDelegate);
	queueTreeWidget->header()->setStretchLastSection(true);
	queueTreeWidget->setAlternatingRowColors(true);
	queueTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	queueTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	queueTreeWidget->setSortingEnabled(true);
	queueTreeWidget->setRootIsDecorated(false);
    queueTreeWidget->setHeaderLabels(QStringList() << tr("TaskID") << tr("名称") << tr("大小") << tr("服务器") 
        << tr("源路径") << tr("目标路径") << tr("状态") << tr("进度") << tr("速度") << tr("剩余时间"));

	setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	setFeatures(QDockWidget::DockWidgetVerticalTitleBar | QDockWidget::DockWidgetClosable
		| QDockWidget::DockWidgetFloatable);
	tabWidget->addTab(queueTreeWidget, tr("队列"));
	tabWidget->setTabPosition(QTabWidget::South);
	setWidget(tabWidget);
}

QueueWidget::~QueueWidget()
{

}

void QueueWidget::addTask(Task *task)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(queueTreeWidget);
	TaskData taskData = task->taskData();
	item->setText(0, QString::number(task->taskId()));
	item->setData(1, Qt::DecorationRole, taskData.fileIcon);
	item->setText(1, taskData.fileName);
	item->setText(2, fileSizeUnitTranslator(taskData.fileSize));
	item->setText(3, taskData.urlAddress.host());
	if (task->taskType() == taskType_Download) {
		item->setText(4, taskData.downloadRemoteDirPathUrl);
		item->setText(5, taskData.downloadLocalDirPath);
	} else if (task->taskType() == taskType_Upload) {
		item->setText(4, taskData.uploadLocalDirPath);
		item->setText(5, taskData.uploadRemoteDirPathUrl);
	}
	item->setText(6, tr("等待"));
/*	item->setText(7, )*/
	queueTreeWidget->openPersistentEditor(item, 7);
    QProgressBar *p = static_cast<QProgressBar*>(queueTreeWidget->itemWidget(item, 7));
    p->setRange(0, taskData.fileSize);
    //item->setData(7, Qt::UserRole, 20);
   /* task->setObject(static_cast<QObject*>(p));*/
/*    QProgressBar *p = static_cast<QProgressBar*>(queueTreeWidget->itemWidget(item, 7));*/
	taskThread->addTask(task);
	if (!taskThread->isRunning()) {
		taskThread->start();
	}
}

void QueueWidget::updateProgressValue(qint64 done, qint64 total)
{
    FTPClient *ftpClient = static_cast<FTPClient*>(sender());
    Task *task = ftpClient->currentTask();
    QTreeWidgetItem *item = findItem(task->taskId());
    item->setData(7, Qt::UserRole, done);
}

QTreeWidgetItem * QueueWidget::findItem(qint64 taskId)
{
    QTreeWidgetItemIterator it(queueTreeWidget);
    while (*it) {
        if ((*it)->text(0).toLongLong() == taskId) {
            return *it;
        }
        ++it;
    }
    return 0;
}

QueueDelegate::QueueDelegate(QObject *parent /*= 0*/)
	: QStyledItemDelegate(parent)
{

}

QWidget * QueueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() == 7) {
		return new QProgressBar(parent);
	}
	return QStyledItemDelegate::createEditor(parent, option, index);
}

void QueueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == 7) {
        qint64 value = qint64(index.model()->data(index, Qt::UserRole).toLongLong());
        QProgressBar *progress = static_cast<QProgressBar*>(editor);
        progress->setValue(value);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void QueueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == 7) {
        QProgressBar *progress = static_cast<QProgressBar*>(editor);
        model->setData(index, progress->value(), Qt::UserRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

TaskThread::TaskThread(QObject *parent /*= 0*/)
	: QThread(parent)
{
	parentTinyFTP = static_cast<TinyFTP*>(parent);
	isStop = true;
	for (int i = 0; i < TASK_THREAD_MAX_FTP_CLIENT; i++) {
		ftpClients.append(new FTPClient(parentTinyFTP));
	}
    object = 0;
}

TaskThread::~TaskThread()
{
	isStop = true;
}

void TaskThread::addTask(Task *task)
{
	QMutexLocker locker(&tasksQueueMutex);
	tasksQueue.enqueue(task);
}

void TaskThread::run()
{
	isStop = false;
	while (!isStop) {
		Task *task = pendingTask();
		FTPClient *ftpClient = idleFtpClient();
		if (task && ftpClient) {
			TaskData taskData = task->taskData();
			ftpClient->disconnect(SIGNAL(ftpMsg(const QString &)));
			ftpClient->disconnect(SIGNAL(refreshLocalDirWidget()));		// 刷新本地目录树
			ftpClient->disconnect(SIGNAL(refreshRemoteDirWidget()));	// 刷新远程目录树
            ftpClient->disconnect(SIGNAL(dataTransferProgress(qint64,qint64)));
			connect(ftpClient, SIGNAL(ftpMsg(const QString &)), task->parent(), SLOT(writeLog(const QString &)));
			connect(ftpClient, SIGNAL(refreshLocalDirWidget()), parentTinyFTP->localCurrentWidget(), SLOT(refresh()));
			connect(ftpClient, SIGNAL(refreshRemoteDirWidget()), task->parent(), SLOT(refresh()));
            QueueWidget *queueWidget = static_cast<QueueWidget*>(this->getObject());
            if (queueWidget) {
                connect(ftpClient, SIGNAL(dataTransferProgress(qint64,qint64)), queueWidget, SLOT(updateProgressValue(qint64,qint64)));
            }
			//*******************************
			// 处理Task
			ftpClient->setCurrentTask(task);
			ftpClient->connectToHost(taskData.urlAddress.host(), taskData.urlAddress.port());
			ftpClient->login(taskData.urlAddress.userName(), taskData.urlAddress.password());
			if (task->taskType() == taskType_Download) {
				ftpClient->download(taskData.downloadRemoteDirPathUrl, taskData.downloadLocalDirPath,
					taskData.fileName, taskData.isDir);
			} else if (task->taskType() == taskType_Upload) {
				ftpClient->upload(taskData.uploadRemoteDirPathUrl, taskData.uploadLocalDirPath + 
					tr("/") + taskData.fileName);
			}
		} 
		sleep(1);
	}
	isStop = true;
}

void TaskThread::stop()
{
	isStop = true;
}

bool TaskThread::isRunning()
{
	return !isStop;
}

FTPClient * TaskThread::idleFtpClient()
{
	QMutexLocker locker(&ftpClientsMutex);
	foreach (FTPClient *ftpClient, ftpClients) {
		if (ftpClient->idle())
			return ftpClient;
	}
	return 0;
}

Task * TaskThread::pendingTask()
{
	QMutexLocker locker(&tasksQueueMutex);
	foreach (Task *task, tasksQueue) {
		if (task->taskStatus() == taskStatus_Pending)
			return task;
	}
	return 0;
}

void TaskThread::setObject(QObject *obj)
{
    object = obj;
}

QObject * TaskThread::getObject()
{
    return object;
}

Task::Task(QObject *parent/* = 0*/)
    : parentObject(parent)
{
	{
		QMutexLocker locker(&mutex);
		id = ++sId;
	}
    parentObject = static_cast<RemoteDirWidget*>(parent);
    /*object = 0;*/
	name = QObject::tr("Task%1").arg(id);
}

QString Task::taskName() const
{
	return name;
}

qint64 Task::taskId() const
{
	return id;
}

QObject * Task::parent()
{
    return parentObject;
}

TaskType Task::taskType() const
{
	return type;
}

TaskStatus Task::taskStatus() const
{
	return status;
}

TaskData Task::taskData() const
{
	return data;
}

// QObject * Task::taskObject()
// {
//     return object;
// }

void Task::setTaskName(const QString &name)
{
	QMutexLocker locker(&mutex);
	this->name = name;
}

void Task::setTaskType(TaskType type)
{
	QMutexLocker locker(&mutex);
	this->type = type;
}

void Task::setTaskStatus(TaskStatus status)
{
	QMutexLocker locker(&mutex);
	this->status = status;
}

void Task::setTaskData(TaskData data)
{
	QMutexLocker locker(&mutex);
	this->data = data;
}

// void Task::setObject(QObject *obj)
// {
//     QMutexLocker locker(&mutex);
//     this->object = obj;
// }

// void Task::updateProgressStatus(qint64 done, qint64 total)
// {
//     QMutexLocker locker(&mutex);
//     QProgressBar *p = static_cast<QProgressBar*>(object);
//     p->setValue(done);
//     p->reset();
// }
