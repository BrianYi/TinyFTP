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
	if (!dir.exists() || 
		dir.dirName() == QObject::tr("..") || 
		dir.dirName() == QObject::tr(".")) {
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

QString fileSizeUnitTranslator(qint64 size)
{
	qreal fileSize = size;
	int level = fileSizeType_Byte;
	QString sizeInfo = "";
	while (qFloor(fileSize / 1024)) {
		fileSize /= 1024.0;
		level++;
		if (level >= fileSizeType_GigaByte)
			break;
	}

	fileSize = QString::number(fileSize, 'f', 2).toDouble();
	if (level == fileSizeType_Byte) {
		sizeInfo = QObject::tr("%1 B").arg(fileSize);
	} else if (level == fileSizeType_KiloByte) {
		sizeInfo = QObject::tr("%1 KB").arg(fileSize);
	} else if (level == fileSizeType_MegaByte) {
		sizeInfo = QObject::tr("%1 MB").arg(fileSize);
	} else if (level >= fileSizeType_GigaByte) {
		sizeInfo = QObject::tr("%1 GB").arg(fileSize);
	}
	return sizeInfo;
}
