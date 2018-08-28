//
// Created by Michael on 8/27/2018.
//

#ifndef SMART_BASEDISCOVERYAGENT_H
#define SMART_BASEDISCOVERYAGENT_H

#include <QtCore/QObject>
#include "BluetoothAdress.h"

class BaseDiscoveryAgent : public QObject{
    Q_OBJECT
private:

public:
    BaseDiscoveryAgent();
    ~BaseDiscoveryAgent();

    virtual void start() = 0;
signals:
    void deviceDiscovered(const BluetoothAdress& address);
    void finished();

};

#endif //SMART_BASEDISCOVERYAGENT_H
