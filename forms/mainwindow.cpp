#include "ui_mainwindow.h"

#include "forms/devicewizard.h"
#include "forms/mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent  )
    :QMainWindow(parent),
     ui(new Ui::MainWindow),
     deviceWizard(new DeviceWizard(this))
{
    ui->setupUi(this);
    connect(ui->actionAddDevice,SIGNAL(triggered()),this,SLOT(deviceAddWizard()));

}

void MainWindow::deviceAddWizard(){
    qDebug() << "Device Wizard";
   deviceWizard->show();

}

MainWindow::~MainWindow(){
    delete ui;
}

