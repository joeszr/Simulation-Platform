/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BTS_UL.cpp
 * Author: cyh
 * 
 * Created on 2021年1月24日, 下午8:24
 */

#include "BTS_UL.h"
#include "BTS.h"
#include "BTSRxer.h"
#include "BTSVMIMORxer_trial_2.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../MobileStation/MS.h"
#include"../Scheduler/UL_SchedulingRenewal.h"
#include "../NetworkDrive/Clock.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "../MobileStation/MSTxer.h"
#include "../Statistician/Statistician.h"
std::mutex BTS_UL_mutex;
extern int G_ICurDrop;
//BTS_UL函数定义_begin
BTS_UL::BTS_UL()
{
    RBUseRate = 0;
}
void BTS_UL::Construct(BTSID& _btsId) {
    btsId = _btsId;
    //初始化调度器
    if (Parameters::Instance().SIM_UL.UL.IMIMOMode == Parameters::IMIMOMode_VirtualMIMO) { //当前如果是V-MIMO模式
        m_SchUL = std::shared_ptr<SchedulerUL>(new UL_SchedulingRenewal(_btsId));
        m_SchUL->SetRxer(m_pRxer);
//        switch (Parameters::Instance().TRAFFIC.ITrafficModel) {
//            case Parameters::ITrafficModel_FullBuffer: //vitural MIMO配对算法
//                m_SchUL = std::shared_ptr<SchedulerUL>(new VMIMOSchedulerTrial_2(_btsId));
//                break;
//            case Parameters::ITrafficModel_FTP:
//                m_SchUL = std::shared_ptr<SchedulerUL>(new FTPVMIMOSchedulerTrial_2(_btsId));
//                break;
//            //add by zhaoyan begin
//            case Parameters::MixedTraffic:
////                m_SchUL = std::shared_ptr<SchedulerUL>(new MixedVMIMOSchedulerTrial_2(_btsId));
//                m_SchUL = std::shared_ptr<SchedulerUL>(new UL_SchedulingRenewal(_btsId));
//                m_SchUL->SetRxer(m_pRxer);
//                break;
//            //add by zhaoyan end
//            default:
//                assert(false);
//        }
//        m_SchUL->SetRxer(m_pRxer);
    }
    else
    {
        assert(false);
    }
}

