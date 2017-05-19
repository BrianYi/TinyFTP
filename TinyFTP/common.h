#pragma once

#include <QtGui>
#include <Windows.h>	// 使用一些Qt没有的功能时需要
#include <QFtp>

#define TASK_THREAD_MAX_FTP_CLIENT 5	// 设定任务线程最多可以支持多少个FTP连接同时进行操作

enum FileSizeType
{
	fileSizeType_Byte,
	fileSizeType_KiloByte,
	fileSizeType_MegaByte,
	fileSizeType_GigaByte
};

enum TaskType 
{
    taskType_Unknown,
    taskType_Upload,
    taskType_Download
};

enum TaskStatus
{
    taskStatus_Unknown,	taskStatus_Doing,	taskStatus_Pending,
    taskStatus_Abort,
	taskStatus_Done
};

enum FtpCommand { 
	//CMD_LIST,		// 显示文件
	CMD_NONE,
	CMD_DOWNLOAD,
	CMD_UPLOAD,
	CMD_QUEUE,
	CMD_EDIT,
	CMD_READ,
	CMD_EXEC,
	CMD_DEL,
	CMD_RENAME,
	CMD_MKDIR,
	CMD_PROPERTY
};

struct Node 
{
	Node()
	{
		isDir = false;
		isSystemLink = false;
		isFile = false;
	}
	~Node()
	{
		if (!children.isEmpty()) {
			qDeleteAll(children);
		}
	}
	QString fileName;
	QIcon   fileIcon;
	qint64  fileSize;
	QString fileType;
	QString modifyDate;
	QString dirPath;
	QString filePath;
	bool isDir;
	bool isSystemLink;
	bool isFile;
	QList<Node*> children;
};

QString encoded(const QString &str);
QString decoded(const QString &str);
bool delDir(const QString &dirPath);
QString fileSizeUnitTranslator(qint64 size);
