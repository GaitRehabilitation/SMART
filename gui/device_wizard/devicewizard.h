#ifndef DEVICEWIZARD_H
#define DEVICEWIZARD_H

#include <QWizard>

class DeviceWizard : public QWizard
{
    Q_OBJECT

public:
    DeviceWizard(QWidget *parent = 0);

    void accept() override;
};

#endif // DEVICEWIZARD_H
