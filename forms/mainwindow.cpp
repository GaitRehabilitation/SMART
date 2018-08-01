/**
* Copyright 2018 GaitRehabilitation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "ui_mainwindow.h"

#include "forms/deviceselectdialog.h"
#include "forms/mainwindow.h"

#include "forms/sensorpanel.h"
#include <QFileDialog>
#include <QtDebug>
#include <QTextEdit>
#include <QListWidget>
#include <QMessageBox>
#include "JlCompress.h"
#include <common/metawearwrapper.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),m_temporaryData(new QTemporaryDir()),m_deviceSelectDialog(new DeviceSelectDialog(this)),m_triggerSingleShot(),m_triggerTime(0),m_updateTriggerTimer(){
    ui->setupUi(this);


    connect(m_deviceSelectDialog,&DeviceSelectDialog::onBluetoothDeviceAccepted, this,&MainWindow::registerDevice);
    connect(this,&MainWindow::onConnectedDevices,m_deviceSelectDialog,&DeviceSelectDialog::updateDeviceBlackList);

    connect(ui->actionAddDevice, &QAction::triggered, this,[=](){
        m_deviceSelectDialog->show();
    });
    connect(ui->captureButton,&QPushButton::clicked,this,&MainWindow::startCapture);
    connect(ui->stopButton,&QPushButton::clicked,this,&MainWindow::stopCapture);
    connect(&m_triggerSingleShot,&QTimer::timeout,this,&MainWindow::stopCapture);


    connect(ui->triggerButton,&QPushButton::clicked,this,[=](){
        m_triggerTime = this->ui->triggerTime->value();
        m_triggerSingleShot.start(m_triggerTime * 1000);
        m_updateTriggerTimer.start();
        startCapture();
    });
    connect(&m_updateTriggerTimer,&QTimer::timeout,this,[=](){
        this->ui->triggerTime->setValue(((float)m_triggerSingleShot.remainingTime())/1000.0);
    });

    m_triggerSingleShot.setSingleShot(true);

}



void MainWindow::registerDevice(const QBluetoothDeviceInfo &info) {
    SensorPanel* panel = new SensorPanel(info,this);
    connect(panel->getMetwareWrapper(),&MetawearWrapper::lastEpoch,this,[=](qint64 epoch){
        if(panel->getOffset() == 0){
            for(int x = 0; x < this->ui->sensorContainer->count();x++){
                SensorPanel* p = static_cast<SensorPanel*>(this->ui->sensorContainer->itemAt(x)->widget());
                p->setOffset(epoch);
                p->clearPlots();
            }
        }
    });
    ui->sensorContainer->addWidget(panel);
    updateConnectedDevices();
}

void MainWindow::startCapture()
{
    this->ui->captureButton->setEnabled(false);
    this->ui->triggerButton->setEnabled(false);
    this->ui->triggerTime->setEnabled(false);
    this->ui->stopButton->setEnabled(true);
    this->ui->description->setEnabled(false);

    if(m_temporaryData){
        delete(m_temporaryData);
        m_temporaryData = new QTemporaryDir();
    }
    for(int x = 0; x < this->ui->sensorContainer->count();x++){
        SensorPanel* panel = static_cast<SensorPanel*>(this->ui->sensorContainer->itemAt(x)->widget());
        panel->setOffset(panel->getMetwareWrapper()->getLatestEpoch());
        panel->startCapture(m_temporaryData);
    }
}

void MainWindow::stopCapture()
{
    this->ui->captureButton->setEnabled(true);
    this->ui->triggerButton->setEnabled(true);
    this->ui->triggerTime->setEnabled(true);
    this->ui->stopButton->setEnabled(false);
    this->ui->description->setEnabled(true);
    this->m_updateTriggerTimer.stop();
    this->ui->triggerTime->setValue(m_triggerTime);

    for(int x = 0; x < this->ui->sensorContainer->count();x++){
        SensorPanel* panel = static_cast<SensorPanel*>(this->ui->sensorContainer->itemAt(x)->widget());
        panel->stopCapture();
    }
    QFile descriptionFile(m_temporaryData->filePath("README"));
    if(descriptionFile.open(QIODevice::ReadWrite)){
        QTextStream stream(&descriptionFile);
        stream << this->ui->description->toPlainText();
    }

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Data"),"",tr("All Files (*)"));
    if (fileName.isEmpty())
        return;
    else {
        if(!fileName.endsWith(".zip")){
            fileName.append(".zip");
        }
        if(!JlCompress::compressDir(fileName,m_temporaryData->path(),true)){
            QMessageBox::information(this,tr("Error"),"Failed to save data.");
        }
    }

    this->ui->description->clear();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::updateConnectedDevices()
{
    QList<QBluetoothDeviceInfo> devices;
    QObjectList children =  ui->sensorContainer->children();
    for(int i = 0; i < ui->sensorContainer->count(); ++i){
        SensorPanel* panel =  dynamic_cast<SensorPanel*>(ui->sensorContainer->itemAt(i)->widget());
        devices.append(panel->getDeviceInfo());
    }
    emit onConnectedDevices(devices);
}
