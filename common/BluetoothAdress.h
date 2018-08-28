//
// Created by Michael on 8/27/2018.
//

#ifndef SMART_BLUETOOTHADRESS_H
#define SMART_BLUETOOTHADRESS_H

#include <QString>

class BluetoothAdress{
private:
    QString m_mac;
    QString m_title;
public:
    BluetoothAdress(const QString& mac, const QString& title);
    BluetoothAdress(const BluetoothAdress& address);

    ~BluetoothAdress();

    const QString& getTitle() const;
    const QString& getMac() const;

};

#endif //SMART_BLUETOOTHADRESS_H
