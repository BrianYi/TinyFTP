#pragma once

#include <QtGui>
#include <Windows.h>	// 使用一些Qt没有的功能时需要

QString encoded(const QString &str);
QString decoded(const QString &str);
bool delDir(const QString &dirPath);
