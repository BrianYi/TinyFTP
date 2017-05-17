#ifndef TINYFTP_H
#define TINYFTP_H

#include <QWidget>
#include <QtGui>
#include <QFtp>

class RemoteDirWidget;
class LocalDirWidget;
class TabWidget;
class QueueWidget;
class TinyFTP : public QMainWindow
{
	Q_OBJECT
public:
	friend LocalDirWidget;
	friend RemoteDirWidget;
public:
	TinyFTP(QWidget *parent = 0);
    ~TinyFTP();
	RemoteDirWidget *remoteCurrentWidget() const;
	LocalDirWidget *localCurrentWidget() const;
    public slots:
        void connectToFTPServer();
		void ftpCommandDone(QFtp::Command command, bool error);
protected:
	void writeSettings();
	void readSettings();
	void closeEvent(QCloseEvent *event);
    bool okToConnectToFTPServer();
    void resizeEvent(QResizeEvent *event);
    private slots:
        void anonymous(int state);
		void currentUsernameChanged(const QString &text);
private:
    QString trimUrl(const QString &url);
	TabWidget *remoteDirTabWidget;
	TabWidget *localDirTabWidget;
	QLabel *userNameLabel;
	QComboBox *userNameComboBox;
	QLabel *passwordLabel;
	QLineEdit *passwordLineEdit;
	QLabel *portLabel;
	QLineEdit *portLineEdit;
	QCheckBox *anonymousCheckBox;
	QLabel *addressLabel;
	QComboBox *addressComboBox;
	QPushButton *goPushButton;
	QSplitter *hSplitter;
	/*QSplitter *vSplitter;*/
	QStatusBar *ftpStatusBar;
	QMap<QString, QString> userNamePasswordMap;
	/*QStringList userNameList;*/
    QStringList addressList;
	QueueWidget *queueWidget;
};

#endif // TINYFTP_H
