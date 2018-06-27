#include "ui_sensorpanel.h"
#include "metawearwrapper.h"
#include "metawear/core/metawearboard.h"
#include "forms/sensorpanel.h"

#include <QLowEnergyController>

SensorPanel::SensorPanel(QBluetoothAddress address, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorPanel)
{
    ui->setupUi(this);

    this->wrapper = new MetawearWrapper(address,this);

    ui->deviceAddress->setText(address.toString());
    connect(this->wrapper,SIGNAL(bluetoothError(QLowEnergyController::Error)),this,SLOT(handleError(QLowEnergyController::Error)));
    connect(this->wrapper,SIGNAL(connected()),this,SLOT(handleConnection()));
    connect(this->wrapper,SIGNAL(disconnected()),this,SLOT(handleDisconnect()));

}

void SensorPanel::handleConnection(){
    qDebug() << "Device Connected";
}

void SensorPanel::handleDisconnect(){
    qDebug() << "Device Disconnected";
}

void SensorPanel::handleError(QLowEnergyController::Error e){
    switch (e) {
        case QLowEnergyController::Error::UnknownError:
                qDebug() << "UnknownError";
            break;
        case QLowEnergyController::Error::UnknownRemoteDeviceError:
                qDebug() << "UnknownRemoteDeviceError";
            break;
        case QLowEnergyController::Error::NetworkError:
                qDebug() << "NetworkError";
            break;
        case QLowEnergyController::Error::InvalidBluetoothAdapterError:
                qDebug() << "InvalidBluetoothAdapterError";
            break;
        case QLowEnergyController::Error::ConnectionError:
                qDebug() << "ConnectionError";
            break;
        case QLowEnergyController::Error::AdvertisingError:
                qDebug() << "AdvertisingError";
            break;

    }
}

void SensorPanel::setName(QString name){
    ui->sensorName->setText(name);
}

SensorPanel::~SensorPanel(){
    delete ui;
}
