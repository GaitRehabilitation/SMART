#ifndef DEVICESELECT_H
#define DEVICESELECT_H

#include <QWizard>
#include <QBluetoothServiceDiscoveryAgent>

class DeviceSelect: public QWizardPage
{
    Q_OBJECT
public:
    DeviceSelect(QWidget *parent = 0);
private:
     QBluetoothServiceDiscoveryAgent* _discoveryAgent;

 public slots:
     void deviceDiscovered(const QBluetoothDeviceInfo &device);
};

#endif // DEVICESELECT_H
