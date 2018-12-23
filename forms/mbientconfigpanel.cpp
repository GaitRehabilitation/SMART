//
// Created by michaelpollind on 21/12/18.
//

#include "forms/mbientconfigpanel.h"
#include "ui_mbientconfigpanel.h"

#include "QCheckBox"

#include <metawear/sensor/gyro_bmi160.h>
#include <metawear/sensor/accelerometer.h>

#include <forms/deviceselectdialog.h>

MbientConfigPanel::MbientConfigPanel(QWidget *parent) : ui(new Ui::MbientConfigPanel){
    ui->setupUi(this);

    ui->accelerationContainer->setEnabled(false);
    ui->gyroscopeContainer->setEnabled(false);
    ui->quaternionContainer->setEnabled(false);
    ui->linearAccelerationContainer->setEnabled(false);
    ui->eularAngleContainer->setEnabled(false);

    connect(ui->deviceFind,&QPushButton::clicked,this,[=](){
        DeviceSelectDialog dialog(this);
        connect(&dialog,&DeviceSelectDialog::onBluetoothDeviceAccepted,this[=](const BluetoothAddress &info){

        });

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
        ui->labelAccSample->setText(QString("%1 Hz").arg(QString::number(ACC_ODR_RANGE[value],10,2)));
    });
    ui->slideAccSample->valueChanged(0);

    connect(ui->slideAccRange,&QSlider::valueChanged,this,[=](int value){
        ui->labelAccRange->setText(QString("%1 g's").arg(QString::number(ACC_FSR_RANGE[value],10,2)));
    });
    ui->slideAccRange->valueChanged(0);

    connect(ui->slideGyroRange,&QSlider::valueChanged,this,[=](int value){
        QString result = QString("%1 °/s");

        switch((MblMwGyroBmi160Range)(5 - value)){
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

        switch((MblMwGyroBmi160Odr)(6 + value)){
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
        ui->labelQuaternionSample->setText(QString("%1 Hz").arg(FUSION_RANGE[value]));
    });
    ui->slideQuaternionSample->valueChanged(0);

    connect(ui->slideEularSample,&QSlider::valueChanged,this,[=](int value){
        ui->labelEularSample->setText(QString("%1 Hz").arg(FUSION_RANGE[value]));
    });
    ui->slideEularSample->valueChanged(0);

    connect(ui->slideLinearAccSample,&QSlider::valueChanged,this,[=](int value){
        ui->labelLinearAccSample->setText(QString("%1 Hz").arg(FUSION_RANGE[value]));
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

QVariantMap MbientConfigPanel::getConfig(){
    QVariantMap map;

    map.insert("mac",ui->deviceMac->text());
    map.insert("name",ui->deviceName->text());
    if(isSensorFusionEnabled()){
        if(ui->toggleEularAngles->isChecked()){
            QVariantMap entries;
            entries.insert("sample_rate",ui->slideEularSample->value());
            map.insert("fusion_eular_angles",entries);
        }
        if(ui->toggleLinearAcc->isChecked())
        {
            QVariantMap entries;
            entries.insert("sample_rate",ui->slideLinearAccSample->value());
            map.insert("fusion_linear_acc",entries);

        }
        if(ui->toggleQuaternion->isChecked()){

            QVariantMap entries;
            entries.insert("sample_rate",ui->slideQuaternionSample->value());
            map.insert("fusion_quaternion",entries);
        }
    } else{
        if(ui->toggleGyro->isChecked()){
            QVariantMap entries;
            entries.insert("sample_range",ui->slideGyroRange->value());
            entries.insert("sample_rate",ui->slideGyroSample->value());
            map.insert("gyro",entries);
        }

        if(ui->toggleAcc->isChecked()){
            QVariantMap entries;
            entries.insert("sample_range",ui->slideAccRange->value());
            entries.insert("sample_rate",ui->slideAccSample->value());
            map.insert("acc",entries);
        }
    }
    return map;
}



void MbientConfigPanel::setConfig(QVariantMap value)
{
    if(value.contains("fusion_eular_angles")){
        QVariantMap s = value["fusion_eular_angles"].toMap();
        ui->slideEularSample->setValue(s["sample_rate"].toFloat());
    }
    if(value.contains("fusion_linear_acc")){
        QVariantMap s = value["fusion_linear_acc"].toMap();
        ui->slideLinearAccSample->setValue(s["sample_rate"].toFloat());
    }
    if(value.contains("fusion_quaternion")){
        QVariantMap s = value["fusion_quaternion"].toMap();
        ui->slideQuaternionSample->setValue(s["sample_rate"].toFloat());
    }
    if(value.contains("gyro")){
        QVariantMap s = value["gyro"].toMap();
        ui->slideGyroRange->setValue(s["sample_range"].toFloat());
        ui->slideGyroSample->setValue(s["sample_rate"].toFloat());
    }
    if(value.contains("acc")){
        QVariantMap s = value["acc"].toMap();
        ui->slideAccRange->setValue(s["sample_range"].toFloat());
        ui->slideAccSample->setValue(s["sample_rate"].toFloat());
    }
    ui->deviceMac->setText(value["mac"].toString());
    ui->deviceName->setText(value["name"].toString());
}