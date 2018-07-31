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
    });

    connect(this->m_wrapper,&MetawearWrapper::onGyro,this,[=](int64_t epoch, float x, float y, float z){
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
        m_temporaryDir = dir;
    }
}

void SensorPanel::stopCapture()
{
    this->m_temporaryDir = nullptr;
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

bool SensorPanel::isReadyToCapture()
{
    return m_isReadyToCapture;
}



