#ifndef DEVICEDISCOVERY_H
#define DEVICEDISCOVERY_H

#include <QWizard>

QT_FORWARD_DECLARE_CLASS(DeviceDiscoveryWizardPage)
QT_FORWARD_DECLARE_CLASS(DeviceSelectWizardPage)


class DeviceWizard : public QWizard
{
    Q_OBJECT
public:
    enum {
        DEVICE_TYPE_PAGE  = 0,
        DEVICE_DISCOVERY_PAGE = 1
    };

    explicit DeviceWizard(QWidget *parent = 0);
    ~DeviceWizard();
private:
    DeviceDiscoveryWizardPage* deviceDiscoveryWizardPage;
    DeviceSelectWizardPage* deviceSelectWizardPage;
};

#endif // DEVICEDISCOVERY_H
