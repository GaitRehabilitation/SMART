#include "metawearwrapper.h"
#include "metawear/core/metawearboard.h"
#include "metawear/core/status.h"
#include "metawear/platform/memory.h"

#include <QBluetoothAddress>
#include <QLowEnergyController>
#include <QLowEnergyController>
#include <QBluetoothUuid>
#include <QObject>
#include <QByteArray>
#include <QThread>

static quint128 convertToQuint128(uint8_t * low,uint8_t * high){
    quint128 result;
    for(int i = 0; i < 8; i++) {
        result.data[i] = high[7-i];
    }
    for(int i = 0; i < 8; i++) {
        result.data[i+8] = low[7-i];
    }
    return result;
}

void MetawearWrapper::read_gatt_char_qt(void* context, const void* caller, const MblMwGattChar* characteristic,MblMwFnIntVoidPtrArray handler) {
    MetawearWrapper* wrapper = (MetawearWrapper *)context;
    wrapper->m_readGattHandler = handler;

    QBluetoothUuid service_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->service_uuid_low,(uint8_t*)&characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->uuid_low,(uint8_t*)&characteristic->uuid_high));

    QLowEnergyService* service =  wrapper->m_services.value(service_uuid.toString());
    if(service == nullptr){
        qWarning() << "Invliad service Uuid(read gatt)" << service_uuid.toString();
        return;
    }
    QLowEnergyCharacteristic c =  service->characteristic(characteristic_uuid);
    if(!c.isValid()){
        qWarning() << "Invalid characteristic Uuid(read gatt)" << characteristic_uuid.toString();
        return;
    }
    service->readCharacteristic(c);
}

void MetawearWrapper::write_gatt_char_qt(void *context, const void* caller, MblMwGattCharWriteType writeType, const MblMwGattChar* characteristic,const uint8_t* value, uint8_t length){
    MetawearWrapper* wrapper = (MetawearWrapper *)context;
    QBluetoothUuid service_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->service_uuid_low,(uint8_t*)&characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->uuid_low,(uint8_t*)&characteristic->uuid_high));


    QLowEnergyService* service =  wrapper->m_services.value(service_uuid.toString());
    if(service == nullptr){
        qWarning() << "failed to parse service uuid(write gatt)" << service_uuid.toString();
        return;
    }
    QLowEnergyCharacteristic c =  service->characteristic(characteristic_uuid);
    if(!c.isValid()){
        qWarning() << "Invalid characteristic Uuid(write gatt)" << characteristic_uuid.toString();
        return;
    }
    QByteArray payload;
    for(int x = 0; x< length; ++x){
        payload.append(value[x]);
    }
    service->writeCharacteristic(c,payload);
}

void MetawearWrapper::enable_char_notify_qt(void* context, const void* caller, const MblMwGattChar* characteristic,MblMwFnIntVoidPtrArray handler, MblMwFnVoidVoidPtrInt ready) {
    MetawearWrapper* wrapper = (MetawearWrapper *)context;
    wrapper->m_notificationHandler = handler;
    QBluetoothUuid service_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->service_uuid_low,(uint8_t*)&characteristic->service_uuid_high));
    QBluetoothUuid characteristic_uuid = QBluetoothUuid(convertToQuint128((uint8_t*)&characteristic->uuid_low,(uint8_t*)&characteristic->uuid_high));

    QLowEnergyService* service =  wrapper->m_services.value(service_uuid.toString());
    if(service == nullptr){
        qWarning() << "failed to parse service uuid(char notify):" << service_uuid.toString();
        return;
    }
    QLowEnergyCharacteristic c =  service->characteristic(characteristic_uuid);
    if(!c.isValid()){
        qWarning() << "Invalid characteristic Uuid(char notify)" << characteristic_uuid.toString();
    }

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
    wrapper->m_disconnectedHandler = handler;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------
// Class
// -------------------------------------------------------------------------------------------------------------------------------------------------------------

