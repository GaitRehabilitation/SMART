#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H

#include <QString>

class DeviceHandler
{
public:
    DeviceHandler();
    ~DeviceHandler();
    virtual QList<QString> getStreamTags();

};

#endif // DEVICEHANDLER_H
