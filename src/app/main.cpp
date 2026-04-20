#include <QApplication>

#include "mainwindow.h"
#include "settingsmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto &manager = SettingsManager::instance();
    manager.init();

    QCoreApplication::setOrganizationName("avandal");
    QCoreApplication::setApplicationName("platrix");
    
    MainWindow window;
    window.applyTheme(manager.get("darkTheme"));
    window.show();

    return app.exec();
}