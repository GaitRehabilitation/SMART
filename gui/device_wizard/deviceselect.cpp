#include "gui/device_wizard/deviceselect.h"
#include <QBluetoothServiceDiscoveryAgent>

DeviceSelect::DeviceSelect(QWidget *parent)
    :QWizardPage(parent)
{
    setTitle("Select Device");
    _discoveryAgent = new QBluetoothServiceDiscoveryAgent(this);
    connect(_discoveryAgent,SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
    _discoveryAgent->start();
}

