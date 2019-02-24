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

#ifndef MBIENT_DEVICE_DISCOVERY_WIDGET_H
#define MBIENT_DEVICE_DISCOVERY_WIDGET_H

#include <QWidget>

namespace Ui {
	class MbientDeviceDiscoveryWidget;
}
class MetawearWrapperBase;
class QMovie;
class MbientDeviceDiscoveryWidget : public QWidget {
	Q_OBJECT
private:
	Ui::MbientDeviceDiscoveryWidget* ui;
	QMovie* m_search_icon;
	QPixmap* m_found_icon;
public:
	enum Status {
		Searching,
		Found
	};

	MbientDeviceDiscoveryWidget(MetawearWrapperBase* wrapper,QWidget *parent = nullptr);
	~MbientDeviceDiscoveryWidget();

	void setStatus(Status status);
};

#endif
