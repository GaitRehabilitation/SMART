#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "gui/mainwindow.h"
#include <QQmlContext>

int main(int argc, char *argv[]){
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    MainWindow lmainWindow;
    engine.rootContext()->setContextProperty("MainWindow",&lmainWindow);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
