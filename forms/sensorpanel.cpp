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


SensorPanel::SensorPanel(QWidget *parent)
    : QWidget(parent), ui(new Ui::SensorPanel) {

  ui->setupUi(this);

  m_chart = new QChart();
  QChartView *chartView = ui->plot;

  QLineSeries *series = new QLineSeries();
  series->append(0, 6);
  series->append(2, 4);
  series->append(3, 8);
  series->append(7, 4);
  series->append(10, 5);
  *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

 m_chart->legend()->hide();
 m_chart->addSeries(series);
 m_chart->createDefaultAxes();
 m_chart->layout()->setContentsMargins(0, 0, 0, 0);
 m_chart->setBackgroundRoundness(0);
 ui->plot->setChart(m_chart);

}

void SensorPanel::setDevice(const QBluetoothDeviceInfo &device) {
  m_currentDevice = device;
  this->m_wrapper = new MetawearWrapper(this);

  connect(this->m_wrapper, SIGNAL(metawareInitialized()), this,
          SLOT(onMetawareInitialized()));

  ui->deviceAddress->setText(device.address().toString());
//  this->m_wrapper->setDevice(device);
}

void SensorPanel::setName(QString name) { ui->sensorName->setText(name); }

void SensorPanel::onMetawareInitialized() {
  QSplineSeries* series = new QSplineSeries(this);

  auto acc_signal =
  mbl_mw_acc_get_acceleration_data_signal(this->m_wrapper->getBoard());
  mbl_mw_datasignal_subscribe(acc_signal, series , [](void *context, const MblMwData *data) -> void {
        QSplineSeries *l = (QSplineSeries *)context;
        auto acceleration =
            (MblMwCartesianFloat *)data->value;
        l->append(data->epoch,(*((float*)acceleration)));
        if(l->count() > 2000){
            l->removePoints(0,1);
        }
  });


  mbl_mw_acc_enable_acceleration_sampling(this->m_wrapper->getBoard());
  mbl_mw_acc_start(this->m_wrapper->getBoard());

//  this->m_chart->addSeries(series);
}
SensorPanel::~SensorPanel() { delete ui; }