void BTS_UL::Initialize() {
    SetSRConfig();
    m_qSchedulingMessageQueueUL = btsId.GetBTS().GetDci()->GetSchMesQueUL();
    iTotalRBNum=0;
    iTotalUsedRBNum=0;
    //初始化接收机
    switch (Parameters::Instance().SIM_UL.UL.IMIMOMode) {
        case Parameters::IMIMOMode_VirtualMIMO:
            //orignal codes 20120418
            m_pRxer = std::shared_ptr<BTSRxer > (new BTSVMIMORxer_trial_2(btsId));
            break;
        default:
            assert(false);
    }

    m_pRxer->Initialize(&btsId.GetBTS().m_ActiveSet, &m_HARQRxStateBTS, m_qSchedulingMessageQueueUL, &m_RxBufferBTS);
    for (auto& msid : btsId.GetBTS().m_ActiveSet) {
        MSID2RBNumSupported[msid.ToInt()] = Parameters::Instance().SIM_UL.UL.RBSupportedNumDoor + 1;
    }
    for (int i = 0; i < static_cast<int> (btsId.GetBTS().m_ActiveSet.size()); ++i) {
        MS& ms = btsId.GetBTS().m_ActiveSet[i].GetMS();
        int iSRSSpace = 2; //这个是有问题的 //平台并没有为 上行SRS建模 zhengyi
        ms.UL.ConfigTxSRS(i % iSRSSpace, iSRSSpace);
    }

    //上行根据用户数对资源进行均分,并计算每段资源的起始和结束的RBid
    int iRBSSize = static_cast<int> (btsId.GetBTS().m_ActiveSet.size());
    //in mu cases,the RB segmentation is different

    if (Parameters::Instance().SIM_UL.UL.IMIMOMode == Parameters::IMIMOMode_VirtualMIMO) {



        assert(Parameters::Instance().SIM_UL.UL.IMSnumOfMUMIMO != 0);
        if (Parameters::Instance().SIM_UL.UL.IMSnumOfMUMIMO != 1) {
            iRBSSize = ceil(static_cast<double> (btsId.GetBTS().m_ActiveSet.size()) / Parameters::Instance().SIM_UL.UL.IMSnumOfMUMIMO + 2);
            //                iRBSSize = ceil(static_cast<double> (m_ActiveSet.size()) / P.SIM.UL.IMSnumOfMUMIMO);
//            cout<<btsId.GetBTS().m_ActiveSet.size()<<endl;
//            cout<<iRBSSize<<endl;
        } else {
            iRBSSize = static_cast<int> (btsId.GetBTS().m_ActiveSet.size());
        }
        //1.3 addtional segements is used for cell edge UE or non-paired UEs
        //
        //            iRBSSize = std::min(ceil(iRBSSize * 1.3), static_cast<double> (m_ActiveSet.size()));
        iRBSSize = std::min((iRBSSize + 1), static_cast<int> (btsId.GetBTS().m_ActiveSet.size()));
    }

    //如果这个参数没有被set 0 ，就不论什么情况下都执行
    if (Parameters::Instance().SIM_UL.UL.iRBsegmentNum != 0) {
        iRBSSize = Parameters::Instance().SIM_UL.UL.iRBsegmentNum;
        assert(Parameters::Instance().SIM_UL.UL.imaxRBsegNum > 0);
        assert(Parameters::Instance().SIM_UL.UL.imaxRBsegNum <= Parameters::Instance().SIM_UL.UL.iRBsegmentNum);
    }
    iRBSSize = 1;  //一个RBSet只包含一个RB， 把RBSet删了有些麻烦，先这样设置
    vRBUsedFlag.resize(Parameters::Instance().BASIC.IRBNum,vector<bool>(Parameters::Instance().SymbolNumPerSlot,false));
    //把SR的资源设为占用
    for(int i = Parameters::Instance().BASIC.IRBNum-1; i >= Parameters::Instance().BASIC.IRBNum - SR_RBNum; i--)
    {
        vRBUsedFlag[i] = vector<bool>(Parameters::Instance().SymbolNumPerSlot,true);
    }
    vRBUsedFlagPerSlot.resize(Parameters::Instance().BASIC.ISlotNumPerSFN, vRBUsedFlag);
    if (iRBSSize != 0) {
        m_vRBSetUL.assign(iRBSSize, Parameters::Instance().BASIC.IRBNum / iRBSSize);
        for (int i = 0; i < Parameters::Instance().BASIC.IRBNum % iRBSSize; ++i) {//确定每个集合占用的RB数目
            m_vRBSetUL[i] += 1;
        }

        for (int irbsid = 0; irbsid < static_cast<int> (m_vRBSetUL.size()); ++irbsid) {
            //每个RBS的起点RBID,和终点RBID
            int iRBsum = 0;
            for (int i = 0; i < irbsid; ++i) { //这里设定的是子载波的起始位置
                iRBsum += m_vRBSetUL[i];
            }
            //当前RBS到RB的映射还是int类型，需要修改
            //            RBID FirstRBid(iRBsum);
            //            RBID LastRBid(iRBsum + m_vRBSetUL[irbsid] - 1);
            m_RBS2RBID[irbsid] = make_pair(iRBsum, iRBsum + m_vRBSetUL[irbsid] - 1);
        }
    } else {
        cout << btsId.GetBSID().ToInt() << "," << btsId.GetIndex() << endl;
        m_vRBSetUL.clear();
        m_RBS2RBID.clear();
    }

    for(auto& msid:btsId.GetBTS().GetActiveSet())
    {
        MSID2LastK2Slot[msid.ToInt()] = 1;
    }
}

