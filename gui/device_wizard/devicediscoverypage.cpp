#include "gui/device_wizard/devicediscoverypage.h"
#include <QBluetoothServiceDiscoveryAgent>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>

DeviceDiscoveryPage::DeviceDiscoveryPage(QWidget *parent)
    :QWizardPage(parent)
{
    setTitle("Select Device");
    _discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(_discoveryAgent,SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
    connect(_discoveryAgent,SIGNAL(finished()),this,SLOT(finishedDiscovery()));

//    _discoveryAgent->start();

    _discoveryList = new QVBoxLayout;
    QScrollArea *ldiscoveryScroll = new QScrollArea;
    ldiscoveryScroll->setWidget(_discoveryList->widget());

     QGridLayout *layout = new QGridLayout;
     layout->addWidget(ldiscoveryScroll,0,0,2,2,Qt::AlignHCenter);
     this->setLayout(layout);
}


void DeviceDiscoveryPage::deviceDiscovered(const QBluetoothDeviceInfo &device){
    QLabel* label = new QLabel;
    label->setText(device.name() +  " (" + device.address().toString() + ")");
     _discoveryList->addWidget(label,0,Qt::AlignLeft);
}

void DeviceDiscoveryPage::finishedDiscovery(){

}
