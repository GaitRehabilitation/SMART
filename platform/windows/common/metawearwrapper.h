//
// Created by michaelpollind on 8/4/18.
//

#ifndef SMART_QTMETAWEARWRAPPER_H
#define SMART_QTMETAWEARWRAPPER_H

#include <QtBluetooth/QLowEnergyController>
#include <QtCore/QPointer>
#include <QtBluetooth/QBluetoothHostInfo>
#include "common/metawearwrapperbase.h"

#include <ppltasks.h>
#include <unordered_map>
#include <Windows.Foundation.h>
#include <Windows.Devices.Bluetooth.h>
#include <Windows.Devices.Bluetooth.Advertisement.h>

using namespace concurrency;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Security::Cryptography;


class MetawearWrapper : public MetawearWrapperBase {
Q_OBJECT
private:
    QPointer<QLowEnergyController> m_controller;

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
    int m_readyCharacteristicCount;

    MblMwFnIntVoidPtrArray m_readGattHandler;
    MblMwFnVoidVoidPtrInt m_disconnectedHandler;
    MblMwFnIntVoidPtrArray m_notificationHandler;

    GattCharacteristic^  findCharacterstic( uint64_t low, uint64_t high);


protected:


   // static GUID convertTo128(uint64_t low, uint64_t high);

    static void read_gatt_char(void *context, const void *caller, const MblMwGattChar *characteristic,
                               MblMwFnIntVoidPtrArray handler);

    static void write_gatt_char(void *context, const void *caller, MblMwGattCharWriteType writeType,
                                const MblMwGattChar *characteristic, const uint8_t *value, uint8_t length);

    static void enable_char_notify(void *context, const void *caller, const MblMwGattChar *characteristic,
                                   MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready);

    static void on_disconnect(void *context, const void *caller, MblMwFnVoidVoidPtrInt handler);



public:
    MetawearWrapper(const QBluetoothHostInfo &local, const QBluetoothDeviceInfo &target);

signals:

    void controllerError(QLowEnergyController::Error);

    void characteristicError(QLowEnergyService::ServiceError);

};

#endif //SMART_QTMETAWEARWRAPPER_H
