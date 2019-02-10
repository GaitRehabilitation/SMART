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

#include "deviceselectdialog.h"
#include "ui_deviceselectdialog.h"
#include <QListView>
#include "common/DiscoveryAgent.h"

DeviceSelectDialog::DeviceSelectDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DeviceSelectDialog),
      m_deviceInfo(),
      m_deviceBlackList(),
      m_discoveryAgent(new DiscoveryAgent()){
    ui->setupUi(this);

    connect(ui->scan, &QPushButton::clicked, this,[=](){
        ui->scan->setEnabled(false);
        m_discoveryAgent->start();
    });
    connect(ui->clear, &QPushButton::clicked,this,[=](){
        ui->deviceList->clear();
        m_discoveryAgent->stop();
    });
   // connect(m_discoveryAgent,&QBluetoothDeviceDiscoveryAgent::finished, this, [=](){ui->scan->setEnabled(true); });

    // An entry is added for every device connected
    qRegisterMetaType<BluetoothAddress>("BluetoothAddress");
    connect(m_discoveryAgent, &DiscoveryAgent::deviceDiscovered, this,[=](BluetoothAddress info){
        //if(info.name() != "MetaWear")
         //   return;
        if(m_deviceBlackList.contains(info.getMac()))
            return;


        QString label = QString("%1 %2").arg(info.getMac()).arg(info.getTitle());
        m_deviceInfo.insert(label, info);

        QList<QListWidgetItem *> items = ui->deviceList->findItems(label, Qt::MatchExactly);
        if (items.empty()) {
            QListWidgetItem *item = new QListWidgetItem(label);
            //QBluetoothLocalDevice::Pairing lpairingStatus = localDevice->pairingStatus(info.address());
            //if (lpairingStatus == QBluetoothLocalDevice::Paired || lpairingStatus == QBluetoothLocalDevice::AuthorizedPaired) {
             //   item->setTextColor(QColor(Qt::green));
            //} else {
                item->setTextColor(QColor(Qt::black));
            //}
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


    m_discoveryAgent->stop();
    ui->scan->setEnabled(true);

    QListWidgetItem *currentItem = ui->deviceList->currentItem();
    QString text = currentItem->text();
    emit onBluetoothDeviceAccepted(m_deviceInfo.value(text));
    QDialog::accept();
}


void DeviceSelectDialog::addDevice(const BluetoothAddress &info) {

//    if(info.name() != "MetaWear" || info.name() != "MetaBoot")
 //       return;
    if(m_deviceBlackList.contains(info.getMac()))
        return;

    QString label = QString("%1 %2").arg(info.getMac()).arg(info.getTitle());
    m_deviceInfo.insert(label, info);

    QList<QListWidgetItem *> items = ui->deviceList->findItems(label, Qt::MatchExactly);
    if (items.empty()) {
        QListWidgetItem *item = new QListWidgetItem(label);
        //QBluetoothLocalDevice::Pairing lpairingStatus = localDevice->pairingStatus(info.getMac());
        //if (lpairingStatus == QBluetoothLocalDevice::Paired || lpairingStatus == QBluetoothLocalDevice::AuthorizedPaired) {
         //   item->setTextColor(QColor(Qt::green));
        //} else {
            item->setTextColor(QColor(Qt::black));
        //}
        ui->deviceList->addItem(item);
    }
}

void DeviceSelectDialog::updateDeviceBlackList(const QList<BluetoothAddress> &info)
{
    m_deviceBlackList.clear();
    for(int i = 0; i < info.length(); ++i){
        m_deviceBlackList.append(info[i].getMac());
    }
    QMap<QString, BluetoothAddress> temp = m_deviceInfo;
    ui->deviceList->clear();
    m_deviceInfo.clear();

    QMap<QString, BluetoothAddress>::iterator i;
    for (auto i = temp.begin(); i != temp.end(); ++i){
        addDevice(i.value());
    }
}

