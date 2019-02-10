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

