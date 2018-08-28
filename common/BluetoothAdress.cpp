//
// Created by Michael on 8/27/2018.
//

#include "common/BluetoothAdress.h"

BluetoothAdress::BluetoothAdress(const QString& mac, const QString& title) :
    m_mac(mac),m_title(title) {}

BluetoothAdress::BluetoothAdress(const BluetoothAdress& address):
    m_title(address.m_title),m_mac(address.m_mac){

}


BluetoothAdress::~BluetoothAdress() {
}

const QString& BluetoothAdress::getTitle() const{
    return m_title;
}

const QString& BluetoothAdress::getMac() const{
    return m_mac;
}

