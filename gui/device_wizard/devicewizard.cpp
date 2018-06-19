#include "gui/device_wizard/devicewizard.h"
#include "gui/device_wizard/deviceselect.h"

DeviceWizard::DeviceWizard(QWidget* parent)
    :QWizard(parent)
{
    setWindowTitle(tr("Add Device"));
    setPage(new DeviceSelect);
    setPage();

}
