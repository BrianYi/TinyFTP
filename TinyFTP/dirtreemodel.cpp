#include "dirtreemodel.h"

DirTreeModel::DirTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
	rootNode = 0;
}

DirTreeModel::~DirTreeModel()
{
    if (rootNode) {
        delete rootNode;
    }
}

int DirTreeModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	if (!rootNode || parent.column() > 0) {
		return 0;
	}
    return rootNode->children.count();
}

int DirTreeModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return 4;
}

QVariant DirTreeModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid() || !rootNode || rootNode->children.isEmpty() || index.row() >= rootNode->children.count()) {
        return QVariant();
    }

    Node *fileNode = static_cast<Node*>(index.internalPointer());
    if (index.column() == 0) {
        if (role == Qt::DecorationRole) {
            return fileNode->fileIcon;
        } else if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return fileNode->fileName;
        }
    } else if (index.column() == 1) {
        if (role == Qt::DisplayRole && !fileNode->isDir && !fileNode->isSystemLink) {
			qreal fileSize = fileNode->fileSize;
			int level = Byte;
			QString sizeInfo = "";
			while (qFloor(fileSize / 1024)) {
				fileSize /= 1024.0;
				level++;
				if (level >= GigaByte)
					break;
			}
			
			fileSize = QString::number(fileSize, 'f', 2).toDouble();
			if (level == Byte) {
				sizeInfo = tr("%1 B").arg(fileSize);
			} else if (level == KiloByte) {
				sizeInfo = tr("%1 KB").arg(fileSize);
			} else if (level == MegaByte) {
				sizeInfo = tr("%1 MB").arg(fileSize);
			} else if (level >= GigaByte) {
				sizeInfo = tr("%1 GB").arg(fileSize);
			}
            return sizeInfo;
        } else if (role == Qt::TextAlignmentRole) {
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    } else if (index.column() == 2) {
        if (role == Qt::DisplayRole) {
            return fileNode->fileType;
        } else if (role == Qt::TextAlignmentRole) {
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        }
    } else if (index.column() == 3) {
        if (role == Qt::DisplayRole) {
            return fileNode->modifyDate;
        } else if (role == Qt::TextAlignmentRole) {
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
    return QVariant();
}

bool DirTreeModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole */)
{
    if (!index.isValid() || !rootNode || rootNode->children.isEmpty() || index.row() >= rootNode->children.count()) {
        return false;
    }

    Node *fileNode = static_cast<Node*>(index.internalPointer());
    if (index.column() == 0 && role == Qt::EditRole) {
        fileNode->fileName = value.toString();
        emit editingFinished(index);
    }
    return true;
}

QVariant DirTreeModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        QString title = tr("无");
        if (section == 0) {
            title = tr("名称");
        } else if (section == 1) {
            title = tr("大小");
        } else if (section == 2) {
            title = tr("类型");
        } else if (section == 3) {
            title = tr("修改时间");
        }
        return title;
    }
    return QVariant();
}

void DirTreeModel::setRootPath(const QString path)
{
    QDir rootPath(QDir::cleanPath(path));
	if (!rootPath.exists()) {
		return ;
	}

    if (rootNode) {
        delete rootNode;
    }
	{
		QFileInfo fileInfo(path);
		rootNode = new Node;
		rootNode->fileName = fileInfo.fileName();
		rootNode->fileIcon = provider.icon(fileInfo);
		rootNode->fileSize = fileInfo.size();
		rootNode->fileType = provider.type(fileInfo);
		rootNode->modifyDate = fileInfo.lastModified().toString("yyyy/MM/dd hh:mm");
		rootNode->isDir = fileInfo.isDir();
		rootNode->isSystemLink = fileInfo.isSymLink();
		rootNode->isFile = fileInfo.isFile();
		rootNode->dirPath = QDir::fromNativeSeparators(fileInfo.absolutePath());
		rootNode->filePath = QDir::fromNativeSeparators(fileInfo.absoluteFilePath());
	}

    foreach(QFileInfo fileInfo, rootPath.entryInfoList(QDir::NoDot | QDir::AllEntries, 
		QDir::DirsFirst | QDir::IgnoreCase | QDir::Name)) {
            Node *p = new Node;
            p->fileName = fileInfo.fileName();
            p->fileIcon = provider.icon(fileInfo);
            p->fileSize = fileInfo.size();
            p->fileType = provider.type(fileInfo);
            p->modifyDate = fileInfo.lastModified().toString("yyyy/MM/dd hh:mm");
            p->isDir = fileInfo.isDir();
            p->isSystemLink = fileInfo.isSymLink();
            p->isFile = fileInfo.isFile();
			p->dirPath = QDir::fromNativeSeparators(fileInfo.absolutePath());
			p->filePath = QDir::fromNativeSeparators(fileInfo.absoluteFilePath());
            rootNode->children.append(p);
    }
	sort(0, Qt::AscendingOrder);
    reset();
}

