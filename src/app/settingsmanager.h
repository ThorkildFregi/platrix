#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QIODevice>
#include <QStandardPaths>
#include <QDir>
#include <QString>

class SettingsManager : public QObject
{
    Q_OBJECT

signals:
    void settingChanged(const QString &key, const QVariant &value);

public:
    QMap<QString, QVariant> settings;

    static SettingsManager& instance() 
    {
        static SettingsManager inst;
        return inst;
    }

    void init()
    {
        loadDefaults();
        loadSettings();
    }

    QVariant get(const QString &key) const {
        return settings.value(key);
    }

    void set(const QString &key, const QVariant &value) 
    {
        if (settings[key] != value) {
            settings[key] = value;
            emit settingChanged(key, value);

            saveSettings();
        }
    }

private:
    QString getSettingsPath() {
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        QDir dir(path);
        dir.mkpath(path);
        return path + "/settings.json";
    }

    void loadDefaults() {
        settings["fontSize"] = 14;
        settings["tabSize"] = 4;
        settings["darkTheme"] = true;
    }

    void loadSettings()
    {
        QFile file(getSettingsPath());

        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            QJsonObject json = doc.object();
            
            for (auto it = settings.begin(); it != settings.end(); ++it) {
                QString key = it.key();
                QVariant value = it.value();

                if (json.contains(key)) {
                    settings[key] = json[key].toVariant();
                }
            }

            file.close();
        }
    }

    void saveSettings()
    {
        QFile file(getSettingsPath());

        if (file.open(QIODevice::WriteOnly)) {
            QJsonObject json;

            for (auto it = settings.begin(); it != settings.end(); ++it) {
                json.insert(it.key(), QJsonValue::fromVariant(it.value()));
            }

            QJsonDocument doc(json);
            
            file.write(doc.toJson());
            file.close();
        }
    }
};

#endif