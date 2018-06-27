#ifndef DEVICESELECTDIALOG_H
#define DEVICESELECTDIALOG_H

#include <QDialog>
#include <QBluetoothDeviceInfo>
#include <QWizardPage>
#include <QBluetoothLocalDevice>
#include <QMap>

QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceDiscoveryAgent)
QT_FORWARD_DECLARE_CLASS(QBluetoothLocalDevice)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)

namespace Ui {
    class DeviceSelectDialog;
}

class DeviceSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceSelectDialog(QWidget *parent = 0);
    ~DeviceSelectDialog();
private:
    Ui::DeviceSelectDialog *ui;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localDevice;
    QMap<QString,QBluetoothDeviceInfo> m_deviceInfo;

public slots:
     void addDevice(const QBluetoothDeviceInfo&);
     void accept();
private slots:
    void startScan();
    void hostModeStateChanged(QBluetoothLocalDevice::HostMode);
    void scanFinished();
    void onItemSelection();
signals:
     void onBluetoothDeviceAccepted(const QBluetoothDeviceInfo &info);


};

#endif // DEVICESELECTDIALOG_H
