#include "deviceselectdialog.h"
#include "ui_deviceselectdialog.h"
#include <QListView>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothservicediscoveryagent.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothuuid.h>

DeviceSelectDialog::DeviceSelectDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DeviceSelectDialog),
      m_deviceInfo(QMap<QString, QBluetoothDeviceInfo>()),
      localDevice(new QBluetoothLocalDevice) {
  ui->setupUi(this);

  discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
  discoveryAgent->setInquiryType(
      QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);

  connect(ui->scan, SIGNAL(clicked()), this, SLOT(startScan()));
  connect(discoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));
  connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this,
          SLOT(addDevice(QBluetoothDeviceInfo)));
  connect(ui->deviceList, SIGNAL(itemSelectionChanged()), this,
          SLOT(onItemSelection()));

  ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
}

DeviceSelectDialog::~DeviceSelectDialog() { delete ui; }

void DeviceSelectDialog::startScan() {
  ui->scan->setEnabled(false);
  discoveryAgent->start();
}

void DeviceSelectDialog::accept() {
  if (ui->deviceList->count() == 0)
    return;
  QListWidgetItem *currentItem = ui->deviceList->currentItem();
  QString text = currentItem->text();
  emit onBluetoothDeviceAccepted(m_deviceInfo.value(text));
  QDialog::accept();
}

void DeviceSelectDialog::hostModeStateChanged(
    QBluetoothLocalDevice::HostMode mode) {

  bool on = !(mode == QBluetoothLocalDevice::HostPoweredOff);
  ui->scan->setEnabled(on);
}

void DeviceSelectDialog::scanFinished() { ui->scan->setEnabled(true); }

void DeviceSelectDialog::addDevice(const QBluetoothDeviceInfo &info) {

    if(info.name() != "MetaWear")
        return;
    QString label =
      QString("%1 %2").arg(info.address().toString()).arg(info.name());
  m_deviceInfo.insert(label, info);


  QList<QListWidgetItem *> items =
      ui->deviceList->findItems(label, Qt::MatchExactly);
  if (items.empty()) {
    QListWidgetItem *item = new QListWidgetItem(label);
    QBluetoothLocalDevice::Pairing lpairingStatus =
        localDevice->pairingStatus(info.address());
    if (lpairingStatus == QBluetoothLocalDevice::Paired ||
        lpairingStatus == QBluetoothLocalDevice::AuthorizedPaired) {
      item->setTextColor(QColor(Qt::green));
    } else {
      item->setTextColor(QColor(Qt::black));
    }
    ui->deviceList->addItem(item);
  }
}

void DeviceSelectDialog::onItemSelection() {
  ui->buttonBox->button(QDialogButtonBox::Ok)
      ->setDisabled(!(ui->deviceList->selectedItems().count() > 0));
}
