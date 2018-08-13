//
// Created by michaelpollind on 8/4/18.
//

#ifndef SMART_QTMETAWEARWRAPPER_H
#define SMART_QTMETAWEARWRAPPER_H

#include <QtBluetooth/QLowEnergyController>
#include <QtCore/QPointer>
#include <QtBluetooth/QBluetoothHostInfo>
#include "common/metawearwrapperbase.h"


class MetawearWrapper : public MetawearWrapperBase {
Q_OBJECT
private:
    QPointer<QLowEnergyController> m_controller;
    QMap<QString, QLowEnergyService *> m_services;

    int m_readyCharacteristicCount;

    MblMwFnIntVoidPtrArray m_readGattHandler;
    MblMwFnVoidVoidPtrInt m_disconnectedHandler;
    MblMwFnIntVoidPtrArray m_notificationHandler;

protected:

    static quint128 convertTo128(uint64_t low, uint64_t high);

    static void read_gatt_char(void *context, const void *caller, const MblMwGattChar *characteristic,
                               MblMwFnIntVoidPtrArray handler);

    static void write_gatt_char(void *context, const void *caller, MblMwGattCharWriteType writeType,
                                const MblMwGattChar *characteristic, const uint8_t *value, uint8_t length);

    static void enable_char_notify(void *context, const void *caller, const MblMwGattChar *characteristic,
                                   MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready);

    static void on_disconnect(void *context, const void *caller, MblMwFnVoidVoidPtrInt handler);

public:
    MetawearWrapper(const QBluetoothHostInfo &local, const QBluetoothDeviceInfo &target);

signals:

    void controllerError(QLowEnergyController::Error);

    void characteristicError(QLowEnergyService::ServiceError);

};

#endif //SMART_QTMETAWEARWRAPPER_H
