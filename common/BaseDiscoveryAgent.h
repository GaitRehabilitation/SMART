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
#ifndef SMART_BASEDISCOVERYAGENT_H
#define SMART_BASEDISCOVERYAGENT_H

#include <QObject>
#include "BluetoothAddress.h"

/**
 * A wrapper object that allows for the discovery of bluetooth devices
 * implementation is platform specific
 */
class BaseDiscoveryAgent : public QObject{
    Q_OBJECT
private:
public:
    BaseDiscoveryAgent(){

    }
    ~BaseDiscoveryAgent(){

    }

    /**
     * start searching for available bluetooth devices
     */
    virtual void start() = 0;
    /**
     * stop searching for available bluetooth devices
     */
    virtual void stop()  = 0;
signals:
    /**
     * @param address of the discovered bluetooth device
     */
    void deviceDiscovered(BluetoothAddress address);
    void finished();

};

#endif //SMART_BASEDISCOVERYAGENT_H
