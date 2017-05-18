#ifndef QUEUEWIDGET_H
#define QUEUEWIDGET_H

#include <QtGui>
#include <QItemDelegate>
#include <QMutex>
#include <QMutexLocker>
#include <QQueue>
#include "common.h"
class Task;
class FTPClient;
class TaskThread;
class TinyFTP;
class QueueDelegate : public QItemDelegate
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
	void addTask(const Task &task);
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
	void addTask(const Task &task);
	void run();
	void stop();
	bool isRunning();
private:
	FTPClient *idleFtpClient();
	QQueue<Task> tasksQueue;
	QList<FTPClient*> ftpClients;
	QMutex tasksQueueMutex;
	QMutex ftpClientsMutex;
	bool isStop;
	TinyFTP *parentTinyFTP;
};

class Task
{
public:
	Task();
	QString taskName() const;
	qint64 taskId() const;
    TaskType type;
    QIcon icon;
    QString fileName;
    qint64 fileSize;
    TaskState state;
    QUrl urlAddress;
	QString downloadRemoteDirPathUrl;
	QString downloadLocalDirPath;
	QString uploadRemoteDirPathUrl;
	QString uploadLocalDirPath;
private:
	QString objName;
	qint64 objId;
	static qint64 id;
	static QMutex mutex;
};
#endif // QUEUEWIDGET_H
