#include "gui/device_wizard/devicewizard.h"
#include "gui/device_wizard/devicediscoverypage.h"
#include "gui/device_wizard/deviceconnectpage.h"

DeviceWizard::DeviceWizard(QWidget* parent)
    :QWizard(parent)
{
    setWindowTitle(tr("Add Device"));
    setPage(DEVICE_DISCOVERY,new DeviceDiscoveryPage);
    setPage(DEVICE_CONNECT,new DeviceConnectPage);

}

void DeviceWizard::accept(){

}
