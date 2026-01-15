#include "FTPModel2.h"
#include "../Utility/SBID.h"
#include "../Utility/RBID.h"
#include "../ChannelModel/AntennaOrientGain.h"
#include "../ChannelModel/Rx.h"
#include "../ChannelModel/WrapAround.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../BaseStation/BTSID.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/BS.h"
#include "../BaseStation/BTS.h"
#include "../NetworkDrive/BSManager.h"
#include "../DetectAlgorithm/MMSE_DL.h"
#include "../NetworkDrive/NetWorkDrive.h"
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
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../BaseStation/BTSMUEBBTxer.h"
extern int G_PacketID;


FTPModel2::FTPModel2(double _dPacketSize, double _dMacroLamda)
:mMSID2ReadTimeMsec(Parameters::Instance().BASIC.ITotalMSNum),
 mMSID2PacketQueueDL(Parameters::Instance().BASIC.ITotalMSNum){
    m_dMacroLamda = _dMacroLamda;
    m_dPacketSize = _dPacketSize;
}

void FTPModel2::WorkSlot(){
    cout<<"this is FTP model2!"<<endl;
    int iTime = Clock::Instance().GetTimeSlot();
    ///系统生成下行包，并随机分配给MS
    double dLamda;
    for(BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        BTS& bts = BSManager::Instance().GetBTS(btsid);
        if(BSManager::IsMacro(btsid)){
            dLamda = m_dMacroLamda;
        }else{
            continue;
        }
        
        vector<MSID> vMSID = bts.GetActiveSet();
        vector<PicoID> vPicoID = BSManager::GetPicoIDsofMacro(btsid);
        for(const auto& PicoID : vPicoID){
            vector<MSID> vPicoUE = PicoID.GetBTS().GetActiveSet();
            for(const auto& msid : vPicoUE){
                vMSID.emplace_back(msid);
            }
        }

        //检查每个UE在BTS缓存里面的包队列，如果队列为空(空只能说明调度了，但是不宜定正确接收到了)，就为该UE产生新的包
        int iMSNum = static_cast<int>(vMSID.size());
        for(int i=0;i<iMSNum;++i){
            MSID msid = vMSID[i];
            if(iTime == 1){
                ///产生一个ReadTime
                int iReadTime = (int)(random.xExponent(dLamda) * 1000);
                mMSID2ReadTimeMsec[msid.ToInt()] = iTime + iReadTime;
            }
            if(!mMSID2PacketQueueDL[msid.ToInt()].empty()){
                assert(static_cast<int>(mMSID2PacketQueueDL[msid.ToInt()].size()) == 1);
                if( (bts.GetMSTxBufferBTS()[msid.ToInt()].GetDLPacketNum() == 0) && (mMSID2PacketQueueDL[msid.ToInt()][0]->IsCompleted())){
                    ///产生一个ReadTime
                    mMSID2PacketQueueDL[msid.ToInt()].pop_front();
                    int iReadTime = (int)(random.xExponent(dLamda) * 1000);
                    mMSID2ReadTimeMsec[msid.ToInt()] = iTime + iReadTime;
                }
            }
            

            if(mMSID2ReadTimeMsec[msid.ToInt()] == iTime){
                ///ReadTime到期后产生一个包
                std::shared_ptr<Packet> pPacket = std::make_shared<Packet > (G_PacketID++,m_dPacketSize,iTime,0,msid,msid.GetMS().GetMainServBTS());
                msid.GetMS().GetMainServBTS().GetBTS().ReceivePacketDL(msid,pPacket);
                mMSID2PacketQueueDL[msid.ToInt()].push_back(pPacket);
                m_qTotalPacketQueueDL.push_back(pPacket);
            }

        }
    }
}

void FTPModel2::OutputTrafficInfo() {
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
}