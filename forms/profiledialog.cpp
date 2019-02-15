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


#include "forms/profiledialog.h"
#include "forms/mbientconfigpanel.h"
#include "ui_profiledialog.h"
#include <common/DiscoveryAgent.h>

#include <QtWidgets/QPushButton>
#include <QStandardPaths>

#include <msgpack.h>
#include <QString>
#include <QRegExp>
#include <QFile>
#include <QtCore/QDirIterator>
#include <QDebug>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QToolBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QListWidget>

ProfileDialog::ProfileDialog(QWidget *parent):QDialog(parent), ui(new Ui::ProfileDialog){
    ui->setupUi(this);
    qDebug() << QString("%1/profiles").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!QDir().mkpath(QString("%1/profiles").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)))){
        qDebug() << "Failed to make profile path";
    }

    QToolBar* toolBar = new QToolBar();
    toolBar->addAction(tr("Import Profiles"),[=](){
        QString fileName = QFileDialog::getOpenFileName(this,tr("Load Data"),"",tr("Profiles (*.profile)"), nullptr, QFileDialog::DontUseNativeDialog);
        if (fileName.isEmpty())
            return;
        else {
            QFile file(fileName);
            qDebug() << file;
            if(file.exists()){
                file.open(QIODevice::ReadOnly);
                QByteArray data = file.readAll();
                file.close();
                auto profiles = MsgPack::unpack(data).toMap();

                QVariantMap::iterator i;
                for (i = profiles.begin(); i != profiles.end(); ++i) {
                    QByteArray data = MsgPack::pack(serialize());
                    qDebug() << "saved profile:" << profilePath(i.key());
                    QFile file(profilePath(i.key()));
                    file.open(QIODevice::WriteOnly);
                    file.write(MsgPack::pack(i.value()));
                    file.close();
                }
            }
        }
        reloadProfiles();
    });

    toolBar->addAction(tr("Export Profiles"),[=](){

        QDialog selectProfiles(this);
        selectProfiles.setWindowTitle("Select Profiles To Save");
        auto* verticalLayout = new QVBoxLayout(&selectProfiles);


        auto* lprofileList = new QListWidget(&selectProfiles);
        //list->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
        verticalLayout->addWidget(lprofileList);

        for (int i = 0; i < ui->listOfProfiles->count(); ++i) {
            QListWidgetItem* item = new QListWidgetItem( ui->listOfProfiles->itemText(i),lprofileList);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
        }

        auto profileButtons = new QDialogButtonBox(&selectProfiles);
        profileButtons->setObjectName(QStringLiteral("buttonBox"));
        profileButtons->setOrientation(Qt::Horizontal);
        profileButtons->setStandardButtons(QDialogButtonBox::Yes|QDialogButtonBox::No);
        connect(profileButtons->button(QDialogButtonBox::Yes),&QPushButton::clicked,[&selectProfiles,lprofileList,this]() {
            QString fileName = QFileDialog::getSaveFileName(&selectProfiles, tr("Save Data"), "", tr("All Files (*)"),
                                                            nullptr, QFileDialog::DontUseNativeDialog);
            if (fileName.isEmpty()) {
                return;
            } else {
                if (!fileName.endsWith(".profile")) {
                    fileName.append(".profile");
                }
                QVariantMap result;
                for (int i = 0; i < lprofileList->count(); i++) {
                    auto item = lprofileList->item(i);
                    if (item->checkState() == Qt::CheckState::Checked) {
                        QFile file(profilePath(item->text()));
                        if (file.exists()) {
                            file.open(QIODevice::ReadOnly);
                            QByteArray data = file.readAll();
                            file.close();
                            QVariantList payload = MsgPack::unpack(data).toList();
                            result.insert(item->text(),payload);
                        }
                    }
                }

                QFile save(fileName);
                QByteArray data = MsgPack::pack(result);
                qDebug() << "saved profile:" << fileName;
                save.open(QIODevice::WriteOnly);
                save.write(data);
                save.close();

            }
            selectProfiles.close();
        });

        connect(profileButtons->button(QDialogButtonBox::No),&QPushButton::clicked,[&selectProfiles]() {
            selectProfiles.close();
        });
        verticalLayout->addWidget(profileButtons);
        selectProfiles.exec();

    });
    this->layout()->setMenuBar(toolBar);



    connect(ui->addDeviceButton,&QPushButton::clicked,this,[=](){
        MbientConfigPanel* panel = new MbientConfigPanel(this);
        ui->deviceListContainer->addWidget(panel);
    });

    // only allow a limited set of characters
    connect(ui->listOfProfiles,&QComboBox::editTextChanged,this,[=](const QString& value){
        ui->listOfProfiles->setCurrentText(QString(value).remove(QRegExp("[^a-zA-Z\\d\\s]")));
    });


    reloadProfiles();

    connect(ui->loadProfileButton,&QPushButton::clicked,this,[=](){

        qDebug() << "load profile:" << profilePath(ui->listOfProfiles->currentText());
        QFile file(profilePath(ui->listOfProfiles->currentText()));
        if(file.exists()) {
            file.open(QIODevice::ReadOnly);
            QByteArray data = file.readAll();
            file.close();
            QVariantList payload = MsgPack::unpack(data).toList();
            this->deserialize(payload);
        }
    });

    connect(ui->deleteProfileButton,&QPushButton::clicked,[=](){
        qDebug() << "delete profile:" << profilePath(ui->listOfProfiles->currentText());
        QFile file(profilePath(ui->listOfProfiles->currentText()));
        if(file.exists()) {
            file.remove();
        }
        ui->listOfProfiles->removeItem(ui->listOfProfiles->currentIndex());
    });

    connect(ui->saveProfileButton,&QPushButton::clicked,this,[=](){
        QString target = ui->listOfProfiles->currentText();

        if(target.isEmpty()){
            bool  isValid = false;
            int c = 0;
            while (!isValid) {
                target = QString("profile%1").arg(c);
                c++;
                isValid = true;
                for (int i = 0; i < ui->listOfProfiles->count(); ++i) {
                    if(ui->listOfProfiles->itemText(i) == target){
                        isValid = false;
                        break;
                    }
                }
            }
        }

        //add the new target and clear the current text
        if(ui->listOfProfiles->findText(target) != 0)
            ui->listOfProfiles->addItem(target);


        QByteArray data = MsgPack::pack(serialize());
        qDebug() << "saved profile:" << profilePath(target);
        QFile file(profilePath(target));
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.close();
    });

    connect(ui->buttonBox->button(QDialogButtonBox::Apply),&QPushButton::clicked,[=](){
        QList<MbientConfigPanel*> result;
        for(int x = 0; x < this->ui->deviceListContainer->count();x++){
            MbientConfigPanel* p = static_cast<MbientConfigPanel*>(ui->deviceListContainer->itemAt(x)->widget());
            result.append(p);
        }
        emit onProfileSelected(result);
        this->close();
    });


    connect(ui->buttonBox->button(QDialogButtonBox::Discard),&QPushButton::clicked,[=](){
        this->close();
    });

}

