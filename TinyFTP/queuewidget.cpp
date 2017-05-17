#include "queuewidget.h"

QueueWidget::QueueWidget(const QString &title, QWidget * parent/* = 0*/)
	: QDockWidget(title, parent)
{
	tabWidget = new QTabWidget(this);
	queueTreeWidget = new QTreeWidget(this);
	queueTreeWidget->header()->setStretchLastSection(true);
	queueTreeWidget->setAlternatingRowColors(true);
	queueTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	queueTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	queueTreeWidget->setSortingEnabled(true);
	queueTreeWidget->setRootIsDecorated(false);
    queueTreeWidget->setHeaderLabels(QStringList() << tr("名称") << tr("大小") << tr("服务器") 
        << tr("源路径") << tr("目标路径") << tr("状态") << tr("进度") << tr("速度") << tr("剩余时间"));

	setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
	tabWidget->addTab(queueTreeWidget, tr("队列"));
	tabWidget->setTabPosition(QTabWidget::South);
	setWidget(tabWidget);
}

QueueWidget::~QueueWidget()
{

}
