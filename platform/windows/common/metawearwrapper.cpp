//
// Created by michaelpollind on 8/4/18.
//

#include <3rdparty/mbientlab/src/metawear/core/status.h>
#include "common/metawearwrapper.h"

#include "common/util.h"
#include <vector>

using namespace Windows::Foundation;


//auto leDevice = co_await  winrt::Windows::Devices::Bluetooth::BluetoothDevice::FromBluetoothAddressAsync(0);


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

}

void MetawearWrapper::write_gatt_char(void *context, const void *caller, MblMwGattCharWriteType writeType,
                                        const MblMwGattChar *characteristic, const uint8_t *value, uint8_t length) {
  
}

void MetawearWrapper::enable_char_notify(void *context, const void *caller, const MblMwGattChar *characteristic,
                                           MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready) {
   
}

void MetawearWrapper::on_disconnect(void *context, const void *caller, MblMwFnVoidVoidPtrInt handler) {

}

MetawearWrapper::MetawearWrapper(const QBluetoothHostInfo &local,const QBluetoothDeviceInfo &target):
	MetawearWrapperBase::MetawearWrapperBase() {

   // Windows::Devices::Bluetooth::GenericAttributeProfile::Guid g;

    task_completion_event<void> discover_device_event;
    task<void> event_set(discover_device_event);
	
    //	auto leDevice = co_await Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(bluetoothAddress);
    create_task(BluetoothLEDevice::FromBluetoothAddressAsync(target.address().toUInt64(), BluetoothAddressType::Public)).then([=](BluetoothLEDevice^ leDevice) {
        if (leDevice == nullptr) {
            qWarning() << "Failed to discover device";
        } else {
            leDevice->ConnectionStatusChanged += ref new TypedEventHandler<BluetoothLEDevice^, Platform::Object^>([=](BluetoothLEDevice^ sender, Platform::Object^ args) {
                switch(sender->ConnectionStatus){
                    case BluetoothConnectionStatus::Disconnected:

                        break;
                }
            });
            this->m_device = leDevice;
            discover_device_event.set();
        }
    });

    event_set.then([=](){
        return create_task(this->m_device->GetGattServicesAsync(BluetoothCacheMode::Uncached));
    }).then([=](GattDeviceServicesResult^ result){
        if(result->Status == GattCommunicationStatus::Success){
            std::vector<task<GattCharacteristicsResult^>> find_gattchar_tasks;
            for(uint x = 0; x < result->Services->Size; ++x){
                auto service = result->Services->GetAt(x);
                m_services.emplace(service->Uuid, service);
                find_gattchar_tasks.push_back(create_task(service->GetCharacteristicsAsync(BluetoothCacheMode::Uncached)));
            }
            return when_all(std::begin(find_gattchar_tasks),std::end(find_gattchar_tasks));
        }
    }).then([=](std::vector<GattCharacteristicsResult^> results){
        for(auto it: results){
            if(it->Status == GattCommunicationStatus::Success){
                for(uint x = 0; x < it->Characteristics->Size; ++x){
                    auto chr = it->Characteristics->GetAt(x);
                    m_characterstics.emplace(chr->Uuid,chr);
                }
            }else{
                qWarning() << "Failed to discover gatt charactersitic (status = " << static_cast<int>(it->Status) << ")";
            }
        }

        MblMwBtleConnection btleConnection;
        btleConnection.context = this;
        btleConnection.write_gatt_char = write_gatt_char;
        btleConnection.read_gatt_char = read_gatt_char;
        btleConnection.enable_notifications = enable_char_notify;
        btleConnection.on_disconnect = on_disconnect;
        this->m_metaWearBoard = mbl_mw_metawearboard_create(&btleConnection);
    });
}
