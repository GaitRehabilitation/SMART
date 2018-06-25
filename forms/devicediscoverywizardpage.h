#ifndef DEVICEDISCOVERYWIZARDPAGE_H
#define DEVICEDISCOVERYWIZARDPAGE_H

#include <QBluetoothDeviceInfo>
#include <QWizardPage>
#include <QBluetoothLocalDevice>

QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceDiscoveryAgent)
QT_FORWARD_DECLARE_CLASS(QBluetoothLocalDevice)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)

namespace Ui {
    class DeviceDiscoveryWizardPage;
}

class DeviceDiscoveryWizardPage : public QWizardPage
{
    Q_OBJECT

public:

    explicit DeviceDiscoveryWizardPage(QWidget *parent = 0);
    ~DeviceDiscoveryWizardPage();
    bool isComplete() const Q_DECL_OVERRIDE;
private:
    Ui::DeviceDiscoveryWizardPage *ui;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localDevice;

public slots:
     void addDevice(const QBluetoothDeviceInfo&);

private slots:
    void startScan();
    void hostModeStateChanged(QBluetoothLocalDevice::HostMode);
    void scanFinished();
    void deviceItemChange(QListWidgetItem*, QListWidgetItem*);


};

#endif // DEVICEDISCOVERYWIZARDPAGE_H
