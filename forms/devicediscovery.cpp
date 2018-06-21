#include "devicediscovery.h"
#include "ui_devicediscovery.h"

DeviceDiscovery::DeviceDiscovery(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::DeviceDiscovery)
{
    ui->setupUi(this);
}

DeviceDiscovery::~DeviceDiscovery()
{
    delete ui;
}
