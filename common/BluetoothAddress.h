//
// Created by Michael on 8/27/2018.
//

#ifndef SMART_BLUETOOTHADRESS_H
#define SMART_BLUETOOTHADRESS_H

#include <QString>

class BluetoothAddress{
private:
    QString m_mac;
    QString m_title;
public:
    BluetoothAddress();
    BluetoothAddress(const QString& mac, const QString& title);
    BluetoothAddress(const BluetoothAddress& address);
    BluetoothAddress(unsigned long long address, const QString& title);

    ~BluetoothAddress();

    const QString& getTitle() const;
    const QString& getMac() const;

};

#endif //SMART_BLUETOOTHADRESS_H
