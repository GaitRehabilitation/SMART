//
// Created by michaelpollind on 21/12/18.
//

#ifndef SMART_PROFILEWINDOW_H
#define SMART_PROFILEWINDOW_H

#include <QDialog>

namespace Ui {
    class ProfileDialog;
}

class ProfileDialog : public QDialog {
    Q_OBJECT
private:
    Ui::ProfileDialog *ui;
    QString profilePath(const QString& profile);

    void accept() override;
    void reject() override;
public:
    ProfileDialog(QWidget *parent = nullptr);
    QVariantList serialize();
    void deserialize(QVariantList payload);
signals:
    void onProfileSelected(const QVariantList &payload);
};


#endif //SMART_PROFILEWINDOW_H