MetawearWrapper::MetawearWrapper(QObject *parent )
    : QObject(parent),
      m_services(QMap<QString,QLowEnergyService*>()),
      m_controller(0),
      m_serviceReady(0),
      m_isMetawareReady(0),
      m_readyCharacteristicCount(0),
      m_notificationHandler(0),
      m_disconnectedHandler(0),
      m_readGattHandler(0)
{
}

void MetawearWrapper::setDevice(const QBluetoothDeviceInfo &device){
    this->m_currentDevice = device;

    // Disconnect and delete old connection
    if (m_controller) {
        m_controller->disconnectFromDevice();
        delete m_controller;
        m_controller = 0;
    }

    m_controller = new QLowEnergyController(m_currentDevice,this);

    //Make connections

    //Service Discovery
    connect(this->m_controller, SIGNAL(serviceDiscovered(QBluetoothUuid)),this, SLOT(onServiceDiscovered(QBluetoothUuid)));
    connect(this->m_controller, SIGNAL(discoveryFinished()),this, SLOT(onServiceDiscoveryFinished()));

    //controller connection/disconnect
    connect(this->m_controller,SIGNAL(disconnected()),this,SLOT(onDisconnect()));
    connect(this->m_controller,SIGNAL(connected()),this,SLOT(onConnected()));

    //controller error
    connect(this->m_controller,SIGNAL(error(QLowEnergyController::Error)),this,SLOT(onControllerError(QLowEnergyController::Error)));

    MblMwBtleConnection btleConnection;
    btleConnection.context = this;
    btleConnection.write_gatt_char = write_gatt_char_qt;
    btleConnection.read_gatt_char = read_gatt_char_qt;
    btleConnection.enable_notifications = enable_char_notify_qt;
    btleConnection.on_disconnect = on_disconnect_qt;
    this->m_metaWearBoard = mbl_mw_metawearboard_create(&btleConnection);

    if(m_controller->state() == QLowEnergyController::UnconnectedState){
        qDebug() << "Starting connection";
        m_controller->connectToDevice();
    }
    else{
        qDebug() << "Controller already connected. Search services..";
        this->m_controller->discoverServices();
    }
}



void MetawearWrapper::onServiceDiscovered(const QBluetoothUuid &newService){
    QString uuid = newService.toString();
    QLowEnergyService* lowEnergyService =  this->m_controller->createServiceObject(newService,this);
    m_services.insert(uuid,lowEnergyService);

    lowEnergyService->connect(lowEnergyService,SIGNAL(characteristicRead(QLowEnergyCharacteristic,QByteArray)),this,SLOT(onCharacteristicRead(QLowEnergyCharacteristic,QByteArray)));
    lowEnergyService->connect(lowEnergyService, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),this,SLOT(onCharacteristicNotifications(QLowEnergyCharacteristic,QByteArray)));

    lowEnergyService->connect(lowEnergyService, SIGNAL(error(QLowEnergyService::ServiceError)),this,SLOT(onCharacteristicError(QLowEnergyService::ServiceError)));

    qDebug() << "Service Name: " << lowEnergyService->serviceName();
    qDebug() << "Service UUID: " << lowEnergyService->serviceUuid().toString();
}

void MetawearWrapper::onCharacteristicRead(QLowEnergyCharacteristic characteristic, QByteArray payload){

    qDebug() <<payload;
    if(this->m_readGattHandler != nullptr){
        this->m_readGattHandler(this->m_metaWearBoard,(uint8_t*)payload.data(),payload.length());
    }
}

void MetawearWrapper::onCharacteristicNotifications(QLowEnergyCharacteristic characteristic, QByteArray payload){
    qDebug() <<payload;
    if(this->m_notificationHandler != nullptr){
        this->m_notificationHandler(this->m_metaWearBoard,(uint8_t*)payload.data(),payload.length());
    }
}

void MetawearWrapper::onServiceDiscoveryFinished(){
    foreach(QString key, this->m_services.keys()){
        QLowEnergyService* lowEnergyService = this->m_services.value(key);
        lowEnergyService->connect(lowEnergyService,&QLowEnergyService::stateChanged,[this,lowEnergyService](QLowEnergyService::ServiceState state){
            if(state == QLowEnergyService::ServiceDiscovered){
                this->m_readyCharacteristicCount++;
                if(this->m_readyCharacteristicCount == this->m_services.count())
                {
                    this->metwareIntialize();
                }
            }
        });
        lowEnergyService->discoverDetails();
    }
}

