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

#include "forms/sensorpanel.h"
#include "ui_sensorpanel.h"

#include "common/metawearwrapper.h"
#include "metawear/core/datasignal.h"
#include "metawear/core/metawearboard.h"
#include "metawear/core/types.h"
#include "metawear/sensor/accelerometer.h"

#include "qcustomplot.h"
#include <QGraphicsLayout>
#include <QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <common/metawearwrapper.h>

SensorPanel::SensorPanel(const QBluetoothDeviceInfo &device, QWidget *parent)
    : QWidget(parent), ui(new Ui::SensorPanel),
      m_settingUpdateTimer(this),
      m_currentDevice(device),
      m_plotUpdatetimer(),
      m_wrapper(new MetawearWrapper(device, this)),
      m_plotoffset(0),
      m_temporaryDir(nullptr),
      m_isReadyToCapture(false){
    ui->setupUi(this);

    connect(this->m_wrapper,&MetawearWrapper::onMetawareInitialized,this,&SensorPanel::metawearInitilized);
    connect(this->m_wrapper,&MetawearWrapper::disconnected,this,&SensorPanel::disconnect);
    connect(this->m_wrapper,&MetawearWrapper::connected,this,&SensorPanel::connected);

    connect(this->m_wrapper,&MetawearWrapper::disconnected,this,[=](){
        this->deleteLater();
    });
    connect(this->m_wrapper,&MetawearWrapper::onControllerError,this,[=](){
        QMessageBox messageBox;
        messageBox.critical(0,"Error",QString("Failed to connect to devce: %0").arg(device.address().toString()) );
        messageBox.setFixedSize(500,200);
        this->deleteLater();
    });
    connect(ui->remove,&QPushButton::clicked,this,[=](){
       this->deleteLater();
    });


    // watch for a text change and remove characters that can cause problems for the path
    connect(ui->sensorName,&QLineEdit::textChanged,this,[=](const QString& text){
        QString updated(text);
        if(updated.isEmpty()){
            updated = this->ui->deviceAddress->text();
        }

        //remove illegal symbols
        updated.replace(":","_");
        updated.replace(" ","_");

        updated.replace("<","");
        updated.replace(">","");
        updated.replace("/","");
        updated.replace("|","");
        updated.replace("*","");
        updated.replace("\\","");
        updated.replace(".","");
        updated.replace("?","");
        updated.replace("!","");

        ui->sensorName->setText(updated);
    });

    this->registerPlotHandlers();
    this->registerDataHandlers();

    connect(this->m_wrapper,&MetawearWrapper::onMetawareInitialized, this,[=](){
        this->m_wrapper->setAccelerationSamplerate(4.f,25.f);
        this->m_wrapper->setAccelerationCapture(true);

        this->m_wrapper->setGyroSamplerate(MBL_MW_GYRO_BMI160_RANGE_125dps,MBL_MW_GYRO_BMI160_ODR_25Hz);
        this->m_wrapper->setGyroCapture(true);

        m_settingUpdateTimer.start();
        this->m_wrapper->readBatteryStatus();
        m_isReadyToCapture = true;
    });

    ui->deviceAddress->setText(device.address().toString());
    ui->sensorName->setText(device.address().toString());

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->plot->xAxis->setTicker(timeTicker);
    ui->plot->axisRect()->setupFullAxesBox();
    ui->plot->yAxis->setRange(-2, 2);

    connect(this->m_wrapper, &MetawearWrapper::batteryPercentage,
            [=](qint8 amount) { this->ui->battery->setValue(amount); });


    // read the battery status every minute
    m_settingUpdateTimer.setInterval(60000);
    connect(&m_settingUpdateTimer, &QTimer::timeout,
            [=]() { this->m_wrapper->readBatteryStatus(); });

    connect(&m_plotUpdatetimer,&QTimer::timeout,this,[=](){
        if(this->m_plotLock.tryLock(100)){
            qint64 xpos = this->getMetwareWrapper()->getLatestEpoch() - m_plotoffset;
            this->ui->plot->xAxis->setRange(
                        xpos, this->ui->xScaleSlider->value(), Qt::AlignRight);
            for (int x = 0; x < this->ui->plot->graphCount(); ++x) {
                QCPGraph *graph = this->ui->plot->graph(x);
                graph->data()->removeBefore(xpos - 50000);
            }
            this->m_plotLock.unlock();
            this->ui->plot->replot();
        }
    });
    m_plotUpdatetimer.setInterval(50);
    m_plotUpdatetimer.start();
}

