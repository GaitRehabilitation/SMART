//
// Created by Michael on 8/27/2018.
//

#include "common/DiscoveryAgent.h"


DiscoveryAgent::DiscoveryAgent() : BaseDiscoveryAgent(){
    Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);

    CoInitializeSecurity(
            nullptr, // TODO: "O:BAG:BAD:(A;;0x7;;;PS)(A;;0x3;;;SY)(A;;0x7;;;BA)(A;;0x3;;;AC)(A;;0x3;;;LS)(A;;0x3;;;NS)"
            -1,
            nullptr,
            nullptr,
            RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IDENTIFY,
            NULL,
            EOAC_NONE,
            nullptr);

    m_bleAdvertisementWatcher = ref new Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher();
    m_bleAdvertisementWatcher->ScanningMode = Bluetooth::Advertisement::BluetoothLEScanningMode::Active;
    m_bleAdvertisementWatcher->Received += ref new Windows::Foundation::TypedEventHandler<Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher ^, Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs ^>(
        [=](Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher ^watcher, Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs^ eventArgs){

    }
}

void DiscoveryAgent::start() {

}

DiscoveryAgent::~DiscoveryAgent() {

}

