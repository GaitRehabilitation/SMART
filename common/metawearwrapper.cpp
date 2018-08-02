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
#include <QBluetoothSocket>
#include <QBluetoothUuid>
#include <QByteArray>
#include <QLowEnergyController>
#include <QObject>
#include <QThread>

quint128 convertQuint128(uint8_t *low, uint8_t *high) {
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


    QBluetoothUuid service_uuid = QBluetoothUuid(convertQuint128((uint8_t *)&characteristic->service_uuid_low,(uint8_t *)&characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(convertQuint128((uint8_t *)&characteristic->uuid_low,(uint8_t *)&characteristic->uuid_high));

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

void MetawearWrapper::write_gatt_char_qt(void *context, const void *caller, MblMwGattCharWriteType writeType, const MblMwGattChar *characteristic, const uint8_t *value, uint8_t length) {
    MetawearWrapper *wrapper = (MetawearWrapper *)context;
    QBluetoothUuid service_uuid = QBluetoothUuid( convertQuint128((uint8_t *)&characteristic->service_uuid_low, (uint8_t *)&characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(convertQuint128((uint8_t *)&characteristic->uuid_low, (uint8_t *)&characteristic->uuid_high));

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


void MetawearWrapper::enable_char_notify_qt(void *context, const void *caller, const MblMwGattChar *characteristic, MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready) {
    MetawearWrapper *wrapper = (MetawearWrapper *)context;
    wrapper->m_notificationHandler = handler;

    QBluetoothUuid service_uuid = QBluetoothUuid(convertQuint128((uint8_t *)&characteristic->service_uuid_low, (uint8_t *)&characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(convertQuint128((uint8_t *)&characteristic->uuid_low, (uint8_t *)&characteristic->uuid_high));

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
    service->connect(service, &QLowEnergyService::descriptorWritten, temp, [caller, ready, temp](const QLowEnergyDescriptor &descriptor, const QByteArray &newValue) {
        ready(caller, MBL_MW_STATUS_OK);
        temp->deleteLater();
    });
}

void MetawearWrapper::on_disconnect_qt(void *context, const void *caller, MblMwFnVoidVoidPtrInt handler) {
    MetawearWrapper *wrapper = (MetawearWrapper *)context;
    wrapper->m_disconnectedHandler = handler;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
// Class
// -------------------------------------------------------------------------------------------------------------------------------------------------------------

MetawearWrapper::MetawearWrapper(const QBluetoothDeviceInfo &device,
                                 QObject *parent)
    : QObject(parent),
      m_services(QMap<QString, QLowEnergyService *>()),
      m_controller(nullptr),
      m_serviceReady(0),
      m_isMetawareReady(0),
      m_readyCharacteristicCount(0),
      m_notificationHandler(nullptr),
      m_disconnectedHandler(nullptr),
      m_readGattHandler(nullptr),
      m_isSensorEnabled(0),
      m_laststEpoch(0),
      m_currentDevice(device){

    MblMwBtleConnection btleConnection;
    btleConnection.context = this;
    btleConnection.write_gatt_char = write_gatt_char_qt;
    btleConnection.read_gatt_char = read_gatt_char_qt;
    btleConnection.enable_notifications = enable_char_notify_qt;
    btleConnection.on_disconnect = on_disconnect_qt;
    this->m_metaWearBoard = mbl_mw_metawearboard_create(&btleConnection);

    resetControllerAndTryAgain();
}



void MetawearWrapper::resetControllerAndTryAgain()
{
    if(this->m_controller){
        this->m_controller->deleteLater();
    }
    m_controller = QLowEnergyController::createCentral(m_currentDevice,this);

    // Service Discovery
    connect(this->m_controller, &QLowEnergyController::serviceDiscovered, this,[=](QBluetoothUuid newService){
        QString uuid = newService.toString();
        QLowEnergyService *lowEnergyService = this->m_controller->createServiceObject(newService, this);
        m_services.insert(uuid, lowEnergyService);

        lowEnergyService->connect(lowEnergyService,&QLowEnergyService::characteristicRead, this,[=](QLowEnergyCharacteristic characteristic, QByteArray payload){
            if (this->m_readGattHandler != nullptr) {
                this->m_readGattHandler(this->m_metaWearBoard, (uint8_t *)payload.data(),payload.length());
            }
        });
        lowEnergyService->connect(lowEnergyService,&QLowEnergyService::characteristicChanged, this,[=](QLowEnergyCharacteristic characteristic, QByteArray payload){
            if (this->m_notificationHandler != nullptr) {
                this->m_notificationHandler(this->m_metaWearBoard,(uint8_t *)payload.data(), payload.length());
            }
        });

        lowEnergyService->connect(lowEnergyService,QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, [=](QLowEnergyService::ServiceError e){
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


    connect(this->m_controller, &QLowEnergyController::discoveryFinished, this,[=](){
        foreach (QString key, this->m_services.keys()) {
            QLowEnergyService *lowEnergyService = this->m_services.value(key);
            lowEnergyService->connect( lowEnergyService, &QLowEnergyService::stateChanged, [this, lowEnergyService](QLowEnergyService::ServiceState state) {
                if (state == QLowEnergyService::ServiceDiscovered) {
                    this->m_readyCharacteristicCount++;
                    if (this->m_readyCharacteristicCount == this->m_services.count()) {
                        this->subscribeMetawearHandlers();
                    }
                }
            });
            lowEnergyService->discoverDetails();
        }
    });

    // controller connection/disconnect
    connect(this->m_controller, &QLowEnergyController::connected, this,[=](){
        qDebug() << "Controller conected. Search services..";
        this->m_controller->discoverServices();
        emit connected();
    });
    connect(this->m_controller,&QLowEnergyController::disconnected,this,[=](){
        qDebug() << "LowEnergy controller disconnected";
        emit disconnected();
    });

    // controller error
    connect(this->m_controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, [=](QLowEnergyController::Error e){
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
        emit onControllerError(e);
    });

    if (m_controller->state() == QLowEnergyController::UnconnectedState) {
        qDebug() << "Starting connection";
        m_controller->connectToDevice();
    } else {
        qDebug() << "Controller already connected. Search services..";
        this->m_controller->discoverServices();
    }
}

MetawearWrapper::~MetawearWrapper() {
    mbl_mw_metawearboard_free(m_metaWearBoard);
}




void MetawearWrapper::updateEpoch(qint64 epoch)
{
    if(epoch > m_laststEpoch)
    {
        m_laststEpoch = epoch;
        emit lastEpoch(m_laststEpoch);
    }
}


void MetawearWrapper::readBatteryStatus() {
    auto battery_signal =
            mbl_mw_settings_get_battery_state_data_signal(m_metaWearBoard);
    mbl_mw_datasignal_read(battery_signal);
}

void MetawearWrapper::setAccelerationSamplerate(float range,float sample) {
    mbl_mw_acc_set_odr(m_metaWearBoard, sample);
    mbl_mw_acc_set_range(m_metaWearBoard,range);
    mbl_mw_acc_write_acceleration_config(m_metaWearBoard);
}

void MetawearWrapper::setAmbientLightSamplerate(float sample) {
    static const std::vector<float> MBL_MW_ALS_LTR329_RATE_VALUES = {
        50.0f, 100.0f, 200, 500, 1000, 2000};
    int index = closest_index(MBL_MW_ALS_LTR329_RATE_VALUES, sample);
    mbl_mw_als_ltr329_set_measurement_rate(m_metaWearBoard,(MblMwAlsLtr329MeasurementRate)index);
    mbl_mw_als_ltr329_write_config(m_metaWearBoard);
}

void MetawearWrapper::setGyroSamplerate(MblMwGyroBmi160Range range, MblMwGyroBmi160Odr sample) {
    mbl_mw_gyro_bmi160_set_range(m_metaWearBoard,range);
    mbl_mw_gyro_bmi160_set_odr(m_metaWearBoard, sample);
    mbl_mw_gyro_bmi160_write_config(m_metaWearBoard);
}

void MetawearWrapper::setMagnetometerRate(float value) {
    mbl_mw_mag_bmm150_enable_b_field_sampling(m_metaWearBoard);
}

void MetawearWrapper::setAccelerationCapture(bool enable) {
    if (enable) {
        mbl_mw_acc_enable_acceleration_sampling(m_metaWearBoard);
        mbl_mw_acc_start(m_metaWearBoard);
    } else {
        mbl_mw_acc_disable_acceleration_sampling(m_metaWearBoard);
        mbl_mw_acc_stop(m_metaWearBoard);
    }
}

void MetawearWrapper::setAmbientLightCapture(bool enable) {
    if (enable) {
        mbl_mw_als_ltr329_start(m_metaWearBoard);
    } else {
        mbl_mw_als_ltr329_stop(m_metaWearBoard);
    }
}

void MetawearWrapper::setGyroCapture(bool enable) {
    if (enable) {
        mbl_mw_gyro_bmi160_enable_rotation_sampling(m_metaWearBoard);
        mbl_mw_gyro_bmi160_start(m_metaWearBoard);
    } else {
        mbl_mw_gyro_bmi160_disable_rotation_sampling(m_metaWearBoard);
        mbl_mw_gyro_bmi160_stop(m_metaWearBoard);
    }
}

void MetawearWrapper::setMagnetometerCapture(bool enable) {
    if (enable) {
        mbl_mw_mag_bmm150_enable_b_field_sampling(m_metaWearBoard);
        mbl_mw_mag_bmm150_start(m_metaWearBoard);
    } else {
        mbl_mw_mag_bmm150_disable_b_field_sampling(m_metaWearBoard);
        mbl_mw_mag_bmm150_stop(m_metaWearBoard);
    }
}

void MetawearWrapper::setBarometerCapture(bool enable) {
    if (enable) {
        mbl_mw_baro_bosch_start(m_metaWearBoard);
    } else {
        mbl_mw_als_ltr329_stop(m_metaWearBoard);
    }
}


void MetawearWrapper::subscribeMetawearHandlers() {
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
                auto battery_signal = mbl_mw_settings_get_battery_state_data_signal(wrapper->m_metaWearBoard);
                mbl_mw_datasignal_subscribe(battery_signal, wrapper,[](void *context, const MblMwData *data) -> void {
                    MetawearWrapper *wrapper = (MetawearWrapper *)context;
                    auto state = (MblMwBatteryState *)data->value;
                    emit wrapper->batteryPercentage(state->charge);
                    emit wrapper->voltage(state->voltage);
                });


                // subscribe to acceleration handler
                auto acc_signal = mbl_mw_acc_get_high_freq_acceleration_data_signal(wrapper->m_metaWearBoard);
                mbl_mw_datasignal_subscribe(acc_signal, wrapper,[](void *context, const MblMwData *data) -> void {
                    MetawearWrapper *wrapper = (MetawearWrapper *)context;
                    auto acceleration = (MblMwCartesianFloat *)data->value;
                    emit wrapper->acceleration(data->epoch, acceleration->x,acceleration->y,acceleration->z);
                    wrapper->updateEpoch(data->epoch);
                });
                // subscribe to gyro handler
                auto gyroSignal = mbl_mw_gyro_bmi160_get_rotation_data_signal(wrapper->m_metaWearBoard);
                mbl_mw_datasignal_subscribe(gyroSignal, wrapper,[](void *context, const MblMwData *data) -> void {
                    MetawearWrapper *wrapper = (MetawearWrapper *)context;
                    auto rotRate = (MblMwCartesianFloat *)data->value;
                    emit wrapper->gyro(data->epoch, rotRate->x, rotRate->y,rotRate->z);
                    wrapper->updateEpoch(data->epoch);
                });


                // subscribe to ambient handler
                auto ambientLightSignal = mbl_mw_als_ltr329_get_illuminance_data_signal(wrapper->m_metaWearBoard);
                mbl_mw_datasignal_subscribe(ambientLightSignal, wrapper,[](void *context, const MblMwData *data) -> void {
                    MetawearWrapper *wrapper = (MetawearWrapper *)context;
                    emit wrapper->ambientLight(data->epoch,*((uint32_t *)data->value));
                    wrapper->updateEpoch(data->epoch);
                });



                auto magnetometerSignal =mbl_mw_mag_bmm150_get_b_field_data_signal(wrapper->m_metaWearBoard);
                mbl_mw_datasignal_subscribe(magnetometerSignal, wrapper,[](void *context, const MblMwData *data) -> void {
                    MetawearWrapper *wrapper = (MetawearWrapper *)context;
                    auto bfield = (MblMwCartesianFloat *)data->value;
                    emit wrapper->magnetometer(data->epoch, bfield->x,bfield->y, bfield->z);
                    wrapper->updateEpoch(data->epoch);
                });

                emit wrapper->onMetawareInitialized();
            } else {
                qWarning() << "Error initializing board:" << status;
                emit wrapper->onMetawareFailedToInitialized(status);
            }
        });
        this->m_isMetawareReady = true;
    }
}




qint64 MetawearWrapper::getLatestEpoch()
{
    return m_laststEpoch;
}