void SensorPanel::registerPlotHandlers()
{
    QCPGraph *xgraphAcc = ui->plot->addGraph();
    xgraphAcc->setPen(QPen(QColor(255,0,0)));
    xgraphAcc->setName("X Acc");
    QCPGraph *ygraphAcc = ui->plot->addGraph();
    ygraphAcc->setPen(QPen(QColor(0,255,0)));
    ygraphAcc->setName("X Acc");
    QCPGraph *zgraphAcc = ui->plot->addGraph();
    zgraphAcc->setName("Z Acc");
    zgraphAcc->setPen(QPen(QColor(0,0,255)));
    connect(this->m_wrapper, &MetawearWrapper::acceleration, this,
            [=](int64_t epoch, float x, float y, float z) {
        this->m_plotLock.lock();
        xgraphAcc->addData(epoch - m_plotoffset, static_cast<double>(x));
        ygraphAcc->addData(epoch - m_plotoffset, static_cast<double>(y));
        zgraphAcc->addData(epoch - m_plotoffset, static_cast<double>(z));
        this->m_plotLock.unlock();
    });
}

void SensorPanel::registerDataHandlers()
{
    connect(this->m_wrapper, &MetawearWrapper::acceleration, this,
            [=](int64_t epoch, float x, float y, float z) {
        if(m_temporaryDir && m_temporaryDir->isValid()){
           bool exist = m_accFile->exists();
           if(!m_accFile->isOpen()){
               m_accFile->open(QIODevice::WriteOnly | QIODevice::Append);
               if(!m_accFile->isOpen())
                   return;
           }
            QTextStream outStream(m_accFile);
            if(!exist){
                outStream << "epoch(ms),acc_x(g),acc_y(g),acc_z(g)" << '\n';
            }
            outStream << epoch << ','<< x << ','<< y << ','<< z << '\n';

        }
    });

    connect(this->m_wrapper,&MetawearWrapper::magnetometer,this,[=](int64_t epoch, float x, float y, float z){
        if(m_temporaryDir && m_temporaryDir->isValid()){
            bool exist = m_magFile->exists();
            if(!m_magFile->isOpen()){
                m_magFile->open(QIODevice::WriteOnly | QIODevice::Append);
                if(!m_magFile->isOpen())
                    return;
            }
                QTextStream outStream(m_magFile);
                if(!exist){
                    outStream << "epoch(ms),mag_x(uT),mag_y(uT),mag_z(uT)" << '\n';
                }
                outStream << epoch << ','<< x << ','<< y << ','<< z << '\n';

        }
    });

    connect(this->m_wrapper,&MetawearWrapper::gyro,this,[=](int64_t epoch, float x, float y, float z){
        if(m_temporaryDir && m_temporaryDir->isValid()){

            bool exist = m_gyroFile->exists();
            if(!m_gyroFile->isOpen()){
                m_gyroFile->open(QIODevice::WriteOnly | QIODevice::Append);
                if(!m_magFile->isOpen())
                    return;
            }
            QTextStream outStream(m_gyroFile);
            if(!exist){
                outStream << "epoch(ms),gyro_x(fdps),gyro_y(fdps),gyro_z(fdps)" << '\n';
            }
            outStream << epoch << ','<< x << ','<< y << ','<< z << '\n';
        }
    });

}

void SensorPanel::setName(QString name) { ui->sensorName->setText(name); }

void SensorPanel::setOffset(qint64 offset) {
    clearPlots();
    m_plotoffset = offset;
}

qint64 SensorPanel::getOffset()
{
    return m_plotoffset;
}


void SensorPanel::startCapture(QTemporaryDir* dir)
{
    if(m_isReadyToCapture){
        ui->sensorName->setEnabled(false);
        m_temporaryDir = dir;

        m_magFile = new QFile(m_temporaryDir->path().append(QString("/%1_%2.csv").arg(ui->sensorName->text(),"mag")),this);
        m_accFile= new QFile(m_temporaryDir->path().append(QString("/%1_%2.csv").arg(ui->sensorName->text(),"acc")),this);
        m_gyroFile = new QFile(m_temporaryDir->path().append(QString("/%1_%2.csv").arg(ui->sensorName->text(),"gyro")),this);
    }
}

void SensorPanel::stopCapture()
{
    ui->sensorName->setEnabled(true);
    this->m_temporaryDir = nullptr;

    m_magFile->close();
    m_accFile->close();
    m_gyroFile->close();
}

void SensorPanel::clearPlots()
{
    this->m_plotLock.lock();
    for (int x = 0; x < this->ui->plot->graphCount(); ++x) {
        QCPGraph *graph = this->ui->plot->graph(x);
        graph->data()->clear();
    }
    this->m_plotLock.unlock();
}



MetawearWrapper* SensorPanel::getMetwareWrapper() {
    return this->m_wrapper;
}

QBluetoothDeviceInfo SensorPanel::getDeviceInfo()
{
    return m_currentDevice;
}

SensorPanel::~SensorPanel() { delete ui; }

bool SensorPanel::isReadyToCapture()
{
    return m_isReadyToCapture;
}



