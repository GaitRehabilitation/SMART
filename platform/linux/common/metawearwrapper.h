/**
* Copyright 2019 GaitRehabilitation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef SMART_QTMETAWEARWRAPPER_H
#define SMART_QTMETAWEARWRAPPER_H


#include "common/metawearwrapperbase.h"

#include <QtBluetooth/QLowEnergyController>
#include <QtCore/QPointer>
#include <QtBluetooth/QBluetoothHostInfo>


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

    /**
     * converts characteristic to 128 bit address for each characteristic
     * @param low the low address
     * @param high the high address
     * @return
     */
    static quint128 convertTo128(uint64_t low, uint64_t high);

    /**
     * Read the value of the characteristic from the device
     * @param context
     * @param caller
     * @param characteristic
     * @param handler
     */
    static void read_gatt_char(void *context, const void *caller, const MblMwGattChar *characteristic,
                               MblMwFnIntVoidPtrArray handler);

    /**
     * Write the value of the characteristic from the device
     * @param context
     * @param caller
     * @param writeType
     * @param characteristic
     * @param value
     * @param length
     */
    static void write_gatt_char(void *context, const void *caller, MblMwGattCharWriteType writeType,
                                const MblMwGattChar *characteristic, const uint8_t *value, uint8_t length);

    static void enable_char_notify(void *context, const void *caller, const MblMwGattChar *characteristic,
                                   MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready);

    static void on_disconnect(void *context, const void *caller, MblMwFnVoidVoidPtrInt handler);

public:
    explicit MetawearWrapper( const BluetoothAddress &target);
    void connectToDevice() override;
    bool isConnected() const override;
    ~MetawearWrapper() override;


};

#endif //SMART_QTMETAWEARWRAPPER_H
