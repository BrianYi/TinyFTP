#include "common.h"
#include "ftpclient.h"
#include "tinyftp.h"
#include "localdirwidget.h"
#include "remotedirwidget.h"

FTPClient::FTPClient(QObject *parent)
	: QFtp(parent)
{
	parentTinyFtp = static_cast<TinyFTP*>(parent);
	connect(this, SIGNAL(listInfo(const QUrlInfo &urlInfo)), this, SLOT(ftpListInfo(const QUrlInfo &urlInfo)));
	connect(this, SIGNAL(done(bool)), this, SLOT(ftpDone(bool)));
}

FTPClient::~FTPClient()
{

}

bool FTPClient::idle()
{
	return state() == QFtp::Unconnected;
}

void FTPClient::sendMsg(const QString &msg)
{
	emit ftpMsg(msg);
}

void FTPClient::download(const QString &remoteDirPathUrl, const QString &localDirPath,
	const QString &fileName, const bool &isDir
	)
{
	currentCommand = CMD_DOWNLOAD;

	currentDownloadBaseDirPathUrl = /*path.left(lstIdx);*//*currentDirPathUrl()*/remoteDirPathUrl;
	currentDownloadRelativeDirPathUrl = /*path.mid(lstIdx);*/tr("/") + /*node->fileName*/fileName;

	// 1. 文件直接下载
	// 2. 目录递归下载
	if (/*node->isDir*/isDir) {
		pendingDownloadRelativeDirPathUrls.append(currentDownloadRelativeDirPathUrl);
		processDirectory();
	} else {
		/*LocalDirWidget *l = parentTinyFtp->localCurrentWidget();*/
		currentDownloadLocalDirPath = /*l->currentDirPath()*/localDirPath;

		QFile *file = new QFile(currentDownloadLocalDirPath + tr("/")
			+ /*node->fileName*/fileName);		

		if (!file->open(QIODevice::WriteOnly)) {
			emit ftpMsg(tr("Warning: Cannot write file %1: %2").arg(
				/*l->currentDirPath()*/localDirPath + tr("/") + 
				file->fileName()).arg(file->errorString()));
		} else {
			this->get(encoded(/*path*/remoteDirPathUrl + tr("/") + fileName), file);
		}
		openedDownloadingFiles.append(file);
	}
}

void FTPClient::upload(const QString &remoteDirPathUrl, const QString &filePath)
{
	currentCommand = CMD_UPLOAD;
	QFileInfo fileInfo(filePath);
	currentUploadBaseDirPathUrl = /*currentDirPathUrl()*/remoteDirPathUrl;
	currentUploadRelativeDirPathUrl = tr("/") + fileInfo.fileName();
	if (fileInfo.isDir()) {
		pendingUploadRelativeDirPathUrls.append(currentUploadRelativeDirPathUrl);
		processDirectory();
	} else {
		QFile *file = new QFile(filePath);		
		if (!file->open(QIODevice::ReadOnly)) {
			emit ftpMsg(tr("Warning: Cannot read file %1: %2").arg(
				filePath).arg(file->errorString()));
		} else {
			this->cd(encoded(currentUploadBaseDirPathUrl));
			this->put(file, encoded(fileInfo.fileName()));
		}
		openedUploadingFiles.append(file);
	}
}

void FTPClient::ftpListInfo(const QUrlInfo &urlInfo)
{
	if (currentCommand == CMD_DOWNLOAD) {
		if (urlInfo.isFile()) {
			QFile *file = new QFile(currentDownloadLocalDirPath + tr("/")
				+ decoded(urlInfo.name()));

			if (!file->open(QIODevice::WriteOnly)) {
				emit ftpMsg(tr("Warning: Cannot write file %1: %2").arg(
					QDir::fromNativeSeparators(
					file->fileName())).arg(file->errorString()));
			} else {
				this->get(urlInfo.name(), file);
			}
			openedDownloadingFiles.append(file);
		} else if (urlInfo.isDir() && !urlInfo.isSymLink()) {
			pendingDownloadRelativeDirPathUrls.append(currentDownloadRelativeDirPathUrl + tr("/") + decoded(urlInfo.name()));
		}
	} /*else if (currentCommand == CMD_DEL) {
		if (urlInfo.isFile()) {
			QString dirPath = currentDelBaseDir + currentDelRelativeDir + tr("/") + urlInfo.name();
			QString path = cacheFilePath() + dirPath;
			QFile file(decoded(path));
			if (file.exists()) {
				file.remove();
			}
			this->remove(urlInfo.name());
		} else if (urlInfo.isDir() && !urlInfo.isSymLink()) {
			hasDir = true;
			pendingDelRelativeDirs.push(currentDelRelativeDir + tr("/") + decoded(urlInfo.name()));
		}
	}*/
}

