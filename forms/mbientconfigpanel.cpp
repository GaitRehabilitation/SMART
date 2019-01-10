//
// Created by michaelpollind on 21/12/18.
//

#include "forms/mbientconfigpanel.h"
#include "ui_mbientconfigpanel.h"

#include "QCheckBox"
#include "mbientconfigpanel.h"
#include "common/metawearwrapperbase.h"

#include <metawear/sensor/gyro_bmi160.h>
#include <metawear/sensor/accelerometer.h>

#include <forms/deviceselectdialog.h>
#include <QtCore/QTimer>
#include <QtWidgets/QMessageBox>

#include "common/metawearwrapper.h"
#include "common/DiscoveryAgent.h"

const float MbientConfigPanel::ACC_ODR_RANGE[] = {12.5f, 50.f,100.0f,200.0f};
const float MbientConfigPanel::ACC_FSR_RANGE[] = {2.0f, 4.0f, 8.0f, 16.0f};
const float MbientConfigPanel::FUSION_RANGE[] = {8.0f,12.5f,50.0f,100.0f};


MbientConfigPanel::MbientConfigPanel(QWidget *parent) : ui(new Ui::MbientConfigPanel){
    ui->setupUi(this);

    ui->accelerationContainer->setEnabled(false);
    ui->gyroscopeContainer->setEnabled(false);
    ui->quaternionContainer->setEnabled(false);
    ui->linearAccelerationContainer->setEnabled(false);
    ui->eularAngleContainer->setEnabled(false);

    connect(ui->deviceFind,&QPushButton::clicked,this,[=](){
        DeviceSelectDialog dialog(this);
        connect(&dialog,&DeviceSelectDialog::onBluetoothDeviceAccepted,this,[=](const BluetoothAddress &info){
            ui->deviceMac->setText(info.getMac());
        });
        dialog.exec();
    });

    connect(ui->removeDevice,&QPushButton::clicked,this,[=](){
        this->deleteLater();
    });

    connect(ui->toggleAcc,&QCheckBox::stateChanged,this,[=](int state){
         ui->accelerationContainer->setEnabled(ui->toggleAcc->isChecked());
        this->updateDisabled();
    });

    connect(ui->toggleGyro,&QCheckBox::stateChanged,this,[=](int state){
        ui->gyroscopeContainer->setEnabled(ui->toggleGyro->isChecked());
        this->updateDisabled();
    });

    connect(ui->toggleQuaternion,&QCheckBox::stateChanged,this,[=](int state){
        ui->quaternionContainer->setEnabled(ui->toggleQuaternion->isChecked());
        this->updateDisabled();
    });

    connect(ui->toggleLinearAcc,&QCheckBox::stateChanged,this,[=](int state){
        ui->linearAccelerationContainer->setEnabled(ui->toggleLinearAcc->isChecked());
        this->updateDisabled();
    });

    connect(ui->toggleEularAngles,&QCheckBox::stateChanged,this,[=](int state){
        ui->eularAngleContainer->setEnabled(ui->toggleEularAngles->isChecked());
        this->updateDisabled();
    });

    connect(ui->slideAccSample,&QSlider::valueChanged,this,[=](int value){
        ui->labelAccSample->setText(QString("%1 Hz").arg(QString::number(toAccSampleIndex(value),10,2)));
    });
    ui->slideAccSample->valueChanged(0);

    connect(ui->slideAccRange,&QSlider::valueChanged,this,[=](int value){
        ui->labelAccRange->setText(QString("%1 g's").arg(QString::number(toAccRangeIndex(value),10,2)));
    });
    ui->slideAccRange->valueChanged(0);

    connect(ui->slideGyroRange,&QSlider::valueChanged,this,[=](int value){
        QString result = QString("%1 °/s");

        switch(toGyroRangeFromIndex(value)){
            case MBL_MW_GYRO_BMI160_RANGE_2000dps:
                ui->labelGyroRange->setText(result.arg("2000"));
                break;
            case MBL_MW_GYRO_BMI160_RANGE_1000dps:
                ui->labelGyroRange->setText(result.arg("1000"));
                break;
            case MBL_MW_GYRO_BMI160_RANGE_500dps:
                ui->labelGyroRange->setText(result.arg("500"));
                break;
            case MBL_MW_GYRO_BMI160_RANGE_250dps:
                ui->labelGyroRange->setText(result.arg("250"));
                break;
            case MBL_MW_GYRO_BMI160_RANGE_125dps:
                ui->labelGyroRange->setText(result.arg("125"));
                break;
        }
    });
    ui->slideGyroRange->valueChanged(0);

    connect(ui->slideGyroSample,&QSlider::valueChanged,this,[=](int value){
        QString result = QString("%1 Hz");

        switch(toGyroSampleFromIndex(value)){
            case MBL_MW_GYRO_BMI160_ODR_25Hz:
                ui->labelGyroSample->setText(result.arg("25"));
                break;
            case MBL_MW_GYRO_BMI160_ODR_50Hz:
                ui->labelGyroSample->setText(result.arg("50"));
                break;
            case MBL_MW_GYRO_BMI160_ODR_100Hz:
                ui->labelGyroSample->setText(result.arg("100"));
                break;
            case MBL_MW_GYRO_BMI160_ODR_200Hz:
                ui->labelGyroSample->setText(result.arg("200"));
                break;
            case MBL_MW_GYRO_BMI160_ODR_400Hz:
                ui->labelGyroSample->setText(result.arg("400"));
                break;
            case MBL_MW_GYRO_BMI160_ODR_800Hz:
                ui->labelGyroSample->setText(result.arg("800"));
                break;
            case MBL_MW_GYRO_BMI160_ODR_1600Hz:
                ui->labelGyroSample->setText(result.arg("1600"));
                break;
            case MBL_MW_GYRO_BMI160_ODR_3200Hz:
                ui->labelGyroSample->setText(result.arg("3200"));
                break;
        }
    });
    ui->slideGyroSample->valueChanged(0);

    connect(ui->slideQuaternionSample,&QSlider::valueChanged,this,[=](int value){
        ui->labelQuaternionSample->setText(QString("%1 Hz").arg(toFusionSampleRangeIndex(value)));
    });
    ui->slideQuaternionSample->valueChanged(0);

    connect(ui->slideEularSample,&QSlider::valueChanged,this,[=](int value){
        ui->labelEularSample->setText(QString("%1 Hz").arg(toFusionSampleRangeIndex(value)));
    });
    ui->slideEularSample->valueChanged(0);

    connect(ui->slideLinearAccSample,&QSlider::valueChanged,this,[=](int value){
        ui->labelLinearAccSample->setText(QString("%1 Hz").arg(toFusionSampleRangeIndex(value)));
    });
    ui->slideLinearAccSample->valueChanged(0);
}



