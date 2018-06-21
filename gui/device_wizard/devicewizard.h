#ifndef DEVICEWIZARD_H
#define DEVICEWIZARD_H
#include <QQmlEngine>
#include <QWizard>
#include <QWidget>

class DeviceWizard : public QWizard
{
    Q_OBJECT

public:

    enum {
        DEVICE_DISCOVERY,
        DEVICE_CONNECT
    };

    DeviceWizard(QWidget *parent = 0);

    void accept() override;
};

#endif // DEVICEWIZARD_H
