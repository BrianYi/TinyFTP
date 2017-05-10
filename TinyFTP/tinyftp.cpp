#include "tinyftp.h"

TinyFTP::TinyFTP(QWidget *parent)
	: QMainWindow(parent)
{
	splitter = new QSplitter(Qt::Horizontal, this);
	
	userNameLabel = new QLabel(tr("用户:"), this);
	userNameLineEdit = new QLineEdit(tr("brianyi"), this);

	passwordLabel = new QLabel(tr("口令:"), this);
	passwordLineEdit = new QLineEdit(tr("123456"), this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);

	portLabel = new QLabel(tr("端口:"), this);
	portLineEdit = new QLineEdit(tr("21"), this);

	anonymousCheckBox = new QCheckBox(tr("匿名"), this);

	addressLabel = new QLabel(tr("地址:"), this);

	addressComboBox = new QComboBox(this);
	addressComboBox->setEditable(true);

	goPushButton = new QPushButton(tr("转到"), this);

	QToolBar *userInfoToolBar = addToolBar(tr("用户信息"));
	userInfoToolBar->addWidget(userNameLabel);
	userInfoToolBar->addWidget(userNameLineEdit);
	userInfoToolBar->addWidget(passwordLabel);
	userInfoToolBar->addWidget(passwordLineEdit);
	userInfoToolBar->addWidget(portLabel);
	userInfoToolBar->addWidget(portLineEdit);
	userInfoToolBar->addWidget(anonymousCheckBox);
	userInfoToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	addToolBarBreak();

	QToolBar *addressInfoToolBar = addToolBar(tr("地址信息"));
	addressInfoToolBar->addWidget(addressLabel);
	addressInfoToolBar->addWidget(addressComboBox);
	addressInfoToolBar->addWidget(goPushButton);
	addressInfoToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

	localDirTabWidget = new TabWidget(this);
    localDirTabWidget->setEnableMutiTab(false);
	localDirTabWidget->addTab(new LocalDirWidget(this), tr("本地"));

	remoteDirTabWidget = new TabWidget(this);
	remoteDirTabWidget->setEnableMutiTab(true);
    remoteDirTabWidget->addTab(new RemoteDirWidget(this), tr(" "));

	/*remoteDirTabWidget->currentWidget()->setEnabled(false);*/

	ftpStatusBar = statusBar();

	splitter->addWidget(localDirTabWidget);
	splitter->addWidget(remoteDirTabWidget);
//     splitter->setStretchFactor(0, 1);
// 	splitter->setStretchFactor(1, 1);
	setCentralWidget(splitter);

	readSettings();

    addressComboBox->addItems(addressList);

	setWindowTitle(tr("TinyFTP"));

    connect(goPushButton, SIGNAL(clicked()), 
		this, SLOT(connectToFTPServer()));
    connect(anonymousCheckBox, SIGNAL(stateChanged(int)), 
		this, SLOT(anonymous(int)));
// 	connect(remoteDirTabWidget->currentWidget(), SIGNAL(ftpCommandDone(QFtp::Command, bool)), 
// 		this, SLOT(ftpCommandDone(QFtp::Command, bool)));
}

TinyFTP::~TinyFTP()
{

}

void TinyFTP::writeSettings()
{
	QSettings settings(tr("MyQt4Projects"), tr("TinyFTP"));
    //settings.beginGroup("TinyFTP");
	settings.setValue("geometry", saveGeometry());
    settings.setValue("addressList", addressList);
//     settings.endGroup();
//     settings.beginGroup("localDirTabWidget");
//     settings.setValue("geometry", localDirTabWidget->saveGeometry());
//     settings.endGroup();
//     settings.beginGroup("remoteDirTabWidget");
//     settings.setValue("geometry", remoteDirTabWidget->saveGeometry());
//     settings.endGroup();
}

void TinyFTP::readSettings()
{
	QSettings settings(tr("MyQt4Projects"), tr("TinyFTP"));
    //settings.beginGroup("TinyFTP");
    restoreGeometry(settings.value("geometry").toByteArray());
    addressList = settings.value("addressList").toStringList();
//     settings.endGroup();
//     settings.beginGroup("localDirTabWidget");
//     localDirTabWidget->restoreGeometry(settings.value("geometry").toByteArray());
//     settings.endGroup();
//     settings.beginGroup("remoteDirTabWidget");
//     remoteDirTabWidget->restoreGeometry(settings.value("geometry").toByteArray());
//     settings.endGroup();
}

void TinyFTP::closeEvent(QCloseEvent *event)
{
	writeSettings();
}

void TinyFTP::connectToFTPServer()
{
    if (!okToConnectToFTPServer()) {
        return ;
    }

    QString port = portLineEdit->text();
    QString address = addressComboBox->currentText();
    QString username = userNameLineEdit->text();
    QString password = passwordLineEdit->text();
    if (addressComboBox->findText(address) == -1) {
        addressComboBox->addItem(address);
        addressList.append(address);
    }
    RemoteDirWidget *remoteDirWidget = qobject_cast<RemoteDirWidget*>(remoteDirTabWidget->currentWidget());
    if (anonymousCheckBox->isChecked()) {
        username = tr("");
        password = tr("");
    }
    remoteDirTabWidget->setTabText(remoteDirTabWidget->currentIndex(), address);
    remoteDirWidget->connectToHost(address, port, username, password);
	/*remoteDirWidget->setEnabled(true);*/
}

void TinyFTP::ftpCommandDone(QFtp::Command command, bool error)
{
	RemoteDirWidget *remoteDirWidget = static_cast<RemoteDirWidget*>(sender());
	if (command == QFtp::ConnectToHost) {
		if (!error) {
// 			remoteDirTabWidget->setTabText(remoteDirTabWidget->currentIndex(), 
// 				addressComboBox->currentText());
		} else {
			/*remoteDirWidget->setEnabled(false);*/
		}
	} else if (command == QFtp::Login) {
		if (!error) {
			/*remoteDirWidget->setEnabled(true);*/
		} 
		else {
			/*remoteDirWidget->setEnabled(false);*/
		}
	} else if (command == QFtp::Close) {
	} else if (command == QFtp::List) {
	} else if (command == QFtp::Cd) {
	} else if (command == QFtp::Get) {
	} else if (command == QFtp::Put) {
	} else if (command == QFtp::Remove) {
	} else if (command == QFtp::Mkdir) {
	} else if (command == QFtp::Rmdir) {
	} else if (command == QFtp::Rename) {
	}
}

bool TinyFTP::okToConnectToFTPServer()
{
//     if (!anonymousCheckBox->isChecked()) {
//         if (userNameLineEdit->text().isEmpty() ||
//             passwordLineEdit->text().isEmpty()) {
//                 ftpStatusBar->showMessage(tr("[*]用户名或密码不能为空!"));
//                 return false;
//         }
//     }
// 
//     if (portLineEdit->text().isEmpty() || 
//         addressComboBox->currentText().isEmpty()) {
//             ftpStatusBar->showMessage(tr("[*]端口号或地址不能为空!"));            
//             return false;
//     }

    return true;
}

void TinyFTP::anonymous(int state)
{
    if (state == Qt::Checked) {
        userNameLineEdit->setEnabled(false);
        passwordLineEdit->setEnabled(false);
    } else {
        userNameLineEdit->setEnabled(true);
        passwordLineEdit->setEnabled(true);
    }
}
