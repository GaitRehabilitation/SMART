#include "ui_mainwindow.h"

#include "forms/deviceselectdialog.h"
#include "forms/mainwindow.h"

#include "forms/sensorpanel.h"
#include <QFileDialog>
#include <QtDebug>
#include <QListWidget>
#include <QMessageBox>
#include "JlCompress.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),m_temporaryData(new QTemporaryDir()),m_triggerSingleShot(),m_triggerTime(0),m_updateTriggerTimer(){
  ui->setupUi(this);

  connect(ui->actionAddDevice, SIGNAL(triggered()), this,
          SLOT(deviceAddWizard()));
  connect(ui->captureButton,&QPushButton::clicked,this,&MainWindow::startCapture);
  connect(ui->stopButton,&QPushButton::clicked,this,&MainWindow::stopCapture);
  m_triggerSingleShot.setSingleShot(true);
  connect(ui->triggerButton,&QPushButton::clicked,this,[=](){
      m_triggerTime = this->ui->triggerTime->value();
      m_triggerSingleShot.start(m_triggerTime * 1000);
      m_updateTriggerTimer.start();
      startCapture();
  });

  connect(&m_triggerSingleShot,SIGNAL(timeout()),this,SLOT(stopCapture()));

  connect(&m_updateTriggerTimer,&QTimer::timeout,this,[=](){
     this->ui->triggerTime->setValue(((float)m_triggerSingleShot.remainingTime())/1000.0);
  });
}

void MainWindow::deviceAddWizard() {
  DeviceSelectDialog deviceSelectDialog(this);
  connect(&deviceSelectDialog,
          SIGNAL(onBluetoothDeviceAccepted(QBluetoothDeviceInfo)), this,
          SLOT(registerDevice(QBluetoothDeviceInfo)));
  deviceSelectDialog.exec();
}

void MainWindow::registerDevice(const QBluetoothDeviceInfo &info) {
    SensorPanel* panel  =new SensorPanel(info,this);
  ui->sensorContainer->addWidget(panel);
}

void MainWindow::startCapture()
{
    this->ui->captureButton->setEnabled(false);
    this->ui->triggerButton->setEnabled(false);
    this->ui->triggerTime->setEnabled(false);
    this->ui->stopButton->setEnabled(true);


    if(m_temporaryData){
       delete(m_temporaryData);
        m_temporaryData = new QTemporaryDir();
    }
    for(int x = 0; x < this->ui->sensorContainer->count();x++){
        SensorPanel* panel = (SensorPanel*)this->ui->sensorContainer->itemAt(x);
        panel->setOffset(panel->getLatestEpoch());
        panel->startCapture(m_temporaryData);
    }
}

void MainWindow::stopCapture()
{
    this->ui->captureButton->setEnabled(true);
    this->ui->triggerButton->setEnabled(true);
    this->ui->triggerTime->setEnabled(true);
    this->ui->stopButton->setEnabled(false);
    this->m_updateTriggerTimer.stop();
    this->ui->triggerTime->setValue(m_triggerTime);

    for(int x = 0; x < this->ui->sensorContainer->count();x++){
        SensorPanel* panel = (SensorPanel*)this->ui->sensorContainer->itemAt(x);
        panel->stopCapture();
    }

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Data"),"",tr("Zip (*.zip);;All Files (*)"));
    if (fileName.isEmpty())
           return;
     else {
        if(!JlCompress::compressDir(fileName,m_temporaryData->path(),true)){
            QMessageBox::information(this,tr("Error"),"Failed to save data.");
        }
    }

}

MainWindow::~MainWindow() { delete ui; }
