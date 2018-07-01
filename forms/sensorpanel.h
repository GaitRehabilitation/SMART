#ifndef SENSORPANEL_H
#define SENSORPANEL_H

#include <QBluetoothDeviceInfo>
#include <QWidget>
#include <QLowEnergyController>
#include <QtCharts/QChart>
#include "metawearconfig.h"

QT_FORWARD_DECLARE_CLASS(MetawearWrapper)
QT_FORWARD_DECLARE_CLASS(MblMwMetaWearBoard)
QT_FORWARD_DECLARE_CLASS(QTimer)

namespace Ui {
    class SensorPanel;
}

QT_CHARTS_USE_NAMESPACE

class SensorPanel : public QWidget
{

    Q_OBJECT
public:
    explicit SensorPanel(const QBluetoothDeviceInfo &device,QWidget *parent = 0);
    virtual ~SensorPanel();
private:
    MetawearWrapper* m_wrapper;
    Ui::SensorPanel* ui;
    QBluetoothDeviceInfo m_currentDevice;
    MetawearConfig* metwareConfig;

    QTimer* settingUpdateTimer;
private slots:
    void onMetawareInitialized();
public slots:
    void setName(QString);
signals:
   void onConnected();
   void onBluetoothError(QLowEnergyController::Error e);


};

#endif // SENSORPANEL_H
