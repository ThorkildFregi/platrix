#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QFileSystemModel>

class FileModel : public QFileSystemModel {
public:
    explicit FileModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndexList &selected) const;

    Qt::DropActions supportedDropActions() const override;
};

#endif