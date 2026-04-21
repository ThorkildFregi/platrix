#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QFileSystemWatcher>

#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>

#include <QMessageBox>

#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject *parent) : QObject(parent) 
{
    fileSystemWatcher = new QFileSystemWatcher();
    fileSystemWatcher->addPath(getSettingsPath());

    connect(fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path) {
        this->loadSettings();
        fileSystemWatcher->addPath(path);
    });
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
    if (!file.open(QIODevice::ReadOnly)) {
        saveSettings();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject json = doc.object();

    for (QString key : json.keys()) {
        if (json[key].isDouble()) {
            settings[key] = json[key].toInt(); 
        } else {
            settings[key] = json[key].toVariant();
        }
    }
    
    file.close();
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