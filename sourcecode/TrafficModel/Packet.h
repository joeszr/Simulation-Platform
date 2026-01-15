///@file  Packet.h
///@brief Packet类的声明
///@author wangfei
#pragma once
#include "../BaseStation/BTSID.h"
#include "../MobileStation/MSID.h"
class MSID;
class BTSID;
///packet类

class Packet {
private:
    double m_OriginSize;
    int m_ID;
    int m_iBornTimeSlot;
    int m_iFinishTimeSlot;
    int m_iDLorUL; //0--DL 1--UL
    bool m_bFullySuccessRecieved;
    bool m_bIsCompleted;
    int m_originalbornslot;
    //true->drop
    bool m_bDrop_DueToTimeout;
    double m_dRemainUntransmittedSizeKbits;
    double m_dSuccessReceivedSizeKbits;
    MSID m_msid;
    BTSID m_btsid;
public:
    int GetBornTimeSlot() const;
    int GetFinishTimeSlot() const;
    int GetDelayTimeSlot() const;
    bool IsDLPacket() const;
    void ReduceUntransmittedSizeKbits(double _dSizeKbits);
    void RecordLostSizeKbits(double _dSizeKbits);
    bool IsFullySuccessRecieved() const;
    bool IsCompleted() const;
    void SetCompleted();
    double GetRemainUntransmitted_SizeKbits() const;
    double GetSuccessRecieved_SizeKbits() const;
    double GetOriginSizeKbits()const;
    void RecordSuccessRecievedSizeKbits(double _dKbits, int _PDSCHReceived_slot = -1);
    int GetID() const;
    MSID GetMSID() const;
    BTSID GetBTSID() const;

    bool IsDrop_DueToTimeout() const;

    void SetDrop_DueToTimeout();

    void DropPacket_DueToTimeout();

    bool IsTimeout(int _iTime) const;
    
    virtual void LogFinishedPacket();
    void LogFinishedPacketXR_multi_stream(bool ifI);
public:
    //Packet(int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL);
    //Packet(int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL, MSID _msid, BTSID _btsid);
    Packet(int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL, int originalbornslot=0);//add xrdropslot for xr jitter
    Packet(int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL, const MSID& _msid, const BTSID& _btsid,int originalbornslot=0);//add xrdropslot for xr jitter
    
//     Packet();
    virtual ~Packet() = default;
};