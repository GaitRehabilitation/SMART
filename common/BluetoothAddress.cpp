//
// Created by Michael on 8/27/2018.
//

#include "common/BluetoothAddress.h"

BluetoothAddress::BluetoothAddress(){

}
BluetoothAddress::BluetoothAddress(const QString& mac, const QString& title) :
    m_mac(mac),m_title(title) {}

BluetoothAddress::BluetoothAddress(const BluetoothAddress& address):
    m_title(address.m_title),m_mac(address.m_mac){
}


BluetoothAddress::~BluetoothAddress() {
}

const QString& BluetoothAddress::getTitle() const{
    return m_title;
}

const QString& BluetoothAddress::getMac() const{
    return m_mac;
}



uint64_t BluetoothAddress::toUint64() const{
    std::string mac_copy(m_mac.toStdString());
    mac_copy.erase(2, 1);
    mac_copy.erase(4, 1);
    mac_copy.erase(6, 1);
    mac_copy.erase(8, 1);
    mac_copy.erase(10, 1);

    size_t temp;
    return std::stoull(mac_copy, &temp, 16);
}

