/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Packet_xrMULTI.h
 * Author: chty
 *
 * Created on 2021年10月19日, 上午11:38
 */

#ifndef PACKET_XRMULTI_H
#define PACKET_XRMULTI_H
#include"Packet.h"
class Packet;
class BTSID;
class MSID;
class Packet_xrMULTI : public Packet{
public:
    bool ifI;
    Packet_xrMULTI(int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL, int originalbornslot=0);
    Packet_xrMULTI(int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL, const MSID& _msid, const BTSID& _btsid,int originalbornslot=0,bool _ifI=true);
    //PacketI(const PacketI& orig);
    void LogFinishedPacket() override;
    ~Packet_xrMULTI() override = default;
private:

};


#endif /* PACKET_XRMULTI_H */

