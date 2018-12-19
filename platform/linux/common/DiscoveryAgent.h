//
// Created by Michael on 8/27/2018.
//
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
