///@file MSTXer.cpp
///@brief  MS发送模块类实现
///@author zhengyi,wangsen

#include "../Utility/IDType.h"
#include "../Utility/SCID.h"
#include "../Utility/RBID.h"
#include "../Utility/SBID.h"
#include "MSID.h"
#include "../BaseStation/BSID.h"
#include "../BaseStation/BTSID.h"
#include "../NetworkDrive/Clock.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../BaseStation/ACKNAKMessageUL.h"
#include "../Statistician/Statistician.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "HARQTxStateMS.h"
#include "MSTxer.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "../BaseStation/BTS.h"
#include "MS.h"
#include "../DCI.h"


MSTxer::MSTxer(const BTSID& _MainServBTSID, const MSID& _MSID, std::shared_ptr<HARQTxStateMS> _pHARQTxStateMS) {
    m_MainServBTSID = _MainServBTSID;
    m_MSID = _MSID;
    m_pHARQTxState_MS_UL = _pHARQTxStateMS;
    m_iSRSBandWidthIndex = -1; //初始化用户占用带宽指示
    m_pSRSBWrbid = make_pair(-1, -1); //初始化

    //20220828 cwq
    SumOfTPCdB = 0;
    AbsTPCdB = 0;
    LastTimeSumOfTPCdB = 0;
    LastTimeTxPowermW = -1;
    phr.initialize(_MSID);
    sr.initialize(_MSID);
}

void MSTxer::PushSchedulingMessage(std::shared_ptr<SchedulingMessageUL> _pSchedulingM) {
    m_qSchedulingMessageQueue.push_back(_pSchedulingM);
}

std::shared_ptr<SchedulingMessageUL> MSTxer::ReceiveSchedulingMessage() {
    std::shared_ptr<SchedulingMessageUL> pSch;
    pSch.reset();
//    while ((!m_qSchedulingMessageQueue.empty()) && (Clock::Instance().GetTimeSlot() - m_qSchedulingMessageQueue.front()->GetBornTime() >= 5)) {
//        pSch = m_qSchedulingMessageQueue.front();
//        m_qSchedulingMessageQueue.pop_front();
//    }
    if(!m_qSchedulingMessageQueue.empty())
    {
        pSch = m_qSchedulingMessageQueue.front();
        if(pSch->m_iK2Slot + pSch->GetBornTime() == Clock::Instance().GetTimeSlot())
        {
            m_qSchedulingMessageQueue.pop_front();
        }
        else
        {
            pSch = nullptr;
        }
    }
    return pSch;

//    if(!m_qSchedulingMessageQueue.empty())
//    {
//        pSch = m_qSchedulingMessageQueue.front();
//        m_qSchedulingMessageQueue.pop_front();
//    }
//
//    return pSch;
}

void MSTxer::ConfigTxSRS(const int& _SRSTxCyclicSCID, const int& _iSRSTxSpace) {
    m_iSRSTxCyclicSCID = _SRSTxCyclicSCID;
    m_iSRSTxSpace = _iSRSTxSpace;
}

//设置SRS的offset，用于在非全带宽发送时，确定用户的发送带宽

void MSTxer::ConfigSRSBWIndex(int _iSRSIndex) {
    m_iSRSBandWidthIndex = _iSRSIndex;
}

int MSTxer::GetSRSBWIndex() {
    return m_iSRSBandWidthIndex;
}

void MSTxer::SetSRSBWspan(int _ifirst, int _isecond) {
    m_pSRSBWrbid = make_pair(_ifirst, _isecond);
}

pair<int, int> MSTxer::GetSRSBWspan() {
    return m_pSRSBWrbid;
}

int MSTxer::GetSRSTxCyclicSCID(void) {
    return m_iSRSTxCyclicSCID;
}