void BTS_UL::WorkSlotUL() {
    //cout<<"WorkSlotUL"<<endl;
    //    m_HARQRxStateBTS.WorkSlot();
    int iTimeInSF = Clock::Instance().GetTimeSlotInSF();
    if(Parameters::Instance().BASIC.IDLORUL != Parameters::DL){
        SetMSSRSoffset();
        m_pRxer->WorkSlot();
        m_SchUL->SetRxer(m_pRxer); //方便调用接收算法,但是设计上是有问题的

        vRBUsedFlagPerSlot[iTimeInSF] = vRBUsedFlag; //这个时隙结束后，重置资源位图
    }
}

void BTS_UL::SendSchedulingMessageUL() {
    //0304chty
//    iTotalRBNum += Parameters::Instance().SIM_UL.UL.IRBNum;
    while (!m_qSchedulingMessageQueueUL->empty()) {
        std::shared_ptr<SchedulingMessageUL> pSM = m_qSchedulingMessageQueueUL->front();
        //0304chty
        vector<RBID> vRBUsed = pSM->GetRBUsed();
//        for (auto& rbid : vRBUsed) {
////            if (!vRBUsedFlag[rbid.ToInt()]) {
////                vRBUsedFlag[rbid.ToInt()]=true;
////                iTotalUsedRBNum++;
////            }
//            if(vRBUsedFlag[rbid.ToInt()][Parameters::Instance().CoresetDuration]){
//                iTotalUsedRBNum++;    //如果MU可能加多次
//            }
//        }
        MSID msid = pSM->GetMSID();
        MS& ms = msid.GetMS();
        if(pSM->GetSendNum()==0){
            btsId.GetBTS().MSID2NewPDCCH[pSM->GetMSID().ToInt()] = true;
        }
        Statistician::Instance().m_MSData_UL[msid.ToInt()].m_dRBCount += vRBUsed.size();
        Statistician::Instance().m_MSData_UL[msid.ToInt()].m_iSchCount++;
        m_RxBufferBTS[msid.ToInt()].m_qScheduleMessageQueue.push_back(pSM);
        ms.UL.ReceiveSchedulingMessage(pSM);
        m_qSchedulingMessageQueueUL->pop_front();
    }
}

void BTS_UL::Reset() {
    m_RxBufferBTS.clear();
//    m_qSchedulingMessageQueueUL->clear();
    m_HARQRxStateBTS.Reset();
    m_vRBSetUL.clear();
        //0304chty
    Observer::Print("TotalRU_UL") << "(" << (this->btsId.GetBSID().ToInt()) << "," << (this->btsId.GetIndex()) << ")" << setw(20) << iTotalRBNum << setw(20) << iTotalUsedRBNum << endl;
    iTotalRBNum=iTotalUsedRBNum=0;
}

