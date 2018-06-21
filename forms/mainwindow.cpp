#include "forms/mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
MainWindow::MainWindow(QWidget *parent  )
    :QMainWindow(parent), _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    connect(_ui->actionAddDevice,SIGNAL(triggered()),this,SLOT(on_device_added()));

}

void MainWindow::on_device_added(){
    qDebug() << "Device Wizard";
}

MainWindow::~MainWindow(){
    delete _ui;
}

