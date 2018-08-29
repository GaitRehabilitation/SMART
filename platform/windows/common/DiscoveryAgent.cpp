//
// Created by Michael on 8/27/2018.
//

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
        BluetoothAddress address(mac,name );
//        qDebug() << "mac:" << mac << " name: " << name;
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


