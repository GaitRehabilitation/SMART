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

#ifndef DEVICE_LOADING_DIALOG_H
#define DEVICE_LOADING_DIALOG_H

#include <QDialog>
#include <QList>
#include <QMap>
#include <QRunnable>
#include <QLabel>
#include <QTimer>

namespace Ui {
	class MbientDeviceDiscoveryDialog;
}

class MetawearWrapperBase;
class MbientConfigPanel;
class DiscoveryAgent;
class MbientDeviceDiscoveryWidget;

class MbientDeviceDiscoveryDialog : public QDialog {
	Q_OBJECT
private:
	Ui::MbientDeviceDiscoveryDialog* ui;
	QList<MetawearWrapperBase*> m_payloads;
	DiscoveryAgent* m_agent;
	QTimer m_timer;
	QMap<QString, MbientDeviceDiscoveryWidget*> m_mappings;
public:
	explicit MbientDeviceDiscoveryDialog(const QList<MbientConfigPanel*>& payload,QWidget *parent = nullptr);
	~MbientDeviceDiscoveryDialog();
	
	 int exec() override;

signals:
	void OnConfigured(MetawearWrapperBase* result);

};
#endif