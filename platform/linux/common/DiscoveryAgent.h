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

#ifndef SMART_DISCOVERYAGENT_H
#define SMART_DISCOVERYAGENT_H

#include "common/BaseDiscoveryAgent.h"

QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceDiscoveryAgent)
QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceInfo)
QT_FORWARD_DECLARE_CLASS(QBluetoothLocalDevice)

class DiscoveryAgent : public BaseDiscoveryAgent{
Q_OBJECT
private:
    QBluetoothDeviceDiscoveryAgent *_discoveryAgent;
    QBluetoothLocalDevice *_localDevice;
public:
    DiscoveryAgent();
    ~DiscoveryAgent() override;
    void start() override;
    void stop() override;
};

#endif //SMART_DISCOVERYAGENT_H
