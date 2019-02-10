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

#ifndef SMART_MBIENTCONFIGPANEL_H
#define SMART_MBIENTCONFIGPANEL_H


#include <QWidget>
#include <QVariantMap>
#include <3rdparty/mbientlab/src/metawear/sensor/gyro_bmi160.h>

#define GYRO "gyro"
#define MAC "mac"
#define NAME "name"
#define ACC "acc"
#define FUSION_QUATERNION "fusion_quaternion"
#define FUSION_LINEAR_ACC "fusion_linear_acc"
#define FUSION_EULAR_ANGLES "fusion_eular_angles"

#define SAMPLE_RATE "sample_rate"
#define SAMPLE_RANGE "sample_range"

class MetawearWrapperBase;
class MetawearWrapper;
namespace Ui {
    class MbientConfigPanel;
}
class MbientConfigPanel : public QWidget {
    Q_OBJECT
private:
    Ui::MbientConfigPanel *ui;
    void updateDisabled();
    MetawearWrapper* m_wrapper;
public:
    static const float ACC_ODR_RANGE[];
    static const float ACC_FSR_RANGE[];
    static const float FUSION_RANGE[];

    static MblMwGyroBmi160Range toGyroRangeFromIndex(int index);
    static MblMwGyroBmi160Odr toGyroSampleFromIndex(int index);

    static float toAccRangeIndex(int index);
    static float toAccSampleIndex(int index);
    static float toFusionSampleRangeIndex(int index);

    QString getMac();
    QString getName();

    MetawearWrapperBase* buildWrapper();

    MbientConfigPanel(QWidget *parent = nullptr);
    QVariantMap serialize();
    void deserialize(QVariantMap map);
    bool isSensorFusionEnabled();

};


#endif //SMART_MBIENTCONFIGPANEL_H
