#include "devicewizard.h"

#include "forms/devicediscoverywizardpage.h"
#include "forms/deviceselectwizardpage.h"
DeviceWizard::DeviceWizard(QWidget *parent) :
    QWizard(parent),
    deviceDiscoveryWizardPage(new DeviceDiscoveryWizardPage),
    deviceSelectWizardPage(new DeviceSelectWizardPage)
{
    this->setPage(DEVICE_DISCOVERY_PAGE,deviceDiscoveryWizardPage);
    this->setPage(DEVICE_TYPE_PAGE, deviceSelectWizardPage);

}


DeviceWizard::~DeviceWizard()
{
    delete deviceDiscoveryWizardPage;
}


