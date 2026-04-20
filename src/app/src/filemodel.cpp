#include <QFileSystemModel>

#include "filemodel.h"

FileModel::FileModel(QObject *parent) : QFileSystemModel(parent) 
{
}

Qt::ItemFlags FileModel::flags(const QModelIndexList &selected) const {
    Qt::ItemFlags combinedFlags = Qt::NoItemFlags;

    if (selected.isEmpty()) {
        return QFileSystemModel::flags(QModelIndex());
    }

    for (const QModelIndex &index : selected) {
        Qt::ItemFlags f = QFileSystemModel::flags(index);
        if (index.isValid()) {
            combinedFlags |= (f | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
        } else {
            combinedFlags |= (f | Qt::ItemIsDropEnabled);
        }
    }

    return combinedFlags;
}

Qt::DropActions FileModel::supportedDropActions() const {
    return Qt::CopyAction | Qt::MoveAction;
}