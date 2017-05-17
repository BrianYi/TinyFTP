#pragma once

#include <QtGui>
#include <Windows.h>	// 使用一些Qt没有的功能时需要

enum TaskType 
{
    None,
    Upload,
    Download
};

enum TaskState 
{
    None,
    Pending,
    Abort
};

struct Task 
{
    TaskType type;
    QIcon icon;
    QString fileName;
    QString fileSize;
    QString host;
    QString sourUrl;
    QString destUrl;
    TaskState state;
    QFtp ftp;
    QUrl url;
};

QString encoded(const QString &str);
QString decoded(const QString &str);
bool delDir(const QString &dirPath);
