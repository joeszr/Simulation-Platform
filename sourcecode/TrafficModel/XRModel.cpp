#include "XRModel.h"
#include "../Utility/SBID.h"
#include "../Utility/RBID.h"
#include "../ChannelModel/AntennaOrientGain.h"
#include "../ChannelModel/Rx.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../BaseStation/BTSID.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../BaseStation/BS.h"
#include "../BaseStation/BTS.h"
#include "../DetectAlgorithm/MMSE_DL.h"
#include "../Statistician/Statistician.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../Utility/SCID.h"
#include "../BaseStation/BTSRxer.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../BaseStation/ACKNAKMessageUL.h"
#include "../DetectAlgorithm/Detector_UL.h"
#include "../BaseStation/BTSTxer.h"
#include "../MobileStation/MS.h"
#include "../MobileStation/CQIMessage.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../BaseStation/BTSMUEBBTxer.h"
#include <cmath>
#include "MSProperty_XR.h"
extern int G_PacketID;
XRModel::XRModel():m_MSData(Parameters::Instance().BASIC.ITotalMSNum) {
    dPeriod_ms = 1000.0/Parameters::Instance().XR.FPS;
    dPacketSizeDefault_Kbit = Parameters::Instance().XR.dPacketSizeDefault_Kbit;
    double Rate=Parameters::Instance().XR.SumRate;
    dPave = 1000*Rate/Parameters::Instance().XR.FPS;
    dPmax = dPave*1.5;
    dPmin=  dPave*0.5;
    dS=dPave*0.105;
    for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
        double dPeriod = dPeriod_ms;
        m_MSData.insert(make_pair(msid.ToInt(), MSProperty_XR(dPeriod)));
    }
}
void XRModel::WorkSlot() {
    cout << "this is XR!" << endl;
    int iTime = Clock::Instance().GetTimeSlot();
    if(Parameters::Instance().BASIC.IDLORUL==Parameters::DL||Parameters::Instance().BASIC.IDLORUL==Parameters::ULandDL){
        
    }
    for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
        if (iTime == m_MSData[msid.ToInt()].m_jitter_packet_slot) {
            double dPeriod_slot = dPeriod_ms / Parameters::Instance().BASIC.DSlotDuration_ms;
            m_MSData[msid.ToInt()].m_iFrameNo++;
            double dFrameSize_Kbit;
            double maxms = 4;
            double minms = -4;
            double mean = 0;
            double stdms = 2;
            double temp = random.xNormal(mean, stdms);
            if (temp > maxms){
                temp = maxms;
            }else if (temp < minms){
                temp = minms;
            }
            temp /= Parameters::Instance().BASIC.DSlotDuration_ms;    
            double dFrameSize_temp = random.xNormal(dPave, dS);
            if (dFrameSize_temp > dPmax) {
                dFrameSize_temp = dPmax;
            } else if (dFrameSize_temp < dPmin) {
                dFrameSize_temp = dPmin;
            }
            dFrameSize_Kbit = dFrameSize_temp;
            Observer::Print("PacketCount") << setw(20) << "Slot" << setw(20) << iTime
                    << setw(20) << "msid" << setw(20) << msid
                    << setw(20) << "PacketSize" << setw(20) << dFrameSize_Kbit
                    << endl;
            std::shared_ptr<Packet> pPacket = std::make_shared<Packet > (G_PacketID++, dFrameSize_Kbit, iTime, 0, msid, msid.GetMS().GetMainServBTS(), m_MSData[msid.ToInt()].m_originalbornslot);
            msid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(msid, pPacket);
            msid.GetMS().SetTrafficState(true);
            m_qTotalPacketQueueDL.push_back(pPacket);
            m_MSData[msid.ToInt()].m_originalbornslot += dPeriod_slot;
            m_MSData[msid.ToInt()].m_jitter_packet_slot = ceil(temp) + m_MSData[msid.ToInt()].m_originalbornslot;          
            if (m_MSData[msid.ToInt()].m_jitter_packet_slot <= iTime) {
                cout << "pause";
            }
            Statistician::Instance().m_MSData_DL[msid.ToInt()].Packetscounts++;
        }
    }
}
void XRModel::OutputTrafficInfo() {
    Observer::Print("FTPInfoRecord") << G_PacketID << endl;
    Observer::Print("FTPInfoRecord") << setw(20) << "ID"
            << setw(20) << "MSID"
            << setw(20) << "BTSID"
            << setw(20) << "OriginSize"
            << setw(20) << "RemainderSize"
            << setw(20) << "TxSize"
            << setw(20) << "RightRxSize"
            << setw(20) << "IsFinished"
            << setw(20) << "IsCompleted"
            << setw(20) << "BornTime"
            << setw(20) << "FinishTime"
            << setw(20) << "Delay" << endl;

    auto it = m_qTotalPacketQueueDL.begin();
    while (it != m_qTotalPacketQueueDL.end()) {
        int iID = (*it)->GetID();
        MSID msid = (*it)->GetMSID();
        BTSID btsid = (*it)->GetBTSID();
        bool iIsFinished = (*it)->IsFullySuccessRecieved();
        bool iIsCompleted = (*it)->IsCompleted();
        int iBornTimeSlot = (*it)->GetBornTimeSlot();
        int iFinishTimeSlot = (*it)->GetFinishTimeSlot();
        int iDelaySlot = (*it)->GetDelayTimeSlot();
        double dOriginSizeKbits = (*it)->GetOriginSizeKbits();
        double dRemainderSizeKbits = (*it)->GetRemainUntransmitted_SizeKbits();
        double dTxSizeKbits = dOriginSizeKbits - dRemainderSizeKbits;
        double dRecievedSizeKbits = (*it)->GetSuccessRecieved_SizeKbits();
        Observer::Print("FTPInfoRecord") << setw(20) << iID
                << setw(20) << msid
                << setw(20) << btsid
                << setw(20) << dOriginSizeKbits
                << setw(20) << dRemainderSizeKbits
                << setw(20) << dTxSizeKbits
                << setw(20) << dRecievedSizeKbits
                << setw(20) << (iIsFinished ? 1 : 0)
                << setw(20) << (iIsCompleted ? 1 : 0)
                << setw(20) << iBornTimeSlot
                << setw(20) << iFinishTimeSlot
                << setw(20) << iDelaySlot << endl;
        it++;
    }
    m_qTotalPacketQueueDL.clear();
}