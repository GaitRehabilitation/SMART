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

#ifndef SMART_BASEDISCOVERYAGENT_H
#define SMART_BASEDISCOVERYAGENT_H

#include <QtCore/QObject>
#include "BluetoothAddress.h"
#include <QMap>

class BaseDiscoveryAgent : public QObject{
    Q_OBJECT
private:
	QMap<QString, BluetoothAddress> _agentCache;
public:
    BaseDiscoveryAgent(QObject *parent = nullptr) : QObject(parent), BaseDiscoveryAgent::_agentCache(QMap<QString, BluetoothAddress>()) {
		connect(this, &BaseDiscoveryAgent::deviceDiscovered, [=](BluetoothAddress addr) {
			_agentCache.insert(addr.getMac(), addr);
		});
    }
    ~BaseDiscoveryAgent(){

    }

    virtual void start() = 0;
	void query();
    virtual void stop()  = 0;
signals:
    void deviceDiscovered(BluetoothAddress address);
    void finished();

};

#endif //SMART_BASEDISCOVERYAGENT_H
