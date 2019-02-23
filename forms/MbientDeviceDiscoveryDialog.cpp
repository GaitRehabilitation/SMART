#include <forms/MbientDeviceDiscoveryDialog.h>
#include <forms/mbientconfigpanel.h>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <common/DiscoveryAgent.h>
#include <common/metawearwrapper.h>
#include <QMessageBox>
#include <QPushButton>
#include "common/DiscoveryAgent.h"

MbientDeviceDiscoveryDialog::MbientDeviceDiscoveryDialog(const QList<MbientConfigPanel*>& panels, QWidget *parent ) :
	QDialog(parent),m_agent(new DiscoveryAgent()){
	QVBoxLayout* verticalLayout = new QVBoxLayout(this);
	foreach(MbientConfigPanel* panel, panels) {
		QLabel* label = new QLabel(this);
		label->setText(panel->getMac());
		m_mappings.insert(panel->getMac(), label);
		m_payloads.append(panel->buildWrapper());
	}
}

int MbientDeviceDiscoveryDialog::exec(){

	qRegisterMetaType<BluetoothAddress>("BluetoothAddress");
	connect(m_agent, &DiscoveryAgent::deviceDiscovered, this, [=](BluetoothAddress info) {
		qDebug() << info.getMac() << "\n";
		foreach(MetawearWrapperBase* payload, m_payloads) {
			if (QString::compare(info.getMac(), payload->getTarget().getMac(), Qt::CaseInsensitive) == 0) {
				QLabel* label = m_mappings[info.getMac()];
				if (label) {
					label->setStyleSheet("QLabel { color : green; }");
				}
				emit OnConfigured(payload);
				m_payloads.removeAll(payload);
				break;
			}
		}
		if (m_payloads.size() == 0) {
			this->accept();
		}
	});

	m_agent->start();
	m_agent->query();

	m_timer.setSingleShot(true);
	m_timer.start(10000);
	connect(&m_timer, &QTimer::timeout, this, [=]() {
		QMessageBox messageBox;
		messageBox.setText("Could not find a device with the mac:" + m_payloads.first()->getTarget().getMac()+ "\n would you like to try searching for the device again?");
		messageBox.setFixedSize(500, 200);
		QPushButton* lbuttonYes = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
		messageBox.addButton(tr("No"), QMessageBox::NoRole);
		messageBox.exec();
		if (messageBox.clickedButton() == lbuttonYes) {
			// run wrapper again and try to init wrapper
			m_timer.start(10000);
		} else {
			this->close();
		}
	});
	

	return QDialog::exec();
}

MbientDeviceDiscoveryDialog::~MbientDeviceDiscoveryDialog() {
	m_agent->stop();
	foreach(MetawearWrapperBase* payload, m_payloads) {
		delete payload;
	}
	delete m_agent;

}

