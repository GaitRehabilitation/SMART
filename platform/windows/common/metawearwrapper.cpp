//
// Created by michaelpollind on 8/4/18.
//

#include <3rdparty/mbientlab/src/metawear/core/status.h>
#include "common/metawearwrapper.h"

#include "common/util.h"
#include <vector>
#include <collection.h>
#include <sstream>
#include <iomanip>
#include <winsock.h>
#include <QMetaObject>
#include <QDebug>

#define MAX_LEN_UUID_STR 37
//auto leDevice = co_await  winrt::Windows::Devices::Bluetooth::BluetoothDevice::FromBluetoothAddressAsync(0);

using namespace Windows::Foundation;


GattCharacteristic^  MetawearWrapper::findCharacterstic(uint64_t low, uint64_t high){
    unsigned int data0 = uint8_t((high >> 56) & 0xFF)  | uint8_t((high >> 48) & 0xFF) << 8 | uint8_t((high >> 40) & 0xFF) << 16 | uint8_t((high >> 32) & 0xFF) << 24;
    unsigned short data1 = uint8_t((high >> 24) & 0xFF)  | uint8_t((high >> 16) & 0xFF) << 8;
    unsigned short data2  = uint8_t((high >> 8) & 0xFF)  | uint8_t(high & 0xFF) << 8;

    unsigned short data3  = uint8_t((low >> 56) & 0xFF)  | uint8_t((low >> 48) & 0xFF) << 8;
    unsigned int data4  = uint8_t((low >> 40) & 0xFF)  | uint8_t((low >> 32) & 0xFF) << 8 | uint8_t((low >> 24) & 0xFF) << 16 | uint8_t((low >> 16) & 0xFF) << 24;
    unsigned short data5  = uint8_t((low >> 8) & 0xFF)  | uint8_t(low & 0xFF) << 8;

    char uuid_str[MAX_LEN_UUID_STR + 3];
    snprintf(uuid_str, MAX_LEN_UUID_STR + 3, "{%.8x-%.4x-%.4x-%.4x-%.8x%.4x}",
		ntohl(data0), ntohs(data1), ntohs(data2), 
		ntohs(data3), ntohl(data4), ntohs(data5));

    std::wstring  ws(&uuid_str[0],&uuid_str[MAX_LEN_UUID_STR+2]);

    GUID uuid;
    auto casted = ref new Platform::String(ws.c_str());
    if (SUCCEEDED(IIDFromString(casted->Data(), &uuid))){
        auto  it = m_characterstics.find(uuid);
		if (it != m_characterstics.end())
			return it->second;

        //return it == m_characterstics.end() ? nullptr : it->second;
    }
	qWarning() << "Failed to find characterstic";
    return nullptr;

}



void MetawearWrapper::read_gatt_char(void *context, const void *caller, const MblMwGattChar *characteristic,
                                       MblMwFnIntVoidPtrArray handler) {
    auto *wrapper = static_cast<MetawearWrapper *>(context);

    GattCharacteristic^ ch = wrapper->findCharacterstic(characteristic->uuid_low,characteristic->uuid_high);
	if (ch == nullptr)
		return;
    create_task(ch->ReadValueAsync()).then([=](GattReadResult^ result){
        if(result->Status == GattCommunicationStatus::Success){
            Platform::Array<byte>^ wrapper = ref new Platform::Array<byte>(result->Value->Length);
            CryptographicBuffer::CopyToByteArray(result->Value, &wrapper);
            handler(caller,(uint8_t*)wrapper->Data, wrapper->Length);

        } else {
            qWarning() << "failed to read gatt";
        }
	}).wait();
}

void MetawearWrapper::write_gatt_char(void *context, const void *caller, MblMwGattCharWriteType writeType,
                                        const MblMwGattChar *characteristic, const uint8_t *value, uint8_t length) {
    auto *wrapper = static_cast<MetawearWrapper *>(context);

    Platform::Array<byte>^ arr = ref new Platform::Array<byte>(length);
    for (uint8_t i = 0; i < length; i++) {
        arr[i] = value[i];
    }

    GattCharacteristic^ ch = wrapper->findCharacterstic(characteristic->uuid_low,characteristic->uuid_high);
	if (ch == nullptr)
		return;
    create_task(ch->WriteValueAsync(CryptographicBuffer::CreateFromByteArray(arr), GattWriteOption::WriteWithResponse)).then([](GattCommunicationStatus status){
        if(status  != GattCommunicationStatus::Success ){
            qWarning() << "Failed to send data";
        }
    }).wait();

}

