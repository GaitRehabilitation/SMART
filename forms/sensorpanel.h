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

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QMutex>
#include <QTemporaryDir>
#include <QWidget>
#include <qtimer.h>

class MetawearWrapper;
class MblMwMetaWearBoard;
class QTimer;
class DataBundle;

namespace Ui {
class SensorPanel;
}

class SensorPanel : public QWidget {

  Q_OBJECT
public:
  explicit SensorPanel(const QBluetoothDeviceInfo &device, QWidget *parent = nullptr);
  virtual ~SensorPanel();

private:
  MetawearWrapper *m_wrapper;
  Ui::SensorPanel *ui;
  QBluetoothDeviceInfo m_currentDevice;
  qint64 m_plotoffset;
  qint64 m_laststEpoch;
  QTimer m_plotUpdatetimer;
  QTimer *settingUpdateTimer;
  QTemporaryDir* m_temporaryDir;
  QMutex m_plotLock;
  QMutex m_directoryLock;
  bool m_isReadyToCapture;

  void registerPlotHandlers();
  void registerDataHandlers();
private slots:
  void metawareInitialized();
public slots:
  void setName(QString);
  void setOffset(qint64 offset);
  qint64 getOffset();
  qint64 getLatestEpoch();
  void startCapture(QTemporaryDir* dir);
  void stopCapture();
  void clearPlots();
signals:
  void onConnected();
  void onDisconnect();
  void onBluetoothError(QLowEnergyController::Error e);

  double onBatteryPercentage(qint8);
  double onVoltage(quint16);

  void setDir();
  void onMagnetometer(qint64, float, float, float);
  void onGyro(qint64, float, float, float);
  void onAcceleration(qint64, float, float, float);
  void onAmbientLight(qint64, qint32);
  void onMetawearInitilized();
  void onLatestEpoch(qint64);

  void onEpoch(qint64);
};

#endif // SENSORPANEL_H
