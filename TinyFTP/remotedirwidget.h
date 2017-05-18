#ifndef REMOTEDIRWIDGET_H
#define REMOTEDIRWIDGET_H

#include <QtGui>
#include <QFtp>
#include "common.h"

class TinyFTP;
class DirTreeView;
class DirTreeModel;
class RemoteDirTreeView;
class RemoteDirWidget : public QWidget
{
	Q_OBJECT
public:
    friend RemoteDirTreeView;
public:
	RemoteDirWidget(QWidget *parent);
	~RemoteDirWidget();
    void connectToHost(const QString &address, const QString &port, const QString &usrname = QString(), 
        const QString &pwd = QString());
	bool isConnected() const;
	void upload(const QString &filePath);
	QString currentDirPathUrl() const;
	QString currentFilePathUrl() const;
	QString currentDirPath() const;
	QString currentFilePath() const;
    QString cacheFilePath() const;
	/*static bool delDir(const QString &dirPath);*/
	void reset();
    void reconnect();  
    void disconnect();
    public slots:
        void writeLog(const QString &logData);
protected:
	void closeEvent(QCloseEvent *event);
	private slots:
        void ftpListInfo(const QUrlInfo &urlInfo);
        void ftpDone(bool error);
        void ftpCommandFinished(int,bool error);
        void ftpCommandStarted(int);
		void ftpStateChanged(int state);
		void setRootIndex(const QModelIndex &index);
		void showContextMenu(const QModelIndex &index);
        void editingFinished(const QModelIndex &index);
		void dotdot();
		void download();
		void queue();
		void refresh();
		void edit();
		void read();
		void changePermission();
		void del();
		void rename();
		void newDir();
		void property();
        void connectOrDisconnect();
signals:
    void updateLoginInfo(const QString &usrname, 
        const QString &pwd, const QString &port, 
        const QString &address, bool isanonymous);
	void ftpCommandDone(QFtp::Command command, bool error);
private:
	bool listing() const;
	void setListing(bool isDoing);
    void listDirectoryFiles(const QString &dirName = "");
	/*void download(const QString &path);*/
	void processDirectory();
	QString url(const QString &str) const;
	DirTreeModel *remoteDirTreeModel;
	RemoteDirTreeView *remoteDirTreeView;
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
    QString currentListDirPathUrl;
    QString currentListLocalDirPath;

// 	QString currentDownloadBaseDir;		// "\xxx" 开头
// 	QString currentDownloadRelativeDir;	// "\xxx" 开头
// 										// ftp 路径:currentDownloadBaseDir + currentDownloadRelativeDir
// 	QString currentDownloadLocalDir;
// 	QStringList pendingDownloadRelativeDirs;
// 
// 	QString currentUploadBaseDir;
// 	QString currentUploadRelativeDir;
// 	QString currentUploadLocalDir;
// 	QStringList pendingUploadRelativeDirs;
// 
     QString currentDelBaseDirPathUrl;
     QString currentDelRelativeDirPathUrl;
//     //QString currentDelLocalDir;
     QStack<QString> pendingDelRelativeDirPathUrls;
     bool hasDir;

    QString currentOldFileName;
    QString currentNewFileName;
// 	QMenu *tabMenu;
// 	QAction *newTabAction;
// 	QAction *closeTabAction;
// 	QAction *closeOtherTabAction;
	QMenu *contextMenu;
	QAction *dotdotAction;
	QAction *downloadAction;
	QAction *queueAction;
	QAction *refreshAction;
	QAction *sendToAction;
	QAction *editAction;
	QAction *readAction;
	QAction *changePermissionAction;
	QAction *delAction;
	QAction *renameAction;
	QAction *newDirAction;
	QAction *propertyAction;
	TinyFTP *parentTinyFtp;
	FtpCommand currentCommand;
// 	QList<QFile *> openedDownloadingFiles;
// 	QList<QFile *> openedUploadingFiles;
	bool isListing;
};

#endif // REMOTEDIRWIDGET_H
