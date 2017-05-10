#include "tabwidget.h"
#include "remotedirwidget.h"
#include "tinyftp.h"

TabWidget::TabWidget(QWidget *parent)
	: QTabWidget(parent)
{
	parentTinyFtp = reinterpret_cast<TinyFTP*>(parent);

    oldTabBar = new QTabBar(this);
	newTabBar = new TabBar(this);
}

TabWidget::~TabWidget()
{
	
}

void TabWidget::newTab()
{
	setCurrentIndex(addTab(new RemoteDirWidget(parentTinyFtp), tr(" ")));
}

void TabWidget::closeTab()
{
    delete currentWidget();
}

void TabWidget::closeOtherTab()
{
	int curIndex = currentIndex();
	for (int index = count() - 1; index >= 0; index--) {
		if (index != curIndex) {
			delete widget(index);
		}
	}
}

void TabWidget::setEnableMutiTab(bool enabled)
{
    if (enabled) {
        setTabBar(newTabBar);
    } else {
        setTabBar(oldTabBar);
    }
}

TabBar::TabBar(QWidget *parent)
{
	parentTabWidget = static_cast<TabWidget*>(parent);

	//*******************************
	// tab context menu
	tabMenu = new QMenu(this);
	newTabAction = new QAction(tr("新建标签"), this);
	closeTabAction = new QAction(tr("关闭标签"), this);
	closeOtherTabAction = new QAction(tr("关闭其他标签"), this);
	tabMenu->addAction(newTabAction);
	tabMenu->addAction(closeTabAction);
	tabMenu->addAction(closeOtherTabAction);

	//*******************************
	// slots and signals
	connect(newTabAction, SIGNAL(triggered()), parentTabWidget, SLOT(newTab()));
	connect(closeTabAction, SIGNAL(triggered()), parentTabWidget, SLOT(closeTab()));
	connect(closeOtherTabAction, SIGNAL(triggered()), parentTabWidget, SLOT(closeOtherTab()));
}

TabBar::~TabBar()
{

}

void TabBar::mousePressEvent(QMouseEvent *event)
{
	if (QApplication::mouseButtons() == Qt::RightButton) {
		foreach (QAction *action, tabMenu->actions()) {
			action->setEnabled(true);
		}
		if (parentTabWidget->count() == 1) {
			closeTabAction->setEnabled(false);
			closeOtherTabAction->setEnabled(false);
		}
		tabMenu->exec(QCursor::pos());
	} 
	QTabBar::mousePressEvent(event);
}
