#ifndef DEVICEDISCOVERYPAGE_H
#define DEVICEDISCOVERYPAGE_H

#include <QWizard>
#include <QBluetoothServiceDiscoveryAgent>

class QVBoxLayout;
class DeviceDiscoveryPage: public QWizardPage
{
    Q_OBJECT
public:
    DeviceDiscoveryPage(QWidget *parent = 0);
private:
     QBluetoothDeviceDiscoveryAgent* _discoveryAgent;
     QVBoxLayout* _discoveryList;

 public slots:
     void deviceDiscovered(const QBluetoothServiceInfo &device);
     void finishedDiscovery();

};

#endif // DEVICEDISCOVERYPAGE_H


