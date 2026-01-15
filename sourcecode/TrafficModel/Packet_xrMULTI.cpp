/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Packet_xrMULTI.h"
#include"Packet.h"

Packet_xrMULTI::Packet_xrMULTI(int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL, int originalbornslot):
Packet( _ID,  _dPackeSizeKbits,  _iBornTimeSlot,  _iDLorUL,  originalbornslot ){
   
    ifI=true;
}
Packet_xrMULTI::Packet_xrMULTI(int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL, const MSID& _msid, const BTSID& _btsid,int originalbornslot,bool _ifI):
Packet( _ID,  _dPackeSizeKbits,  _iBornTimeSlot,  _iDLorUL,  _msid,  _btsid, originalbornslot){
    ifI=_ifI;
}

void Packet_xrMULTI::LogFinishedPacket() {
    LogFinishedPacketXR_multi_stream(ifI);
}