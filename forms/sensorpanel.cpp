#include "forms/sensorpanel.h"
#include "ui_sensorpanel.h"

#include "metawear/core/datasignal.h"
#include "metawear/core/metawearboard.h"
#include "metawear/core/types.h"
#include "metawear/sensor/accelerometer.h"
#include "metawearwrapper.h"

#include <QSplineSeries>
#include <QTimer>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "xyseriesstream.h"
#include <QGraphicsLayout>


SensorPanel::SensorPanel(const QBluetoothDeviceInfo &device,QWidget *parent)
    : QWidget(parent),
      ui(new Ui::SensorPanel),
      metwareConfig(new MetawearConfig(this)),
      settingUpdateTimer(new QTimer(this)),
      m_currentDevice(device),
      m_wrapper(new MetawearWrapper(device,this)){

    ui->setupUi(this);

    QCPGraph* xgraph = ui->plot->addGraph();
    QCPGraph* ygraph = ui->plot->addGraph();
    QCPGraph* zgraph = ui->plot->addGraph();
    connect(this->m_wrapper,&MetawearWrapper::onAcceleration,this,[=](int64_t epoch,float x ,float y,float z){
      xgraph->addData(epoch,x);
      ygraph->addData(epoch,y);
      zgraph->addData(epoch,z);
    });

    connect(this->m_wrapper, SIGNAL(metawareInitialized()), this,SLOT(onMetawareInitialized()));
    ui->deviceAddress->setText(device.address().toString());

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->plot->xAxis->setTicker(timeTicker);
    ui->plot->axisRect()->setupFullAxesBox();
    ui->plot->yAxis->setRange(-1.2, 1.2);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect( ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->plot->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->plot->yAxis2, SLOT(setRange(QCPRange)));

    connect(this->m_wrapper,&MetawearWrapper::onBatteryPercentage,[=](qint8 amount){
        this->ui->battery->setValue(amount);
    });

    connect(this->m_wrapper,&MetawearWrapper::onVoltage,[=](qint16 value){
        this->ui->voltage->setText(QString("%1V").arg(((double)value) * .001));
    });

    // read the battery status every minute
    connect(settingUpdateTimer,&QTimer::timeout,[=](){
        this->m_wrapper->readBatteryStatus();
    });


    connect(this->m_wrapper,&MetawearWrapper::onEpoch,this,[this](qint64 epoch){
        this->ui->plot->xAxis->setRange(epoch,10000,Qt::AlignRight);
        this->ui->plot->replot();
    });

}



void SensorPanel::setName(QString name) { ui->sensorName->setText(name); }

void SensorPanel::onMetawareInitialized() {
  QSplineSeries* series = new QSplineSeries(this);
  this->m_wrapper->enableAcceleration();

  settingUpdateTimer->start(60000);
  this->m_wrapper->readBatteryStatus();
}

SensorPanel::~SensorPanel() { delete ui; }
