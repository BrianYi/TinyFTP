#ifndef QUEUEWIDGET_H
#define QUEUEWIDGET_H

#include <QtGui>

class QueueWidget : public QDockWidget
{
	Q_OBJECT

public:
	QueueWidget(const QString & title, QWidget * parent = 0);
	~QueueWidget();

private:
	QTabWidget *tabWidget;
	QTreeWidget *queueTreeWidget;
    
};

#endif // QUEUEWIDGET_H