void MbientConfigPanel::updateDisabled() {
    if (isSensorFusionEnabled()) {
        ui->mainAccelerationContainer->hide();
        ui->mainGyroscopeContainer->hide();
    } else {
        ui->mainAccelerationContainer->show();
        ui->mainGyroscopeContainer->show();
    }
}

bool MbientConfigPanel::isSensorFusionEnabled(){
    return ui->toggleEularAngles->isChecked() || ui->toggleLinearAcc->isChecked() || ui->toggleQuaternion->isChecked();
}

QVariantMap MbientConfigPanel::serialize(){
    QVariantMap map;

    map.insert(MAC,ui->deviceMac->text());
    map.insert(NAME,ui->deviceName->text());
    if(isSensorFusionEnabled()){
        if(ui->toggleEularAngles->isChecked()){
            QVariantMap entries;
            entries.insert(SAMPLE_RATE,ui->slideEularSample->value());
            map.insert(FUSION_EULAR_ANGLES,entries);
        }
        if(ui->toggleLinearAcc->isChecked())
        {
            QVariantMap entries;
            entries.insert(SAMPLE_RATE,ui->slideLinearAccSample->value());
            map.insert(FUSION_LINEAR_ACC,entries);

        }
        if(ui->toggleQuaternion->isChecked()){
            QVariantMap entries;
            entries.insert(SAMPLE_RATE,ui->slideQuaternionSample->value());
            map.insert(FUSION_QUATERNION,entries);
        }
    } else{
        if(ui->toggleGyro->isChecked()){
            QVariantMap entries;
            entries.insert(SAMPLE_RANGE,ui->slideGyroRange->value());
            entries.insert(SAMPLE_RATE,ui->slideGyroSample->value());
            map.insert(GYRO,entries);
        }

        if(ui->toggleAcc->isChecked()){
            QVariantMap entries;
            entries.insert(SAMPLE_RANGE,ui->slideAccRange->value());
            entries.insert(SAMPLE_RATE,ui->slideAccSample->value());
            map.insert(ACC,entries);
        }
    }
    return map;
}



