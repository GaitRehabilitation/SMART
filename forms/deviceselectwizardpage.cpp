#include "deviceselectwizardpage.h"
#include "ui_deviceselectwizardpage.h"
#include <QListWidget>

#include "common/devicehandler.h"

#include "common/metawearhandler.h"

const QString METAWEAR_SENSOR = "MetaWearSensor(MbientLab";

DeviceSelectWizardPage::DeviceSelectWizardPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::DeviceSelectWizardPage)
{
    ui->setupUi(this);
    ui->deviceList->addItem(new QListWidgetItem(METAWEAR_SENSOR));
    ui->deviceList->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->deviceList, SIGNAL(itemActivated(QListWidgetItem*)),
             this, SLOT(itemActivated(QListWidgetItem*)));
}

DeviceSelectWizardPage::~DeviceSelectWizardPage()
{
    delete ui;
}

bool DeviceSelectWizardPage::isComplete() const{
    return ui->deviceList->selectedItems().count() > 0;
}

void DeviceSelectWizardPage::itemActivated(QListWidgetItem *item){
    this->selectedSensor = item->text();
    emit completeChanged();
}


DeviceHandler*  DeviceSelectWizardPage::requestDeviceHandler(){
    if(this->selectedSensor == METAWEAR_SENSOR){
        return new MetaWearHandler;
    }
    return nullptr;
}
