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

#ifndef SMART_QTMETAWEARWRAPPER_H
#define SMART_QTMETAWEARWRAPPER_H

#include <QtCore/QPointer>
#include "common/metawearwrapperbase.h"

#include <ppltasks.h>
#include <unordered_map>
#include <Windows.Foundation.h>
#include <Windows.Devices.Bluetooth.h>
#include <Windows.Devices.Bluetooth.Advertisement.h>
#include <wrl/wrappers/corewrappers.h>
#include "common/BluetoothAddress.h"

using namespace concurrency;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Security::Cryptography;

class MetawearWrapper : public MetawearWrapperBase {
Q_OBJECT
private:
    struct Hasher {
        size_t operator() (Platform::Guid key) const {
            return key.GetHashCode();
        }
    };
    struct EqualFn {
        bool operator() (Platform::Guid t1, Platform::Guid t2) const {
            return t1.Equals(t2);
        }
    };
    std::unordered_map<Platform::Guid, GattDeviceService^, Hasher, EqualFn> m_services;
    std::unordered_map<Platform::Guid, GattCharacteristic^, Hasher, EqualFn> m_characterstics;
    BluetoothLEDevice^ m_device;

    GattCharacteristic^  findCharacterstic( uint64_t low, uint64_t high);
	void startDiscovery();
protected:


    static void read_gatt_char(void *context, const void *caller, const MblMwGattChar *characteristic,
                               MblMwFnIntVoidPtrArray handler);

    static void write_gatt_char(void *context, const void *caller, MblMwGattCharWriteType writeType,
                                const MblMwGattChar *characteristic, const uint8_t *value, uint8_t length);

    static void enable_char_notify(void *context, const void *caller, const MblMwGattChar *characteristic,
                                   MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready);

    static void on_disconnect(void *context, const void *caller, MblMwFnVoidVoidPtrInt handler);

public:
    MetawearWrapper( const BluetoothAddress &target);
    void connectToDevice();
    bool isConnected() const;
	~MetawearWrapper();

};

#endif //SMART_QTMETAWEARWRAPPER_H
