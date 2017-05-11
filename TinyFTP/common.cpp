#include "common.h"

QString encoded(const QString &str) 
{
	return QString::fromLatin1(str.toUtf8());
}

QString decoded(const QString &str)
{
	return QString::fromUtf8(str.toLatin1());
}

bool delDir(const QString &dirPath)
{
	if (dirPath.isEmpty()) {
		return false;
	}
	QDir dir(dirPath);
	if (!dir.exists()) {
		return true;
	}
	foreach (QFileInfo fileInfo, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
		QString fileName = fileInfo.fileName();
		if (fileInfo.isDir()) {
			delDir(fileInfo.absoluteFilePath());
		} else {
			fileInfo.dir().remove(fileInfo.fileName());
		}
	}
	return QDir().rmdir(dirPath);
}