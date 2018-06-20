#include <QApplication>
#include <QQmlApplicationEngine>
#include "gui/mainwindow.h"
#include <QQmlContext>
#include "gui/device_wizard/devicewizard.h"

int main(int argc, char *argv[]){
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("MainWindow",new MainWindow);

    qmlRegisterType<DeviceWizard>("org.grail",1,0,"DeviceWizard");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;



    return app.exec();
}
