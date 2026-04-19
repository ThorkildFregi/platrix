#include <QFileSystemModel>

class FileModel : public QFileSystemModel {
public:
    using QFileSystemModel::QFileSystemModel;

    Qt::ItemFlags flags(const QModelIndexList &selected) const {
        for (const QModelIndex &index : selected) {
            Qt::ItemFlags defaultFlags = QFileSystemModel::flags(index);
            if (index.isValid()) {
                return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
            }
            return defaultFlags | Qt::ItemIsDropEnabled;
        }
    }

    Qt::DropActions supportedDropActions() const override {
        return Qt::CopyAction | Qt::MoveAction;
    }
};