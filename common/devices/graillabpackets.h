#ifndef GRAILLABPACKETS_H
#define GRAILLABPACKETS_H


struct SetColor
{
    uint8 packetID;
    byte code;
    byte red;
    byte green;
    byte blue;
};

struct SensorTag
{
    byte code;
    char* payload;
};

struct SensorData
{
    byte code;
};



#endif // GRAILLABPACKETS_H
