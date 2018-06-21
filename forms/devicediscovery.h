#ifndef DEVICEDISCOVERY_H
#define DEVICEDISCOVERY_H

#include <QWizard>

namespace Ui {
class DeviceDiscovery;
}

class DeviceDiscovery : public QWizard
{
    Q_OBJECT

public:
    explicit DeviceDiscovery(QWidget *parent = 0);
    ~DeviceDiscovery();

private:
    Ui::DeviceDiscovery *ui;
};

#endif // DEVICEDISCOVERY_H
