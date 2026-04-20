#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>

#include <QMap>
#include <QString>
#include <QVariant>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    QMap<QString, QVariant> settings;

    static SettingsManager& instance();

    void init();

    QVariant get(const QString &key) const;
    void set(const QString &key, const QVariant &value);

signals:
    void settingChanged(const QString &key, const QVariant &value);

private:
    explicit SettingsManager(QObject *parent = nullptr);

    QString getSettingsPath();

    void loadDefaults();
    void loadSettings();
    void saveSettings();
};

#endif