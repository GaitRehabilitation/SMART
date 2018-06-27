#include "metawearwrapper.h"
#include "metawear/core/metawearboard.h"
#include "metawear/core/status.h"

#include <QBluetoothAddress>
#include <QLowEnergyController>
#include <QLowEnergyController>
#include <QBluetoothUuid>
#include <QObject>
#include <QByteArray>


static quint128 convertToQuint128(uint8_t * low,uint8_t * high){
     quint128 result;
    for(int i = 0; i < 8; i++) {
        result.data[i] = high[i];
    }
    for(int i = 0; i < 8; i++) {
        result.data[i+8] = low[i];
    }
    return result;
}


void MetawearWrapper::read_gatt_char_qt(void* context, const void* caller, const MblMwGattChar* characteristic,MblMwFnIntVoidPtrArray handler) {
    MetawearWrapper* wrapper = (MetawearWrapper *)context;
    wrapper->readGattHandler = handler;

    QBluetoothUuid service_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->service_uuid_low,(uint8_t*)&characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->service_uuid_low,(uint8_t*)&characteristic->service_uuid_high));

    QLowEnergyService* service =  wrapper->services.value(service_uuid.toString());
    if(service == nullptr){
        qWarning() << "failed to parse service uuid" << service_uuid.toString();
        return;
    }
    QLowEnergyCharacteristic c =  service->characteristic(characteristic_uuid);
    service->readCharacteristic(c);
}

 void MetawearWrapper::write_gatt_char_qt(void *context, const void* caller, MblMwGattCharWriteType writeType, const MblMwGattChar* characteristic,const uint8_t* value, uint8_t length){
     MetawearWrapper* wrapper = (MetawearWrapper *)context;
     QBluetoothUuid service_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->service_uuid_low,(uint8_t*)&characteristic->service_uuid_high));
     QBluetoothUuid characteristic_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->service_uuid_low,(uint8_t*)&characteristic->service_uuid_high));

      QLowEnergyService* service =  wrapper->services.value(service_uuid.toString());
      if(service == nullptr){
          qWarning() << "failed to parse service uuid" << service_uuid.toString();
          return;
      }
      QLowEnergyCharacteristic c =  service->characteristic(characteristic_uuid);
      QByteArray payload;
      for(int x = 0; x< length; ++x){
          payload.append(value[x]);
      }
      service->writeCharacteristic(c,payload);
}

 void MetawearWrapper::enable_char_notify_qt(void* context, const void* caller, const MblMwGattChar* characteristic,MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready) {
     MetawearWrapper* wrapper = (MetawearWrapper *)context;
     wrapper->notificationHandler = handler;
     QBluetoothUuid service_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->service_uuid_low,(uint8_t*)&characteristic->service_uuid_high));
     QBluetoothUuid characteristic_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->service_uuid_low,(uint8_t*)&characteristic->service_uuid_high));

     QLowEnergyService* service =  wrapper->services.value(service_uuid.toString());
     if(service == nullptr){
         qWarning() << "failed to parse service uuid" << service_uuid.toString();
         return;
     }
     QLowEnergyCharacteristic c =  service->characteristic(characteristic_uuid);

     QLowEnergyDescriptor notification = c.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
     if (!notification.isValid())
            return;

     service->writeDescriptor(notification, QByteArray::fromHex("0100"));
     QObject* temp = new QObject();
     service->connect(service,&QLowEnergyService::descriptorWritten,temp,[caller,ready,temp](const QLowEnergyDescriptor &descriptor, const QByteArray &newValue){
         ready(caller,MBL_MW_STATUS_OK);
         temp->deleteLater();
     });
}

 void MetawearWrapper::on_disconnect_qt(void *context, const void* caller, MblMwFnVoidVoidPtrInt handler){
    MetawearWrapper* wrapper = (MetawearWrapper *)context;

    wrapper->disconnectedHandler = handler;
}

// ------------------------------------------------------------------------------------------


MetawearWrapper::MetawearWrapper(QBluetoothAddress address,QObject *parent )
    : QObject(parent),
      controller(new QLowEnergyController(address,this)),
      services(QMap<QString,QLowEnergyService*>())
{
    this->connectImpl.context = this;
    this->connectImpl.write_gatt_char = write_gatt_char_qt;
    this->connectImpl.read_gatt_char = read_gatt_char_qt;
    this->connectImpl.enable_notifications = enable_char_notify_qt;
    this->connectImpl.on_disconnect = on_disconnect_qt;
    this->board = mbl_mw_metawearboard_create(&this->connectImpl);


    connect(this->controller, SIGNAL(serviceDiscovered(QBluetoothUuid)),this, SLOT(onServiceDiscovered(QBluetoothUuid)));
    connect(this->controller, SIGNAL(discoveryFinished()),this, SLOT(onServiceScanFinished()));

    connect(this->controller,SIGNAL(disconnected()),this,SIGNAL(disconnected()));

    connect(this->controller,SIGNAL(connected()),this,SLOT(onDeviceConnected()));
    connect(this->controller,SIGNAL(error(QLowEnergyController::Error)),this,SIGNAL(bluetoothError(QLowEnergyController::Error)));

    if(this->controller->state() == QLowEnergyController::UnconnectedState) {
        qDebug() << "LE Controller: now connect to device ...";
       this->controller->connectToDevice();
    } else if(this->controller->state() == QLowEnergyController::ConnectedState){
        qDebug() << "LE Controller: allready connected - now discover services ...";
        this->controller->discoverServices();
    }

}

void MetawearWrapper::onDeviceConnected(){
   qDebug() << "connected to device";
   emit connected();
   qDebug() << "now discovering services";
   controller->discoverServices();

}

void MetawearWrapper::onServiceDiscovered(const QBluetoothUuid &gatt){
    QLowEnergyService* service =  this->controller->createServiceObject(gatt,this);
    service->discoverDetails();
    services.insert(gatt.toString(),service);

    service->connect(service,&QLowEnergyService::characteristicRead,this,[this](QLowEnergyCharacteristic characteristic,QByteArray data){
        this->readGattHandler(this->board,(uint8_t*)data.data(),data.length());
    });
    this->connect(service,SIGNAL(error(QLowEnergyService::ServiceError)),this,SIGNAL(bluetoothCharacteristicError(QLowEnergyService::ServiceError)));

    qDebug() << "Service Name: " << service->serviceName();
    qDebug() << "Service UUID: " << service->serviceUuid().toString();

}

void MetawearWrapper::onServiceScanFinished(){
    mbl_mw_metawearboard_initialize(this->board, nullptr, [](void* context, MblMwMetaWearBoard* board, int32_t status) -> void {
        if (!status) {
            qWarning() << QString("Error initializing board: %d\n").arg(status);
        } else {
            printf("Board initialized\n");
        }
    });

}

MetawearWrapper::~MetawearWrapper(){
    handleDisconnect();
}

QLowEnergyController* MetawearWrapper::getLowEnergyController() {
    return this->getLowEnergyController();
}
void MetawearWrapper::handleConnection(){
    this->controller->discoverServices();
}

void MetawearWrapper::handleDisconnect(){
    this->disconnectedHandler(board,0);
}




QLowEnergyService* MetawearWrapper::getSerivce(const QBluetoothUuid &uuid){
//    QLowEnergyService* service = controller->addService(uuid);
//    if(!service){
//        qWarning() << "Cannot create service for uuid";
//        return nullptr;
//    }
//    return service;
    return nullptr;
}



