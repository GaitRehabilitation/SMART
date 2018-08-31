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

#include "common/DiscoveryAgent.h"
#include "common/util.h"
#include <QDebug>
#include <sstream>
#include <iomanip>

DiscoveryAgent::DiscoveryAgent() : BaseDiscoveryAgent() {
	m_bleAdvertisementWatcher = ref new Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher();
	m_bleAdvertisementWatcher->ScanningMode = Bluetooth::Advertisement::BluetoothLEScanningMode::Active;
	m_bleAdvertisementWatcher->Received += ref new Windows::Foundation::TypedEventHandler<Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher ^, Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs ^>([=](Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher^ watcher,Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs^ eventArgs) {

		std::wostringstream ret;
		ret << std::hex << std::setfill(L'0')
			<< std::setw(2) << ((eventArgs->BluetoothAddress >> (5 * 8)) & 0xff) << ":"
			<< std::setw(2) << ((eventArgs->BluetoothAddress >> (4 * 8)) & 0xff) << ":"
			<< std::setw(2) << ((eventArgs->BluetoothAddress >> (3 * 8)) & 0xff) << ":"
			<< std::setw(2) << ((eventArgs->BluetoothAddress >> (2 * 8)) & 0xff) << ":"
			<< std::setw(2) << ((eventArgs->BluetoothAddress >> (1 * 8)) & 0xff) << ":"
			<< std::setw(2) << ((eventArgs->BluetoothAddress >> (0 * 8)) & 0xff);

		std::wstring localname(eventArgs->Advertisement->LocalName->Begin());

        QString mac =  QString::fromWCharArray(ret.str().c_str());
        QString name = QString::fromWCharArray(localname.c_str());
        BluetoothAddress address(mac,name);
        emit deviceDiscovered(address);
	});
}

DiscoveryAgent::~DiscoveryAgent() {

}
void DiscoveryAgent::start() {
    qDebug() << "started advertisement";
    m_bleAdvertisementWatcher->Start();
}

void DiscoveryAgent::stop() {
    qDebug() << "stop advertisement";
    m_bleAdvertisementWatcher->Stop();

}


