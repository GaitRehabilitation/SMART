#ifndef DEVICESELECT_H
#define DEVICESELECT_H

#include <QWizard>

class QBluetoothServiceDiscoveryAgent;
class DeviceSelect: public QWizardPage
{
public:
    DeviceSelect(QWidget *parent = 0);
private:
     QBluetoothServiceDiscoveryAgent* _discoveryAgent;


};

#endif // DEVICESELECT_H
