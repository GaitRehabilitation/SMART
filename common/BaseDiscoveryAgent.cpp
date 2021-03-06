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

#include <common/BaseDiscoveryAgent.h>
#include <common/DiscoveryAgent.h>

BaseDiscoveryAgent::BaseDiscoveryAgent(QObject *parent) : QObject(parent), BaseDiscoveryAgent::_agentCache(QMap<QString, BluetoothAddress>()) {
	qRegisterMetaType<BluetoothAddress>("BluetoothAddress");
	connect(this, &BaseDiscoveryAgent::deviceDiscovered, [=](BluetoothAddress addr) {
		_agentCache.insert(addr.getMac(), addr);
	});
}

void BaseDiscoveryAgent::query() {
	QMap<QString, BluetoothAddress>::iterator i;
	for (i = _agentCache.begin(); i != _agentCache.end(); ++i) {
		emit deviceDiscovered(i.value());
	}
}