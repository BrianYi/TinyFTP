#include "queuewidget.h"

QueueWidget::QueueWidget(const QString &title, QWidget * parent/* = 0*/)
	: QDockWidget(title, parent)
{
	tabWidget = new QTabWidget(this);
	queueTreeWidget = new QTreeWidget(this);

	setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
	tabWidget->addTab(queueTreeWidget, tr("╤сап"));
	tabWidget->setTabPosition(QTabWidget::South);
	setWidget(tabWidget);
}

QueueWidget::~QueueWidget()
{

}
