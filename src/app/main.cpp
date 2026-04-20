#include <QApplication>

#include "mainwindow.h"
#include "settingsmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SettingsManager::instance().init();

    QCoreApplication::setOrganizationName("avandal");
    QCoreApplication::setApplicationName("platrix");
    
    MainWindow window;
    window.show();

    return app.exec();
}