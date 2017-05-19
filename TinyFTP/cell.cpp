#include "cell.h"

Cell::Cell(int type /*= Type*/)
	: QTreeWidgetItem(type)
{

}

Cell::Cell(QTreeWidget * parent, int type /*= Type */)
	: QTreeWidgetItem(parent, type)
{

}

Cell::~Cell()
{

}
