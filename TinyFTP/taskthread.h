#ifndef TASKTHREAD_H
#define TASKTHREAD_H

#include <QThread>
#include <QQueue>

class TaskThread : public QThread
{
    Q_OBJECT
public:
    TaskThread(QObject *parent);
    ~TaskThread();

private:
    QQueue
};

#endif // TASKTHREAD_H
