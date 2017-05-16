#include "tinyftp.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("gbk"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("gbk"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("gbk"));

	QFile file(":/tinyftp.qss");
	if (!file.open(QFile::ReadOnly))
		qDebug() << file.errorString() << endl;
	app.setStyleSheet(file.readAll());

	TinyFTP w;
	w.show();
	return app.exec();
}
