#ifndef SENSORPANEL_H
#define SENSORPANEL_H

#include <QBluetoothDeviceInfo>
#include <QWidget>
#include <QLowEnergyController>
#include "metawearconfig.h"

QT_FORWARD_DECLARE_CLASS(MetawearWrapper)
QT_FORWARD_DECLARE_CLASS(MblMwMetaWearBoard)
QT_FORWARD_DECLARE_CLASS(QTimer)

namespace Ui {
    class SensorPanel;
}

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
    MetawearConfig * m_metawearConfig;
private slots:
    void onMetawareInitialized();
public slots:
    void setName(QString);
    void setOffset(qint64 offset);
    void updateConfig();
signals:
   void onConnected();
   void onBluetoothError(QLowEnergyController::Error e);

   double onBatteryPercentage(qint8);
   double onVoltage(quint16);

   void onMagnetometer(qint64,float,float,float);
   void onGyro(qint64, float, float, float);
   void onAcceleration(qint64,float,float,float);
   void onAmbientLight(qint64,qint32);

   void onEpoch(qint64);
};

#endif // SENSORPANEL_H
