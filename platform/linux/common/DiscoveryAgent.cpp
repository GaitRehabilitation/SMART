/**
* Copyright 2019 GaitRehabilitation
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


