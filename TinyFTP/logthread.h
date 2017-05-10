// #ifndef LOGTHREAD_H
// #define LOGTHREAD_H
// 
// #include <QThread>
// #include <QtGui>
// 
// class RemoteDirWidget;
// 
// typedef QPair<RemoteDirWidget*, QString> DataPair;
// 
// class LogThread : public QThread
// {
//     Q_OBJECT
// 
// public:
//     static LogThread& log();
//     friend LogThread& operator << (LogThread &logThread, DataPair data);
// protected:
//     void run();
//     void enqueue(DataPair data);
//     void stop();
// private:
//     LogThread(QObject *parent = 0);
//     LogThread(const LogThread &);
//     static LogThread *instance;
//     QQueue<DataPair> logQueue;
//     bool isStop;
//     QMutex mutex;
// };
// 
// #define LOGSTREAM LogThread::log()
// 
// #endif // LOGT