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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QTemporaryDir>
#include <QTimer>
#include "common/BluetoothAddress.h"

namespace Ui {
    class MainWindow;
}
class SensorPanel;
class DeviceSelectDialog;
class QFileDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    int m_deviceIndex;
    double m_triggerTime;
    QTemporaryDir* m_temporaryData;
    QTimer m_triggerSingleShot;
    QTimer m_updateTriggerTimer;
public:
  MainWindow(QWidget *parent = nullptr);
  virtual ~MainWindow();

  void connectedDevices(QList<BluetoothAddress>&);
  SensorPanel *registerDevice(const BluetoothAddress &info);
  void startCapture();
  void stopCapture();

};

#endif // MAINWINDOW_H
