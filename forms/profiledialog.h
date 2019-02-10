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

#ifndef SMART_PROFILEWINDOW_H
#define SMART_PROFILEWINDOW_H

#include <QDialog>

namespace Ui {
    class ProfileDialog;
}
class MbientConfigPanel;
class ProfileDialog : public QDialog {
    Q_OBJECT
private:
    Ui::ProfileDialog *ui;
    QString profilePath(const QString& profile);
public:
    ProfileDialog(QWidget *parent = nullptr);
    QVariantList serialize();
    void deserialize(QVariantList payload);
    void reloadProfiles();
signals:
    void onProfileSelected(const QList<MbientConfigPanel*> &payload);
};


#endif //SMART_PROFILEWINDOW_H
