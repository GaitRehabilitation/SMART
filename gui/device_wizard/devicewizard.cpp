#include "gui/device_wizard/devicewizard.h"
#include "gui/device_wizard/deviceselect.h"
#include "gui/device_wizard/deviceconnect.h"

DeviceWizard::DeviceWizard(QWidget* parent)
    :QWizard(parent)
{
    setWindowTitle(tr("Add Device"));
    setPage(0,new DeviceSelect);
    setPage(1,new DeviceConnect);

}

void DeviceWizard::accept(){

}
