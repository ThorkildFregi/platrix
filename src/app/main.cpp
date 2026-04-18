#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("avandal");
    QCoreApplication::setApplicationName("paltrix");
    
    MainWindow window;
    window.show();

    return app.exec();
}