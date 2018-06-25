#include "devicediscoverywizardpage.h"
#include "ui_devicediscoverywizardpage.h"
#include <qbluetoothservicediscoveryagent.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothuuid.h>
#include <QListView>

DeviceDiscoveryWizardPage::DeviceDiscoveryWizardPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::DeviceDiscoveryWizardPage),
    localDevice(new QBluetoothLocalDevice)
{
    ui->setupUi(this);

     discoveryAgent = new  QBluetoothDeviceDiscoveryAgent();
     discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);

    connect(ui->scan,SIGNAL(clicked()),this,SLOT(startScan()));
    connect(ui->deviceList,SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),this,SLOT(deviceItemChange(QListWidgetItem*,QListWidgetItem*)));

    connect(discoveryAgent,SIGNAL(finished()),this,SLOT(scanFinished()));
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(addDevice(QBluetoothDeviceInfo)));

    hostModeStateChanged(localDevice->hostMode());
}

DeviceDiscoveryWizardPage::~DeviceDiscoveryWizardPage()
{
    delete ui;
}

bool DeviceDiscoveryWizardPage::isComplete() const{
    return false;
}

void DeviceDiscoveryWizardPage::startScan(){
    ui->scan->setEnabled(false);
    discoveryAgent->start();
}


void DeviceDiscoveryWizardPage::hostModeStateChanged(QBluetoothLocalDevice::HostMode mode){

    bool on = !(mode == QBluetoothLocalDevice::HostPoweredOff);
    ui->scan->setEnabled(on);
}

void DeviceDiscoveryWizardPage::scanFinished(){
    ui->scan->setEnabled(true);
}

void DeviceDiscoveryWizardPage::addDevice(const QBluetoothDeviceInfo &info)
{
    QString label = QString("%1 %2").arg(info.address().toString()).arg(info.name());
    QList<QListWidgetItem*> items = ui->deviceList->findItems(label,Qt::MatchExactly);
    if(items.empty()){
        QListWidgetItem* item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing lpairingStatus = localDevice->pairingStatus(info.address());
        if(lpairingStatus == QBluetoothLocalDevice::Paired || lpairingStatus == QBluetoothLocalDevice::AuthorizedPaired){
            item->setTextColor(QColor(Qt::green));
        }
        else
        {
            item->setTextColor(QColor(Qt::black));
        }
        ui->deviceList->addItem(item);
    }

}

void DeviceDiscoveryWizardPage::deviceItemChange(QListWidgetItem* current, QListWidgetItem* previous){

}


