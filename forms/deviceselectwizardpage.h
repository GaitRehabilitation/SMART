#ifndef DEVICESELECTWIZARDPAGE_H
#define DEVICESELECTWIZARDPAGE_H

#include <QWizardPage>

QT_FORWARD_DECLARE_CLASS(DeviceHandler)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem);

namespace Ui {
    class DeviceSelectWizardPage;
}

class DeviceSelectWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit DeviceSelectWizardPage(QWidget *parent = 0);
    ~DeviceSelectWizardPage();
    // QWizardPage
    bool isComplete() const;

private:
    Ui::DeviceSelectWizardPage *ui;
    QString selectedSensor;
private slots:
    void itemActivated(QListWidgetItem *item);
public slots:
     DeviceHandler* requestDeviceHandler();

};

#endif // DEVICESELECTWIZARDPAGE_H
