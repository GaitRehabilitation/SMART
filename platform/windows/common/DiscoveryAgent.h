//
// Created by Michael on 8/27/2018.
//
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
private:
    Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher^ m_bleAdvertisementWatcher;

public:
    DiscoveryAgent();
    ~DiscoveryAgent();

    void start();

};

#endif //SMART_DISCOVERYAGENT_H
