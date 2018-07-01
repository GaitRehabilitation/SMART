#ifndef SENSORPANEL_H
#define SENSORPANEL_H

#include <QBluetoothDeviceInfo>
#include <QWidget>
#include <QLowEnergyController>
#include <QtCharts/QChart>

QT_FORWARD_DECLARE_CLASS(MetawearWrapper)
QT_FORWARD_DECLARE_CLASS(MblMwMetaWearBoard)
QT_FORWARD_DECLARE_CLASS(QTime)

QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QValueAxis;
class QXYSeries;
QT_CHARTS_END_NAMESPACE

namespace Ui {
    class SensorPanel;
}

QT_CHARTS_USE_NAMESPACE

class SensorPanel : public QWidget
{

    Q_OBJECT
public:
    explicit SensorPanel(QWidget *parent = 0);
    virtual ~SensorPanel();
private:
    QChart* m_chart;
    MetawearWrapper* m_wrapper;
    Ui::SensorPanel* ui;
    QBluetoothDeviceInfo m_currentDevice;

    QValueAxis* m_xAxis;
    QValueAxis* m_yAxis;

private slots:
    void onMetawareInitialized();
public slots:
    void setName(QString);
    void setDevice(const QBluetoothDeviceInfo &device);
signals:
   void onConnected();
   void onBluetoothError(QLowEnergyController::Error e);


};

#endif // SENSORPANEL_H
