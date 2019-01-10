//
// Created by michaelpollind on 8/4/18.
//

#include <3rdparty/mbientlab/src/metawear/core/status.h>
#include "common/metawearwrapper.h"
#include "metawearwrapper.h"

#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyService>
#include <QtBluetooth/QBluetoothUuid>

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
            MetawearWrapper::convertTo128(characteristic->service_uuid_low, characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(
            MetawearWrapper::convertTo128(characteristic->uuid_low, characteristic->uuid_high));

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

bool MetawearWrapper::isConnected() const {
    return !m_controller;
}

MetawearWrapper::MetawearWrapper(const BluetoothAddress &target):
    MetawearWrapperBase::MetawearWrapperBase(target) ,
    m_readyCharacteristicCount(0),
    m_readGattHandler(nullptr),
    m_disconnectedHandler(nullptr),
    m_notificationHandler(nullptr) {
    m_controller = new QLowEnergyController(QBluetoothAddress(target.toUint64()), this);

    MblMwBtleConnection btleConnection;
    btleConnection.context = this;
    btleConnection.write_gatt_char = write_gatt_char;
    btleConnection.read_gatt_char = read_gatt_char;
    btleConnection.enable_notifications = enable_char_notify;
    btleConnection.on_disconnect = on_disconnect;
    this->m_metaWearBoard = mbl_mw_metawearboard_create(&btleConnection);


    // Service Discovery
    connect(this->m_controller, &QLowEnergyController::serviceDiscovered, this, [=](QBluetoothUuid newService) {
        QString uuid = newService.toString();
        QLowEnergyService *lowEnergyService = this->m_controller->createServiceObject(newService, this);
        m_services.insert(uuid, lowEnergyService);

        connect(lowEnergyService, &QLowEnergyService::characteristicRead, this,
                [=](QLowEnergyCharacteristic characteristic, QByteArray payload) {
                    if (this->m_readGattHandler != nullptr) {
                        this->m_readGattHandler(this->m_metaWearBoard, (uint8_t *) payload.data(),
                                                static_cast<uint8_t>(payload.length()));
                    }
                });
        connect(lowEnergyService, &QLowEnergyService::characteristicChanged, this,
                [=](QLowEnergyCharacteristic characteristic, QByteArray payload) {
                    if (this->m_notificationHandler != nullptr) {
                        this->m_notificationHandler(this->m_metaWearBoard, (uint8_t *) payload.data(),
                                                    static_cast<uint8_t>(payload.length()));
                    }
                });

        connect(lowEnergyService, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this,
                [=](QLowEnergyService::ServiceError e) {
                    switch (e) {
                        case QLowEnergyService::ServiceError::UnknownError:
                            qWarning() << "Characteristic UnknownError";
                            break;
                        case QLowEnergyService::ServiceError::OperationError:
                            qWarning() << "Characteristic OperationError";
                            break;
                        case QLowEnergyService::ServiceError::CharacteristicReadError:
                            qWarning() << "Characteristic CharacteristicReadError";
                            break;
                        case QLowEnergyService::ServiceError::CharacteristicWriteError:
                            qWarning() << "Characteristic CharacteristicWriteError";
                            break;
                        case QLowEnergyService::ServiceError::DescriptorReadError:
                            qWarning() << "Characteristic DescriptorReadError";
                            break;
                        case QLowEnergyService::ServiceError::DescriptorWriteError:
                            qWarning() << "Characteristic DescriptorWriteError";
                            break;
                        case QLowEnergyService::ServiceError::NoError:
                            break;
                    }
                });

        qDebug() << "Service Name: " << lowEnergyService->serviceName();
        qDebug() << "Service UUID: " << lowEnergyService->serviceUuid().toString();
    });


    connect(this->m_controller, &QLowEnergyController::discoveryFinished, this, [=]() {
                foreach (QString key, this->m_services.keys()) {
                QLowEnergyService *lowEnergyService = this->m_services.value(key);
                connect(lowEnergyService, &QLowEnergyService::stateChanged,
                        [this, lowEnergyService](QLowEnergyService::ServiceState state) {
                            if (state == QLowEnergyService::ServiceDiscovered) {
                                this->m_readyCharacteristicCount++;
                                if (this->m_readyCharacteristicCount == this->m_services.count()) {
                                    configureHandlers();
                                }
                            }
                        });
                lowEnergyService->discoverDetails();
            }
    });

    // controller connection/disconnect
    connect(this->m_controller, &QLowEnergyController::connected, this, [=]() {
        qDebug() << "Controller connected. Search services..";
        this->m_controller->discoverServices();
        emit connected();
    });
    connect(this->m_controller, &QLowEnergyController::disconnected, this, [=]() {
        qDebug() << "LowEnergy controller disconnected";
        emit disconnected();
    });

    // controller error
    connect(this->m_controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this,
            [=](QLowEnergyController::Error e) {
                switch (e) {
                    case QLowEnergyController::Error::UnknownError:
                        qWarning() << "Controller UnknownError";
                        break;
                    case QLowEnergyController::Error::UnknownRemoteDeviceError:
                        qWarning() << "Controller UnknownRemoteDeviceError";
                        break;
                    case QLowEnergyController::Error::NetworkError:
                        qWarning() << "Controller NetworkError";
                        break;
                    case QLowEnergyController::Error::InvalidBluetoothAdapterError:
                        qWarning() << "Controller InvalidBluetoothAdapterError";
                        break;
                    case QLowEnergyController::Error::ConnectionError:
                        qWarning() << "Controller ConnectionError";
                        break;
                    case QLowEnergyController::Error::AdvertisingError:
                        qWarning() << "Controller AdvertisingError";
                        break;
                    case QLowEnergyController::NoError:
                        break;
                    case QLowEnergyController::RemoteHostClosedError:break;
                }
            });

}

MetawearWrapper::~MetawearWrapper() {

}

void MetawearWrapper::connectToDevice() {
    if (m_controller->state() == QLowEnergyController::UnconnectedState) {
        qDebug() << "Starting connection";
        m_controller->connectToDevice();
    } else {
        qDebug() << "Controller already connected. Search services..";
        this->m_controller->discoverServices();
    }
}
