#ifndef SENSORPANEL_H
#define SENSORPANEL_H

#include "metawearwrapper.h"

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(MblMwMetaWearBoard)

namespace Ui {
    class SensorPanel;
}

class SensorPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SensorPanel(QWidget *parent = 0);
    virtual ~SensorPanel();
private:
    MetawearWrapper* wrapper;
    Ui::SensorPanel* ui;
    QBluetoothDeviceInfo m_currentDevice;
private slots:
public slots:
    void setName(QString);
    void setDevice(const QBluetoothDeviceInfo &device);
signals:
   void onConnected();
   void onBluetoothError(QLowEnergyController::Error e);

};

#endif // SENSORPANEL_H
