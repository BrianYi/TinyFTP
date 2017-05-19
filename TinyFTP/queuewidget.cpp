#include "queuewidget.h"
#include "ftpclient.h"
#include "tinyftp.h"
#include "remotedirwidget.h"
#include "localdirwidget.h"

qint64 Task::sId = 0;
QMutex Task::sMutex;

QueueWidget::QueueWidget(const QString &title, QWidget * parent/* = 0*/)
	: QDockWidget(title, parent)
{
	tabWidget = new QTabWidget(parent);
	queueTreeWidget = new QTreeWidget(parent);
	queueDelegate = new QueueDelegate(queueTreeWidget);
	taskThread = new TaskThread(parent);
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
	//queueTreeWidget->openPersistentEditor(item, 7);
	taskThread->addTask(task);
	if (!taskThread->isRunning()) {
		taskThread->start();
	}
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

TaskThread::TaskThread(QObject *parent /*= 0*/)
	: QThread(parent)
{
	parentTinyFTP = static_cast<TinyFTP*>(parent);
	isStop = true;
	for (int i = 0; i < TASK_THREAD_MAX_FTP_CLIENT; i++) {
		ftpClients.append(new FTPClient(parentTinyFTP));
	}
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
			connect(ftpClient, SIGNAL(ftpMsg(const QString &)), task->parent(), SLOT(writeLog(const QString &)));
			connect(ftpClient, SIGNAL(refreshLocalDirWidget()), parentTinyFTP->localCurrentWidget(), SLOT(refresh()));
			connect(ftpClient, SIGNAL(refreshRemoteDirWidget()), task->parent(), SLOT(refresh()));

			//*******************************
			// 处理Task
			ftpClient->setCurrentTask(task);
			//ftpClient->sendMsg(tr(">>>>>>>>>>>开始任务[%1]<<<<<<<<<<<").arg(task->taskName()));
			ftpClient->connectToHost(taskData.urlAddress.host(), taskData.urlAddress.port());
			ftpClient->login(taskData.urlAddress.userName(), taskData.urlAddress.password());
			/*sleep(3);*/
			if (task->taskType() == taskType_Download) {
				// 					ftpClient->sendMsg(tr("开始下载 %1 到 %2").arg(taskData.downloadRemoteDirPathUrl + 
				// 						tr("/") + taskData.fileName).arg(
				// 						taskData.downloadLocalDirPath + tr("/") + taskData.fileName));
				ftpClient->download(taskData.downloadRemoteDirPathUrl, taskData.downloadLocalDirPath,
					taskData.fileName, taskData.isDir);
			} else if (task->taskType() == taskType_Upload) {
				// 					ftpClient->sendMsg(tr("开始上传 %1 到 %2").arg(taskData.uploadRemoteDirPathUrl + 
				// 						tr("/") + taskData.fileName).arg(
				// 						taskData.uploadLocalDirPath + tr("/") + taskData.fileName));
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

Task::Task(QWidget *parent/* = 0*/)
    : parentWidget(parent)
{
	{
		QMutexLocker locker(&mutex);
		id = ++sId;
	}
    parentWidget = static_cast<RemoteDirWidget*>(parent);
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

QWidget * Task::parent()
{
    return parentWidget;
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
