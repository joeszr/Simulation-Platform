#include "FTPModel.h"
#include "../Utility/IDType.h"
#include "../Utility/SBID.h"
#include "../Utility/RBID.h"
#include "../ChannelModel/AntennaOrientGain.h"
#include "../ChannelModel/Rx.h"
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
#include "../NetworkDrive/NetWorkDrive.h"
#include "../MobileStation/HARQTxStateMS.h"
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
#include "../MobileStation/MSTxer.h"
extern int G_PacketID;

FTPModel::FTPModel() {
    lamda1=Parameters::Instance().TRAFFIC.DMacroLamda;
    lamda2=Parameters::Instance().TRAFFIC.DMacroLamda_2;
    m_dPacketSize1 = Parameters::Instance().TRAFFIC.DPacketSize;
    m_dPacketSize2 = Parameters::Instance().TRAFFIC.DPacketSize_2;
}

void FTPModel::WorkSlot(){
    cout<<"this is FTP Model3!"<<endl;
    int iTime = Clock::Instance().GetTimeSlot();
    ///系统生成下行包，并随机分配给MS
    if(Parameters::Instance().BASIC.IDLORUL==Parameters::DL||Parameters::Instance().BASIC.IDLORUL==Parameters::ULandDL){
        for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
            if(msid.GetMS().GetMainServBTS().ToInt()==-1)
                continue;
            double lamda = 0;
            double m_dPacketSize = 0;
            if (msid.GetMS().gettype() == 1)
            {
                lamda = lamda1;
                m_dPacketSize = m_dPacketSize1;
            } else if (msid.GetMS().gettype() == 2)
            {
                lamda = lamda2;
                m_dPacketSize = m_dPacketSize2;
            }
            if (msid.GetMS().packettime == iTime) {
                std::shared_ptr<Packet > pPacket = std::make_shared<Packet > (G_PacketID++, m_dPacketSize, iTime, 0, msid, msid.GetMS().GetMainServBTS());
                msid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(msid, pPacket);
                m_qTotalPacketQueueDL.push_back(pPacket);
                msid.GetMS().SetTrafficState(true);
                double interval = random.xExponent(lamda / 1000 * Parameters::Instance().BASIC.DSlotDuration_ms);
                while (interval < 1) {
                    std::shared_ptr<Packet > pPacket1 = std::make_shared<Packet > (G_PacketID++, m_dPacketSize, iTime, 0, msid, msid.GetMS().GetMainServBTS());
                    msid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(msid, pPacket1);
                    m_qTotalPacketQueueDL.push_back(pPacket1);
                    interval += random.xExponent(lamda / 1000 * Parameters::Instance().BASIC.DSlotDuration_ms);
                }
                msid.GetMS().packettime += interval;
            }
        }
    }
    if(Parameters::Instance().BASIC.IDLORUL==Parameters::UL||Parameters::Instance().BASIC.IDLORUL==Parameters::ULandDL){
        for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
            double lamda = 0;
            double m_dPacketSize = 0;
            if (msid.GetMS().gettype() == 1) {
                lamda = lamda1;
                m_dPacketSize = m_dPacketSize1;
                
            } else if (msid.GetMS().gettype() == 2) {
                lamda = lamda2;
                m_dPacketSize = m_dPacketSize2;
            }
            if (msid.GetMS().UL.packettime == iTime) {
                std::shared_ptr<Packet > pPacket = std::make_shared<Packet > (G_PacketID++, m_dPacketSize, iTime, 1, msid, msid.GetMS().GetMainServBTS());
                msid.GetMS().UL.MSTxBuffer.ReceivePacketUL(pPacket);
                msid.GetMS().UL.m_pMSTxer->GetSR().GenerateSR();
                //m_qTotalPacketQueue.push_back(pPacket);
                msid.GetMS().SetTrafficState(true);
                double interval = random.xExponent(lamda / 1000 * Parameters::Instance().BASIC.DSlotDuration_ms);
                while (interval < 1) {
                    std::shared_ptr<Packet > pPacket1 = std::make_shared<Packet >(G_PacketID++, m_dPacketSize, iTime, 1, msid, msid.GetMS().GetMainServBTS());
                    msid.GetMS().UL.MSTxBuffer.ReceivePacketUL(pPacket1);
                    //m_qTotalPacketQueue.push_back(pPacket1);
                    interval += random.xExponent(lamda / 1000 * Parameters::Instance().BASIC.DSlotDuration_ms);
                }
                msid.GetMS().UL.packettime += interval;
            }
        }
    }
}

void FTPModel::OutputTrafficInfo() {

    Observer::Print("FTPInfoRecord")<<G_PacketID<<endl;
    Observer::Print("FTPInfoRecord")<<setw(20)<<"ID"
            <<setw(20)<<"MSID"
            <<setw(20)<<"BTSID"
            <<setw(20)<<"OriginSize"
            <<setw(20)<<"RemainderSize"
            <<setw(20)<<"TxSize"
            <<setw(20)<<"RightRxSize"
            <<setw(20)<<"IsFinished"
            <<setw(20)<<"IsCompleted"
            <<setw(20)<<"BornTime"
            <<setw(20)<<"FinishTime"
            <<setw(20)<<"Delay"<<endl;

    auto it = m_qTotalPacketQueueDL.begin();
    while( it!=m_qTotalPacketQueueDL.end()){
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
        Observer::Print("FTPInfoRecord")<<setw(20)<<iID
                <<setw(20)<<msid
                <<setw(20)<<btsid
                <<setw(20)<<dOriginSizeKbits
                <<setw(20)<<dRemainderSizeKbits
                <<setw(20)<<dTxSizeKbits
                <<setw(20)<<dRecievedSizeKbits
                <<setw(20)<<(iIsFinished? 1:0)
                <<setw(20)<<(iIsCompleted? 1:0)
                <<setw(20)<<iBornTimeSlot
                <<setw(20)<<iFinishTimeSlot
                <<setw(20)<<iDelaySlot<<endl;
        it++;
    }
    m_qTotalPacketQueueDL.clear();
}