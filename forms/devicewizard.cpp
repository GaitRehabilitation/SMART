#include "devicewizard.h"

#include "forms/devicediscoverywizardpage.h"

DeviceWizard::DeviceWizard(QWidget *parent) :
    QWizard(parent),
    deviceDiscoveryWizardPage(new DeviceDiscoveryWizardPage)
{
    this->addPage(deviceDiscoveryWizardPage);

}


DeviceWizard::~DeviceWizard()
{
    delete deviceDiscoveryWizardPage;
}


