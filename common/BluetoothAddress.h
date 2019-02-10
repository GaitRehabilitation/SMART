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

#ifndef SMART_BLUETOOTHADRESS_H
#define SMART_BLUETOOTHADRESS_H

#include <QString>

class BluetoothAddress{
private:
    QString m_mac;
    QString m_title;
public:
    BluetoothAddress();
    BluetoothAddress(const QString& mac, const QString& title);
    BluetoothAddress(const BluetoothAddress& address);
    BluetoothAddress(unsigned long long address, const QString& title);

    ~BluetoothAddress();

    const QString& getTitle() const;
    const QString& getMac() const;
    uint64_t toUint64() const;

};

#endif //SMART_BLUETOOTHADRESS_H
