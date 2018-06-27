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
    explicit SensorPanel(QBluetoothAddress address,QWidget *parent = 0);
    virtual ~SensorPanel();
private:
    MetawearWrapper* wrapper;
    Ui::SensorPanel* ui;
private slots:
    void handleConnection();
    void handleError(QLowEnergyController::Error);\
    void handleDisconnect();
public slots:
    void setName(QString);

signals:
   void onConnected();
   void onBluetoothError(QLowEnergyController::Error e);

};

#endif // SENSORPANEL_H