void BTS_UL::SetMSSRSoffset() {
    vector<MSID> ActiveSet = btsId.GetBTS().m_ActiveSet;
    sort(ActiveSet.begin(), ActiveSet.end());
    for (int i = 0; i < static_cast<int> (ActiveSet.size()); ++i) {
        MS& ms = ActiveSet[i].GetMS();
        //下面设置用户的SRS offset
        if (ms.UL.GetSRSBWIndex() == -1) {
            ms.UL.ConfigSRSBWIndex(mod(i, Parameters::Instance().MSS_UL.UL.ISRSBandWidthIndex));
        }
    }
}
void BTS_UL::SetSRConfig(){
    vector<MSID> vMSID = btsId.GetBTS().GetActiveSet();
    double SRProhibitTimer = Parameters::Instance().MSS_UL.UL.SR_ProhibitPeriod;
    int SRTransMax = Parameters::Instance().MSS_UL.UL.SR_TransMax;
    int SRCounter = -1;

//    int SRPeriod = Parameters::Instance().MSS_UL.UL.sr_Periodicity_slot;
    int SRPeriod = 0;
    int SRRBNum = 0;
    if(vMSID.size() > 0)
    {
        CalcSRConfig(SRPeriod, SRRBNum);
        assert(SRRBNum > 0 && SRRBNum < 273);
    }

    SR_RBNum = SRRBNum;
    BTS_UL_mutex.lock();
    Observer::Print("SR_Record")<<G_ICurDrop
                                << setw(20) << btsId
                                << setw(20) << vMSID.size()
                                << setw(20) << RBUseRate
                                << setw(20) << SRPeriod
                                << setw(20) << SRRBNum << endl;
    BTS_UL_mutex.unlock();
    vector<int> ULslot;
    for(int i=1;i<=SRPeriod;i++){
        if(DownOrUpLink(i) == 1)
            ULslot.emplace_back(i);
    }
    int ULslotNum = ULslot.size();
    int i = 0;
    for(auto msid:vMSID){
        i %= ULslotNum;
        msid.GetMS().UL.m_pMSTxer->SetSRConfig(SRProhibitTimer, SRTransMax, SRCounter, SRPeriod, ULslot[i]-1);
        i++;
    }
}
void BTS_UL::CalcSRConfig(int& _SR_Period, int& _SR_RBNum)
{
    int N_SR_Slot_PRB = 1;
    int n_SR_PRB = 1;
    int I_SR_CodeCH = 4;
    double RBUseRateThreshold = 0.9;


    int MSNum = btsId.GetBTS().GetActiveSet().size();

    int InitSRPeriod_UL_SlotNum = GetULSlotNum(Parameters::Instance().MSS_UL.UL.sr_Periodicity_slot);
    int InitPRBNum = Parameters::Instance().MSS_UL.UL.SR_PRBNum;


    int MinSRPeriod_UL_SlotNum = GetULSlotNum(10);
    double dMSNum = MSNum;
    int MaxPRBNum = 2 * ceil(double(dMSNum/(MinSRPeriod_UL_SlotNum * N_SR_Slot_PRB  / n_SR_PRB * Parameters::Instance().MSS_UL.UL.SR_InitCyc / I_SR_CodeCH)));
    assert(MaxPRBNum > 0);

    vector<int>SR_Period;
    if(Parameters::Instance().BASIC.ISCSConfig == 0)
    {
        SR_Period = {10, 20, 40, 80};
    }
    else if(Parameters::Instance().BASIC.ISCSConfig == 1)
    {
        SR_Period = {10, 20, 40, 80, 160};
    }
    else if(Parameters::Instance().BASIC.ISCSConfig == 2)
    {
        SR_Period = {20, 40, 80, 160, 320};
    }
    if(Parameters::Instance().MSS_UL.UL.SR_PeriodSwitch == true && Parameters::Instance().MSS_UL.UL.SR_RBNumSwitch == false)
    {
        for(int i = 0; i < SR_Period.size(); i++)
        {
            int ULSlotNum = GetULSlotNum(SR_Period[i]);

            int res = ULSlotNum * N_SR_Slot_PRB * InitPRBNum / n_SR_PRB * Parameters::Instance().MSS_UL.UL.SR_InitCyc / I_SR_CodeCH;
            if(Parameters::Instance().MSS_UL.UL.SR_PUCCHFormat == 1)
            {
                res *= Parameters::Instance().MSS_UL.UL.SR_Format1_OCC;
            }
            if(MSNum <= res)
            {
                _SR_Period = SR_Period[i];
                _SR_RBNum = InitPRBNum;
                return;
            }
        }
    }
    else if(Parameters::Instance().MSS_UL.UL.SR_PeriodSwitch == false && Parameters::Instance().MSS_UL.UL.SR_RBNumSwitch == true)
    {
        for(int i = 1; i <= MaxPRBNum; i++)
        {
            int ULSlotNum = InitSRPeriod_UL_SlotNum;
            int res = ULSlotNum * N_SR_Slot_PRB * i / n_SR_PRB * Parameters::Instance().MSS_UL.UL.SR_InitCyc / I_SR_CodeCH;
            if(Parameters::Instance().MSS_UL.UL.SR_PUCCHFormat == 1)
            {
                res *= Parameters::Instance().MSS_UL.UL.SR_Format1_OCC;
            }
            if(MSNum <= res)
            {
                _SR_Period = Parameters::Instance().MSS_UL.UL.sr_Periodicity_slot;
                _SR_RBNum = i;
                return;
            }
        }
    }
    else if(Parameters::Instance().MSS_UL.UL.SR_PeriodSwitch == true && Parameters::Instance().MSS_UL.UL.SR_RBNumSwitch == true)
    {
        if(RBUseRate <= RBUseRateThreshold)
        {
            for(int i = 0; i < SR_Period.size(); i++)
            {
                int ULSlotNum = GetULSlotNum(SR_Period[i]);

                for(int j = 1; j <= MaxPRBNum; j++)
                {
                    int res = ULSlotNum * N_SR_Slot_PRB * j / n_SR_PRB * Parameters::Instance().MSS_UL.UL.SR_InitCyc / I_SR_CodeCH;
                    if(Parameters::Instance().MSS_UL.UL.SR_PUCCHFormat == 1)
                    {
                        res *= Parameters::Instance().MSS_UL.UL.SR_Format1_OCC;
                    }
                    if(MSNum <= res)
                    {
                        _SR_Period = SR_Period[i];
                        _SR_RBNum = j;
                        return;
                    }
                }
            }
        }
        else
        {
            for(int j = 1; j <= MaxPRBNum; j++)
            {
                for(int i = 0; i < SR_Period.size(); i++)
                {
                    int ULSlotNum = GetULSlotNum(SR_Period[i]);

                    int res = ULSlotNum * N_SR_Slot_PRB * j / n_SR_PRB * Parameters::Instance().MSS_UL.UL.SR_InitCyc / I_SR_CodeCH;
                    if(Parameters::Instance().MSS_UL.UL.SR_PUCCHFormat == 1)
                    {
                        res *= Parameters::Instance().MSS_UL.UL.SR_Format1_OCC;
                    }
                    if(MSNum <= res)
                    {
                        _SR_Period = SR_Period[i];
                        _SR_RBNum = j;
                        return;
                    }
                }
            }
        }
    }
    else
    {
        _SR_Period = Parameters::Instance().MSS_UL.UL.sr_Periodicity_slot;
        _SR_RBNum = InitPRBNum;
        return;
    }
}
void BTS_UL::ReceivePHR(const std::pair<MSID, int>& phr){
    BTS_UL_mutex.lock();
    MSID2PHR[phr.first.ToInt()] = phr.second;
    PhrUpdateFlag[phr.first.ToInt()] = true;
    BTS_UL_mutex.unlock();
}
void BTS_UL::ReceiveSR(const SRMessage& SRMes){
    MSID msid = SRMes.m_ID;
    BTS_UL_mutex.lock();
    if(MSID2SRflag[msid.ToInt()] == 0){
        MSID2SRflag[msid.ToInt()]++;
        //hyl 冗余输出
        Observer::Print("SRlog")<<Clock::Instance().GetTimeSlot()
                                <<setw(20)<<msid.GetMS().GetMainServBTS().GetBTS().GetTxID()
                                <<setw(20)<<msid.ToInt()
                                <<setw(20)<<"join"<<endl;
    }
    BTS_UL_mutex.unlock();
}

void BTS_UL::SetPRBRate(double rate)
{
    RBUseRate = rate;
}
