//
// Created by michaelpollind on 21/12/18.
//

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
