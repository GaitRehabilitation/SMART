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
      settingUpdateTimer(new QTimer(this)),
      m_currentDevice(device),
      m_plotUpdatetimer(),
      m_wrapper(new MetawearWrapper(device, this)),
      m_plotoffset(0),
      m_temporaryDir(nullptr),
      m_laststEpoch(0),
      m_isReadyToCapture(false){
    ui->setupUi(this);

    connect(this->m_wrapper, SIGNAL(onMagnetometer(qint64, float, float, float)),
            this, SIGNAL(onMagnetometer(qint64, float, float, float)));
    connect(this->m_wrapper, SIGNAL(onGyro(qint64, float, float, float)), this,
            SIGNAL(onGyro(qint64, float, float, float)));
    connect(this->m_wrapper, SIGNAL(onAcceleration(qint64, float, float, float)),
            this, SIGNAL(onAcceleration(qint64, float, float, float)));
    connect(this->m_wrapper, SIGNAL(onAmbientLight(qint64, qint32)), this,
            SIGNAL(onAmbientLight(qint64, qint32)));
    connect(this->m_wrapper,SIGNAL(disconnected()),this,SIGNAL(onDisconnect()));
    connect(this->m_wrapper,&MetawearWrapper::disconnected,this,[=](){
        this->deleteLater();
    });
    connect(this->m_wrapper,&MetawearWrapper::controllerError,this,[=](){
        this->deleteLater();
    });
    connect(this->m_wrapper, SIGNAL(onEpoch(qint64)), this,
            SIGNAL(onEpoch(qint64)));
    connect(this->m_wrapper, SIGNAL(onVoltage(quint16)), this,
            SIGNAL(onVoltage(quint16)));
    connect(this->m_wrapper, SIGNAL(onBatteryPercentage(qint8)), this,
            SIGNAL(onBatteryPercentage(qint8)));

    // watch for a text change and remove characters that can cause problems for the path
    connect(ui->sensorName,&QLineEdit::textChanged,this,[=](const QString& text){
        QString updated(text);
        if(updated.isEmpty()){
            updated = this->ui->deviceAddress->text();
        }

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

    connect(this->m_wrapper, SIGNAL(metawareInitialized()), this,
            SLOT(metawareInitialized()));
    connect(this->m_wrapper,SIGNAL(metawareInitialized()),this,SIGNAL(onMetawearInitilized()));
    connect(this->m_wrapper,&MetawearWrapper::metawareInitialized,[=](){
        m_isReadyToCapture = true;
    });
    ui->deviceAddress->setText(device.address().toString());
    ui->sensorName->setText(device.address().toString());

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->plot->xAxis->setTicker(timeTicker);
    ui->plot->axisRect()->setupFullAxesBox();
    ui->plot->yAxis->setRange(-2, 2);

    connect(this->m_wrapper, &MetawearWrapper::onBatteryPercentage,
            [=](qint8 amount) { this->ui->battery->setValue(amount); });


    // read the battery status every minute
    settingUpdateTimer->setInterval(60000);
    connect(settingUpdateTimer, &QTimer::timeout,
            [=]() { this->m_wrapper->readBatteryStatus(); });

    connect(this->m_wrapper, &MetawearWrapper::onEpoch, this,
            [this](qint64 epoch) {
        if(epoch < m_laststEpoch)
            return;
        m_laststEpoch = epoch;
        emit onLatestEpoch(m_laststEpoch);
    });

    connect(&m_plotUpdatetimer,&QTimer::timeout,this,[=](){
        if(this->m_plotLock.tryLock(100)){
            qint64 xpos = this->m_laststEpoch - m_plotoffset;
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
    connect(this->m_wrapper, &MetawearWrapper::onAcceleration, this,
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
    connect(this->m_wrapper, &MetawearWrapper::onAcceleration, this,
            [=](int64_t epoch, float x, float y, float z) {
//        m_directoryLock.lock();
        if(m_temporaryDir && m_temporaryDir->isValid()){
            QString path =  m_temporaryDir->path().append(QString("/%1_%2.csv").arg(ui->sensorName->text(),"acc")) ;
            QFile file(path);
            bool exist = file.exists();
            if(file.open(QIODevice::WriteOnly | QIODevice::Append)){
                QTextStream outStream(&file);
                if(!exist){
                    outStream << "epoch(ms),acc_x(g),acc_y(g),acc_z(g)" << '\n';
                }
                outStream << epoch << ','<< x << ','<< y << ','<< z << '\n';
            }
            file.close();
        }
//        m_directoryLock.unlock();
    });

    connect(this->m_wrapper,&MetawearWrapper::onGyro,this,[=](int64_t epoch, float x, float y, float z){
//        m_directoryLock.lock();
        if(m_temporaryDir && m_temporaryDir->isValid()){
            QString path =  m_temporaryDir->path().append(QString("/%1_%2.csv").arg(ui->sensorName->text(),"gyro")) ;
            QFile file(path);
            bool exist = file.exists();
            if(file.open(QIODevice::WriteOnly | QIODevice::Append)){
                QTextStream outStream(&file);
                if(!exist){
                    outStream << "epoch(ms),gyro_x(fdps),gyro_y(fdps),gyro_z(fdps)" << '\n';
                }
                outStream << epoch << ','<< x << ','<< y << ','<< z << '\n';
            }
            file.close();
        }
//        m_directoryLock.unlock();
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

qint64 SensorPanel::getLatestEpoch()
{
    return m_laststEpoch;
}

void SensorPanel::startCapture(QTemporaryDir* dir)
{
    if(m_isReadyToCapture){
        m_directoryLock.lock();
        m_temporaryDir = dir;
        m_directoryLock.unlock();
    }
}

void SensorPanel::stopCapture()
{
    m_directoryLock.lock();
    this->m_temporaryDir = nullptr;
    m_directoryLock.unlock();
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


void SensorPanel::metawareInitialized() {
    this->m_wrapper->setAccelerationSamplerate(100);
    this->m_wrapper->setGyroSamplerate(MBL_MW_GYRO_BMI160_ODR_50Hz);
    this->m_wrapper->setGyroCapture(true);
    this->m_wrapper->setAccelerationCapture(true);
    settingUpdateTimer->start();
    this->m_wrapper->readBatteryStatus();

}

SensorPanel::~SensorPanel() { delete ui; }



