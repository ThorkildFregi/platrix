#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>

#include <QMap>
#include <QString>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QFileSystemWatcher;
QT_END_NAMESPACE

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    QMap<QString, QVariant> settings;

    static SettingsManager& instance();
    void init();

    static QString getSettingsPath();

    QVariant get(const QString &key) const;
    void set(const QString &key, const QVariant &value);

signals:
    void settingChanged(const QString &key, const QVariant &value);

private:
    explicit SettingsManager(QObject *parent = nullptr);

    QFileSystemWatcher *fileSystemWatcher;

    void loadDefaults();
    void loadSettings();
    void saveSettings();
};

#endif