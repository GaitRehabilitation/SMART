#include "ui_mainwindow.h"

#include "forms/devicewizard.h"
#include "forms/mainwindow.h"

#include <QtDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent  )
    :QMainWindow(parent),
     ui(new Ui::MainWindow),
     saveDialog(new QFileDialog(this))
{
    ui->setupUi(this);
    ui->save->setDisabled(true);

    connect(ui->actionAddDevice,SIGNAL(triggered()),this,SLOT(deviceAddWizard()));
}

void MainWindow::deviceAddWizard(){
    qDebug() << "Device Wizard";
   DeviceWizard deviceWizard(this);
   deviceWizard.exec();

}

MainWindow::~MainWindow(){
    delete ui;
}

