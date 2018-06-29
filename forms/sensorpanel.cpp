#include "ui_sensorpanel.h"
#include "forms/sensorpanel.h"

#include "metawearwrapper.h"
#include "metawear/core/metawearboard.h"
#include "metawear/sensor/accelerometer.h"
#include "metawear/core/datasignal.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>



SensorPanel::SensorPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorPanel){

    ui->setupUi(this);
    m_chart =  new QChart();

    ui->plot->setChart(m_chart);

}

void SensorPanel::setDevice(const QBluetoothDeviceInfo &device){
    m_currentDevice = device;
    this->m_wrapper = new MetawearWrapper(this);

    connect(this->m_wrapper,SIGNAL(metawareInitialized()),this,SLOT(onMetawareInitialized()));

    ui->deviceAddress->setText(device.name());
    this->m_wrapper->setDevice(device);
}

void SensorPanel::setName(QString name){
    ui->sensorName->setText(name);
}

void SensorPanel::onMetawareInitialized(){
    QLineSeries * accZLineSeries = new QLineSeries();


    auto acc_root = mbl_mw_acc_get_acceleration_data_signal(this->m_wrapper->getBoard());
    auto acc_z = mbl_mw_datasignal_get_component(acc_root,MBL_MW_ACC_ACCEL_Z_AXIS_INDEX);
    mbl_mw_datasignal_subscribe(acc_z,accZLineSeries,[](void* context,const MblMwData* data) -> void {
        QLineSeries* l = (QLineSeries*)context;
        l->append(data->epoch, *((float*) data->value));
    });
    this->m_chart->addSeries(accZLineSeries);
}
SensorPanel::~SensorPanel(){
    delete ui;
}


