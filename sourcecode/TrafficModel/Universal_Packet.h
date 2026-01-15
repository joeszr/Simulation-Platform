//
// Created by ChTY on 2022/10/7.
//

#ifndef CHTY_UNIVERSAL_PACKET_H
#define CHTY_UNIVERSAL_PACKET_H
#include "Packet.h"
#include "../Statistician/Observer.h"
enum PacketType{
    XRsingle=Parameters::XRSinglePacket,XRmultiI=Parameters::XRMultiI,XRmultiP=Parameters::XRMultiP,FTPPacket=Parameters::FTPPacket
};
ostream& operator <<(ostream&,PacketType);
class Universal_Packet: public Packet {
public:
    int qos;
    PacketType type;
    Universal_Packet(int _type, int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL, const MSID& _msid, const BTSID& _btsid,int originalbornslot=0);
    ~Universal_Packet() override;
    void LogFinishedPacket() override;
};
void deleteUP(Universal_Packet* p);



#endif //CHTY_UNIVERSAL_PACKET_H
