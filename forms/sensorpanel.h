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

#ifndef SENSORPANEL_H
#define SENSORPANEL_H

#include <QMutex>
#include <QTemporaryDir>
#include <QWidget>
#include <qtimer.h>
#include <iostream>
#include <fstream>
#include "common/BluetoothAddress.h"

class MetawearWrapperBase;
class MblMwMetaWearBoard;
class QTimer;
class DataBundle;
namespace Ui {
class SensorPanel;
}

class SensorPanel : public QWidget {
    Q_OBJECT
private:
    Ui::SensorPanel *ui;

  MetawearWrapperBase *m_wrapper;
  BluetoothAddress m_currentDevice;

  qint64 m_plotoffset;

  QTimer m_reconnectTimer;
  QTimer m_plotUpdatetimer;
  QTimer m_settingUpdateTimer;

  QTemporaryDir* m_temporaryDir;
  QMutex m_plotLock;

  std::ofstream m_magFile;
  std::ofstream m_accFile;
  std::ofstream m_gyroFile;

  bool m_isReadyToCapture;

  void registerPlotHandlers();
  void registerDataHandlers();
  void configureWrapper(BluetoothAddress device);

public:
  explicit SensorPanel(const BluetoothAddress &target, QWidget *parent = nullptr);
  virtual ~SensorPanel();

  bool isReadyToCapture();
  MetawearWrapperBase* getMetwareWrapper();
  const BluetoothAddress& getDeviceInfo() const;
  void setName(QString);
  void setOffset(qint64 offset);
  qint64 getOffset() const;
  void startCapture(QTemporaryDir* dir);
  void stopCapture();
  void clearPlots();

signals:
  void connected();
  void disconnect();
  //void bluetoothError(QLowEnergyController::Error e);

  void metawearInitilized();
};

#endif // SENSORPANEL_H
