//
// Created by Michael on 8/27/2018.
//

#include "common/DiscoveryAgent.h"

#include <QtBluetooth/QBluetoothAddress>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothLocalDevice>

#include <QDebug>
#include <QtBluetooth/QBluetoothDeviceInfo>

DiscoveryAgent::DiscoveryAgent() : BaseDiscoveryAgent() {
    _discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(_discoveryAgent,&QBluetoothDeviceDiscoveryAgent::deviceDiscovered,this,[=](const QBluetoothDeviceInfo &info) -> void{
        BluetoothAddress address(info.address().toString(),info.name());
        emit deviceDiscovered(address);
    });
}

DiscoveryAgent::~DiscoveryAgent() {

}
void DiscoveryAgent::start() {
    qDebug() << "started advertisement";
    _discoveryAgent->start();
}

void DiscoveryAgent::stop() {
    qDebug() << "stop advertisement";
    _discoveryAgent->stop();

}


