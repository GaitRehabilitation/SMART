#include <forms/deviceloadingdialog.h>
#include <forms/mbientconfigpanel.h>
#include <QVBoxLayout>

DeviceLoadingDialog::DeviceLoadingDialog(const QList<MbientConfigPanel*>& payload, QWidget *parent ) :
	QDialog(parent), m_payloads(payload){
	QVBoxLayout* verticalLayout = new QVBoxLayout(this);


}

DeviceLoadingDialog::~DeviceLoadingDialog() {

}