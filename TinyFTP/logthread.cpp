// #include "logthread.h"
// #include "remotedirwidget.h"
// 
// LogThread * LogThread::instance = 0;
// 
// LogThread::LogThread(QObject *parent)
//     : QThread(parent)
// {
//     isStop = false;
// }
// 
// void LogThread::run()
// {
//     while (!isStop) {
//         {
//             QMutexLocker locker(&mutex);
//             while (!logQueue.isEmpty()) {
//                 DataPair data = logQueue.dequeue();
//                 data.first->writeLog("[" + QDateTime::currentDateTime().toString("hh:mm:ss")  + "] " + data.second);
//             }
// 			msleep(100);
//         }
//     }
// }
// 
// LogThread &LogThread::log()
// {
//     if (instance == 0) {
//         instance = new LogThread;
//         instance->start();
//     }
//     return *instance;
// }
// 
// LogThread& operator<<(LogThread &logThread, DataPair data)
// {
//     if (data.first) {
//         logThread.enqueue(data);
//     }
//     return logThread;
// }
// 
// void LogThread::enqueue(DataPair data)
// {
//     QMutexLocker locker(&mutex);
//     logQueue.enqueue(data);
// }
// 
// void LogThread::stop()
// {
//     isStop = true;
// }