void MetawearWrapper::metwareIntialize(){
    if(this->m_isMetawareReady == false){
        mbl_mw_metawearboard_initialize(this->m_metaWearBoard, this, [](void* context, MblMwMetaWearBoard* board, int32_t status) -> void {
            MetawearWrapper* wrapper = (MetawearWrapper *)context;
            if (!status) {
                qWarning() << "Error initializing board:" << status;
                emit wrapper->metawareFailedToInitialized(status);
            } else {
                qDebug() << "Board initialized";

                auto dev_info = mbl_mw_metawearboard_get_device_information(board);
                qDebug() << "firmware = " << dev_info->firmware_revision << endl;
                mbl_mw_memory_free((void*) dev_info);

                qDebug() << "model = " << mbl_mw_metawearboard_get_model(board) << endl;

                emit wrapper->metawareInitialized();
            }
        });
        this->m_isMetawareReady = true;
    }
}

void MetawearWrapper::onConnected(){
    qDebug() << "Controller conected. Search services..";
    this->m_controller->discoverServices();
    emit connected();
}

void MetawearWrapper::onDisconnect(){
    qDebug() << "LowEnergy controller disconnected";
    emit disconnected();
}

void MetawearWrapper::onControllerError(QLowEnergyController::Error e){
    switch (e) {
    case QLowEnergyController::Error::UnknownError:
        qWarning() << "UnknownError";
        break;
    case QLowEnergyController::Error::UnknownRemoteDeviceError:
        qWarning() << "UnknownRemoteDeviceError";
        break;
    case QLowEnergyController::Error::NetworkError:
        qWarning() << "NetworkError";
        break;
    case QLowEnergyController::Error::InvalidBluetoothAdapterError:
        qWarning() << "InvalidBluetoothAdapterError";
        break;
    case QLowEnergyController::Error::ConnectionError:
        qWarning() << "ConnectionError";
        break;
    case QLowEnergyController::Error::AdvertisingError:
        qWarning() << "AdvertisingError";
        break;

    }
}

void MetawearWrapper::onStateChange(QLowEnergyController::ControllerState state)
{
    switch(state){
    case QLowEnergyController::UnconnectedState:
        qDebug() << "UnconnectedState";
        break;
    case QLowEnergyController::ConnectingState:
        qDebug() << "ConnectingState";
        break;
    case QLowEnergyController::ConnectedState:
        qDebug() << "ConnectedState";
        break;
    case QLowEnergyController::DiscoveringState:
        qDebug() << "DiscoveringState";
        break;
    case QLowEnergyController::DiscoveredState:
        qDebug() << "DiscoveredState";
        break;
    case QLowEnergyController::ClosingState:
        qDebug() << "ClosingState";
        break;
    case QLowEnergyController::AdvertisingState:
        qDebug() << "AdvertisingState";
        break;
    }
}

void MetawearWrapper::onCharacteristicError(QLowEnergyService::ServiceError e){
    switch (e) {
    case QLowEnergyService::ServiceError::UnknownError:
        qWarning() << "UnknownError";
        break;
    case QLowEnergyService::ServiceError::OperationError:
        qWarning() << "OperationError";
        break;
    case QLowEnergyService::ServiceError::CharacteristicReadError:
        qWarning() << "CharacteristicReadError";
        break;
    case QLowEnergyService::ServiceError::CharacteristicWriteError:
        qWarning() << "CharacteristicWriteError";
        break;
    case QLowEnergyService::ServiceError::DescriptorReadError:
        qWarning() << "DescriptorReadError";
        break;
    case QLowEnergyService::ServiceError::DescriptorWriteError:
        qWarning() << "DescriptorWriteError";
        break;
    }
}

MblMwMetaWearBoard* MetawearWrapper::getBoard(){
    return m_metaWearBoard;
}

MetawearWrapper::~MetawearWrapper(){
}

QLowEnergyController* MetawearWrapper::getController() {
    return this->m_controller;
}






