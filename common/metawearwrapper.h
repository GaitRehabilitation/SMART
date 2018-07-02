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
       bool m_isSensorEnabled;

       static quint128 convertQuint128(uint8_t *low, uint8_t *high);
       static void read_gatt_char_qt(void* context, const void* caller, const MblMwGattChar* characteristic,MblMwFnIntVoidPtrArray handler);
       static void write_gatt_char_qt(void *context, const void* caller, MblMwGattCharWriteType writeType, const MblMwGattChar* characteristic,const uint8_t* value, uint8_t length);
       static void enable_char_notify_qt(void* context, const void* caller, const MblMwGattChar* characteristic,MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready);
       static void on_disconnect_qt(void *context, const void* caller, MblMwFnVoidVoidPtrInt handler);
public:
    explicit MetawearWrapper(const QBluetoothDeviceInfo &device,QObject *parent = 0);
    virtual ~MetawearWrapper();
    QLowEnergyController* getController();
    MblMwMetaWearBoard* getBoard();

    int m_readyCharacteristicCount;
    bool m_isMetawareReady;

public slots:

    void setAccelerationSamplerate(float);
    void setAmbientLightSamplerate(float);
    void setGyroSamplerate(float);
    void setMagnetometerRate(float);

    void setAccelerationCapture(bool);
    void setAmbientLightCapture(bool);
    void setGyroCapture(bool);
    void setMagnetometerCapture(bool);
    void setBarometerCapture(bool);

    void readBatteryStatus();

private slots:
    //QLowEnergyController
   void onServiceDiscovered(const QBluetoothUuid &newService);
   void onServiceDiscoveryFinished();

   void onConnected();
   void onDisconnect();

   void onCharacteristicRead(QLowEnergyCharacteristic,QByteArray);
   void onCharacteristicNotifications(QLowEnergyCharacteristic,QByteArray);
   void onControllerError(QLowEnergyController::Error);
   void onCharacteristicError(QLowEnergyService::ServiceError);

   void onStateChange(QLowEnergyController::ControllerState state);

   void metwareIntialize();

signals:
    void connected();
    void disconnected();

    double onBatteryPercentage(qint8);
    double onVoltage(quint16);

    void onMagnetometer(qint64,float,float,float);
    void onGyro(qint64, float, float, float);
    void onAcceleration(qint64,float,float,float);
    void onAmbientLight(qint64,qint32);

    void onEpoch(qint64);

    void metawareInitialized();
    void metawareFailedToInitialized(int32_t status);

    void controllerError(QLowEnergyController::Error);
    void characteristicError(QLowEnergyService::ServiceError);
};

#endif // METAWEARWRAPPER_H
