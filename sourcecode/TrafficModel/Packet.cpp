#include "../Utility/Include.h"
#include "../NetworkDrive/Clock.h"
#include "../Utility/IDType.h"
#include "../Utility/SBID.h"
#include "../Utility/RBID.h"
#include "../ChannelModel/AntennaOrientGain.h"
#include "../ChannelModel/Rx.h"
#include "../ChannelModel/WrapAround.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../MobileStation/MSID.h"
#include "../BaseStation/BTSID.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../BaseStation/BS.h"
#include "../BaseStation/BTS.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../DetectAlgorithm/MMSE_DL.h"
#include "../Statistician/Statistician.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../Utility/SCID.h"
#include "../BaseStation/BTSRxer.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../BaseStation/ACKNAKMessageUL.h"
#include "../BaseStation/CodeBook.h"
#include "../BaseStation/BTSTxer.h"
#include "../MobileStation/MS.h"
#include "../MobileStation/CQIMessage.h"

#include "Packet.h"
std::mutex FTPPacketlock;
Packet::Packet(int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL,int originalbornslot) {
    m_OriginSize = _dPackeSizeKbits;
    m_ID = _ID;
    m_iBornTimeSlot = _iBornTimeSlot;
    
    m_iDLorUL = _iDLorUL;
    m_bFullySuccessRecieved = false;
    m_bIsCompleted = false;

    m_bDrop_DueToTimeout = false;

    m_iFinishTimeSlot = -1;
    m_dRemainUntransmittedSizeKbits = _dPackeSizeKbits;
    m_dSuccessReceivedSizeKbits = 0;
    m_originalbornslot=originalbornslot;

}

Packet::Packet(int _ID, double _dPackeSizeKbits, int _iBornTimeSlot, int _iDLorUL, const MSID& _msid, const BTSID& _btsid,int originalbornslot) {
    m_OriginSize = _dPackeSizeKbits;
    m_ID = _ID;
    m_iBornTimeSlot = _iBornTimeSlot;
    m_iDLorUL = _iDLorUL;
    m_bFullySuccessRecieved = false;
    m_bIsCompleted = false;
    m_iFinishTimeSlot = -1;

    m_bDrop_DueToTimeout = false;

    m_dRemainUntransmittedSizeKbits = _dPackeSizeKbits;
    m_dSuccessReceivedSizeKbits = 0;
    m_msid = _msid;
    m_btsid = _btsid;
    m_originalbornslot=originalbornslot;

}

int Packet::GetBornTimeSlot() const{
    return m_iBornTimeSlot;
}

int Packet::GetFinishTimeSlot() const{
    return m_iFinishTimeSlot;
}

int Packet::GetDelayTimeSlot() const{
//    if (IsFullySuccessRecieved() && m_iFinishTimeSlot != -1) {
    if(m_iFinishTimeSlot != -1){
        return (m_iFinishTimeSlot - m_iBornTimeSlot + 1);
    } else {
        return -1;
    }
}

bool Packet::IsDLPacket() const{
    return (m_iDLorUL == 0);
}

void Packet::ReduceUntransmittedSizeKbits(double _dSizeKbits) {
    if (m_dRemainUntransmittedSizeKbits < _dSizeKbits) {
        cout << "Packet::DecreasePacketSize() wrong!" << endl;
        assert(false);
    }
    m_dRemainUntransmittedSizeKbits -= _dSizeKbits;
    if (m_dRemainUntransmittedSizeKbits < 0.001) {
        m_dRemainUntransmittedSizeKbits = 0;
    }
    //    m_dTransmitingSizeKbits = _dSizeKbits;
}

bool Packet::IsFullySuccessRecieved() const{
    return m_bFullySuccessRecieved;
}

bool Packet::IsCompleted() const{
    return m_bIsCompleted;
}

void Packet::SetCompleted() {
    m_bIsCompleted = true;
}

