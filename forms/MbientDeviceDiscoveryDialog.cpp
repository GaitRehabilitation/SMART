#include <forms/MbientDeviceDiscoveryDialog.h>
#include <forms/mbientconfigpanel.h>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <common/DiscoveryAgent.h>

MbientDeviceDiscoveryDialog::MbientDeviceDiscoveryDialog(const QList<MbientConfigPanel*>& payload, QWidget *parent ) :
	QDialog(parent), m_payloads(payload){
	QVBoxLayout* verticalLayout = new QVBoxLayout(this);
	foreach(MbientConfigPanel* panel, payload) {
		QLabel* label = new QLabel(this);
		label->setText(panel->getMac());
		m_mappings.insert(panel->getMac(), label);
	}
}
void MbientDeviceDiscoveryDialog::run() {
	DiscoveryAgent agent;
	foreach(MbientConfigPanel* panel, m_payloads) {
		MetawearWrapperBase* wrapper = panel->buildWrapper(agent);
		QLabel* label = m_mappings[panel->getMac()];
		if (wrapper) {
			QMetaObject::invokeMethod(this, [=] {
				label->setStyleSheet("QLabel { color : green; }");
			});
			emit OnConfigured(panel, wrapper);
		}
		else {
			QMetaObject::invokeMethod(this, [=] {
				label->setStyleSheet("QLabel { color : red; }");
			});
		}
	}
	QMetaObject::invokeMethod(this, [=] {
		this->accept();
	});
}

int MbientDeviceDiscoveryDialog::exec(){
	QtConcurrent::run(this,&MbientDeviceDiscoveryDialog::run);
	return QDialog::exec();
}

MbientDeviceDiscoveryDialog::~MbientDeviceDiscoveryDialog() {

}