void ProfileDialog::reloadProfiles(){
    ui->listOfProfiles->clear();
    // search and load up profiles
    QDirIterator it( QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/profiles", QStringList() << "*.dat", QDir::Files, QDirIterator::Subdirectories);
    qDebug() << "--- Found Profiles ---";
    while (it.hasNext()) {
        QFile file(it.next());
        qDebug() << file;
        if(file.exists()){
            QFileInfo fileInfo(file.fileName());
            ui->listOfProfiles->addItem(fileInfo.fileName().split('.')[0]);
        }
    }
}

QString ProfileDialog::profilePath(const QString& profile){
    return QString("%1/profiles/%2.dat").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).arg(profile);
}


void ProfileDialog::deserialize(QVariantList payload){
    for(int x = 0; x < this->ui->deviceListContainer->count();x++){
        MbientConfigPanel* p = static_cast<MbientConfigPanel*>(ui->deviceListContainer->itemAt(x)->widget());
        p->deleteLater();
    }
    for(int i = 0; i < payload.count(); ++i){
        MbientConfigPanel* panel = new MbientConfigPanel(this);
        ui->deviceListContainer->addWidget(panel);
        panel->deserialize(payload.at(i).toMap());

    }
}

QVariantList ProfileDialog::serialize() {
    QVariantList result;
    for(int x = 0; x < this->ui->deviceListContainer->count();x++){
        MbientConfigPanel* p = static_cast<MbientConfigPanel*>(ui->deviceListContainer->itemAt(x)->widget());
        result.append(p->serialize());
    }
    return result;
}

