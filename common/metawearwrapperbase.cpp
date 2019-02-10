/**
* Copyright 2019 GaitRehabilitation
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
#include "metawear/sensor/sensor_fusion.h"

#include <QDebug>
#include <3rdparty/mbientlab/src/metawear/processor/time.h>


MetawearWrapperBase::MetawearWrapperBase(const BluetoothAddress& address):
    m_metaWearBoard(nullptr),
    m_firmwareVersion(""),
    m_model(""),
    m_isMetawearReady(false),
    m_latestEpoch(0),
    m_target(address)
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

/**
 * Configure acceleration
 * @param range
 * @param sample
 */
void MetawearWrapperBase::configureAccelerometer(float range,float sample){
    mbl_mw_acc_set_odr(m_metaWearBoard, sample);
    mbl_mw_acc_set_range(m_metaWearBoard,range);
    mbl_mw_acc_write_acceleration_config(m_metaWearBoard);
}

/**
 * Configure sensor fusion
 * @param mode
 * @param acc
 * @param gyro
 */
void MetawearWrapperBase::configureFusion(MblMwSensorFusionMode mode,MblMwSensorFusionAccRange acc,MblMwSensorFusionGyroRange gyro){
    mbl_mw_sensor_fusion_set_mode(m_metaWearBoard,mode);
    mbl_mw_sensor_fusion_set_acc_range(m_metaWearBoard,acc);
    mbl_mw_sensor_fusion_set_gyro_range(m_metaWearBoard,gyro);
    mbl_mw_sensor_fusion_write_config(m_metaWearBoard);
}

/**
 * configure Metawear gyroscope with range and sample
 * @param range
 * @param sample
 */
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

    // subscribe to acceleration handler
    auto acc_signal = mbl_mw_acc_get_packed_acceleration_data_signal(m_metaWearBoard);

    mbl_mw_datasignal_unsubscribe(acc_signal);
    mbl_mw_datasignal_subscribe(acc_signal, this, [](void *context, const MblMwData *data) -> void {
        MetawearWrapperBase *wrapper = static_cast<MetawearWrapperBase *>(context);
        auto acceleration = static_cast<MblMwCartesianFloat *>(data->value);
        emit wrapper->acceleration(data->epoch, acceleration->x, acceleration->y, acceleration->z);
        wrapper->updateEpoch(data->epoch);
    });

    mbl_mw_acc_enable_acceleration_sampling(m_metaWearBoard);
    mbl_mw_acc_start(m_metaWearBoard);
}

void MetawearWrapperBase::startAmbientLightCapture(){
    mbl_mw_als_ltr329_start(m_metaWearBoard);
}

void MetawearWrapperBase::startGyroscopeCapture(){
    // subscribe to gyro handler
    auto gyroSignal = mbl_mw_gyro_bmi160_get_packed_rotation_data_signal(m_metaWearBoard);
    mbl_mw_datasignal_unsubscribe(gyroSignal);
    mbl_mw_datasignal_subscribe(gyroSignal, this, [](void *context, const MblMwData *data) -> void {
        MetawearWrapperBase *wrapper = static_cast<MetawearWrapperBase *>(context);
        auto rotRate = (MblMwCartesianFloat *) data->value;
        emit wrapper->gyroscope(data->epoch, rotRate->x, rotRate->y, rotRate->z);
        wrapper->updateEpoch(data->epoch);
    });

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

                                            emit wrapper->metawareInitialized();
                                            emit wrapper->postMetawearInitialized();
                                            emit wrapper->connected();
                                            qDebug() << "Board initialized";
                                        } else {
                                            emit wrapper->metawareFailedToInitialized(status);
                                            qDebug() << "Error initializing board: " << status;
                                        }
                                    });


    connect(this, &MetawearWrapperBase::metawareInitialized, [=]() {
        mbl_mw_settings_set_connection_parameters(m_metaWearBoard, 7.5f, 7.5f, 0, 6000);
        auto battery_signal = mbl_mw_settings_get_battery_state_data_signal(m_metaWearBoard);
        mbl_mw_datasignal_subscribe(battery_signal, this, [](void *context, const MblMwData *data) -> void {
            MetawearWrapperBase *wrapper = static_cast<MetawearWrapperBase *>(context);
            auto state = static_cast<MblMwBatteryState *>(data->value);

            emit wrapper->batteryPercentage(state->charge);
            emit wrapper->voltage(state->voltage);
        });
    });
}

