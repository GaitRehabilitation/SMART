#include "forms/sensorpanel.h"
#include "ui_sensorpanel.h"

#include "common/metawearwrapper.h"
#include "metawear/core/datasignal.h"
#include "metawear/core/metawearboard.h"
#include "metawear/core/types.h"
#include "metawear/sensor/accelerometer.h"

#include "qcustomplot.h"
#include <QGraphicsLayout>
#include <QSplineSeries>
#include <QTimer>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <common/databundle.h>

SensorPanel::SensorPanel(const QBluetoothDeviceInfo &device, QWidget *parent)
    : QWidget(parent), ui(new Ui::SensorPanel),
      m_metawearConfig(new MetawearConfig(this)),
      settingUpdateTimer(new QTimer(this)), m_currentDevice(device),
      m_wrapper(new MetawearWrapper(device, this)),m_plotoffset(0),m_temporaryDir(0),m_laststEpoch(0){
  ui->setupUi(this);

  connect(this->m_wrapper, SIGNAL(onMagnetometer(qint64, float, float, float)),
          this, SIGNAL(onMagnetometer(qint64, float, float, float)));
  connect(this->m_wrapper, SIGNAL(onGyro(qint64, float, float, float)), this,
          SIGNAL(onGyro(qint64, float, float, float)));
  connect(this->m_wrapper, SIGNAL(onAcceleration(qint64, float, float, float)),
          this, SIGNAL(onAcceleration(qint64, float, float, float)));
  connect(this->m_wrapper, SIGNAL(onAmbientLight(qint64, qint32)), this,
          SIGNAL(onAmbientLight(qint64, qint32)));
  connect(this->m_wrapper,SIGNAL(disconnected()),this,SIGNAL(onDisconnect()));
  connect(this->m_wrapper,&MetawearWrapper::disconnected,this,[=](){
      this->deleteLater();
  });
  connect(this->m_wrapper,&MetawearWrapper::controllerError,this,[=](){
      this->deleteLater();
  });
  connect(this->m_wrapper, SIGNAL(onEpoch(qint64)), this,
          SIGNAL(onEpoch(qint64)));
  connect(this->m_wrapper, SIGNAL(onVoltage(quint16)), this,
          SIGNAL(onVoltage(quint16)));
  connect(this->m_wrapper, SIGNAL(onBatteryPercentage(qint8)), this,
          SIGNAL(onBatteryPercentage(qint8)));

  this->registerPlotHandlers();
  this->registerDataHandlers();

  connect(this->m_wrapper, SIGNAL(metawareInitialized()), this,
          SLOT(onMetawareInitialized()));
  ui->deviceAddress->setText(device.address().toString());

  QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
  timeTicker->setTimeFormat("%h:%m:%s");
  ui->plot->xAxis->setTicker(timeTicker);
  ui->plot->axisRect()->setupFullAxesBox();
  ui->plot->yAxis->setRange(-4, 4);

  connect(this->m_wrapper, &MetawearWrapper::onBatteryPercentage,
          [=](qint8 amount) { this->ui->battery->setValue(amount); });


  // read the battery status every minute
  connect(settingUpdateTimer, &QTimer::timeout,
          [=]() { this->m_wrapper->readBatteryStatus(); });

  connect(this->m_wrapper, &MetawearWrapper::onEpoch, this,
          [this](qint64 epoch) {
            if(epoch < m_laststEpoch)
                return;
            m_laststEpoch = epoch;

            qint64 xpos = epoch - m_plotoffset;
            this->ui->plot->xAxis->setRange(
                xpos, this->ui->xScaleSlider->value(), Qt::AlignRight);
            double finalMax = 0;
            double finalMin = 0;

            for (int x = 0; x < this->ui->plot->graphCount(); ++x) {
              QCPGraph *graph = this->ui->plot->graph(x);
              graph->data()->removeBefore(xpos - 50000);
            }
            this->ui->plot->replot();
          });

}

void SensorPanel::registerPlotHandlers()
{
    QCPGraph *xgraphAcc = ui->plot->addGraph();
    xgraphAcc->setPen(QPen(QColor(255,0,0)));
    xgraphAcc->setName("X Acc");
    QCPGraph *ygraphAcc = ui->plot->addGraph();
    ygraphAcc->setPen(QPen(QColor(0,255,0)));
    ygraphAcc->setName("X Acc");
    QCPGraph *zgraphAcc = ui->plot->addGraph();
    zgraphAcc->setName("Z Acc");
    zgraphAcc->setPen(QPen(QColor(0,0,255)));
    connect(this->m_wrapper, &MetawearWrapper::onAcceleration, this,
        [=](int64_t epoch, float x, float y, float z) {
          xgraphAcc->addData(epoch - m_plotoffset, x);
          ygraphAcc->addData(epoch - m_plotoffset, y);
          zgraphAcc->addData(epoch - m_plotoffset, z);
        });
}

void SensorPanel::registerDataHandlers()
{
    connect(this->m_wrapper, &MetawearWrapper::onAcceleration, this,
        [=](int64_t epoch, float x, float y, float z) {
            if(m_temporaryDir && m_temporaryDir->isValid()){
                QString title = ui->sensorName->text();
                if(title.isEmpty())
                {
                    title = this->ui->deviceAddress->text();
                }
                    QString path =  m_temporaryDir->path().append(QString("/%1_%2.csv").arg(title,"acc")) ;
                    QFile file(path);
                    bool exist = file.exists();
                    if(file.open(QIODevice::WriteOnly | QIODevice::Append)){
                        QTextStream outStream(&file);
                        if(!exist){
                           outStream << "epoch(ms),acc_x(g),acc_y(g),acc_z(g)" << '\n';
                        }
                        outStream << epoch << ','<< x << ','<< y << ','<< z << '\n';
                    }
                    file.close();
            }
        });
}

void SensorPanel::setName(QString name) { ui->sensorName->setText(name); }

void SensorPanel::setOffset(qint64 offset) {
 clearPlots();
 m_plotoffset = offset;
}

qint64 SensorPanel::getLatestEpoch()
{
    return m_laststEpoch;
}

void SensorPanel::startCapture(QTemporaryDir* dir)
{
    m_temporaryDir = dir;
}

void SensorPanel::stopCapture()
{
    this->m_temporaryDir = nullptr;
}

void SensorPanel::clearPlots()
{
    for (int x = 0; x < this->ui->plot->graphCount(); ++x) {
      QCPGraph *graph = this->ui->plot->graph(x);
      graph->data()->clear();
    }
}


void SensorPanel::onMetawareInitialized() {
  this->m_wrapper->setAccelerationSamplerate(50);
  this->m_wrapper->setAccelerationCapture(true);
  settingUpdateTimer->start(60000);
  this->m_wrapper->readBatteryStatus();
}

SensorPanel::~SensorPanel() { delete ui; }



