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

#ifndef SMART_DISCOVERYAGENT_H
#define SMART_DISCOVERYAGENT_H


#include "common/BaseDiscoveryAgent.h"
#include <pplawait.h>
#include <Windows.Foundation.h>
#include <Windows.Devices.Bluetooth.h>
#include <Windows.Devices.Bluetooth.Advertisement.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/event.h>

using namespace Platform;
using namespace Windows::Devices;

class DiscoveryAgent : public BaseDiscoveryAgent{
Q_OBJECT
private:
    Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher^ m_bleAdvertisementWatcher;
public:
    DiscoveryAgent();
    ~DiscoveryAgent();
    void start();
    void stop();
};

#endif //SMART_DISCOVERYAGENT_H
