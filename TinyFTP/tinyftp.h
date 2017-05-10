#ifndef TINYFTP_H
#define TINYFTP_H

#include <QWidget>
#include <QtGui>
#include "dirtreemodel.h"
#include "localdirwidget.h"
#include "remotedirwidget.h"
#include "tabwidget.h"

class TinyFTP : public QMainWindow
{
	Q_OBJECT

public:
	TabWidget *remoteDirTabWidget;
	TabWidget *localDirTabWidget;
	TinyFTP(QWidget *parent = 0);
    ~TinyFTP();
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
