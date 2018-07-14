/**
* Copyright 2018 GaitRehabilitation
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

#include "metawearwrapper.h"
#include "metawear/core/data.h"
#include "metawear/core/datasignal.h"
#include "metawear/core/metawearboard.h"
#include "metawear/core/settings.h"
#include "metawear/core/status.h"
#include "metawear/core/types.h"
#include "metawear/platform/memory.h"
#include "metawear/sensor/ambientlight_ltr329.h"
#include "metawear/sensor/barometer_bosch.h"
#include "metawear/sensor/cpp/utils.h"
#include "metawear/sensor/gyro_bmi160.h"
#include "metawear/sensor/magnetometer_bmm150.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QByteArray>
#include <QLowEnergyController>
#include <QObject>
#include <QThread>

quint128 MetawearWrapper::convertQuint128(uint8_t *low, uint8_t *high) {
    quint128 result;
    for (int i = 0; i < 8; i++) {
        result.data[i] = high[7 - i];
    }
    for (int i = 0; i < 8; i++) {
        result.data[i + 8] = low[7 - i];
    }
    return result;
}

void MetawearWrapper::read_gatt_char_qt(void *context, const void *caller,
                                        const MblMwGattChar *characteristic,
                                        MblMwFnIntVoidPtrArray handler) {
    MetawearWrapper *wrapper = (MetawearWrapper *)context;
    wrapper->m_readGattHandler = handler;

    QBluetoothUuid service_uuid = QBluetoothUuid(
                convertQuint128((uint8_t *)&characteristic->service_uuid_low,
                                (uint8_t *)&characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid =
            QBluetoothUuid(convertQuint128((uint8_t *)&characteristic->uuid_low,
                                           (uint8_t *)&characteristic->uuid_high));

    QLowEnergyService *service =
            wrapper->m_services.value(service_uuid.toString());
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

void MetawearWrapper::write_gatt_char_qt(void *context, const void *caller,
                                         MblMwGattCharWriteType writeType,
                                         const MblMwGattChar *characteristic,
                                         const uint8_t *value, uint8_t length) {
    MetawearWrapper *wrapper = (MetawearWrapper *)context;
    QBluetoothUuid service_uuid = QBluetoothUuid(
                convertQuint128((uint8_t *)&characteristic->service_uuid_low,
                                (uint8_t *)&characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid =
            QBluetoothUuid(convertQuint128((uint8_t *)&characteristic->uuid_low,
                                           (uint8_t *)&characteristic->uuid_high));

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

void MetawearWrapper::enable_char_notify_qt(void *context, const void *caller,
                                            const MblMwGattChar *characteristic,
                                            MblMwFnIntVoidPtrArray handler,
                                            MblMwFnVoidVoidPtrInt ready) {
    MetawearWrapper *wrapper = (MetawearWrapper *)context;
    wrapper->m_notificationHandler = handler;

    QBluetoothUuid service_uuid = QBluetoothUuid(
                convertQuint128((uint8_t *)&characteristic->service_uuid_low,
                                (uint8_t *)&characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid =
            QBluetoothUuid(convertQuint128((uint8_t *)&characteristic->uuid_low,
                                           (uint8_t *)&characteristic->uuid_high));

    QLowEnergyService *service =
            wrapper->m_services.value(service_uuid.toString());
    if (service == nullptr) {
        qWarning() << "failed to parse service uuid(char notify):"
                   << service_uuid.toString();
        return;
    }
    QLowEnergyCharacteristic c = service->characteristic(characteristic_uuid);
    if (!c.isValid()) {
        qWarning() << "Invalid characteristic Uuid(char notify)"
                   << characteristic_uuid.toString();
    }

    QLowEnergyDescriptor notification =
            c.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    if (!notification.isValid())
        return;

    service->writeDescriptor(notification, QByteArray::fromHex("0100"));
    QObject *temp = new QObject();
    service->connect(service, &QLowEnergyService::descriptorWritten, temp,
                     [caller, ready, temp](const QLowEnergyDescriptor &descriptor,
                     const QByteArray &newValue) {
        ready(caller, MBL_MW_STATUS_OK);
        temp->deleteLater();
    });
}

void MetawearWrapper::on_disconnect_qt(void *context, const void *caller,
                                       MblMwFnVoidVoidPtrInt handler) {
    MetawearWrapper *wrapper = (MetawearWrapper *)context;
    wrapper->m_disconnectedHandler = handler;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
// Class
// -------------------------------------------------------------------------------------------------------------------------------------------------------------

MetawearWrapper::MetawearWrapper(const QBluetoothDeviceInfo &device,
                                 QObject *parent)
    : QObject(parent), m_services(QMap<QString, QLowEnergyService *>()),
      m_controller(0), m_serviceReady(0), m_isMetawareReady(0),
      m_readyCharacteristicCount(0), m_notificationHandler(0),
      m_disconnectedHandler(0), m_readGattHandler(0), m_isSensorEnabled(0) {

    this->m_currentDevice = device;

    // Disconnect and delete old connection
    if (m_controller) {
        m_controller->disconnectFromDevice();
        delete m_controller;
        m_controller = 0;
    }

    m_controller = new QLowEnergyController(m_currentDevice, this);

    // Service Discovery
    connect(this->m_controller, SIGNAL(serviceDiscovered(QBluetoothUuid)), this,
            SLOT(onServiceDiscovered(QBluetoothUuid)));
    connect(this->m_controller, SIGNAL(discoveryFinished()), this,
            SLOT(onServiceDiscoveryFinished()));

    // controller connection/disconnect
    connect(this->m_controller, SIGNAL(disconnected()), this,
            SLOT(onDisconnect()));
    connect(this->m_controller, SIGNAL(connected()), this, SLOT(onConnected()));

    // controller error
    connect(this->m_controller, SIGNAL(error(QLowEnergyController::Error)), this,
            SLOT(onControllerError(QLowEnergyController::Error)));

    MblMwBtleConnection btleConnection;
    btleConnection.context = this;
    btleConnection.write_gatt_char = write_gatt_char_qt;
    btleConnection.read_gatt_char = read_gatt_char_qt;
    btleConnection.enable_notifications = enable_char_notify_qt;
    btleConnection.on_disconnect = on_disconnect_qt;
    this->m_metaWearBoard = mbl_mw_metawearboard_create(&btleConnection);

    if (m_controller->state() == QLowEnergyController::UnconnectedState) {
        qDebug() << "Starting connection";
        m_controller->connectToDevice();
    } else {
        qDebug() << "Controller already connected. Search services..";
        this->m_controller->discoverServices();
    }
}

void MetawearWrapper::readBatteryStatus() {
    auto battery_signal =
            mbl_mw_settings_get_battery_state_data_signal(this->getBoard());
    mbl_mw_datasignal_read(battery_signal);
}

void MetawearWrapper::setAccelerationSamplerate(float sample) {
    mbl_mw_acc_set_odr(this->getBoard(), sample);
    mbl_mw_acc_write_acceleration_config(this->getBoard());
}

void MetawearWrapper::setAmbientLightSamplerate(float sample) {
    static const std::vector<float> MBL_MW_ALS_LTR329_RATE_VALUES = {
        50.0f, 100.0f, 200, 500, 1000, 2000};
    int index = closest_index(MBL_MW_ALS_LTR329_RATE_VALUES, sample);
    mbl_mw_als_ltr329_set_measurement_rate(this->getBoard(),
                                           (MblMwAlsLtr329MeasurementRate)index);
    mbl_mw_als_ltr329_write_config(this->getBoard());
}

void MetawearWrapper::setGyroSamplerate(float value) {
    static const std::vector<float> MBL_MW_GYRO_BMI160_RATE_VALUES = {
        25.0f, 50.0f, 100.0f, 200.0f, 400.0f, 800.0f, 1600.0f, 3200.0f};
    int index = closest_index(MBL_MW_GYRO_BMI160_RATE_VALUES, value);
    mbl_mw_gyro_bmi160_set_odr(this->getBoard(), (MblMwGyroBmi160Odr)(6 + index));
    mbl_mw_gyro_bmi160_write_config(this->getBoard());
}

void MetawearWrapper::setMagnetometerRate(float value) {
    mbl_mw_mag_bmm150_enable_b_field_sampling(this->getBoard());
    mbl_mw_mag_bmm150_start(this->getBoard());
}

void MetawearWrapper::setAccelerationCapture(bool enable) {
    if (enable) {
        mbl_mw_acc_enable_acceleration_sampling(this->getBoard());
        mbl_mw_acc_start(this->getBoard());
    } else {
        mbl_mw_acc_disable_acceleration_sampling(this->getBoard());
        mbl_mw_acc_stop(this->getBoard());
    }
}

void MetawearWrapper::setAmbientLightCapture(bool enable) {
    if (enable) {
        mbl_mw_als_ltr329_start(this->getBoard());
    } else {
        mbl_mw_als_ltr329_stop(this->getBoard());
    }
}

void MetawearWrapper::setGyroCapture(bool enable) {
    if (enable) {
        mbl_mw_gyro_bmi160_enable_rotation_sampling(this->getBoard());
        mbl_mw_gyro_bmi160_start(this->getBoard());
    } else {
        mbl_mw_gyro_bmi160_disable_rotation_sampling(this->getBoard());
        mbl_mw_gyro_bmi160_stop(this->getBoard());
    }
}

void MetawearWrapper::setMagnetometerCapture(bool enable) {
    if (enable) {
        mbl_mw_mag_bmm150_enable_b_field_sampling(this->getBoard());
        mbl_mw_mag_bmm150_start(this->getBoard());
    } else {
        mbl_mw_mag_bmm150_disable_b_field_sampling(this->getBoard());
        mbl_mw_mag_bmm150_stop(this->getBoard());
    }
}

void MetawearWrapper::setBarometerCapture(bool enable) {
    if (enable) {
        mbl_mw_baro_bosch_start(this->getBoard());
    } else {
        mbl_mw_als_ltr329_stop(this->getBoard());
    }
}

void MetawearWrapper::onServiceDiscovered(const QBluetoothUuid &newService) {
    QString uuid = newService.toString();
    QLowEnergyService *lowEnergyService =
            this->m_controller->createServiceObject(newService, this);
    m_services.insert(uuid, lowEnergyService);

    lowEnergyService->connect(
                lowEnergyService,
                SIGNAL(characteristicRead(QLowEnergyCharacteristic, QByteArray)), this,
                SLOT(onCharacteristicRead(QLowEnergyCharacteristic, QByteArray)));
    lowEnergyService->connect(
                lowEnergyService,
                SIGNAL(characteristicChanged(QLowEnergyCharacteristic, QByteArray)), this,
                SLOT(
                    onCharacteristicNotifications(QLowEnergyCharacteristic, QByteArray)));

    lowEnergyService->connect(
                lowEnergyService, SIGNAL(error(QLowEnergyService::ServiceError)), this,
                SLOT(onCharacteristicError(QLowEnergyService::ServiceError)));

    qDebug() << "Service Name: " << lowEnergyService->serviceName();
    qDebug() << "Service UUID: " << lowEnergyService->serviceUuid().toString();
}

void MetawearWrapper::onCharacteristicRead(
        QLowEnergyCharacteristic characteristic, QByteArray payload) {
    if (this->m_readGattHandler != nullptr) {
        this->m_readGattHandler(this->m_metaWearBoard, (uint8_t *)payload.data(),
                                payload.length());
    }
}

void MetawearWrapper::onCharacteristicNotifications(
        QLowEnergyCharacteristic characteristic, QByteArray payload) {
    if (this->m_notificationHandler != nullptr) {
        this->m_notificationHandler(this->m_metaWearBoard,
                                    (uint8_t *)payload.data(), payload.length());
    }
}

void MetawearWrapper::onServiceDiscoveryFinished() {
    foreach (QString key, this->m_services.keys()) {
        QLowEnergyService *lowEnergyService = this->m_services.value(key);
        lowEnergyService->connect(
                    lowEnergyService, &QLowEnergyService::stateChanged,
                    [this, lowEnergyService](QLowEnergyService::ServiceState state) {
            if (state == QLowEnergyService::ServiceDiscovered) {
                this->m_readyCharacteristicCount++;
                if (this->m_readyCharacteristicCount == this->m_services.count()) {
                    this->metwareIntialize();
                }
            }
        });
        lowEnergyService->discoverDetails();
    }
}

void MetawearWrapper::metwareIntialize() {
    if (this->m_isMetawareReady == false) {
        mbl_mw_metawearboard_initialize(
                    this->m_metaWearBoard, this,
                    [](void *context, MblMwMetaWearBoard *board, int32_t status) -> void {
            MetawearWrapper *wrapper = (MetawearWrapper *)context;
            if (status == MBL_MW_STATUS_OK) {
                wrapper->m_isSensorEnabled = true;

                qDebug() << "Board initialized";
                auto dev_info = mbl_mw_metawearboard_get_device_information(board);
                qDebug() << "firmware = " << dev_info->firmware_revision;
                mbl_mw_memory_free((void *)dev_info);
                qDebug() << "model = " << mbl_mw_metawearboard_get_model(board);

                // subscribe batter handler
                auto battery_signal = mbl_mw_settings_get_battery_state_data_signal(
                            wrapper->getBoard());
                mbl_mw_datasignal_subscribe(
                            battery_signal, wrapper,
                            [](void *context, const MblMwData *data) -> void {
                    MetawearWrapper *wrapper = (MetawearWrapper *)context;
                    auto state = (MblMwBatteryState *)data->value;
                    emit wrapper->onBatteryPercentage(state->charge);
                    emit wrapper->onVoltage(state->voltage);
                });

                // subscribe to acceleration handler
                auto acc_signal =
                        mbl_mw_acc_get_high_freq_acceleration_data_signal(wrapper->getBoard());
                mbl_mw_datasignal_subscribe(
                            acc_signal, wrapper,
                            [](void *context, const MblMwData *data) -> void {
                    MetawearWrapper *wrapper = (MetawearWrapper *)context;
                    auto acceleration = (MblMwCartesianFloat *)data->value;
                    emit wrapper->onAcceleration(data->epoch, acceleration->x,
                                                 acceleration->y,
                                                 acceleration->z);
                    emit wrapper->onEpoch(data->epoch);
                });

                // subscribe to ambient handler
                auto ambientLightSignal =
                        mbl_mw_als_ltr329_get_illuminance_data_signal(
                            wrapper->getBoard());
                mbl_mw_datasignal_subscribe(
                            ambientLightSignal, wrapper,
                            [](void *context, const MblMwData *data) -> void {
                    MetawearWrapper *wrapper = (MetawearWrapper *)context;
                    emit wrapper->onAmbientLight(data->epoch,
                                                 *((uint32_t *)data->value));
                    emit wrapper->onEpoch(data->epoch);
                });

                // subscribe to gyro handler
                auto gyroSignal =
                        mbl_mw_gyro_bmi160_get_packed_rotation_data_signal(
                            wrapper->getBoard());
                mbl_mw_datasignal_subscribe(
                            gyroSignal, wrapper,
                            [](void *context, const MblMwData *data) -> void {
                    MetawearWrapper *wrapper = (MetawearWrapper *)context;
                    auto rotRate = (MblMwCartesianFloat *)data->value;
                    emit wrapper->onGyro(data->epoch, rotRate->x, rotRate->y,
                                         rotRate->z);
                    emit wrapper->onEpoch(data->epoch);
                });

                auto magnetometerSignal =
                        mbl_mw_mag_bmm150_get_b_field_data_signal(wrapper->getBoard());
                mbl_mw_datasignal_subscribe(
                            magnetometerSignal, wrapper,
                            [](void *context, const MblMwData *data) -> void {
                    MetawearWrapper *wrapper = (MetawearWrapper *)context;
                    auto bfield = (MblMwCartesianFloat *)data->value;
                    emit wrapper->onMagnetometer(data->epoch, bfield->x,
                                                 bfield->y, bfield->z);
                    emit wrapper->onEpoch(data->epoch);
                });

                emit wrapper->metawareInitialized();
            } else {
                qWarning() << "Error initializing board:" << status;
                emit wrapper->metawareFailedToInitialized(status);
            }
        });
        this->m_isMetawareReady = true;
    }
}

void MetawearWrapper::onConnected() {
    qDebug() << "Controller conected. Search services..";
    this->m_controller->discoverServices();
    emit connected();
}

void MetawearWrapper::onDisconnect() {
    qDebug() << "LowEnergy controller disconnected";
    emit disconnected();
}

void MetawearWrapper::onControllerError(QLowEnergyController::Error e) {
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
    }
}

void MetawearWrapper::onStateChange(
        QLowEnergyController::ControllerState state) {
    switch (state) {
    case QLowEnergyController::UnconnectedState:
        qDebug() << "UnconnectedState";
        break;
    case QLowEnergyController::ConnectingState:
        qDebug() << "ConnectingState";
        break;
    case QLowEnergyController::ConnectedState:
        qDebug() << "ConnectedState";
        break;
    case QLowEnergyController::DiscoveringState:
        qDebug() << "DiscoveringState";
        break;
    case QLowEnergyController::DiscoveredState:
        qDebug() << "DiscoveredState";
        break;
    case QLowEnergyController::ClosingState:
        qDebug() << "ClosingState";
        break;
    case QLowEnergyController::AdvertisingState:
        qDebug() << "AdvertisingState";
        break;
    }

}

void MetawearWrapper::onCharacteristicError(QLowEnergyService::ServiceError e) {
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
}

MblMwMetaWearBoard *MetawearWrapper::getBoard() { return m_metaWearBoard; }

MetawearWrapper::~MetawearWrapper() {


}

QLowEnergyController *MetawearWrapper::getController() {
    return this->m_controller;
}
