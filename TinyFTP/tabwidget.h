#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QtGui>
class TabBar;
class TinyFTP;
class TabWidget : public QTabWidget
{
	Q_OBJECT

public:
	TabWidget(QWidget *parent);
	~TabWidget();
    void setEnableMutiTab(bool);
	public slots:
		void newTab();
		void closeTab();
		void closeOtherTab();
private:
    QTabBar *oldTabBar;
	TabBar *newTabBar;
	TinyFTP *parentTinyFtp;
};

class TabBar : public QTabBar
{
	Q_OBJECT
public:
	TabBar(QWidget *parent);
	~TabBar();
	void mousePressEvent(QMouseEvent *event);
private:
	QMenu *tabMenu;
	QAction *newTabAction;
	QAction *closeTabAction;
	QAction *closeOtherTabAction;
	TabWidget *parentTabWidget;
};

#endif // TABWIDGET_H
