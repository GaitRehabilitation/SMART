#ifndef METAWEARWRAPPER_H
#define METAWEARWRAPPER_H

#include <QBluetoothAddress>
#include <QLowEnergyController>
#include <QObject>
#include <functional>

#include "metawear/platform/btle_connection.h"
#include "metawear/sensor/accelerometer.h"

QT_FORWARD_DECLARE_CLASS(MblMwMetaWearBoard)
class MetawearWrapper : public QObject
{
    Q_OBJECT
private:
       MblMwFnIntVoidPtrArray m_readGattHandler;
       MblMwFnVoidVoidPtrInt m_disconnectedHandler;
       MblMwFnIntVoidPtrArray m_notificationHandler;

       QLowEnergyController* m_controller;
       MblMwMetaWearBoard* m_metaWearBoard;
       int m_serviceReady;

       QMap<QString,QLowEnergyService*> m_services;
       QBluetoothDeviceInfo m_currentDevice;

       static void read_gatt_char_qt(void* context, const void* caller, const MblMwGattChar* characteristic,MblMwFnIntVoidPtrArray handler);
       static void write_gatt_char_qt(void *context, const void* caller, MblMwGattCharWriteType writeType, const MblMwGattChar* characteristic,const uint8_t* value, uint8_t length);
       static void enable_char_notify_qt(void* context, const void* caller, const MblMwGattChar* characteristic,MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready);
       static void on_disconnect_qt(void *context, const void* caller, MblMwFnVoidVoidPtrInt handler);
public:
    explicit MetawearWrapper(QObject *parent = 0);
    virtual ~MetawearWrapper();
    QLowEnergyController* getController();
    MblMwMetaWearBoard* getBoard();

    int m_readyCharacteristicCount;
    bool m_isMetawareReady;
    void setDevice(const QBluetoothDeviceInfo &device);

    void enableAcceleration();
public slots:

private slots:
    //QLowEnergyController
   void onServiceDiscovered(const QBluetoothUuid &newService);
   void onServiceDiscoveryFinished();

   void onConnected();
   void onDisconnect();

   void onCharacteristicRead(QLowEnergyCharacteristic,QByteArray);
   void onCharacteristicNotifications(QLowEnergyCharacteristic,QByteArray);

   void onControllerError(QLowEnergyController::Error);
   void onStateChange(QLowEnergyController::ControllerState state);

   void onCharacteristicError(QLowEnergyService::ServiceError);

   void metwareIntialize();

signals:
    void connected();
    void disconnected();
    void onAcceleration(int64_t,float,float,float);
    void onEpoch(int64_t);

    void metawareInitialized();
    void metawareFailedToInitialized(int32_t status);

    void controllerError(QLowEnergyController::Error);
    void characteristicError(QLowEnergyService::ServiceError);
};

#endif // METAWEARWRAPPER_H
