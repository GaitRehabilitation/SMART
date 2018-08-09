//
// Created by michaelpollind on 8/4/18.
//

#include <3rdparty/mbientlab/src/metawear/core/status.h>
#include "common/metawearwrapper.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>

#include <ppltasks.h>


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
	//	auto leDevice = co_await Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(bluetoothAddress);
	
	
}
