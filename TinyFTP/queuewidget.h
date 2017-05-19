#ifndef QUEUEWIDGET_H
#define QUEUEWIDGET_H

#include <QtGui>
#include <QStyledItemDelegate>
#include <QMutex>
#include <QMutexLocker>
#include <QQueue>
#include "common.h"
class Task;
class FTPClient;
class TaskThread;
class TinyFTP;
class RemoteDirWidget;
class QueueDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	QueueDelegate(QObject *parent = 0);
	QWidget *createEditor(QWidget *parent,
		const QStyleOptionViewItem &option,
		const QModelIndex &index) const;
};

class QueueWidget : public QDockWidget
{
	Q_OBJECT

public:
	QueueWidget(const QString & title, QWidget * parent = 0);
	~QueueWidget();
	void addTask(Task *task);
private:
	QTabWidget *tabWidget;
	QTreeWidget *queueTreeWidget;
	QueueDelegate *queueDelegate;
	TaskThread *taskThread;
};

class TaskThread : public QThread 
{
	Q_OBJECT
public:
	TaskThread(QObject *parent = 0);
	~TaskThread();
	void addTask(Task *task);
	void run();
	void stop();
	bool isRunning();
private:
	FTPClient *idleFtpClient();
	Task *pendingTask();
	QQueue<Task*> tasksQueue;
	QList<FTPClient*> ftpClients;
	QMutex tasksQueueMutex;
	QMutex ftpClientsMutex;
	bool isStop;
	TinyFTP *parentTinyFTP;
};

struct TaskData
{
	QIcon fileIcon;
	QString fileName;
	bool isDir;
	qint64 fileSize;
	QUrl urlAddress;
	QString downloadRemoteDirPathUrl;
	QString downloadLocalDirPath;
	QString uploadRemoteDirPathUrl;
	QString uploadLocalDirPath;
};

class Task
{
public:
	Task(QWidget *parent = 0);
	QString taskName() const;
	qint64 taskId() const;
    QWidget *parent();  // RemoteDirWidget
	TaskType taskType() const;
	TaskStatus taskStatus() const;
	TaskData taskData() const;
	void setTaskName(const QString &name);
	void setTaskType(TaskType type);
	void setTaskStatus(TaskStatus status);
	void setTaskData(TaskData data);
protected:
	TaskType type;
	TaskStatus status;
	TaskData data;
	QString name;
	qint64 id;
	QMutex mutex;
	static qint64 sId;
	static QMutex sMutex;
private:
	QWidget *parentWidget;
};
#endif // QUEUEWIDGET_H