void MbientConfigPanel::deserialize(QVariantMap value)
{
    if(value.contains(FUSION_EULAR_ANGLES)){
        QVariantMap s = value[FUSION_EULAR_ANGLES].toMap();
        ui->slideEularSample->setValue(s[SAMPLE_RATE].toInt());
    }
    if(value.contains(FUSION_LINEAR_ACC)){
        QVariantMap s = value[FUSION_LINEAR_ACC].toMap();
        ui->slideLinearAccSample->setValue(s[SAMPLE_RATE].toInt());
    }
    if(value.contains(FUSION_QUATERNION)){
        QVariantMap s = value[FUSION_QUATERNION].toMap();
        ui->slideQuaternionSample->setValue(s[SAMPLE_RATE].toInt());
    }
    if(value.contains(GYRO)){
        QVariantMap s = value[GYRO].toMap();
        ui->slideGyroRange->setValue(s[SAMPLE_RANGE].toInt());
        ui->slideGyroSample->setValue(s[SAMPLE_RATE].toInt());
    }
    if(value.contains(ACC)){
        QVariantMap s = value[ACC].toMap();
        ui->slideAccRange->setValue(s[SAMPLE_RANGE].toInt());
        ui->slideAccSample->setValue(s[SAMPLE_RATE].toInt());
    }
    ui->deviceMac->setText(value[MAC].toString());
    ui->deviceName->setText(value[NAME].toString());
}


MetawearWrapperBase* MbientConfigPanel::buildWrapper(){

    // clear wrapper
    m_wrapper = nullptr;

    DiscoveryAgent discoveryAgent;
    connect(&discoveryAgent, &DiscoveryAgent::deviceDiscovered, this,[this](BluetoothAddress info) {
        if (QString::compare(info.getMac(), ui->deviceMac->text(), Qt::CaseInsensitive)) {
            m_wrapper = new MetawearWrapper(info);

            MetawearWrapper* lwrapper = m_wrapper;

            bool enableGyro = ui->toggleGyro->isChecked();
            bool enableAcc = ui->toggleAcc->isChecked();
            bool hasSensorFusion = isSensorFusionEnabled();

            MblMwGyroBmi160Range gyrorange = toGyroRangeFromIndex(ui->slideGyroRange->value());
            MblMwGyroBmi160Odr gyroSample = toGyroSampleFromIndex(ui->slideGyroSample->value());

            float accRange = toAccRangeIndex(ui->slideAccRange->value());
            float accSample = toAccSampleIndex(ui->slideAccSample->value());

            connect(this->m_wrapper,&MetawearWrapperBase::metawareInitialized,[=](){
                if(!hasSensorFusion) {
                    if (enableGyro)
                        lwrapper->configureGyroscope(gyrorange, gyroSample);
                    if(enableAcc)
                        lwrapper->configureAccelerometer(accRange,accSample);
                }
            });

            connect(this->m_wrapper, &MetawearWrapperBase::postMetawearInitialized,[=](){
                if(!hasSensorFusion) {
                    if (enableGyro)
                        lwrapper->startGyroscopeCapture();
                    if (enableAcc)
                        lwrapper->startAccelerationCapture();
                }
            });

        }
    });
    QTimer timer;
    timer.setSingleShot(true);

    QEventLoop loop;
    connect(&discoveryAgent,&DiscoveryAgent::finished,&loop,&QEventLoop::quit);
    connect(&timer,&QTimer::timeout,&loop,&QEventLoop::quit);

    // run timer for 10 seconds and if no deivce does not show up then kill it
    timer.start(10000);
    discoveryAgent.start();

    loop.exec();
    if(m_wrapper == nullptr){
        QMessageBox messageBox;
        messageBox.setText("Could not find a device with the mac:" + ui->deviceMac->text() + "\n would you like to try searching for the device again?");
        messageBox.setFixedSize(500,200);
        QAbstractButton* lbuttonYes = messageBox.addButton(tr("Yes"),QMessageBox::YesRole);
        messageBox.addButton(tr("No"),QMessageBox::NoRole);
        messageBox.exec();
        if(messageBox.clickedButton() == lbuttonYes){
            // run wrapper again and try to init wrapper
            return buildWrapper();
        }
    }

    return m_wrapper;
}

MblMwGyroBmi160Range MbientConfigPanel::toGyroRangeFromIndex(int index){
    return (MblMwGyroBmi160Range)(5 - index);
}
MblMwGyroBmi160Odr MbientConfigPanel::toGyroSampleFromIndex(int index){
    return (MblMwGyroBmi160Odr)(6 + index);
}

float MbientConfigPanel::toAccRangeIndex(int index) {
    if(index < 0 || index > 3){
        return 0;
    }
    return MbientConfigPanel::ACC_ODR_RANGE[index];
}

float MbientConfigPanel::toAccSampleIndex(int index) {
    if(index < 0 || index > 3){
        return 0;
    }
    return MbientConfigPanel::ACC_FSR_RANGE[index];
}

float MbientConfigPanel::toFusionSampleRangeIndex(int index) {
    if(index < 0 || index > 3){
        return 0;
    }
    return FUSION_RANGE[index];
}
