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

#include <QDialog>
#include <QMap>
#include <QWizardPage>
#include "common/BluetoothAddress.h"

class QListWidgetItem;
class DiscoveryAgent;
namespace Ui {
class DeviceSelectDialog;
}

class DeviceSelectDialog : public QDialog {
    Q_OBJECT
private:
  Ui::DeviceSelectDialog *ui;
  DiscoveryAgent* m_discoveryAgent;
  QMap<QString, BluetoothAddress> m_deviceInfo;
  QList<QString> m_deviceBlackList;

public:
  explicit DeviceSelectDialog(QWidget *parent = nullptr);
  ~DeviceSelectDialog();

  void addDevice(const BluetoothAddress&);
  void updateDeviceBlackList(const QList<BluetoothAddress>&);
  void accept();

signals:
  void onBluetoothDeviceAccepted(const BluetoothAddress &info);
};

#endif // DEVICESELECTDIALOG_H
