#include <QFileSystemModel>

class FileModel : public QFileSystemModel {
public:
    using QFileSystemModel::QFileSystemModel;

    Qt::ItemFlags flags(const QModelIndexList &selected) const {
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

    Qt::DropActions supportedDropActions() const override {
        return Qt::CopyAction | Qt::MoveAction;
    }
};