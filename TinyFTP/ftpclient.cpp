// #include "ftpclient.h"
// #include "remotedirwidget.h"
// 
// FTPClient::FTPClient(QObject *parent)
//     : QFtp(parent)
// {
// //     parentWidget = qobject_cast<RemoteDirWidget*>(parent);
// //     connect(this, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
// }
// 
// FTPClient::~FTPClient()
// {
//     if (state() != QFtp::Unconnected) {
//         close();
//     }
// }
// 
// int FTPClient::cd(const QString & dir)
// {
//     /*parentWidget->writeLog(tr("CD \"%1\" is current directory").arg(dir));*/
//     return QFtp::cd(encoded(dir));
// }
// 
// int FTPClient::close()
// {
//     /*parentWidget->writeLog(tr("CLOSE"));*/
//     return QFtp::close();
// }
// 
// int FTPClient::connectToHost(const QString & host, quint16 port /*= 21*/)
// {
//     /*parentWidget->writeLog(tr("connect to host %1, port %2").arg(host).arg(port));*/
//     return QFtp::connectToHost(host, port);
// }
// 
// int FTPClient::get(const QString & file, QIODevice * dev /*= 0*/, TransferType type /*= Binary*/)
// {
//     /*parentWidget->writeLog(tr("GET file %1").arg(file));*/
//     return QFtp::get(encoded(file), dev, type);
// }
// 
// int FTPClient::list(const QString & dir /*= QString()*/)
// {
//     /*parentWidget->writeLog(tr("LIST %1").arg(dir));*/
//     return QFtp::list(encoded(dir));
// }
// 
// int FTPClient::login(const QString & user /*= QString()*/, const QString & password /*= QString()*/)
// {
//     /*parentWidget->writeLog(tr("LOGIN username is %1, password is (hidden)").arg(user));*/
//     return QFtp::login(encoded(user), encoded(password));
// }
// 
// int FTPClient::mkdir(const QString & dir)
// {
//     /*parentWidget->writeLog(tr("MKDIR %1").arg(dir));*/
//     return QFtp::mkdir(encoded(dir));
// }
// 
// int FTPClient::put(QIODevice * dev, const QString & file, TransferType type /*= Binary*/)
// {
//    /* parentWidget->writeLog(tr("PUT file %1").arg(file));*/
//     return QFtp::put(dev, encoded(file), type);
// }
// 
// int FTPClient::remove(const QString & file)
// {
//     /*parentWidget->writeLog(tr("REMOVE %1").arg(file));*/
//     return QFtp::remove(encoded(file));
// }
// 
// int FTPClient::rename(const QString & oldname, const QString & newname)
// {
//     /*parentWidget->writeLog(tr("RENAME %1 to %2").arg(oldname).arg(newname));*/
//     return QFtp::rename(encoded(oldname), encoded(newname));
// }
// 
// int FTPClient::rmdir(const QString & dir)
// {
//     /*parentWidget->writeLog(tr("RMDIR %1").arg(dir));*/
//     return QFtp::rmdir(encoded(dir));
// }
// 
// // void FTPClient::stateChanged(int state)
// // {
// //     QString stateInfo = tr("");
// //     switch (state)
// //     {
// //     case QFtp::Unconnected:
// //     	{
// //             stateInfo = tr("There is no connection to the host.");
// //     		break;
// //     	}
// //     case QFtp::HostLookup:
// //         {
// //             stateInfo = tr("A host name lookup is in progress.");
// //             break;
// //         }
// //     case QFtp::Connecting:
// //         {
// //             stateInfo = tr("An attempt to connect to the host is in progress.");
// //             break;
// //         }
// //     case QFtp::Connected:
// //         {
// //             stateInfo = tr("Connection to the host has been achieved.");
// //             break;
// //         }
// //     case QFtp::LoggedIn:
// //         {
// //             stateInfo = tr("Connection and user login have been achieved.");
// //             break;
// //         }
// //     case QFtp::Closing:
// //         {
// //             stateInfo = tr("The connection is closing down, but it is not yet closed.");
// //             break;
// //         }
// //     default:
// //         {
// //             stateInfo = tr("UNKNOW state!!");
// //     	    break;
// //         }
// //     }
// //     parentWidget->writeLog(stateInfo);
// // }
// 
// // QString FTPClient::encoded(const QString &str)
// // {
// //     return QString::fromLatin1(str.toUtf8());
// // }
// // 
// // QString FTPClient::decoded(const QString &str)
// // {
// //     return QString::fromUtf8(str.toLatin1());
// // }
