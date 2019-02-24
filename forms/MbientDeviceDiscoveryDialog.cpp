/**
* Copyright 2019 GaitRehabilitation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <forms/MbientDeviceDiscoveryDialog.h>
#include <forms/mbientconfigpanel.h>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <common/DiscoveryAgent.h>
#include <common/metawearwrapper.h>
#include <QMessageBox>
#include <QPushButton>
#include "common/DiscoveryAgent.h"
#include <forms/MbientDeviceDiscoveryWidget.h>
#include <ui_MbientDeviceDiscoveryDialog.h>

MbientDeviceDiscoveryDialog::MbientDeviceDiscoveryDialog(const QList<MbientConfigPanel*>& panels, QWidget *parent ) :
	QDialog(parent),m_agent(new DiscoveryAgent()), ui(new Ui::MbientDeviceDiscoveryDialog())
{
	ui->setupUi(this);
	
	foreach(MbientConfigPanel* panel, panels) {
		MetawearWrapperBase* wrapper = panel->buildWrapper();
		MbientDeviceDiscoveryWidget* widget = new MbientDeviceDiscoveryWidget(wrapper,this);
		widget->setStatus(MbientDeviceDiscoveryWidget::Status::Searching);

		ui->items->addWidget(widget);
		m_mappings.insert(panel->getMac(), widget);
		m_payloads.append(wrapper);
	}
}

int MbientDeviceDiscoveryDialog::exec(){
	connect(m_agent, &DiscoveryAgent::deviceDiscovered, this, [=](BluetoothAddress info) {
		qDebug() << info.getMac() << "\n";
		foreach(MetawearWrapperBase* payload, m_payloads) {
			if (QString::compare(info.getMac(), payload->getTarget().getMac(), Qt::CaseInsensitive) == 0) {
				MbientDeviceDiscoveryWidget* widget = m_mappings[info.getMac()];
				if (widget) {
					widget->setStatus(MbientDeviceDiscoveryWidget::Status::Found);
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
		if (m_payloads.isEmpty()) {
			return;
		}
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
	delete ui;

}

