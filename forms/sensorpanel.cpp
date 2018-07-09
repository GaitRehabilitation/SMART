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

SensorPanel::SensorPanel(const QBluetoothDeviceInfo &device, QWidget *parent)
    : QWidget(parent), ui(new Ui::SensorPanel),
      m_metawearConfig(new MetawearConfig(this)),
      settingUpdateTimer(new QTimer(this)), m_currentDevice(device),
      m_wrapper(new MetawearWrapper(device, this)) {
  ui->setupUi(this);

  connect(this->m_wrapper, SIGNAL(onMagnetometer(qint64, float, float, float)),
          this, SIGNAL(onMagnetometer(qint64, float, float, float)));
  connect(this->m_wrapper, SIGNAL(onGyro(qint64, float, float, float)), this,
          SIGNAL(onGyro(qint64, float, float, float)));
  connect(this->m_wrapper, SIGNAL(onAcceleration(qint64, float, float, float)),
          this, SIGNAL(onAcceleration(qint64, float, float, float)));
  connect(this->m_wrapper, SIGNAL(onAmbientLight(qint64, qint32)), this,
          SIGNAL(onAmbientLight(qint64, qint32)));

  connect(this->m_wrapper, SIGNAL(onEpoch(qint64)), this,
          SIGNAL(onEpoch(qint64)));

  connect(this->m_wrapper, SIGNAL(onVoltage(quint16)), this,
          SIGNAL(onVoltage(quint16)));
  connect(this->m_wrapper, SIGNAL(onBatteryPercentage(qint8)), this,
          SIGNAL(onBatteryPercentage(qint8)));

  this->registerPlotHandlers();

  connect(this->m_wrapper, SIGNAL(metawareInitialized()), this,
          SLOT(onMetawareInitialized()));
  ui->deviceAddress->setText(device.address().toString());

  QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
  timeTicker->setTimeFormat("%h:%m:%s");
  ui->plot->xAxis->setTicker(timeTicker);
  ui->plot->axisRect()->setupFullAxesBox();
  ui->plot->yAxis->setRange(-4, 4);

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->xAxis2,
          SLOT(setRange(QCPRange)));
  connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->yAxis2,
          SLOT(setRange(QCPRange)));

  connect(this->m_wrapper, &MetawearWrapper::onBatteryPercentage,
          [=](qint8 amount) { this->ui->battery->setValue(amount); });

  connect(this->m_wrapper, &MetawearWrapper::onVoltage, [=](qint16 value) {
    this->ui->voltage->setText(QString("%1V").arg(((double)value) * .001));
  });

  // read the battery status every minute
  connect(settingUpdateTimer, &QTimer::timeout,
          [=]() { this->m_wrapper->readBatteryStatus(); });

  connect(this->m_wrapper, &MetawearWrapper::onEpoch, this,
          [this](qint64 epoch) {
            this->ui->plot->xAxis->setRange(
                epoch, this->ui->xScaleSlider->value(), Qt::AlignRight);
            double finalMax = 0;
            double finalMin = 0;

            for (int x = 0; x < this->ui->plot->graphCount(); ++x) {
              QCPGraph *graph = this->ui->plot->graph(x);
              graph->data()->removeBefore(epoch - 50000);
            }
            this->ui->plot->replot();
          });

  this->ui->configButton->setEnabled(false);
  connect(this->ui->configButton, &QPushButton::clicked,
          [=]() { m_metawearConfig->show(); });

  connect(this->m_metawearConfig, SIGNAL(accepted()), SLOT(updateConfig()));
}

void SensorPanel::registerPlotHandlers()
{

    QCPGraph *xgraph = ui->plot->addGraph();
    QCPGraph *ygraph = ui->plot->addGraph();
    QCPGraph *zgraph = ui->plot->addGraph();
    connect(this->m_wrapper, &MetawearWrapper::onAcceleration, this,
        [=](int64_t epoch, float x, float y, float z) {
          xgraph->addData(epoch, x);
          ygraph->addData(epoch, y);
          zgraph->addData(epoch, z);
        });


}

void SensorPanel::setName(QString name) { ui->sensorName->setText(name); }

void SensorPanel::setOffset(qint64 offset) {
  for (int x = 0; x < this->ui->plot->graphCount(); ++x) {
    QCPGraph *graph = this->ui->plot->graph(x);
    graph->data()->clear();
  }
}

void SensorPanel::updateConfig() {
  if (this->m_metawearConfig->isAcceleromterActive()){
    this->m_wrapper->setAccelerationSamplerate(this->m_metawearConfig->getAcceleromterSampleRate());
  }
  this->m_wrapper->setAccelerationCapture(this->m_metawearConfig->isAcceleromterActive());
  this->m_wrapper->setAmbientLightCapture(this->m_metawearConfig->isAmbientLightActive());
  this->m_wrapper->setGyroCapture(this->m_metawearConfig->isGyroscopeActive());
  this->m_wrapper->setMagnetometerCapture(this->m_metawearConfig->isMagnetometerActive());
  this->m_wrapper->setBarometerCapture(this->m_metawearConfig->isPressureActive());

}

void SensorPanel::startCapture()
{

}

void SensorPanel::stopCapture()
{

}

void SensorPanel::onMetawareInitialized() {
  this->ui->configButton->setEnabled(true);
  this->updateConfig();
  settingUpdateTimer->start(60000);
  this->m_wrapper->readBatteryStatus();
}

SensorPanel::~SensorPanel() { delete ui; }



void SensorPanel::registerSignalHandlers()
{

}
