//
// Created by michaelpollind on 21/12/18.
//


#include "forms/profiledialog.h"
#include "forms/mbientconfigpanel.h"
#include "ui_profiledialog.h"

#include <QtWidgets/QPushButton>
#include <QStandardPaths>

#include <msgpack.h>
#include <QString>
#include <QRegExp>
#include <QFile>
#include <QtCore/QDirIterator>
#include <QDebug>

ProfileDialog::ProfileDialog(QWidget *parent): ui(new Ui::ProfileDialog) {
    qDebug() << QString("%1/profiles").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!QDir().mkpath(QString("%1/profiles").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)))){
        qDebug() << "Failed to make profile path";
    }

    ui->setupUi(this);
    connect(ui->addDeviceButton,&QPushButton::clicked,this,[=](){
        MbientConfigPanel* panel = new MbientConfigPanel(this);
        ui->deviceListContainer->addWidget(panel);
    });

    // only allow a limited set of characters
    connect(ui->listOfProfiles,&QComboBox::editTextChanged,this,[=](const QString& value){
        ui->listOfProfiles->setCurrentText(QString(value).remove(QRegExp("[^a-zA-Z\\d\\s]")));
    });


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
        if(!ui->listOfProfiles->findText(target))
            ui->listOfProfiles->addItem(target);


        QByteArray data = MsgPack::pack(serialize());
        qDebug() << "saved profile:" << profilePath(target);
        QFile file(profilePath(target));
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.close();
    });
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

void ProfileDialog::accept() {
    emit onProfileSelected(serialize());
    this->close();
}

void ProfileDialog::reject() {
    this->close();
}