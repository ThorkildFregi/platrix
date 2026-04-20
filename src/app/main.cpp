#include <QApplication>

#include "mainwindow.h"
#include "settingsmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("avandal");
    QCoreApplication::setApplicationName("platrix");

    auto &manager = SettingsManager::instance();
    manager.init();

    MainWindow window;
    window.applyTheme(manager.get("darkTheme"));
    window.show();

    return app.exec();
}