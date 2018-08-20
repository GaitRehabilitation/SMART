/**
* Copyright %yyyy% GaitRehabilitation
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

#include "common/metawearwrapperbase.h"

#include "metawear/platform/memory.h"
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

#include <QDebug>


MetawearWrapperBase::MetawearWrapperBase():
    m_metaWearBoard(nullptr),
    m_firmwareVersion(""),
    m_model(""),
    m_isMetawearReady(false),
    m_latestEpoch(0)
{
}

void MetawearWrapperBase::configureMagnetometer(MblMwMagBmm150Preset preset)
{
    mbl_mw_mag_bmm150_set_preset(m_metaWearBoard,preset);
}

void MetawearWrapperBase::readBatteryStatus()
{
    auto battery_signal = mbl_mw_settings_get_battery_state_data_signal(m_metaWearBoard);
    mbl_mw_datasignal_read(battery_signal);
}

void MetawearWrapperBase::configureAccelerometer(float range,float sample){
    mbl_mw_acc_set_odr(m_metaWearBoard, sample);
    mbl_mw_acc_set_range(m_metaWearBoard,range);
    mbl_mw_acc_write_acceleration_config(m_metaWearBoard);
}

void MetawearWrapperBase::configureGyroscope(MblMwGyroBmi160Range range, MblMwGyroBmi160Odr sample){
   mbl_mw_gyro_bmi160_set_odr(m_metaWearBoard, sample);
   mbl_mw_gyro_bmi160_set_range(m_metaWearBoard, range);
   mbl_mw_gyro_bmi160_write_config(m_metaWearBoard);
}

void MetawearWrapperBase::configureAmbientLight(MblMwAlsLtr329Gain gain, MblMwAlsLtr329IntegrationTime integrationTime, MblMwAlsLtr329MeasurementRate measurementRate)
{
    mbl_mw_als_ltr329_set_gain(m_metaWearBoard, gain);
    mbl_mw_als_ltr329_set_integration_time(m_metaWearBoard, integrationTime);
    mbl_mw_als_ltr329_set_measurement_rate(m_metaWearBoard, measurementRate);
    mbl_mw_als_ltr329_write_config(m_metaWearBoard);
}

void MetawearWrapperBase::startAccelerationCapture(){
    mbl_mw_acc_enable_acceleration_sampling(m_metaWearBoard);
    mbl_mw_acc_start(m_metaWearBoard);
}

void MetawearWrapperBase::startAmbientLightCapture(){
    mbl_mw_als_ltr329_start(m_metaWearBoard);
}

void MetawearWrapperBase::startGyroscopeCapture(){
    mbl_mw_gyro_bmi160_enable_rotation_sampling(m_metaWearBoard);
    mbl_mw_gyro_bmi160_start(m_metaWearBoard);
}

void MetawearWrapperBase::startMagnetometerCapture(){
    mbl_mw_mag_bmm150_enable_b_field_sampling(m_metaWearBoard);
    mbl_mw_mag_bmm150_start(m_metaWearBoard);
}

void MetawearWrapperBase::stopAccelerationCapture(){
    mbl_mw_acc_disable_acceleration_sampling(m_metaWearBoard);
    mbl_mw_acc_stop(m_metaWearBoard);
}

void MetawearWrapperBase::stopAmbientLightCapture(){
    mbl_mw_als_ltr329_stop(m_metaWearBoard);
}

void MetawearWrapperBase::stopGyroscopeCapture(){
    mbl_mw_gyro_bmi160_disable_rotation_sampling(m_metaWearBoard);
    mbl_mw_gyro_bmi160_stop(m_metaWearBoard);
}

void MetawearWrapperBase::stopMagnetometerCapture() {
    mbl_mw_mag_bmm150_disable_b_field_sampling(m_metaWearBoard);
    mbl_mw_mag_bmm150_stop(m_metaWearBoard);
}

void MetawearWrapperBase::configureHandlers() {


    mbl_mw_metawearboard_initialize(m_metaWearBoard, this,
                                    [](void *context, MblMwMetaWearBoard *board, int32_t status) -> void {
                                        auto *wrapper = static_cast<MetawearWrapperBase *>(context);

                                        if (!status) {

                                            auto dev_info = mbl_mw_metawearboard_get_device_information(
                                                    wrapper->m_metaWearBoard);
                                            qDebug() << "firmware = " << dev_info->firmware_revision;
                                            wrapper->m_firmwareVersion = dev_info->firmware_revision;
                                            wrapper->m_model = QString(dev_info->model_number);
                                            mbl_mw_memory_free((void *) dev_info);
                                            qDebug() << "model = " << wrapper->m_model;

                                            if (!wrapper->m_isMetawearReady) {
                                                emit wrapper->metawareInitialized();
                                                emit wrapper->postMetawearInitialized();
                                            }
                                            wrapper->m_isMetawearReady = true;
                                            qDebug() << "Board initialized";
                                        } else {
                                            emit wrapper->metawareFailedToInitialized(status);
                                            qDebug() << "Error initializing board: " << status;
                                        }
                                    });


    connect(this, &MetawearWrapperBase::metawareInitialized, [=]() {
        auto battery_signal = mbl_mw_settings_get_battery_state_data_signal(m_metaWearBoard);
        mbl_mw_datasignal_subscribe(battery_signal, this, [](void *context, const MblMwData *data) -> void {
            MetawearWrapperBase *wrapper = static_cast<MetawearWrapperBase *>(context);
            auto state = static_cast<MblMwBatteryState *>(data->value);

            emit wrapper->batteryPercentage(state->charge);
            emit wrapper->voltage(state->voltage);
        });

        // subscribe to acceleration handler
        auto acc_signal = mbl_mw_acc_get_packed_acceleration_data_signal(m_metaWearBoard);
        mbl_mw_datasignal_subscribe(acc_signal, this, [](void *context, const MblMwData *data) -> void {
            MetawearWrapperBase *wrapper = static_cast<MetawearWrapperBase *>(context);
            auto acceleration = static_cast<MblMwCartesianFloat *>(data->value);
            emit wrapper->acceleration(data->epoch, acceleration->x, acceleration->y, acceleration->z);
            wrapper->updateEpoch(data->epoch);
        });
        // subscribe to gyro handler
        auto gyroSignal = mbl_mw_gyro_bmi160_get_packed_rotation_data_signal(m_metaWearBoard);
        mbl_mw_datasignal_subscribe(gyroSignal, this, [](void *context, const MblMwData *data) -> void {
            MetawearWrapperBase *wrapper = static_cast<MetawearWrapperBase *>(context);
            auto rotRate = (MblMwCartesianFloat *) data->value;
            emit wrapper->gyroscope(data->epoch, rotRate->x, rotRate->y, rotRate->z);
            wrapper->updateEpoch(data->epoch);
        });
    });
}

void MetawearWrapperBase::updateEpoch(qint64 epoch)
{
    if(epoch > m_latestEpoch)
    {
        m_latestEpoch = epoch;
        emit latestEpoch(m_latestEpoch);
    }
}

bool MetawearWrapperBase::isMetawearReady(){
    return m_isMetawearReady;
}

QString MetawearWrapperBase::getFirmwareVersion(){
    return m_firmwareVersion;
}

QString MetawearWrapperBase::getModel(){
    return m_model;
}

qint64 MetawearWrapperBase::getLatestEpoch()
{
    return m_latestEpoch;
}

QString MetawearWrapperBase::getHost() const {
    return m_host;
}

QString MetawearWrapperBase::getTarget() const {
    return m_target;
}
