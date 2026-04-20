#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("avandal");
    QCoreApplication::setApplicationName("platrix");
    
    MainWindow window;
    window.show();

    return app.exec();
}