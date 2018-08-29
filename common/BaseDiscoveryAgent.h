//
// Created by Michael on 8/27/2018.
//

#ifndef SMART_BASEDISCOVERYAGENT_H
#define SMART_BASEDISCOVERYAGENT_H

#include <QtCore/QObject>
#include "BluetoothAddress.h"

class BaseDiscoveryAgent : public QObject{
    Q_OBJECT
private:
public:
    BaseDiscoveryAgent(){

    }
    ~BaseDiscoveryAgent(){

    }

    virtual void start() = 0;
    virtual void stop()  = 0;
signals:
    void deviceDiscovered(BluetoothAddress address);
    void finished();

};

#endif //SMART_BASEDISCOVERYAGENT_H
