#include "ui_sensorpanel.h"
#include "metawearwrapper.h"
#include "metawear/core/metawearboard.h"
#include "forms/sensorpanel.h"

#include <QLowEnergyController>

SensorPanel::SensorPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorPanel)
{
    ui->setupUi(this);
}

void SensorPanel::setDevice(const QBluetoothDeviceInfo &device){
    m_currentDevice = device;
    this->wrapper = new MetawearWrapper(this);
    ui->deviceAddress->setText(device.name());
    this->wrapper->setDevice(device);

}

void SensorPanel::setName(QString name){
    ui->sensorName->setText(name);
}

SensorPanel::~SensorPanel(){
    delete ui;
}