void Packet::RecordSuccessRecievedSizeKbits(double _dKbits, int _PDSCHReceived_slot) {
    m_dSuccessReceivedSizeKbits += _dKbits;
    if (m_OriginSize - m_dSuccessReceivedSizeKbits < 0.001) {
        m_bFullySuccessRecieved = true;
        m_bIsCompleted = true;

        if (_PDSCHReceived_slot == -1)
            m_iFinishTimeSlot = Clock::Instance().GetTimeSlot();
        else
            m_iFinishTimeSlot = _PDSCHReceived_slot;
    }
}

void Packet::DropPacket_DueToTimeout() {
//    RecordSuccessRecievedSizeKbits(m_OriginSize - m_dSuccessReceivedSizeKbits);
    m_iFinishTimeSlot = - 1;
    SetDrop_DueToTimeout();
}

void Packet::RecordLostSizeKbits(double _dKbits) {
    m_dRemainUntransmittedSizeKbits += _dKbits;
}

double Packet::GetRemainUntransmitted_SizeKbits() const{
    if (IsFullySuccessRecieved())
        return 0;
    else {
        //        assert(m_dRemainderSizeKbits != 0);
        return m_dRemainUntransmittedSizeKbits;
    }
}

double Packet::GetSuccessRecieved_SizeKbits() const{
    return m_dSuccessReceivedSizeKbits;
}

double Packet::GetOriginSizeKbits() const{
    return m_OriginSize;
}

int Packet::GetID() const{
    return m_ID;
}

MSID Packet::GetMSID() const{
    return m_msid;
}

BTSID Packet::GetBTSID() const{
    return m_btsid;
}

bool Packet::IsDrop_DueToTimeout() const{
    return m_bDrop_DueToTimeout;
}

void Packet::SetDrop_DueToTimeout() {
    m_bDrop_DueToTimeout = true;
}

bool Packet::IsTimeout(int _iTime) const{
    return _iTime >= (m_iBornTimeSlot + Parameters::Instance().XR.iDelayBudget_slot);
}

