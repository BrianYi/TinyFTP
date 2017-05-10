#include "tinyftp.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("gbk"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("gbk"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("gbk"));

	TinyFTP w;
	w.show();
	return a.exec();
}
