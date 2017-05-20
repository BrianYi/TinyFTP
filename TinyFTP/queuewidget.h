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
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

class QueueWidget : public QDockWidget
{
	Q_OBJECT

public:
	QueueWidget(const QString & title, QWidget * parent = 0);
	~QueueWidget();
	void addTask(Task *task);
    QTreeWidgetItem *findItem(qint64 taskId);
    public slots:
        void updateProgressValue(qint64 done, qint64 total);
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
    void setObject(QObject *obj);
    QObject *getObject();
private:
	FTPClient *idleFtpClient();
	Task *pendingTask();
	QQueue<Task*> tasksQueue;
	QList<FTPClient*> ftpClients;
	QMutex tasksQueueMutex;
	QMutex ftpClientsMutex;
	bool isStop;
	TinyFTP *parentTinyFTP;
    QObject *object;
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

class Task : public QObject
{
    Q_OBJECT
public:
	Task(QObject *parent = 0);
	QString taskName() const;
	qint64 taskId() const;
    QObject *parent();  // RemoteDirWidget
	TaskType taskType() const;
	TaskStatus taskStatus() const;
	TaskData taskData() const;
/*    QObject *taskObject();*/
	void setTaskName(const QString &name);
	void setTaskType(TaskType type);
	void setTaskStatus(TaskStatus status);
	void setTaskData(TaskData data);
 /*   void setObject(QObject *obj);*/
//     public slots:
//         void updateProgressStatus(qint64, qint64);
protected:
	TaskType type;
	TaskStatus status;
	TaskData data;
	/*QObject *object;*/
private:
    QString name;
    qint64 id;
    QMutex mutex;
    static qint64 sId;
    static QMutex sMutex;
	QObject *parentObject;
};
#endif // QUEUEWIDGET_H
