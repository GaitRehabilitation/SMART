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

