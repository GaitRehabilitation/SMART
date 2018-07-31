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

#ifndef DEVICESELECTDIALOG_H
#define DEVICESELECTDIALOG_H

#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QDialog>
#include <QMap>
#include <QWizardPage>

class QBluetoothDeviceDiscoveryAgent;
class QBluetoothLocalDevice;
class QListWidgetItem;

namespace Ui {
class DeviceSelectDialog;
}

class DeviceSelectDialog : public QDialog {
  Q_OBJECT

public:
  explicit DeviceSelectDialog(QWidget *parent = nullptr);
  ~DeviceSelectDialog();

private:
  Ui::DeviceSelectDialog *ui;
  QBluetoothDeviceDiscoveryAgent *discoveryAgent;
  QBluetoothLocalDevice *localDevice;
  QMap<QString, QBluetoothDeviceInfo> m_deviceInfo;

public slots:
  void addDevice(const QBluetoothDeviceInfo &);
  void accept();
private slots:
  void startScan();
  void hostModeStateChanged(QBluetoothLocalDevice::HostMode);
  void scanFinished();
  void onItemSelection();
signals:
  void onBluetoothDeviceAccepted(const QBluetoothDeviceInfo &info);
};

#endif // DEVICESELECTDIALOG_H
