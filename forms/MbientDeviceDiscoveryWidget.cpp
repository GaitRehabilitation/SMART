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

#include <forms/MbientDeviceDiscoveryWidget.h>
#include <ui_MbientDeviceDiscoveryWidget.h>
#include <QMovie>
#include <common/metawearwrapper.h>

MbientDeviceDiscoveryWidget::MbientDeviceDiscoveryWidget(MetawearWrapperBase* wrapper, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MbientDeviceDiscoveryWidget())
{
	ui->setupUi(this);
	ui->name->setText(wrapper->getName());
	ui->mac->setText(wrapper->getTarget().getMac());
	m_search_icon = new QMovie(":/icons/loading.gif");
	m_found_icon = new QPixmap(":/icons/check-solid.svg");

	m_search_icon->start();
}


void MbientDeviceDiscoveryWidget::setStatus(Status status) {
	switch (status) {
		case Searching:
			ui->icon->setMovie(m_search_icon);
			break;
		case Found:
			ui->icon->setPixmap((*m_found_icon).scaled(18,18, Qt::KeepAspectRatio));
			break;
	}
}

MbientDeviceDiscoveryWidget::~MbientDeviceDiscoveryWidget() {
	delete m_found_icon;
	delete m_search_icon;
}
