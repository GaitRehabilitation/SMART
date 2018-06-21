#include "gui/device_wizard/devicediscoverypage.h"
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>

DeviceDiscoveryPage::DeviceDiscoveryPage(QWidget *parent)
    :QWizardPage(parent)
{
    setTitle("Select Device");

    _discoveryAgent = new QBluetoothDeviceDiscoveryAgent();


    connect(_discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),this, SLOT(addDevice(QBluetoothDeviceInfo)));
//    connect(_discoveryAgent,SIGNAL(finished()),this,SLOT(finishedDiscovery()));

    _discoveryAgent->start();

    _discoveryList = new QVBoxLayout;
    QScrollArea *ldiscoveryScroll = new QScrollArea;
    ldiscoveryScroll->setWidget(_discoveryList->widget());

     QGridLayout *layout = new QGridLayout;
     layout->addWidget(ldiscoveryScroll,0,0,2,2,Qt::AlignHCenter);
     this->setLayout(layout);
}


void DeviceDiscoveryPage::deviceDiscovered(const QBluetoothServiceInfo &info){
    if (info.serviceName().isEmpty())
           return;
     QString line = info.serviceName();
    if (!info.serviceDescription().isEmpty())
        line.append("\n\t" + info.serviceDescription());
    if (!info.serviceProvider().isEmpty())
        line.append("\n\t" + info.serviceProvider());

    QLabel* out = new QLabel();
    out->setText(line);
     _discoveryList->addWidget(out,0,Qt::AlignLeft);
}

void DeviceDiscoveryPage::finishedDiscovery(){

}
