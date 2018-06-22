#ifndef DEVICEDISCOVERY_H
#define DEVICEDISCOVERY_H

#include <QWizard>

class DeviceDiscoveryWizardPage;

class DeviceWizard : public QWizard
{
    Q_OBJECT
public:
    enum {
        DEVICE_DISCOVERY_PAGE = 0,
        DEVICE_TYPE_PAGE = 1
    };

    explicit DeviceWizard(QWidget *parent = 0);
    ~DeviceWizard();
private:
    DeviceDiscoveryWizardPage* deviceDiscoveryWizardPage;

};

#endif // DEVICEDISCOVERY_H
