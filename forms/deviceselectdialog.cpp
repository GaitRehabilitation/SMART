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

#include "deviceselectdialog.h"
#include "ui_deviceselectdialog.h"
#include <QListView>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothservicediscoveryagent.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothuuid.h>

DeviceSelectDialog::DeviceSelectDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DeviceSelectDialog),
      m_deviceInfo(QMap<QString, QBluetoothDeviceInfo>()),
      localDevice(new QBluetoothLocalDevice),
      m_deviceBlackList(){
    ui->setupUi(this);

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);

    connect(ui->scan, &QPushButton::clicked, this,[=](){
        ui->scan->setEnabled(false);
        discoveryAgent->start();
    });
    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::finished, this, [=](){ui->scan->setEnabled(true); });

    // An entry is added for every device connected
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,[=](const QBluetoothDeviceInfo& info){
        if(info.name() != "MetaWear")
            return;
        if(m_deviceBlackList.contains(info.address().toString()))
            return;

        QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());
        m_deviceInfo.insert(label, info);

        QList<QListWidgetItem *> items = ui->deviceList->findItems(label, Qt::MatchExactly);
        if (items.empty()) {
            QListWidgetItem *item = new QListWidgetItem(label);
            QBluetoothLocalDevice::Pairing lpairingStatus = localDevice->pairingStatus(info.address());
            if (lpairingStatus == QBluetoothLocalDevice::Paired || lpairingStatus == QBluetoothLocalDevice::AuthorizedPaired) {
                item->setTextColor(QColor(Qt::green));
            } else {
                item->setTextColor(QColor(Qt::black));
            }
            ui->deviceList->addItem(item);
        }
    });

    connect(ui->deviceList, &QListWidget::itemSelectionChanged, this,[=](){
        ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(!(ui->deviceList->selectedItems().count() > 0));
    });

    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
}

DeviceSelectDialog::~DeviceSelectDialog() { delete ui; }

void DeviceSelectDialog::accept() {
    if (ui->deviceList->count() == 0)
        return;
    QListWidgetItem *currentItem = ui->deviceList->currentItem();
    QString text = currentItem->text();
    emit onBluetoothDeviceAccepted(m_deviceInfo.value(text));
    QDialog::accept();
}


void DeviceSelectDialog::addDevice(const QBluetoothDeviceInfo &info) {

    if(info.name() != "MetaWear")
        return;
    if(m_deviceBlackList.contains(info.address().toString()))
        return;

    QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());
    m_deviceInfo.insert(label, info);

    QList<QListWidgetItem *> items = ui->deviceList->findItems(label, Qt::MatchExactly);
    if (items.empty()) {
        QListWidgetItem *item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing lpairingStatus = localDevice->pairingStatus(info.address());
        if (lpairingStatus == QBluetoothLocalDevice::Paired || lpairingStatus == QBluetoothLocalDevice::AuthorizedPaired) {
            item->setTextColor(QColor(Qt::green));
        } else {
            item->setTextColor(QColor(Qt::black));
        }
        ui->deviceList->addItem(item);
    }
}

void DeviceSelectDialog::updateDeviceBlackList(const QList<QBluetoothDeviceInfo> &info)
{
    m_deviceBlackList.clear();
    for(int i = 0; i < info.length(); ++i){
        m_deviceBlackList.append(info[i].address().toString());
    }
    QMap<QString, QBluetoothDeviceInfo> temp = m_deviceInfo;
    ui->deviceList->clear();
    m_deviceInfo.clear();

    QMap<QString, QBluetoothDeviceInfo>::iterator i;
    for (auto i = temp.begin(); i != temp.end(); ++i){
        addDevice(i.value());
    }
}

