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
#include <common/metawearwrapperbase.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),m_temporaryData(new QTemporaryDir()),m_triggerSingleShot(),m_triggerTime(0),m_updateTriggerTimer(),m_deviceIndex(0){
    ui->setupUi(this);

//    connect(this,&MainWindow::onConnectedDevices,m_deviceSelectDialog,&DeviceSelectDialog::updateDeviceBlackList);

    connect(ui->actionAddDevice, &QAction::triggered, this,[=](){
		DeviceSelectDialog dialog;
		connect(&dialog, &DeviceSelectDialog::onBluetoothDeviceAccepted, this, &MainWindow::registerDevice);

		QList<QBluetoothDeviceInfo> devices;
		connectedDevices(devices);
		dialog.updateDeviceBlackList(devices);

		dialog.exec();
        //m_deviceSelectDialog->show();
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



SensorPanel* MainWindow::registerDevice(const QBluetoothDeviceInfo &info) {
    m_deviceIndex++;

    QBluetoothHostInfo host;
    if(QBluetoothLocalDevice::allDevices().length() > 0 ){
        QList<QBluetoothHostInfo> hosts = QBluetoothLocalDevice::allDevices();
        host = hosts[(m_deviceIndex % hosts.length())];
    }

    SensorPanel* panel = new SensorPanel(host,info,this);
    connect(panel->getMetwareWrapper(),&MetawearWrapperBase::latestEpoch,this,[=](qint64 epoch){
        if(panel->getOffset() == 0){
            for(int x = 0; x < this->ui->sensorContainer->count();x++){
                SensorPanel* p = static_cast<SensorPanel*>(this->ui->sensorContainer->itemAt(x)->widget());
                p->setOffset(epoch);
                p->clearPlots();
            }
        }
    });
    ui->sensorContainer->addWidget(panel);
    return panel;
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


void MainWindow::connectedDevices(QList<QBluetoothDeviceInfo>& devices) {
	QObjectList children = ui->sensorContainer->children();
	for (int i = 0; i < ui->sensorContainer->count(); ++i) {
		SensorPanel* panel = dynamic_cast<SensorPanel*>(ui->sensorContainer->itemAt(i)->widget());
		devices.append(panel->getDeviceInfo());
	}
}