void MetawearWrapperBase::startQuaternionCapture(float sample_rate){
    auto quaternion_signal = mbl_mw_sensor_fusion_get_data_signal(m_metaWearBoard, MBL_MW_SENSOR_FUSION_DATA_QUATERNION);
    mbl_mw_datasignal_unsubscribe(quaternion_signal);
    //mbl_mw_dataprocessor_time_create(quaternion_signal, MBL_MW_TIME_ABSOLUTE,static_cast<uint32_t>((1.0f / sample_rate) * 1000), this, [](void *context, MblMwDataProcessor* processor) -> void{
        mbl_mw_datasignal_subscribe(quaternion_signal,this,[](void *context, const MblMwData *data) -> void {
            auto wrapper = static_cast<MetawearWrapperBase *>(context);
            auto quaternion = static_cast<MblMwQuaternion*>(data->value);
            emit wrapper->quaternion(data->epoch,quaternion->x,quaternion->y,quaternion->z,quaternion->w);
            wrapper->updateEpoch(data->epoch);
        });
    //});
    mbl_mw_sensor_fusion_enable_data(m_metaWearBoard, MBL_MW_SENSOR_FUSION_DATA_QUATERNION);
    mbl_mw_sensor_fusion_start(m_metaWearBoard);
}

void MetawearWrapperBase::linearEulerAngleCapture(float sample_rate){
    auto linearEualr_signal = mbl_mw_sensor_fusion_get_data_signal(m_metaWearBoard, MBL_MW_SENSOR_FUSION_DATA_EULER_ANGLE);
    mbl_mw_datasignal_unsubscribe(linearEualr_signal);
    //mbl_mw_dataprocessor_time_create(quaternion_signal, MBL_MW_TIME_ABSOLUTE,static_cast<uint32_t>((1.0f / sample_rate) * 1000), this, [](void *context, MblMwDataProcessor* processor) -> void{
        mbl_mw_datasignal_subscribe(linearEualr_signal ,this,[](void *context, const MblMwData *data) -> void {
            auto wrapper = static_cast<MetawearWrapperBase *>(context);
            auto result = static_cast<MblMwEulerAngles*>(data->value);
            emit wrapper->eularAngles(data->epoch,result->heading,result->pitch,result->roll,result->yaw);
            wrapper->updateEpoch(data->epoch);
        });
    //});
    mbl_mw_sensor_fusion_enable_data(m_metaWearBoard, MBL_MW_SENSOR_FUSION_DATA_EULER_ANGLE);
    mbl_mw_sensor_fusion_start(m_metaWearBoard);
}

void MetawearWrapperBase::linearAccelerationCapture(float sample_rate){
    auto quaternion_signal = mbl_mw_sensor_fusion_get_data_signal(m_metaWearBoard, MBL_MW_SENSOR_FUSION_DATA_LINEAR_ACC);
    mbl_mw_datasignal_unsubscribe(quaternion_signal);
    //mbl_mw_dataprocessor_time_create(quaternion_signal, MBL_MW_TIME_ABSOLUTE,static_cast<uint32_t>((1.0f / sample_rate) * 1000), this, [](void *context, MblMwDataProcessor* processor) -> void{
        mbl_mw_datasignal_subscribe(quaternion_signal,this,[](void *context, const MblMwData *data) -> void {
            auto wrapper = static_cast<MetawearWrapperBase *>(context);
            auto result = static_cast<MblMwCartesianFloat*>(data->value);
            emit wrapper->linearAcceleration(data->epoch,result->x,result->y,result->z);
            wrapper->updateEpoch(data->epoch);
        });
    //});
    mbl_mw_sensor_fusion_enable_data(m_metaWearBoard, MBL_MW_SENSOR_FUSION_DATA_LINEAR_ACC);
    mbl_mw_sensor_fusion_start(m_metaWearBoard);
}


void MetawearWrapperBase::updateEpoch(qint64 epoch)
{
    if(epoch > m_latestEpoch)
    {
        m_latestEpoch = epoch;
        emit latestEpoch(m_latestEpoch);
    }
}


const QString& MetawearWrapperBase::getFirmwareVersion() const{
    return m_firmwareVersion;
}

const QString& MetawearWrapperBase::getModel() const{
    return m_model;
}

/**
 * the current epoch in milliseconds from the start of capture
 * @return
 */
qint64 MetawearWrapperBase::getLatestEpoch()
{
    return m_latestEpoch;
}


const BluetoothAddress& MetawearWrapperBase::getTarget() const {
    return m_target;
}
