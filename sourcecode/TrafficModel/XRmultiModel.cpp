/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "../Utility/SBID.h"
#include "../Utility/RBID.h"
#include "../ChannelModel/AntennaOrientGain.h"
#include "../ChannelModel/Rx.h"
#include "../ChannelModel/WrapAround.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../BaseStation/BTSID.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../BaseStation/BS.h"
#include "../BaseStation/BTS.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../Utility/SCID.h"
#include "../BaseStation/BTSRxer.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../BaseStation/ACKNAKMessageUL.h"
#include "../BaseStation/BTSTxer.h"
#include "../MobileStation/MS.h"
#include "../MobileStation/CQIMessage.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../BaseStation/BTSMUEBBTxer.h"
#include <cmath>
#include "./XRmultiModel.h"
#include"./Packet_xrMULTI.h"
extern int G_PacketID;

XRmultiModel::XRmultiModel(): m_MSData(Parameters::Instance().BASIC.DNumMSPerBTS) {
    fps = Parameters::Instance().XR.FPS;
    K=8;assert(K>0);
    alpha=Parameters::Instance().XR.alpha;
    SumRate=Parameters::Instance().XR.SumRate;
    meanI=1000*SumRate*alpha/(K-1+alpha) *K/fps;
    meanP=1000*SumRate*(K-1)/(K-1+alpha) *K/((K-1)*fps);
    for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
        double dPeriod = 1000/fps;
        m_MSData.insert(make_pair(msid.ToInt(), MSProperty_XRmulti(dPeriod,K)));
    }
}

void XRmultiModel::WorkSlot(){
     cout << "this is XRmulti!" << endl;
    int iTime = Clock::Instance().GetTimeSlot();
    for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
        if (iTime == m_MSData[msid.ToInt()].m_jitter_packet_slot)
        { 
            m_MSData[msid.ToInt()].m_iFrameNo++;
            double maxms=4;double minms=-4;double mean=0;double stdms=2;        
            double temp=random.xNormal(mean,stdms);
            if(temp>maxms)
            { temp=maxms;}
            else if(temp<minms){
                temp=minms;
            }
            temp/=Parameters::Instance().BASIC.DSlotDuration_ms;

            int order=m_MSData[msid.ToInt()].get_and_renew_order();
            double dFrameSize=0;
            if(order==0){
                dFrameSize = random.xNormal(meanI, 0.105*meanI);
                if(dFrameSize>1.5*meanI)
                { dFrameSize=1.5*meanI;}
                else if(dFrameSize<0.5*meanI)
                { dFrameSize=0.5*meanI;}
             std::shared_ptr<Packet_xrMULTI> pPacket = std::make_shared<Packet_xrMULTI> (G_PacketID++, dFrameSize, iTime, 0, msid, msid.GetMS().GetMainServBTS(),m_MSData[msid.ToInt()].m_originalbornslot,true);
            msid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(msid, pPacket);
            msid.GetMS().SetTrafficState(true);
             m_qTotalPacketQueueDL.push_back(pPacket);
            }
            else{
                dFrameSize = random.xNormal(meanP, 0.105*meanP);
                if(dFrameSize>1.5*meanP)
                { dFrameSize=1.5*meanP;}
                else if(dFrameSize<0.5*meanP)
                { dFrameSize=0.5*meanP;}
              std::shared_ptr<Packet_xrMULTI>   pPacket= std::make_shared<Packet_xrMULTI > (G_PacketID++, dFrameSize, iTime, 0, msid, msid.GetMS().GetMainServBTS(),m_MSData[msid.ToInt()].m_originalbornslot,false);
            msid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(msid, pPacket);
            msid.GetMS().SetTrafficState(true);
            m_qTotalPacketQueueDL.push_back(pPacket);

            }            
            Observer::Print("PacketCount") << setw(20) << "Slot" << setw(20) << iTime
                    << setw(20) << "msid" << setw(20) << msid
                    << setw(20) << "PacketSize" << setw(20) << dFrameSize<<setw(20)<<((order==0)?1:0)
                    << endl;
            
            m_MSData[msid.ToInt()].m_originalbornslot+=1000/fps/Parameters::Instance().BASIC.DSlotDuration_ms;
            m_MSData[msid.ToInt()].m_jitter_packet_slot=ceil(temp)+m_MSData[msid.ToInt()].m_originalbornslot;         
            
        }
    }
}
void XRmultiModel::OutputTrafficInfo(){
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