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
    : QMainWindow(parent), ui(new Ui::MainWindow),m_panels(),m_temporaryData(0) {
  ui->setupUi(this);
  ui->save->setDisabled(true);

  connect(ui->actionAddDevice, SIGNAL(triggered()), this,
          SLOT(deviceAddWizard()));
//  connect(ui->actionCapture, &QAction::triggered,
//          [=]() { this->ui->pages->setCurrentWidget(this->ui->Capture); });

//  connect(ui->actionSessions, JlCompress&QAction::triggered,
//          [=]() { this->ui->pages->setCurrentWidget(this->ui->Sessions); });

  connect(ui->captureButton,&QPushButton::clicked,this,&MainWindow::startCapture);
  connect(ui->stopButton,&QPushButton::clicked,this,&MainWindow::stopCapture);
  connect(ui->triggerButton,&QPushButton::clicked,this,[=](){
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
  // created a new list widget
  QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->sensorList);

  ui->sensorList->addItem(listWidgetItem);
  SensorPanel *sensorPanel = new SensorPanel(info, this);

  // make the widget the same size as the panel
  listWidgetItem->setSizeHint(sensorPanel->sizeHint());
  ui->sensorList->setItemWidget(listWidgetItem, sensorPanel);
}

void MainWindow::startCapture()
{
    if(!m_temporaryData){
        delete m_temporaryData;
        m_temporaryData = new QTemporaryDir();
    }
    foreach (auto panel, m_panels) {
        panel->startCapture(m_temporaryData);
    }
}

void MainWindow::stopCapture()
{
    foreach (auto panel, m_panels) {
        panel->stopCapture();
    }

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Data"),"",tr("Zip (*.zip);;All Files (*)"));
    if (fileName.isEmpty())
           return;
     else {
           QFile file(fileName);
           if (!file.open(QIODevice::WriteOnly)) {
               QMessageBox::information(this, tr("Unable to open file"),
                   file.errorString());
               return;
           }

//           QuaZip zip(file);

    }

}

MainWindow::~MainWindow() { delete ui; }
