#include <QApplication>

#include <QScreen>
#include <QGuiApplication>

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

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    int height = screenGeometry.height();
    int width = screenGeometry.width();

    window.resize(width, height);

    window.show();

    return app.exec();
}