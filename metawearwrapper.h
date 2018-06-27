#ifndef METAWEARWRAPPER_H
#define METAWEARWRAPPER_H

#include <QBluetoothAddress>
#include <QLowEnergyController>
#include <QObject>
#include "metawear/platform/btle_connection.h"

QT_FORWARD_DECLARE_CLASS(MblMwMetaWearBoard)


class MetawearWrapper : public QObject
{
    Q_OBJECT
public:

    explicit MetawearWrapper(QBluetoothAddress address,QObject *parent = 0);
    virtual ~MetawearWrapper();
    QLowEnergyService* getSerivce(const QBluetoothUuid &uuid);
    QLowEnergyController* getLowEnergyController();
private:
       static void read_gatt_char_qt(void* context, const void* caller, const MblMwGattChar* characteristic,MblMwFnIntVoidPtrArray handler);
       static void write_gatt_char_qt(void *context, const void* caller, MblMwGattCharWriteType writeType, const MblMwGattChar* characteristic,const uint8_t* value, uint8_t length);
       static void enable_char_notify_qt(void* context, const void* caller, const MblMwGattChar* characteristic,MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready);
       static void on_disconnect_qt(void *context, const void* caller, MblMwFnVoidVoidPtrInt handler);

       MblMwFnIntVoidPtrArray  readGattHandler;
       MblMwFnVoidVoidPtrInt disconnectedHandler;
       MblMwFnIntVoidPtrArray notificationHandler;
       MblMwBtleConnection connectImpl;
       QLowEnergyController* controller;
       MblMwMetaWearBoard* board;
       QMap<QString,QLowEnergyService*> services;
public slots:

       void onDeviceConnected();
private slots:
       void handleConnection();
       void handleDisconnect();

       void onServiceDiscovered(const QBluetoothUuid &newService);
       void onServiceScanFinished();

signals:
    void connected();
    void disconnected();

    void bluetoothError(QLowEnergyController::Error);
    void bluetoothCharacteristicError(QLowEnergyService::ServiceError);

};

#endif // METAWEARWRAPPER_H