void Packet::LogFinishedPacket() {
//    assert(IsFullySuccessRecieved() == true);
    std::lock_guard<std::mutex>l(FTPPacketlock);
    if (m_bDrop_DueToTimeout) {
        Statistician::Instance().m_MSData_DL[GetMSID().ToInt()].LostPackets++;
    } else {
        Statistician::Instance().m_MSData_DL[GetMSID().ToInt()].SuccessPackets++;
    }
    
    double dTxSizeKbits = GetOriginSizeKbits() - GetRemainUntransmitted_SizeKbits();
    int iTime = Clock::Instance().GetTimeSlot();

    //UPT（用户包吞吐量），利用单个数据包的数据包大小除以该数据包的传输时间（单位：Mbps）
    double UPT;
    double Scaling;
     //带宽折算
    if ( Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 0){
        Scaling = 5;
    }else{
        Scaling = 2;
    }
    UPT= GetOriginSizeKbits() / GetDelayTimeSlot() * Scaling / Parameters::Instance().BASIC.DSlotDuration_ms;

//hyl 冗余
//    Observer::Print("FTPPacket")
//            << setw(20) << GetID()
//            << setw(20) << iTime
//            << setw(20) << GetMSID()
//            << setw(20) << GetBTSID()
//            << setw(20) << GetOriginSizeKbits()
//            << setw(20) << GetRemainUntransmitted_SizeKbits()
//            << setw(20) << dTxSizeKbits
//            << setw(20) << GetSuccessRecieved_SizeKbits()
//            << setw(20) << (IsFullySuccessRecieved() ? 1 : 0)
//            << setw(20) << (IsCompleted() ? 1 : 0)
//            << setw(20) << (IsDrop_DueToTimeout() ? 1 : 0)
//            << setw(20) << GetBornTimeSlot()
//            << setw(20) << GetFinishTimeSlot()
//            << setw(20) << (GetDelayTimeSlot() * Parameters::Instance().BASIC.DSlotDuration_ms )
////            << setw(20) << ((GetDelayTimeSlot()==-1)?"Droped":std::to_string(GetDelayTimeSlot()*Parameters::Instance().BASIC.DSlotDuration_ms))
//            << setw(20) << m_originalbornslot
//            << setw(20) << UPT
//            << endl;

	//RedCap FTPPacket
	if (GetMSID() < floor(Parameters::Instance().BASIC.ITotalMSNum * Parameters::Instance().BASIC.type1MSRate + 0.5))
	{
        //hyl 冗余
//		Observer::Print("FTPPacket_eMBB") << GetID()
//			<< setw(20) << iTime
//			<< setw(20) << GetMSID()
//			<< setw(20) << GetBTSID()
//			<< setw(20) << GetOriginSizeKbits()
//			<< setw(20) << GetRemainUntransmitted_SizeKbits()
//			<< setw(20) << dTxSizeKbits
//			<< setw(20) << GetSuccessRecieved_SizeKbits()
//			<< setw(20) << (IsFullySuccessRecieved() ? 1 : 0)
//			<< setw(20) << (IsCompleted() ? 1 : 0)
//			<< setw(20) << (IsDrop_DueToTimeout() ? 1 : 0)
//			<< setw(20) << GetBornTimeSlot()
//			<< setw(20) << GetFinishTimeSlot()
//			<< setw(20) << GetDelayTimeSlot()*Parameters::Instance().BASIC.DSlotDuration_ms
//			<< setw(20) << m_originalbornslot
//			<< setw(20) << UPT
//			<< endl;
	}
	else {
        //hyl 冗余
//		Observer::Print("FTPPacket_RedCap") << GetID()
//			<< setw(20) << iTime
//			<< setw(20) << GetMSID()
//			<< setw(20) << GetBTSID()
//			<< setw(20) << GetOriginSizeKbits()
//			<< setw(20) << GetRemainUntransmitted_SizeKbits()
//			<< setw(20) << dTxSizeKbits
//			<< setw(20) << GetSuccessRecieved_SizeKbits()
//			<< setw(20) << (IsFullySuccessRecieved() ? 1 : 0)
//			<< setw(20) << (IsCompleted() ? 1 : 0)
//			<< setw(20) << (IsDrop_DueToTimeout() ? 1 : 0)
//			<< setw(20) << GetBornTimeSlot()
//			<< setw(20) << GetFinishTimeSlot()
//			<< setw(20) << GetDelayTimeSlot()*Parameters::Instance().BASIC.DSlotDuration_ms
//			<< setw(20) << m_originalbornslot
//			<< setw(20) << UPT
//			<< endl;
	}

}
void Packet::LogFinishedPacketXR_multi_stream(bool ifI){
    if(m_bDrop_DueToTimeout){
        Statistician::Instance().m_MSData_DL[GetMSID().ToInt()].LostPackets++;
    }
    else{
        Statistician::Instance().m_MSData_DL[GetMSID().ToInt()].SuccessPackets++;
    }
    double dTxSizeKbits = GetOriginSizeKbits() - GetRemainUntransmitted_SizeKbits();
    int iTime = Clock::Instance().GetTimeSlot();
    //hyl 冗余
//    Observer::Print("XR_multi_stream_Packet") << setw(20) << GetID()
//            << setw(20) << iTime
//            << setw(20) << GetMSID()
//            << setw(20) << GetBTSID()
//            << setw(20) << GetOriginSizeKbits()
//            << setw(20) << GetRemainUntransmitted_SizeKbits()
//            << setw(20) << dTxSizeKbits
//            << setw(20) << GetSuccessRecieved_SizeKbits()
//            << setw(20) << (IsFullySuccessRecieved() ? 1 : 0)
//            << setw(20) << (IsCompleted() ? 1 : 0)
//            << setw(20) << (IsDrop_DueToTimeout() ? 1 : 0)
//            << setw(20) << GetBornTimeSlot()
//            << setw(20) << GetFinishTimeSlot()
//            << setw(20) << GetDelayTimeSlot() *Parameters::Instance().BASIC.DSlotDuration_ms
//            << setw(20) << m_originalbornslot
//            << setw(20)<<ifI
//            << endl;
}
