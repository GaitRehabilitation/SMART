//
// Created by michaelpollind on 21/12/18.
//

#ifndef SMART_MBIENTCONFIGPANEL_H
#define SMART_MBIENTCONFIGPANEL_H

#include <QWidget>
#include <QVariantMap>

namespace Ui {
    class MbientConfigPanel;
}

class MbientConfigPanel : public QWidget {
    Q_OBJECT
private:
    Ui::MbientConfigPanel *ui;
    void updateDisabled();
public:
    const float ACC_ODR_RANGE[4] = {12.5f, 50.f,100.0f,200.0f};
    const float ACC_FSR_RANGE[4] = {2.0f,4.0f,8.0f,16.0f};

    const float FUSION_RANGE[4] = {8.0f,12.5f,50.0f,100.0f};

    MbientConfigPanel(QWidget *parent = nullptr);
    QVariantMap getConfig();
    void setConfig(QVariantMap map);
    bool isSensorFusionEnabled();

};


#endif //SMART_MBIENTCONFIGPANEL_H
