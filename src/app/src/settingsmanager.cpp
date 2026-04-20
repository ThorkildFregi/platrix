#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>

#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject *parent) : QObject(parent) 
{
}

SettingsManager& SettingsManager::instance() 
{
    static SettingsManager inst;
    return inst;
}

void SettingsManager::init()
{
    loadDefaults();
    loadSettings();
}

QVariant SettingsManager::get(const QString &key) const {
    return settings.value(key);
}

void SettingsManager::set(const QString &key, const QVariant &value) 
{
    if (settings.value(key) != value) {
        settings[key] = value;
        emit settingChanged(key, value);

        saveSettings();
    }
}

QString SettingsManager::getSettingsPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(path);
    return path + "/settings.json";
}

void SettingsManager::loadDefaults() {
    settings["fontSize"] = 14;
    settings["tabSize"] = 4;
    settings["darkTheme"] = true;
}

void SettingsManager::loadSettings()
{
    QFile file(getSettingsPath());

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject json = doc.object();
        
        for (auto it = settings.begin(); it != settings.end(); ++it) {
            QString key = it.key();

            if (json.contains(key)) {
                settings[key] = json[key].toVariant();
            }
        }

        file.close();
    }
}

void SettingsManager::saveSettings()
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