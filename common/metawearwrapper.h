/**
* Copyright 2018 GaitRehabilitation
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

#ifndef METAWEARWRAPPER_H
#define METAWEARWRAPPER_H

#include <QBluetoothAddress>
#include <QLowEnergyController>
#include <QObject>
#include <functional>

#include "metawear/platform/btle_connection.h"
#include "metawear/sensor/accelerometer.h"
#include "metawear/sensor/gyro_bmi160.h"

class MblMwMetaWearBoard;

class MetawearWrapper : public QObject {
  Q_OBJECT
private:
  // metware configurations
  MblMwFnIntVoidPtrArray m_readGattHandler;
  MblMwFnVoidVoidPtrInt m_disconnectedHandler;
  MblMwFnIntVoidPtrArray m_notificationHandler;
  MblMwMetaWearBoard *m_metaWearBoard;

  QLowEnergyController *m_controller;  
  QMap<QString, QLowEnergyService *> m_services;
  QBluetoothDeviceInfo m_currentDevice;

  qint64 m_laststEpoch;

  int m_serviceReady;
  bool m_isSensorEnabled;


  static void read_gatt_char_qt(void *context, const void *caller,
                                const MblMwGattChar *characteristic,
                                MblMwFnIntVoidPtrArray handler);
  static void write_gatt_char_qt(void *context, const void *caller,
                                 MblMwGattCharWriteType writeType,
                                 const MblMwGattChar *characteristic,
                                 const uint8_t *value, uint8_t length);
  static void enable_char_notify_qt(void *context, const void *caller,
                                    const MblMwGattChar *characteristic,
                                    MblMwFnIntVoidPtrArray handler,
                                    MblMwFnVoidVoidPtrInt ready);
  static void on_disconnect_qt(void *context, const void *caller,
                               MblMwFnVoidVoidPtrInt handler);

  void handleEpoch(qint64 epoch);
public:
  explicit MetawearWrapper(const QBluetoothDeviceInfo &device,
                           QObject *parent = nullptr);
  virtual ~MetawearWrapper();
  QLowEnergyController *getController();
  MblMwMetaWearBoard *getBoard();

  int m_readyCharacteristicCount;
  bool m_isMetawareReady;
  void tryReconnect();
  qint64 getLatestEpoch();

public slots:

  void setAccelerationSamplerate(float,float);
  void setAmbientLightSamplerate(float);
  void setGyroSamplerate(MblMwGyroBmi160Range range, MblMwGyroBmi160Odr sample);
  void setMagnetometerRate(float);

  void setAccelerationCapture(bool);
  void setAmbientLightCapture(bool);
  void setGyroCapture(bool);
  void setMagnetometerCapture(bool);
  void setBarometerCapture(bool);

  void readBatteryStatus();

private slots:
  // QLowEnergyController
  void handleServiceDiscovered(const QBluetoothUuid &newService);
  void handleServiceDiscoveryFinished();

  void handleConnected();
  void handleDisconnect();

  void handleCharacteristicRead(QLowEnergyCharacteristic, QByteArray);
  void handleCharacteristicNotifications(QLowEnergyCharacteristic, QByteArray);
  void handleControllerError(QLowEnergyController::Error);
  void handleCharacteristicError(QLowEnergyService::ServiceError);

  void onStateChange(QLowEnergyController::ControllerState state);

  void metwareIntialize();

signals:
  void onConnected();
  void onDisconnected();

  double onBatteryPercentage(qint8);
  double onVoltage(quint16);

  void onMagnetometer(qint64, float, float, float);
  void onGyro(qint64, float, float, float);
  void onAcceleration(qint64, float, float, float);
  void onAmbientLight(qint64, qint32);
  void onLastEpoch(qint64);

  void onMetawareInitialized();
  void onMetawareFailedToInitialized(int32_t status);

  void onControllerError(QLowEnergyController::Error);
  void onCharacteristicError(QLowEnergyService::ServiceError);
};

#endif // METAWEARWRAPPER_H
