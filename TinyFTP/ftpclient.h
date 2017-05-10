// #ifndef FTPCLIENT_H
// #define FTPCLIENT_H
// 
// #include <QFtp>
// #include "logthread.h"
// 
// class RemoteDirWidget;
// class FTPClient : public QFtp
// {
//     Q_OBJECT
// 
// public:
// 	enum Command { 
// 		CMD_LIST,		// ÏÔÊ¾ÎÄ¼þ
// 		CMD_DOWNLOAD,
// 		CMD_UPLOAD,
// 		CMD_QUEUE,
// 		CMD_EDIT,
// 		CMD_READ,
// 		CMD_EXEC,
// 		CMD_DEL,
// 		CMD_RENAME,
// 		CMD_PROPERTY
// 	};
//     FTPClient(QObject *parent = 0);
//     ~FTPClient();
//     int	cd( const QString & dir);
//     int	close();
//     int	connectToHost(const QString & host, quint16 port = 21);
//     int	get(const QString & file, QIODevice * dev = 0, TransferType type = Binary);
//     int	list(const QString & dir = QString());
//     int	login(const QString & user = QString(), const QString & password = QString());
//     int	mkdir(const QString & dir);
//     int	put(QIODevice * dev, const QString & file, TransferType type = Binary);
//     int	remove(const QString & file);
//     int	rename(const QString & oldname, const QString & newname);
//     int	rmdir(const QString & dir);
// //     QString encoded(const QString &str);
// //     QString decoded(const QString &str);
// //     public slots:
// //         void stateChanged(int state);
// // private:
// //     RemoteDirWidget *parentWidget;
// };
// 
// #endif // FTPCLIENT_H
