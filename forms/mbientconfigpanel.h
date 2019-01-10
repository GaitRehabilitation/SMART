//
// Created by michaelpollind on 21/12/18.
//

#ifndef SMART_MBIENTCONFIGPANEL_H
#define SMART_MBIENTCONFIGPANEL_H

#include <QWidget>
#include <QVariantMap>
#include <3rdparty/mbientlab/src/metawear/sensor/gyro_bmi160.h>

namespace Ui {
    class MbientConfigPanel;
}

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