void MetawearWrapper::enable_char_notify(void *context, const void *caller, const MblMwGattChar *characteristic,
                                           MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready) {
    auto *wrapper = static_cast<MetawearWrapper *>(context);

    GattCharacteristic^ ch = wrapper->findCharacterstic(characteristic->uuid_low,characteristic->uuid_high);
	if (ch == nullptr)
		return;
    create_task(ch->WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue::Notify))
        .then([=](GattCommunicationStatus status){
            if(status == GattCommunicationStatus::Success){
                ch->ValueChanged += ref new TypedEventHandler<GattCharacteristic^, GattValueChangedEventArgs^>([=](GattCharacteristic^ sender, GattValueChangedEventArgs^ obj){
                    Platform::Array<byte>^ wrapper = ref new Platform::Array<byte>(obj->CharacteristicValue->Length);
                    CryptographicBuffer::CopyToByteArray(obj->CharacteristicValue, &wrapper);
                    handler(caller,(uint8_t*)wrapper->Data, wrapper->Length);
                });
                ready(caller, MBL_MW_STATUS_OK);
            } else{
                ready(caller, MBL_MW_STATUS_ERROR_ENABLE_NOTIFY);
            }

       }).wait();

}

void MetawearWrapper::on_disconnect(void *context, const void *caller, MblMwFnVoidVoidPtrInt handler) {
    auto *wrapper = static_cast<MetawearWrapper *>(context);

}


MetawearWrapper::MetawearWrapper(const BluetoothAddress &target):
	MetawearWrapperBase::MetawearWrapperBase(target){
}

void MetawearWrapper::connectToDevice(){
    if(this->m_device == nullptr) {
        create_task(BluetoothLEDevice::FromBluetoothAddressAsync(this->m_target.toUint64())).then(
                [=](BluetoothLEDevice^leDevice) {
                    if (leDevice == nullptr) {
                        qWarning() << "Failed to discover device";
                    } else {
                        leDevice->ConnectionStatusChanged += ref new TypedEventHandler<BluetoothLEDevice ^ , Platform::Object^>(
                                [=](BluetoothLEDevice^sender, Platform::Object^args) {
                                    switch (sender->ConnectionStatus) {
                                        case BluetoothConnectionStatus::Disconnected:
                                            qWarning() << "Failed to connect to device";
                                            emit disconnected();
                                            break;
                                    }
                                });
                        this->m_device = leDevice;
                        this->startDiscovery();
                    }
                });
    }
    else{
        qDebug() << "Device already connected";
    }
}


bool MetawearWrapper::isConnected() const{
    return this->m_device != nullptr;
}

void MetawearWrapper::startDiscovery() {
    qDebug() << "Started Discovery";
    try {
        create_task(this->m_device->GetGattServicesAsync(BluetoothCacheMode::Uncached)).then(
                [=](GattDeviceServicesResult^result) {
                    if (result->Status == GattCommunicationStatus::Success) {
                        std::vector<task<GattCharacteristicsResult ^ >> find_gattchar_tasks;
                        for (uint x = 0; x < result->Services->Size; ++x) {
                            auto service = result->Services->GetAt(x);
                            m_services.emplace(service->Uuid, service);
                            find_gattchar_tasks.push_back(create_task(service->GetCharacteristicsAsync(BluetoothCacheMode::Uncached)));
                        }
                        return when_all(std::begin(find_gattchar_tasks), std::end(find_gattchar_tasks));
                    }
                    qWarning() << "Failed to discover Gatt service";
                    throw ref new Platform::Exception(-1, "Failed to discover Gatt service");
                }).then([=](std::vector<GattCharacteristicsResult ^ > results) {
            for (auto it : results) {
                if (it->Status == GattCommunicationStatus::Success) {
                    for (uint x = 0; x < it->Characteristics->Size; ++x) {
                        auto chr = it->Characteristics->GetAt(x);
                        m_characterstics.emplace(chr->Uuid, chr);
                    }
                } else {
                    qWarning() << "Failed to discover gatt charactersitic (status = " << static_cast<int>(it->Status) << ")";
                    throw ref new Platform::Exception(-1, "Failed to discover gatt charactersitic");
                }
            }

            qDebug() << "Configuring Mbientsensor";
            MblMwBtleConnection btleConnection;
            btleConnection.context = this;
            btleConnection.write_gatt_char = write_gatt_char;
            btleConnection.read_gatt_char = read_gatt_char;
            btleConnection.enable_notifications = enable_char_notify;
            btleConnection.on_disconnect = on_disconnect;
            this->m_metaWearBoard = mbl_mw_metawearboard_create(&btleConnection);
            this->configureHandlers();
        }).wait();
    }
    catch (Platform::Exception^e) {
        qWarning() << QString::fromWCharArray(e->Message->Data());

        for (auto it : m_characterstics)
            delete it.second;
        for (auto it : m_services)
            delete it.second;

        m_characterstics.clear();
        m_services.clear();
        if (m_device != nullptr) {
            delete m_device;
            m_device = nullptr;
        }
        emit disconnected();
    }
}

MetawearWrapper::~MetawearWrapper() {

}


