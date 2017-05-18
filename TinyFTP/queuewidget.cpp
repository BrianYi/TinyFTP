#include "queuewidget.h"
#include "ftpclient.h"
#include "tinyftp.h"

qint64 Task::id = 0;
QMutex Task::mutex;

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

void QueueWidget::addTask(const Task &task)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(queueTreeWidget);
	item->setText(0, QString::number(task.taskId()));
	item->setData(1, Qt::DecorationRole, task.icon);
	item->setText(1, task.fileName);
	item->setText(2, fileSizeUnitTranslator(task.fileSize));
	item->setText(3, task.urlAddress.host());
	if (task.type == taskType_Download) {
		item->setText(4, task.downloadRemoteDirPathUrl);
		item->setText(5, task.downloadLocalDirPath);
	} else if (task.type == taskType_Upload) {
		item->setText(4, task.uploadLocalDirPath);
		item->setText(5, task.uploadRemoteDirPathUrl);
	}
	item->setText(6, tr("等待"));
	queueTreeWidget->openPersistentEditor(item, 7);
	taskThread->addTask(task);
	if (!taskThread->isRunning()) {
		taskThread->start();
	}
}

QueueDelegate::QueueDelegate(QObject *parent /*= 0*/)
	: QItemDelegate(parent)
{

}

QWidget * QueueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() == 7) {
		QProgressBar *p = new QProgressBar(parent);
		p->setValue(20);
		return p;
	}
	return QItemDelegate::createEditor(parent, option, index);
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

void TaskThread::addTask(const Task &task)
{
	QMutexLocker locker(&tasksQueueMutex);
	tasksQueue.enqueue(task);
}

void TaskThread::run()
{
	isStop = false;
	while (!isStop) {
		if (!tasksQueue.isEmpty()) {
			FTPClient *f = idleFtpClient();
			if (f) {
				Task task = tasksQueue.dequeue();
				//*******************************
				// 处理Task


			} else {
				sleep(1);
			}
		} else {
			sleep(1);
		}
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
	foreach (FTPClient *f, ftpClients) {
		if (f->idle())
			return f;
	}
	return 0;
}

Task::Task()
{
	{
		QMutexLocker locker(&mutex);
		objId = ++id;
	}
	objName = QObject::tr("Task%1").arg(objId);
}

QString Task::taskName() const
{
	return objName;
}

qint64 Task::taskId() const
{
	return objId;
}