void MSTxer::ReceiveACK(std::shared_ptr<ACKNAKMessageUL> _pACKNAKMessageUL) {
    bool bACKNAK = _pACKNAKMessageUL->GetACKNAK();
    std::shared_ptr<SchedulingMessageUL> pSchM = _pACKNAKMessageUL->GetSchMessage();
    MSID msid = pSchM->GetMSID();
    imat iMCS = pSchM->GetMCSmat();
    int iHARQID = pSchM->GetHARQID();
    //    vector<RBID> vRBID = pSchM->GetRBUsed();
    //    int iUsedRBNum = static_cast<int> (vRBID.size());
    
    if (bACKNAK) {
        m_MainServBTSID.GetBTS().m_MSID2HARQStateList_UL[msid.ToInt()][iHARQID] = false;

//        int iUsedRBNum=pSchM->GetRBUsed().size();
//        int iUsedRENum = LinkLevelInterface::Instance_UL().GetRENum(iUsedRBNum);
//        double dCorrectKbit = LinkLevelInterface::Instance_UL().MCS2TBSKBit(iMCS, iUsedRENum);
        double dCorrectKbit = pSchM->GetTBSizeKbit();
        if(Clock::Instance().GetTimeSlot() >= Parameters::Instance().BASIC.IWarmUpSlot)
        {
            Statistician::Instance().m_MSData_UL[msid.ToInt()].UL.m_dAveRateKbpsUL += dCorrectKbit;
            Statistician::Instance().m_MSData_UL[msid.ToInt()].UL.m_iFormat2TrueFalseFrameHitUL[0][iMCS(0, 0)]++;
            if (iMCS.rows() == 2)
                Statistician::Instance().m_MSData_UL[msid.ToInt()].UL.m_iFormat2TrueFalseFrameHitUL[0][iMCS(1, 0)]++;
        }
        //add by zhaoyan begin
        if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::MixedTraffic)//使用混合业务时要通过用户的MAC_MS修改数据包队列
        {
//            m_MSID.GetMS().m_clsMacMs.ReduceBuffer(dCorrectKbit);
        }
        else if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP)//使用混合业务时要通过用户的MSTxBuffer修改数据包队列
        {
            m_MSID.GetMS().UL.MSTxBuffer.PopFinishedPacket();
        }
        //add by zhaoyan end
    }
    else
    {
        if(Clock::Instance().GetTimeSlot() >= Parameters::Instance().BASIC.IWarmUpSlot)
        {
            Statistician::Instance().m_MSData_UL[msid.ToInt()].UL.m_iFormat2TrueFalseFrameHitUL[1][iMCS(0, 0)]++;
            if (iMCS.rows() == 2)
                Statistician::Instance().m_MSData_UL[msid.ToInt()].UL.m_iFormat2TrueFalseFrameHitUL[1][iMCS(1, 0)]++;
        }

        if (pSchM->GetSendNum() < Parameters::Instance().SIM_UL.UL.IHARQMaxTransNum)
        {
//            m_pHARQTxState_MS_UL->RegistReTxMessage(_pACKNAKMessageUL);
        }
        else
        {
            m_MainServBTSID.GetBTS().m_MSID2HARQStateList_UL[msid.ToInt()][iHARQID] = false;
        }
    }
    
        

    //如果传错了，则保存消息，用于下次重传，如果传对了，则删除消息
    //    if ((!bACKNAK) && pSchM->GetSendNum() < Parameters::Instance().SIM_UL.UL.IHARQMaxTransNum)
    //        m_pHARQTxState_MS_UL->RegistReTxMessage(_pACKNAKMessageUL);
    //    else if (pSchM->GetSendNum() == Parameters::Instance().SIM_UL.UL.IHARQMaxTransNum || bACKNAK) {
    //        m_pHARQTxState_MS_UL->Reset();
    //    } else {
    //        assert(false);
    //        m_pHARQTxState_MS_UL->Reset(); //zhengyi
    //    }
}

SR& MSTxer::GetSR() {
    return sr;
}
PHR& MSTxer::GetPHR(){
    return phr;
}
//20220828 cwq
void MSTxer::ReceiveTPCCommand(TPCMessage TPC){
    TPCQue.push_back(TPC);
}
double MSTxer::Generate_f(){

    if(TPCQue.empty()){
        return 0;
    }

    TPCMessage TPC;
    while(!TPCQue.empty()){
        TPC = TPCQue.front();
        TPCQue.pop_front();
        Accumlation_f_Process(TPC.tpc);
        Abs_f_Process(TPC.tpc);
    }

    double f;
    if(TPC.IsAccumulation){
        if(LastTimeTxPowermW > 0){
            if(LastTimeTxPowermW >= DB2L(Parameters::Instance().MSS_UL.UL.DMaxTxPowerDbm) && SumOfTPCdB >= 0){
                f = LastTimeSumOfTPCdB;
            }
            else if(LastTimeTxPowermW <= DB2L(Parameters::Instance().MSS_UL.UL.DMinTxPowerDbm) && SumOfTPCdB <= 0){
                f = LastTimeSumOfTPCdB;
            }
            else{
                f = LastTimeSumOfTPCdB + SumOfTPCdB;
            }
        }
        SumOfTPCdB = 0;
        LastTimeSumOfTPCdB = f;
    }
    else{
        f = AbsTPCdB;
    }
    return f;

}
void MSTxer::Accumlation_f_Process(int tpc){
    double table[]={-1, 0, 1, 3};
    SumOfTPCdB += table[tpc];
}
void MSTxer::Abs_f_Process(int tpc){
    double table[]={-4, -1, 1, 4};
    AbsTPCdB = table[tpc];
}

void MSTxer::ResetTPC(){
    LastTimeSumOfTPCdB = 0;
    SumOfTPCdB = 0;
    TPCQue.clear();
}
void MSTxer::SetSRConfig(double _ProhibitTimer, int _TransMax, int _Counter, int Period, int offset){
    sr.SetConfig(_ProhibitTimer, _TransMax, _Counter, Period, offset);
}

//增加TXer的初始化

//void MSTxer::Initialize(HARQTxStateMS* _pHARQTxStateMS) { //zhengyi
//    m_pHARQTxState_MS_UL = _pHARQTxStateMS;
//}