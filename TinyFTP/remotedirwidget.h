#ifndef REMOTEDIRWIDGET_H
#define REMOTEDIRWIDGET_H

#include <QtGui>
#include <QFtp>
#include "dirtreemodel.h"
#include "ftpclient.h"

class TinyFTP;
class RemoteDirWidget : public QWidget
{
	Q_OBJECT

public:
    enum Command { 
        CMD_LIST,		// ÏÔÊ¾ÎÄ¼þ
        CMD_DOWNLOAD,
        CMD_UPLOAD,
        CMD_QUEUE,
        CMD_EDIT,
        CMD_READ,
        CMD_EXEC,
        CMD_DEL,
        CMD_RENAME,
        CMD_PROPERTY
    };
	RemoteDirWidget(QWidget *parent);
	~RemoteDirWidget();
//     void setLoginInfo(const QString &port, const QString &address, 
//         const QString &usrname = QString(), const QString &pwd = QString());
    void connectToHost(const QString &address, const QString &port, const QString &usrname = QString(), 
        const QString &pwd = QString());
	bool isConnected() const;
	void upload(const QString &path);
	QString currentDirPath() const;
protected:
	void closeEvent(QCloseEvent *event);
	private slots:
        void ftpListInfo(const QUrlInfo &urlInfo);
        void ftpDone(bool error);
        void ftpCommandFinished(int,bool error);
        void ftpCommandStarted(int);
		void setRootIndex(const QModelIndex &index);
		void showContextMenu(const QModelIndex &index);
		void download();
		void queue();
		void edit();
		void read();
		void changePermission();
		void del();
		void rename();
		void property();
signals:
    void updateLoginInfo(const QString &usrname, 
        const QString &pwd, const QString &port, 
        const QString &address, bool isanonymous);
	void ftpCommandDone(QFtp::Command command, bool error);
private:
	void writeLog(const QString &logData);
    void listDirectoryFiles(const QString &dir);
	void download(const QString &path);
	void processDirectory();
    bool delDir(const QString &path);
    QString encoded(const QString &str);
    QString decoded(const QString &str);
	/*DirTreeModel *remoteDirTreeModel;*/
	QTreeView *remoteDirTreeView;
	QFileSystemModel *remoteDirFileSystemModel;
	QTreeView *remoteDirComboTreeView;
	QComboBox *remoteDirComboBox;
	QToolButton *connectButton;
	QToolButton *dotdotDirToolButton;
	QToolButton *refreshDirToolButton;
	QStatusBar *remoteDirStatusBar;
	QTextEdit *logTextEdit;
//     QString username;
//     QString password;
//     QString port;
//     QString address;
	QUrl urlAddress;
    QFtp *ftpClient;
    QFileIconProvider provider;
// 	QQueue<qint64> filesSize;
//     QStringList filesModifyDate;
    QMap<QString, QUrlInfo> filesInfoMap;
	
    QString cacheDir;
    QString currentListDir;
    QString currentListLocalDir;

	QString currentDownloadDir;
	QString currentDownloadLocalDir;
	QStringList pendingDownloadDirs;
// 	QMenu *tabMenu;
// 	QAction *newTabAction;
// 	QAction *closeTabAction;
// 	QAction *closeOtherTabAction;
	QMenu *contextMenu;
	QAction *downloadAction;
	QAction *queueAction;
	QAction *sendToAction;
	QAction *editAction;
	QAction *readAction;
	QAction *changePermissionAction;
	QAction *delAction;
	QAction *renameAction;
	QAction *propertyAction;
	TinyFTP *parentTinyFtp;
	Command currentCommand;
	QList<QFile *> openedDownloadingFiles;
};

#endif // REMOTEDIRWIDGET_H
