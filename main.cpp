#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "gui/device_wizard/devicewizard.h"
#include "forms/mainwindow.h"

int main(int argc, char *argv[]){
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    return app.exec();
}
