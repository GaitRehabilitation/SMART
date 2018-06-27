#ifndef DEVICESERVICE_H
#define DEVICESERVICE_H

#include <QObject>

class DeviceService : public QObject
{
    Q_OBJECT
public:
    explicit DeviceService(QObject *parent = nullptr);

signals:

public slots:
};

#endif // DEVICESERVICE_H