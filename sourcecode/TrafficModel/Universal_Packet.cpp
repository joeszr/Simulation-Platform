//
// Created by ChTY on 2022/10/7.
//

#include "Universal_Packet.h"
#include "../NetworkDrive/Clock.h"
std::mutex PacketRecord;
std::mutex FTPPackettxt;
ostream &operator<<(ostream &o, PacketType t) {
    string s;
    switch (t) {
        case XRsingle:
            s = "XRsingle";
            break;
        case XRmultiI:
            s = "XRmultiI";
            break;
        case XRmultiP:
            s = "XRmultiP";
            break;
        case FTPPacket:
            s = "FTPPacket";
            break;
    }
    return o << s;
}

Universal_Packet::Universal_Packet(int _type, int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL,
                                   const MSID& _msid, const BTSID& _btsid, int originalbornslot)
        : Packet(_ID, _dPackeSizeKbits, _iBornTimeSlot, _iDLorUL, _msid, _btsid, originalbornslot) {
    type = PacketType(_type);
    qos = _type;
}

Universal_Packet::~Universal_Packet() {
////    static int U=0;
//    std::lock_guard<std::mutex> l(PacketRecord);
////    cout<<"U="<<U++<<endl;
//    int iID = GetID();
//    MSID msid = GetMSID();
//    BTSID btsid = GetBTSID();
//    bool iIsFinished = IsFullySuccessRecieved();
////    bool iIsCompleted = IsCompleted();
//    int iBornTimeSlot = GetBornTimeSlot();
//    int iFinishTimeSlot = GetFinishTimeSlot();
////    if(IsDrop_DueToTimeout()||iFinishTimeSlot!=-1){
//        int iDelaySlot = GetDelayTimeSlot();
//        double dOriginSizeKbits = GetOriginSizeKbits();
//        double dRemainderSizeKbits = GetRemainUntransmitted_SizeKbits();
////        double dTxSizeKbits = dOriginSizeKbits - dRemainderSizeKbits;
//        double dRecievedSizeKbits = GetSuccessRecieved_SizeKbits();
//        Observer::Print("PacketRecord") << setw(20) << iID
//                << setw(20) << msid
//                << setw(20) << btsid
//                << setw(20) << dOriginSizeKbits
//                << setw(20) << dRemainderSizeKbits
//                << setw(20) << dRecievedSizeKbits
//                << setw(20) << (iIsFinished ? "True" : "False")
//                << setw(20) << iBornTimeSlot
//                << setw(20) << iFinishTimeSlot
//                << setw(20) << (IsDrop_DueToTimeout()?"Droped":std::to_string(iDelaySlot))
//                << setw(20) << type
//                << setw(20) << qos
//                << setw(20) << (IsDLPacket()?"DL":"UL");
//    if(IsDrop_DueToTimeout()||iFinishTimeSlot!=-1){
//        Observer::Print("PacketRecord")<< setw(20) << "have";
//    }
//    Observer::Print("PacketRecord")  << endl;
//    //}
}


void Universal_Packet::LogFinishedPacket() {
//    static int U=0;
    std::lock_guard<std::mutex> l(PacketRecord);
//    cout<<"U="<<U++<<endl;
    int iID = GetID();
    MSID msid = GetMSID();
    BTSID btsid = GetBTSID();
    bool iIsFinished = IsFullySuccessRecieved();
//    bool iIsCompleted = IsCompleted();
    int iBornTimeSlot = GetBornTimeSlot();
    int iFinishTimeSlot = GetFinishTimeSlot();
//    if(IsDrop_DueToTimeout()||iFinishTimeSlot!=-1){
    int iDelaySlot = GetDelayTimeSlot();
    double dOriginSizeKbits = GetOriginSizeKbits();
    double dRemainderSizeKbits = GetRemainUntransmitted_SizeKbits();
//        double dTxSizeKbits = dOriginSizeKbits - dRemainderSizeKbits;
    double dRecievedSizeKbits = GetSuccessRecieved_SizeKbits();
    Observer::Print("PacketRecord") << setw(20) << iID
                                    << setw(20) << msid
                                    << setw(20) << btsid
                                    << setw(20) << dOriginSizeKbits
                                    << setw(20) << dRemainderSizeKbits
                                    << setw(20) << dRecievedSizeKbits
                                    << setw(20) << (iIsFinished ? "True" : "False")
                                    << setw(20) << iBornTimeSlot
                                    << setw(20) << iFinishTimeSlot
                                    << setw(20) << (IsDrop_DueToTimeout()?"Droped":std::to_string(iDelaySlot))
                                    << setw(20) << type
                                    << setw(20) << qos
                                    << setw(20) << (IsDLPacket()?"DL":"UL");
    Observer::Print("PacketRecord")  << endl;
    //}
}

void deleteUP(Universal_Packet* p){
    //static int SUP=0;
    delete p;
    //cout<<"SUP="<<SUP++<<endl;
}