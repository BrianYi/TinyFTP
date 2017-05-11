#ifndef TINYFTP_H
#define TINYFTP_H

#include <QWidget>
#include <QtGui>
#include <QFtp>

class RemoteDirWidget;
class LocalDirWidget;
class TabWidget;
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
    private slots:
        void anonymous(int state);
private:
	TabWidget *remoteDirTabWidget;
	TabWidget *localDirTabWidget;
	QLabel *userNameLabel;
	QLineEdit *userNameLineEdit;
	QLabel *passwordLabel;
	QLineEdit *passwordLineEdit;
	QLabel *portLabel;
	QLineEdit *portLineEdit;
	QCheckBox *anonymousCheckBox;
	QLabel *addressLabel;
	QComboBox *addressComboBox;
	QPushButton *goPushButton;
	QSplitter *splitter;
	QStatusBar *ftpStatusBar;
    QStringList addressList;
};

#endif // TINYFTP_H
