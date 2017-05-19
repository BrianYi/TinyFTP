#ifndef CELL_H
#define CELL_H

#include <QTreeWidgetItem>

class Cell : public QTreeWidgetItem
{
public:
	Cell(int type = Type);
	Cell(QTreeWidget * parent, int type = Type );
	~Cell();

private:
	
};

#endif // CELL_H