void FTPClient::ftpDone(bool error)
{
	if (currentCommand == CMD_DOWNLOAD) {
		if (error) {
			emit ftpMsg(tr("Error: ") + this->errorString());
			qDeleteAll(openedDownloadingFiles);
			openedDownloadingFiles.clear();
			currentCommand = CMD_NONE;
			return ;
		} else {
			QString dirPath = currentDownloadBaseDirPathUrl + currentDownloadRelativeDirPathUrl;
			if (openedDownloadingFiles.count() == 1) {
				QFileInfo fileInfo(*openedDownloadingFiles.first());
				emit ftpMsg(tr("文件下载 %1 到 %2 完成").arg(
					dirPath + tr("/") + fileInfo.fileName()).arg(
					currentDownloadLocalDirPath + tr("/") + fileInfo.fileName()));
			} else {
				emit ftpMsg(tr("文件夹下载 %1 到 %2 完成").arg(
					dirPath).arg(currentDownloadLocalDirPath));
			}

			qDeleteAll(openedDownloadingFiles);
			openedDownloadingFiles.clear();

			processDirectory();
		}
	} else if (currentCommand == CMD_UPLOAD) {
		if (error) {
			emit ftpMsg(tr("Error: ") + this->errorString());
			qDeleteAll(openedUploadingFiles);
			openedUploadingFiles.clear();
			currentCommand = CMD_NONE;
			return ;
		} else {
			QString dirPath = currentUploadBaseDirPathUrl + currentUploadRelativeDirPathUrl;
			if (openedUploadingFiles.count() == 1) {
				QFileInfo fileInfo(*openedUploadingFiles.first());
				emit ftpMsg(tr("文件上传 %1 到 %2 完成").arg(
					currentUploadLocalDirPath + tr("/") + fileInfo.fileName()).arg(
					dirPath + tr("/") + fileInfo.fileName())
					);
			} else {
				emit ftpMsg(tr("文件夹上传 %1 到 %2 完成").arg(
					currentUploadLocalDirPath).arg(dirPath));
			}

			qDeleteAll(openedUploadingFiles);
			openedUploadingFiles.clear();

			processDirectory();
		}
	} /*else if (currentCommand == CMD_DEL) {
		if (error) {
			writeLog(tr("Error: ") + this->errorString());
			currentCommand = CMD_NONE;
			return ;
		} else {
			if (!hasDir && !pendingDelRelativeDirs.isEmpty()) {
				QString dirPath = currentDelBaseDir + pendingDelRelativeDirs.pop();
				QString delLocalDir = cacheFilePath() + dirPath;
				delDir(delLocalDir);
				this->rmdir(encoded(dirPath));
				writeLog(tr("文件夹 %1 删除完成").arg(dirPath));
			}
			processDirectory();
		}
	}*/
}

void FTPClient::processDirectory()
{
	if (currentCommand == CMD_DOWNLOAD) {
		if (pendingDownloadRelativeDirPathUrls.isEmpty()) {
			//*******************************
			// 下载完成
			emit ftpMsg(tr("所有文件已下载完成"));

			//*******************************
			// 让本地窗口进行重置，显示文件下载后的目录树
 			LocalDirWidget *l = parentTinyFtp->localCurrentWidget();
 			l->reset();
			currentCommand = CMD_NONE;
			return ;
		}
		LocalDirWidget *l = parentTinyFtp->localCurrentWidget();
		currentDownloadRelativeDirPathUrl = pendingDownloadRelativeDirPathUrls.takeFirst();
		currentDownloadLocalDirPath = l->currentDirPath() + currentDownloadRelativeDirPathUrl;
		if (QFileInfo(currentDownloadLocalDirPath).exists()) {
			//*******************************
			// 是否进行覆盖文件
		}
		QDir().mkdir(currentDownloadLocalDirPath);
		this->cd(encoded(currentDownloadBaseDirPathUrl + currentDownloadRelativeDirPathUrl));
		this->list();
	} else if (currentCommand == CMD_UPLOAD) {
		if (pendingUploadRelativeDirPathUrls.isEmpty()) {
			//*******************************
			// 下载完成
			emit ftpMsg(tr("所有文件已上传完成"));

			//*******************************
			// 让远程窗口进行重置，显示文件下载后的目录树
			RemoteDirWidget *r = parentTinyFtp->remoteCurrentWidget();
			r->reset();
			currentCommand = CMD_NONE;
			return ;
		}
		LocalDirWidget *l = parentTinyFtp->localCurrentWidget();
		currentUploadRelativeDirPathUrl = pendingUploadRelativeDirPathUrls.takeFirst();
		currentUploadLocalDirPath = l->currentDirPath() + currentUploadRelativeDirPathUrl;
		QString dirPath = currentUploadBaseDirPathUrl + currentUploadRelativeDirPathUrl;
		QString cacheFilePath = QDir().path() + currentUploadRelativeDirPathUrl;
		if (!QDir(cacheFilePath).exists()) {
			QDir().mkdir(cacheFilePath);
		}
		this->mkdir(encoded(dirPath));
		this->cd(encoded(dirPath));
		foreach (QFileInfo fileInfo, QDir(currentUploadLocalDirPath).entryInfoList(
			QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot)) {
				if (fileInfo.isDir()) {
					pendingUploadRelativeDirPathUrls.append(currentUploadRelativeDirPathUrl + tr("/") + fileInfo.fileName());
				} else {
					QFile *file = new QFile(fileInfo.absoluteFilePath());

					if (!file->open(QIODevice::ReadOnly)) {
						emit ftpMsg(tr("Warning: Cannot read file %1: %2").arg(
							QDir::fromNativeSeparators(
							file->fileName())).arg(file->errorString()));
					} else {
						this->put(file, encoded(fileInfo.fileName()));
					}
					openedUploadingFiles.append(file);
				}
		}
	} /*else if (currentCommand == CMD_DEL) {
        hasDir = false;
        if (pendingDelRelativeDirs.isEmpty()) {
			//*******************************
			// 下载完成
			writeLog(tr("所有文件已删除完成"));
            
			//*******************************
			// 让远程窗口进行重置，显示文件下载后的目录树
			RemoteDirWidget *r = parentTinyFtp->remoteCurrentWidget();
			r->reset();
			currentCommand = CMD_NONE;
			return ;
		}

		currentDelRelativeDir = pendingDelRelativeDirs.top();
        this->cd(encoded(currentDelBaseDir + currentDelRelativeDir));
        this->list();
	}*/
}
