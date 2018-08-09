//
// Created by michaelpollind on 8/4/18.
//

#include <3rdparty/mbientlab/src/metawear/core/status.h>
#include "common/metawearwrapper.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>


quint128 MetawearWrapper::convertTo128(uint64_t low,uint64_t high){
    quint128 result{};
    result.data[0] = uint8_t((high >> 56) & 0xFF);
    result.data[1] = uint8_t((high >> 48) & 0xFF);
    result.data[2] = uint8_t((high >> 40) & 0xFF);
    result.data[3] = uint8_t((high >> 32) & 0xFF);
    result.data[4] = uint8_t((high >> 24) & 0xFF);
    result.data[5] = uint8_t((high >> 16) & 0xFF);
    result.data[6] = uint8_t((high >> 8) & 0xFF);
    result.data[7] = uint8_t(high & 0xFF);

    result.data[8] =  uint8_t((low >> 56) & 0xFF);
    result.data[9] =  uint8_t((low >> 48) & 0xFF);
    result.data[10] = uint8_t((low >> 40) & 0xFF);
    result.data[11] = uint8_t((low >> 32) & 0xFF);
    result.data[12] = uint8_t((low >> 24) & 0xFF);
    result.data[13] = uint8_t((low >> 16) & 0xFF);
    result.data[14] = uint8_t((low >> 8) & 0xFF);
    result.data[15] = uint8_t(low & 0xFF);

    return result;
}

void MetawearWrapper::read_gatt_char(void *context, const void *caller, const MblMwGattChar *characteristic,
                                       MblMwFnIntVoidPtrArray handler) {

    MetawearWrapper *wrapper = static_cast<MetawearWrapper *>(context);
    wrapper->m_readGattHandler = handler;

    QBluetoothUuid service_uuid = QBluetoothUuid(
            convertTo128(characteristic->service_uuid_low, characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(
            convertTo128(characteristic->uuid_low, characteristic->uuid_high));

    QLowEnergyService *service = wrapper->m_services.value(service_uuid.toString());
    if (service == nullptr) {
        qWarning() << "Invliad service Uuid(read gatt)" << service_uuid.toString();
        return;
    }
    QLowEnergyCharacteristic c = service->characteristic(characteristic_uuid);
    if (!c.isValid()) {
        qWarning() << "Invalid characteristic Uuid(read gatt)"
                   << characteristic_uuid.toString();
        return;
    }
    service->readCharacteristic(c);
}

void MetawearWrapper::write_gatt_char(void *context, const void *caller, MblMwGattCharWriteType writeType,
                                        const MblMwGattChar *characteristic, const uint8_t *value, uint8_t length) {
    MetawearWrapper *wrapper = static_cast<MetawearWrapper *>(context);
    QBluetoothUuid service_uuid = QBluetoothUuid(
            convertTo128(characteristic->service_uuid_low, characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(
            convertTo128(characteristic->uuid_low, characteristic->uuid_high));

    QLowEnergyService *service =
            wrapper->m_services.value(service_uuid.toString());
    if (service == nullptr) {
        qWarning() << "failed to parse service uuid(write gatt)"
                   << service_uuid.toString();
        return;
    }
    QLowEnergyCharacteristic c = service->characteristic(characteristic_uuid);
    if (!c.isValid()) {
        qWarning() << "Invalid characteristic Uuid(write gatt)"
                   << characteristic_uuid.toString();
        return;
    }
    QByteArray payload;
    for (int x = 0; x < length; ++x) {
        payload.append(value[x]);
    }
    service->writeCharacteristic(c, payload);
}

void MetawearWrapper::enable_char_notify(void *context, const void *caller, const MblMwGattChar *characteristic,
                                           MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready) {
    MetawearWrapper *wrapper = static_cast<MetawearWrapper *>(context);
    wrapper->m_notificationHandler = handler;

    QBluetoothUuid service_uuid = QBluetoothUuid(
            convertTo128(characteristic->service_uuid_low, characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(
            convertTo128(characteristic->uuid_low, characteristic->uuid_high));

    QLowEnergyService *service = wrapper->m_services.value(service_uuid.toString());
    if (service == nullptr) {
        qWarning() << "failed to parse service uuid(char notify): " << service_uuid.toString();
        return;
    }
    QLowEnergyCharacteristic c = service->characteristic(characteristic_uuid);
    if (!c.isValid()) {
        qWarning() << "Invalid characteristic Uuid(char notify): " << characteristic_uuid.toString();
    }

    QLowEnergyDescriptor notification = c.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    if (!notification.isValid())
        return;

    service->writeDescriptor(notification, QByteArray::fromHex("0100"));
    QObject *temp = new QObject();
    connect(service, &QLowEnergyService::descriptorWritten, temp,
            [caller, ready, temp](const QLowEnergyDescriptor &descriptor, const QByteArray &newValue) {
                ready(caller, MBL_MW_STATUS_OK);
                temp->deleteLater();
            });
}

void MetawearWrapper::on_disconnect(void *context, const void *caller, MblMwFnVoidVoidPtrInt handler) {
    MetawearWrapper *wrapper = static_cast<MetawearWrapper*>(context);
    wrapper->m_disconnectedHandler = handler;
}

MetawearWrapper::MetawearWrapper(const QBluetoothHostInfo &local,const QBluetoothDeviceInfo &target):
	MetawearWrapperBase::MetawearWrapperBase() {
	//	auto leDevice = co_await Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(bluetoothAddress);
	
	auto leDevice = co_await  winrt::Windows::Devices::Bluetooth::BluetoothDevice::FromBluetoothAddressAsync(0);
}