void DirTreeModel::sort(int column, Qt::SortOrder order /* = Qt::AscendingOrder */)
{
    if (!rootNode) {
        return ;
    }
	
	if (rootNode->children.count() > 1) {
		QList<Node*>::Iterator beg = rootNode->children.begin();
		if ((*beg)->fileName == tr("..")) {
			beg = beg+1;
		}
		qStableSort(beg, rootNode->children.end(), [&](const Node *fileNode1, const Node *fileNode2)->bool{
			if ((fileNode1->isDir && fileNode2->isDir) ||
				(!fileNode1->isDir && !fileNode2->isDir)) {
				if (column == 0) {	// 名称
					if (order == Qt::AscendingOrder) {
						return fileNode1->fileName.toLower() < fileNode2->fileName.toLower();
					}
					return fileNode1->fileName.toLower() > fileNode2->fileName.toLower();
				} else if (column == 1) {	// 大小
					if (order == Qt::AscendingOrder) {
						return fileNode1->fileSize < fileNode2->fileSize;
					}
					return fileNode1->fileSize > fileNode2->fileSize;
				} else if (column == 2) {	// 类型
					if (order == Qt::AscendingOrder) {
						return fileNode1->fileType < fileNode2->fileType;
					}
					return fileNode1->fileType > fileNode2->fileType;
				} else if (column == 3) {	// 修改时间
					if (order == Qt::AscendingOrder) {
						return fileNode1->modifyDate < fileNode2->modifyDate;
					}
					return fileNode1->modifyDate > fileNode2->modifyDate;
				}
			}
			return false;
		});
		reset();
	}
}

QDir DirTreeModel::currentDir(bool *ok/* = 0*/) const
{
	if (!rootNode) {
		if (ok) {
			*ok = false;
		}
		return QDir();
	}
	if (ok) {
		*ok = true;
	}
	return (rootNode->isDir ? 
		rootNode->filePath : rootNode->dirPath );
}

QString DirTreeModel::currentDirPath() const
{
	if (!rootNode) {
		return QString();
	}
	return (rootNode->isDir ? 
		rootNode->filePath : rootNode->dirPath );
}

void DirTreeModel::setRootIndex(const QModelIndex &index)
{
	if (!index.isValid() || !rootNode || rootNode->children.isEmpty()) {
		return ;
	}
    
	Node *fileNode = static_cast<Node*>(index.internalPointer());
	if (fileNode->isDir) {
		setRootPath(fileNode->filePath);
	}
}

QModelIndex DirTreeModel::index(int row, int column, const QModelIndex & parent /*= QModelIndex()*/) const
{
    if (!rootNode || rootNode->children.isEmpty() || row >= rootNode->children.count() || row < 0 || parent.column() > 0) {
        return QModelIndex();
    }

    Node *childNode = 0;
    if (!parent.isValid()) {
        childNode = rootNode->children.at(row);
    } else {
        return QModelIndex();
    }
    return createIndex(row, column, childNode);
}

QModelIndex DirTreeModel::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

Qt::ItemFlags DirTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    Node *n = static_cast<Node*>(index.internalPointer());
    if (index.column() == 0 && n->fileName != tr("..")) {
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    }
    return QAbstractItemModel::flags(index);
}
