#ifndef DIRTABLEMODEL_H
#define DIRTABLEMODEL_H

#include <QAbstractItemModel>
#include <QtGui>

struct Node 
{
	Node()
	{
		isDir = false;
		isSystemLink = false;
		isFile = false;
	}
    ~Node()
    {
        if (!children.isEmpty()) {
            qDeleteAll(children);
        }
    }
    QString fileName;
    QIcon   fileIcon;
    qint64  fileSize;
    QString fileType;
    QString modifyDate;
    QString dirPath;
	QString filePath;
    bool isDir;
    bool isSystemLink;
    bool isFile;
    QList<Node*> children;
};

class DirTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
	enum {
		Byte,
		KiloByte,
		MegaByte,
		GigaByte
	};
    DirTreeModel(QObject *parent = 0);
    ~DirTreeModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;
    QModelIndex	index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QModelIndex	parent(const QModelIndex & index ) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
	bool insertRow(int row, const QModelIndex & parent = QModelIndex());
    void setRootPath(const QString path);
	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder );
	QDir currentDir(bool *ok = 0) const;
	QString currentDirPath() const;
signals:
    void editingFinished(const QModelIndex &index);
	public slots:
		void setRootIndex(const QModelIndex &index);
private:
 /*   QList<QPair<QFileInfo, qint64>> files;*/
    Node *rootNode;
    QFileIconProvider provider;
};

#endif // DIRTABLEMODEL_H
