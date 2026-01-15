//
// Created by LAI on 2023/2/16.
//
#include "../Utility/IDType.h"
#include "../Utility/SBID.h"
#include "../Utility/RBID.h"
#include "../Utility/SCID.h"
#include "../MobileStation/MSID.h"
#include "../BaseStation/BTSID.h"
#include "../BaseStation/CodeBook.h"
#include "../NetworkDrive/Clock.h"
#include "../MobileStation/CQIMessage.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/MSRxBufferBS.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../Scheduler/SchedulingMessageDL.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../BaseStation/ACKNAKMessageUL.h"
#include "../DetectAlgorithm/Detector_UL.h"
#include "../MobileStation/MS.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../BaseStation/BS.h"
#include "../BaseStation/BTSRxer.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../NetworkDrive/BSManager.h"
#include "SchedulerUL.h"
#include "../BaseStation/BTS.h"
#include "../Scheduler/UL_SchedulingRenewal.h"
#include "../MobileStation/MAC/MAC_MS.h"
#include "../Statistician/Statistics.h"
#include "../Statistician/Statistician.h"

std::mutex LogInfoLock;
std::mutex LogBSRInfoLock;
std::mutex LogInfoCompetor;

UL_SchedulingRenewal::UL_SchedulingRenewal(const BTSID& _MainServBTSID) : SchedulerUL(_MainServBTSID), m_PdcchAllocator(_MainServBTSID) {
    //初始化MU配对时的PL门限、MU最大配对用户数、LCG的数量
    PLWindowForPairing = 5;
    m_iMUNum = Parameters::Instance().SIM_UL.UL.IMSnumOfMUMIMO;
    LCGNum = 4;
    mDebug = false;
    auto& MSSet = m_MainServBTSID.GetBTS().GetActiveSet();
}

void UL_SchedulingRenewal::Scheduling(vector<MSID>& _vActiveSet,
                                HARQRxStateBTS& _HARQRxState_BTS_UL,
                                std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer,
                                deque<std::shared_ptr<SchedulingMessageUL> >& _qSchM,
                                      vector<vector<vector<bool>>>& _vRBUsedFlagUL, vector<vector<bool>>& _vRBUsedFlagDL) {

    // 若没有服务的用户，直接返回
    if (_vActiveSet.empty())
    {
        return;
    }
    Reset();

    int iTime = Clock::Instance().GetTimeSlot();

    if(iTime <= Parameters::Instance().MSS_UL.UL.ISRS_PERIOD)
    {
        if(!m_MSScheduleListRR.empty())
        {
            m_MSScheduleListRR.clear();
        }
        return;
    }
    vector<MSID> _vActiveSetUpdate;  //更新用户激活集

    if(m_MSScheduleListRR.empty())
    {
        _vActiveSetUpdate = _vActiveSet; //存储参与调度竞争的移动台ID
    }
    else
    {
        while(!m_MSScheduleListRR.empty())
        {
            _vActiveSetUpdate.push_back(m_MSScheduleListRR.front());
            m_MSScheduleListRR.pop_front();
        }
    }
    vector<MSID> vMSSetCopy = _vActiveSetUpdate;

    BTS& MainBTS = m_MainServBTSID.GetBTS();
    for(auto msid:_vActiveSet)
    {
        for (auto i = 0; i < LCGNum; i++) {
            MSID2LCGBuffer[msid.ToInt()][i] = msid.GetMS().m_clsMacMs.GetBuffer(i);
        }
    }
//    MSID2LCGBuffer = MainBTS.m_clsMac.GetLGCbuffer();
    LCG2MSID_HistoryThroughput = MainBTS.m_clsMac.GetHistoryThroughput();

    //小区资源位图
    vRBUsedFlag = _vRBUsedFlagUL;
    vector<vector<bool>>vRBUsedFlagDL = _vRBUsedFlagDL;


    iRBNum = Parameters::Instance().BASIC.IRBNum;

    ///小区级指标新增
    int BTS2TxID = m_MainServBTSID.GetBTS().GetTxID();
    Statistics::Instance().m_BS_UL_PRB_Rate_Total[BTS2TxID] += iRBNum;


    //记录BSR信息
    LogBSRInfo(iTime);

    //去除DRX非激活用户
    DeleteUnAvaliableMS_DRX(_vActiveSetUpdate);

    //SR授权
    UpdateSRFlag(_vActiveSetUpdate, vRBUsedFlagDL);

    //根据SRSSINR计算全带宽上的MCS
    CalculateMSID2EstimatedSINR(_vActiveSetUpdate, _mMSRxBuffer);

    //根据正比例公平算法，计算不同用户，不同LCG上的优先级
    CalculatePriority(_vActiveSetUpdate, _mMSRxBuffer);

    //重传
    ScheduleReTranList(_vActiveSetUpdate, _HARQRxState_BTS_UL, vReMSID, vReRBID, vRBUsedFlag, vRBUsedFlagDL, _qSchM);

    //分配HARQID，通过mMSID2AvaiHARQID记录,erase分配不到HARQID的用户
    AvaiHARQIDState(_vActiveSetUpdate);
    //更新QoS新传队列
    UpdateQoS(_vActiveSetUpdate, vReMSID);

    //MUPairList，通过竞争体输出MU配对队列
    MakeMUPair(_vActiveSetUpdate, _mMSRxBuffer);

    //到这个位置,vCompetors相当于更新后的QoS队列
    if (_vActiveSetUpdate.empty())
    {
        return;
    }

    //计算RBLimit,在分配RB的时候需要使用到(MU的RBLimit需要着重考虑）
    for (int i = 0; i < static_cast<int> (_vActiveSetUpdate.size()); i++) {
        int RBLimit = 0;
        if (Parameters::Instance().MSS_UL.UL.IsPhrOn) {
            RBLimit = m_MainServBTSID.GetBTS().UL.MSID2RBNumSupported[_vActiveSetUpdate[i].ToInt()];
        } else {
            RBLimit = _vActiveSetUpdate[i].GetMS().UL.GetRBNumSupport();
        }
        MSID2RBLimit[_vActiveSetUpdate[i].ToInt()] = RBLimit;
//        MSID2RBLimit[_vActiveSetUpdate[i].ToInt()] = 10000;
    }

    //TBSize是LCG的BSR-Buffer大小；MCS计算全带宽的MCS；
    //[min(RBNend, RBLimited)]
    CalculatePUSCHRBNum(_vActiveSetUpdate, _mMSRxBuffer);


    //分配PDCCH CCE位置(原来的说法)
    //给竞争体中的每个用户分配PDCCH，如果都分不到就删除该竞争体(下行的说法)
    //PDCCH
    AllocatePDCCH(_vActiveSetUpdate, vRBUsedFlagDL);

    //目前为空
//    PUCCH_Allocator();

    //分配RB位置,选取最大的连续RB段，暂不考虑，实际分配的RB数和RBNumAllocated有差的错误
    //分配的RB位置保存在竞争体中,资源位图依旧采用vRBUsedFlag保存
    AllocateRBPosition(_vActiveSetUpdate, vRBUsedFlag, vRBUsedFlagDL);

    //构造调度信息
    GenerateSchedulingMessage(_mMSRxBuffer,_qSchM);

    int len = vMSSetCopy.size();
    for(int idx = len-1; idx >= 0; idx--)
    {
        if(find(m_MSScheduleListRR.begin(), m_MSScheduleListRR.end(), vMSSetCopy[idx]) == m_MSScheduleListRR.end())
        {
            m_MSScheduleListRR.push_front(vMSSetCopy[idx]);
        }
    }

    //更新小区资源位图
    _vRBUsedFlagUL = vRBUsedFlag;
    _vRBUsedFlagDL = vRBUsedFlagDL;

    ///小区级指标新增
    int i_Statistics_PRBNum = 0;
    for(auto& Competor:vCompetors){
        i_Statistics_PRBNum += Competor.vRBAllocated.size();
        if(Competor.size() > 1){
            Statistics::Instance().m_BS_UL_MIMO_Layer[BTS2TxID] += Competor.size();
            Statistics::Instance().m_BS_UL_MIMO_PRBNum[BTS2TxID] += Competor.vRBAllocated.size();
            Statistics::Instance().m_BS_UL_MIMO_PairTimes[BTS2TxID] += 1;
        }
    }
    Statistics::Instance().m_BS_UL_PRB_Rate_Use[BTS2TxID] += i_Statistics_PRBNum;

    if(iTime >= Parameters::Instance().BASIC.IWarmUpSlot)
    {
        for(auto& Competor:vCompetors)
        {
            Statistician::Instance().m_BTSData[m_MainServBTSID.GetTxID()].UL.m_iScheMSNum += Competor.size();
        }
        Statistician::Instance().m_BTSData[m_MainServBTSID.GetTxID()].UL.m_iScheTime += 1;
    }

    return;

}
void UL_SchedulingRenewal::Reset()
{
    MSID2Priority.clear();
    MUPairMS.clear();
    MSID2EstimatedSINR.clear();
    MSID2MCS.clear();
    LCG2MSID_HistoryThroughput.clear();
    MSID2LCGBuffer.clear();
    vCompetors.clear();
    vReRBID.clear();
    vReMSID.clear();

    MSID2RBNeed.clear();
    MUAvaliableFlag.clear();

    MSID2CCESB.clear();
    MSID2CCELevel.clear();
}
void UL_SchedulingRenewal::LogBSRInfo(int _iTime) {

    LogBSRInfoLock.lock();
    for (auto iter = MSID2LCGBuffer.begin(); iter != MSID2LCGBuffer.end(); iter++) {
        Observer::Print("BS_buffer_record") << _iTime
                                            << setw(20) << m_MainServBTSID
                                            << setw(20) << iter->first
                                            << setw(20) << (iter->second)[0]
                                            << setw(20) << (iter->second)[1]
                                            << setw(20) << (iter->second)[2]
                                            << setw(20) << (iter->second)[3] << endl;
    }
    LogBSRInfoLock.unlock();
}

void UL_SchedulingRenewal::DeleteUnAvaliableMS_DRX(vector<MSID>& _vMSSet)
{
    vector<MSID>vMSSet = _vMSSet;
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    BTS& bts = m_MainServBTSID.GetBTS();
    //去掉DRX非激活状态的用户
    for(vector<MSID>::iterator it = vMSSet.begin(); it!=vMSSet.end();){
        MSID msid = *it;
        if( bts.MSID2DRX[msid.ToInt()].GetDRXState() == state_sleep){  // || lm.GetCouplingLossDB(bts, it->GetMS()) < -130
            it = vMSSet.erase(it);
            continue;
        }
        else{
            it++;
        }
    }
    _vMSSet = vMSSet;
}

void UL_SchedulingRenewal::UpdateSRFlag(vector<MSID>& _vActiveSet, vector<vector<bool>>& _vRBUsedFlagDL){

    int iTime = Clock::Instance().GetTimeSlot();
    auto  vMSSet = _vActiveSet;
    BTS& MainBTS = m_MainServBTSID.GetBTS();
    //产生UL_Grant  UL_Grant和调度信息应该是一个东西，只是现在先不考虑BSR的资源占用，所以先区分开
    if(Parameters::Instance().MSS_UL.UL.IsSROn)
    {
        ///////////针对快掉话用户////////////////
        auto iter = vMSSet.begin();
        if(Parameters::Instance().MSS_UL.UL.SR_SmartSchSwitch == true)
        {

            while(iter != vMSSet.end())
            {
                if(MainBTS.UL.MSID2SRflag[iter->ToInt()] >= Parameters::Instance().MSS_UL.UL.SR_TransMax - 2 && m_PdcchAllocator.Allocate(*iter, _vRBUsedFlagDL, MSID2CCELevel, MSID2CCESB, Parameters::UL) == true)
                {
                    ULGrant _ULGrant = {*iter, iTime, true};
                    MainBTS.GetDci()->CollectULGrant(_ULGrant);
                    MainBTS.UL.MSID2SRflag[iter->ToInt()] = 0;

                    iter = vMSSet.erase(iter);
                }
                else
                {
                    iter++;
                }
            }
        }

        //////////////针对其他SR用户/////////////////
        iter = vMSSet.begin();
        while(iter != vMSSet.end())
        {
            if(MainBTS.UL.MSID2SRflag[iter->ToInt()] > 0 && m_PdcchAllocator.Allocate(*iter, _vRBUsedFlagDL, MSID2CCELevel, MSID2CCESB, Parameters::UL) == true)
            {
                ULGrant _ULGrant = {*iter, iTime, true};
                MainBTS.GetDci()->CollectULGrant(_ULGrant);
                MainBTS.UL.MSID2SRflag[iter->ToInt()] = 0;

                iter = vMSSet.erase(iter);
            }
            else
            {
                iter++;
            }
        }
    }
    _vActiveSet = vMSSet;
}
extern std::mutex Scheduling_Record_lock;
void UL_SchedulingRenewal::ScheduleReTranList(vector<MSID> &_vActiveSet,
                                              HARQRxStateBTS& _HARQRxState_BTS,
                                              vector<MSID>& _vReMSID,
                                              vector<RBID>& _vReRBID,
                                              vector<vector<vector<bool>>>& vRBUsedFlag,
                                              vector<vector<bool>>& vRBUsedFlagDL,
                                              deque<std::shared_ptr<SchedulingMessageUL> >& _qSchM) {

    //获取重传的消息队列
    int iTimeSlot = Clock::Instance().GetTimeSlot();
    deque<std::shared_ptr<ACKNAKMessageUL>>& qReTranMsgQue = _HARQRxState_BTS.GetReTxMsgQueRef();
    auto iter = qReTranMsgQue.begin();
    while(iter != qReTranMsgQue.end())
    {
        std::shared_ptr<SchedulingMessageUL> pReSchM = (*iter)->GetSchMessage();

        //记录重传的MSID
        MSID _MSID = pReSchM->GetMSID();

        //记录重传的RBID
        vector<RBID> vRBAllocated = pReSchM->GetRBUsed();
        bool success = false;
        if( find(_vReMSID.begin(), _vReMSID.end(), _MSID) == _vReMSID.end() && find(_vActiveSet.begin(), _vActiveSet.end(), _MSID) != _vActiveSet.end())
        {
            int LastK2Slot = m_MainServBTSID.GetBTS().UL.MSID2LastK2Slot[_MSID.ToInt()];
            int iTimeInSFN;
            for(int k2 = 1; k2 <= 8; k2++)
            {
                iTimeInSFN = (iTimeSlot - 1 + k2) % Parameters::Instance().BASIC.ISlotNumPerSFN;
                auto vRBUsedFlagCopy = vRBUsedFlag[iTimeInSFN];
                if( DownOrUpLink(iTimeSlot + k2) == 1 && iTimeSlot + k2 > LastK2Slot &&
                    AllocateRBPositionPerCompetorReTx(vRBUsedFlagCopy, vRBAllocated) == true &&
                    m_PdcchAllocator.Allocate(_MSID, vRBUsedFlagDL, MSID2CCELevel, MSID2CCESB, Parameters::UL) == true ) //PDCCH放最后，因为这个函数内部会直接修改资源位图，必须前两个条件满足之后再走
                {
                    pReSchM->SetCCE(MSID2CCELevel[_MSID.ToInt()], MSID2CCESB[_MSID.ToInt()]);

                    vRBUsedFlag[iTimeInSFN] = vRBUsedFlagCopy;      //更新资源位图

                    _vReMSID.push_back(_MSID);

                    // 在vReRBID.end()前，插入vRBAllocated.begin() 到 vRBAllocated.end() 之间的元素
                    _vReRBID.insert(_vReRBID.end(), vRBAllocated.begin(), vRBAllocated.end());


                    pReSchM->SetBornTime(iTimeSlot);
                    pReSchM->m_iK2Slot = k2;

                    m_MainServBTSID.GetBTS().UL.MSID2LastK2Slot[_MSID.ToInt()] = iTimeSlot + k2; //更新k2标识

                    _qSchM.push_back(pReSchM);

                    success = true;


                    {
                        Scheduling_Record_lock.lock();
                        auto PDSCH_RB = pReSchM->GetRBUsed();
                        auto PDCCH_RB = pReSchM->GetCCESB();
                        Observer::Print("Scheduling_Record") << iTimeSlot
                                                             << setw(20) << m_MainServBTSID
                                                             << setw(20) << _MSID.ToInt()
                                                             << setw(20) << "UL"
                                                             << setw(20) << pReSchM->GetHARQID()
                                                             << setw(20) << pReSchM->GetRank()
                                                             << setw(20) << (pReSchM->GetMCSmat())(0, 0)
                                                             << setw(20) << pReSchM->GetSendNum() + 1
                                                             << setw(20) << pReSchM->GetTBSizeKbit()
                                                             << setw(20) << iTimeSlot + k2
                                                             << setw(20) << '[' << PDSCH_RB[0].ToInt() << ','
                                                             << PDSCH_RB[PDSCH_RB.size() - 1].ToInt() << ']'
                                                             << setw(20);
                        for (int j = 0; j < PDCCH_RB.size(); j++) {
                            Observer::Print("Scheduling_Record") << PDCCH_RB[j] << "\\";
                        }
                        Observer::Print("Scheduling_Record") << endl;
                        Scheduling_Record_lock.unlock();
                    }

                    break;
                }

            }
        }

        if(success == false)
        {
            iter++;
        }
        else
        {
            iter = qReTranMsgQue.erase(iter);
        }
    }

    //删除vReRBID中重复的元素
    sort(_vReRBID.begin(), _vReRBID.end());
    _vReRBID.erase(unique(_vReRBID.begin(), _vReRBID.end()), _vReRBID.end());

    //资源位图相关
//    for (int i = 0; i<static_cast<int> (_vReRBID.size()); ++i)
//    {
//        for(int j = Parameters::Instance().CoresetDuration; j< Parameters::Instance().SymbolNumPerSlot; ++j)
//        {
//            vRBUsedFlag[_vReRBID[i].ToInt()][j] = true;
//        }
//    }

}

void UL_SchedulingRenewal::AvaiHARQIDState(vector<MSID> &_vActiveSet) {

    std::unordered_map<int, vector<bool> >& mMSID2HARQStateList = m_MainServBTSID.GetBTS().GetMSID2HARQStateList_UL();

    for (int i = 0; i < static_cast<int> (_vActiveSet.size());) {
        //被调度的用户
        MSID& MSIDNew = _vActiveSet[i];

        int iAvaiHARQProcessID = -1;
        for (int j = 0; j < static_cast<int> (mMSID2HARQStateList[MSIDNew.ToInt()].size()); ++j) {
            if (!mMSID2HARQStateList[MSIDNew.ToInt()][j]) {
                iAvaiHARQProcessID = j;
                break;
            }
        }

        if (iAvaiHARQProcessID == -1)
        {
            _vActiveSet.erase(std::remove(_vActiveSet.begin(), _vActiveSet.end(), MSIDNew), _vActiveSet.end());
        } else {
            mMSID2AvaiHARQID[MSIDNew.ToInt()] = iAvaiHARQProcessID;
            ++i;
        }
    }
}

void UL_SchedulingRenewal::CalculatePriority(vector<MSID> &_vActiveSet,
                                             std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer) {

#ifndef RR
    vector<MSID> vMSSet = _vActiveSet;
    //遍历所有UE
    auto iter = vMSSet.begin();
    while(iter != vMSSet.end())
    {
        MSID msid = (*iter);
        //初始化优先级，每个UE每个LCG上的优先级
        MSID2Priority[msid.ToInt()] = vector<double>(LCGNum, 0.0);
        MSRxBufferBTS& RxBuffer = _mMSRxBuffer[msid.ToInt()];

        //平台中计算每个RB的SINR，singleRBSINR求平均作为maxSINR，然后用这个SINR映射MCS，用当前最大带宽对应的RB数，求出TBSize，作为PF的分子
        //CalculateMSID2EstimatedSINR();

//        double dHistoryThroughput = _mMSRxBuffer[msid.ToInt()].m_dSuccessRxKbit;    //历史吞吐量（目前只是成功传输的kBit）

        //平台中计算每个RB的SINR，singleRBSINR求平均作为maxSINR，然后用这个SINR映射MCS，用当前最大带宽对应的RB数，求出TBSize，作为PF的分子
        //int iUsedRESum = LinkLevelInterface::Instance_UL().GetRENum(全带宽RB数目, iLayerNum);
        int iUsedRESum = LinkLevelInterface::Instance_UL().GetRENum(iRBNum, 0);

        itpp::imat iCombinedMCS = MSID2MCS[msid.ToInt()];
        //int EstimatedSINR = MSID2EstimatedSINR[msid.ToInt()];

        double dMaxSINR_TBSize = LinkLevelInterface::Instance_UL().MCS2TBSKBit(iCombinedMCS, iUsedRESum);

        //正比例公平算法计算优先级
        for(int LCGID = 0; LCGID < LCGNum ; LCGID++){
//            dHistoryThroughput = _mMSRxBuffer[msid.ToInt()].m_dSuccessRxKbit;

            double dHistoryThroughput = LCG2MSID_HistoryThroughput[LCGID][msid.ToInt()];
            double dPriority = std::pow(dMaxSINR_TBSize, Parameters::Instance().BTS_UL.UL.dPFfactor) / dHistoryThroughput;
            MSID2Priority[msid.ToInt()][LCGID] = dPriority;
        }
        iter++;
    }

    //根据优先级排序 MSID2LCG_Priority
    SortPriorityList(_vActiveSet);
#endif
}

void UL_SchedulingRenewal::CalculateMSID2EstimatedSINR(vector<MSID> &_vActiveSet,
                                                       std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer){
    //根据SRSSINR计算全带宽上的MCS
    for (int i = 0; i < static_cast<int> (_vActiveSet.size());i++) {
        vector<mat> vEstimateSINR = _mMSRxBuffer[_vActiveSet[i].ToInt()].m_vSRSSINR;
        for(auto& dSINR: vEstimateSINR){
            dSINR *= DB2L(_mMSRxBuffer[_vActiveSet[i].ToInt()].m_dOLLAOffsetDB);
        }
        MSID2MCS[_vActiveSet[i].ToInt()] = LinkLevelInterface::Instance_UL().SINR2MCS(vEstimateSINR);
        MSID2EstimatedSINR[_vActiveSet[i].ToInt()] = LinkLevelInterface::Instance_UL().SINRCombineUL2(vEstimateSINR);
    }
}

void UL_SchedulingRenewal::SortPriorityList(vector<MSID> &_vActiveSetUpdate) {

    BTS& MainBTS = m_MainServBTSID.GetBTS();

    for (int i = 0; i < static_cast<int> (_vActiveSetUpdate.size()); i++)
    {
        MSID2LCGIndex[_vActiveSetUpdate[i].ToInt()] = MainBTS.m_clsMac.GetLCGIDOfHighestPriority(_vActiveSetUpdate[i].ToInt());
    }

    sort(_vActiveSetUpdate.begin(), _vActiveSetUpdate.end(), [&](const MSID& msid1, const MSID& msid2)
    {
        return MSID2LCGIndex[msid1.ToInt()] < MSID2LCGIndex[msid2.ToInt()] ||
               (MSID2LCGIndex[msid1.ToInt()] == MSID2LCGIndex[msid2.ToInt()] && MSID2Priority[msid1.ToInt()][MSID2LCGIndex[msid2.ToInt()]] > MSID2Priority[msid2.ToInt()][MSID2LCGIndex[msid2.ToInt()]]);
    });

}

void UL_SchedulingRenewal::MakeMUPair(vector<MSID> &_vActiveSetUpdate, std::unordered_map<int, MSRxBufferBTS> &_mMSRxBuffer) {

    //_vActiveSetUpdate是按照优先级排序后的队列,MU输出采用vector<Competor> vCompetors保存信息

    cm::LinkMatrix &lm = cm::LinkMatrix::Instance();
    BTS &MainBTS = m_MainServBTSID.GetBTS();
    double PLofMainMS, PLofPairMS;
    for(auto MainMS:_vActiveSetUpdate)
    {
        for(auto PairMS:_vActiveSetUpdate)
        {
            PLofMainMS = lm.GetCouplingLossDB(MainBTS, MainMS.GetMS());
            PLofPairMS = lm.GetCouplingLossDB(MainBTS, PairMS.GetMS());

            if (abs(PLofPairMS - PLofMainMS) <= PLWindowForPairing &&
                lm.GetStrongestBSBeamIndex(MainBTS, MainMS.GetMS()) == lm.GetStrongestBSBeamIndex(MainBTS, PairMS.GetMS()))
            {
                MUAvaliableFlag[MainMS.ToInt()][PairMS.ToInt()] = true;
            }
            else
            {
                MUAvaliableFlag[MainMS.ToInt()][PairMS.ToInt()] = false;
            }
        }
    }
//    auto iter = _vActiveSetUpdate.begin();
//    while(iter != _vActiveSetUpdate.end()) {
//        //查看该用户是否被配对,如果已经被配对,则返回
//        if (find(MUPairMS.begin(), MUPairMS.end(), *iter) != MUPairMS.end())
//        {
//            iter++;
//            continue;
//        }
//        else
//        {
//            vector<MSID> vPairMS;
//            vPairMS.emplace_back(*iter);
//            double PLofMainMS = lm.GetCouplingLossDB(MainBTS, iter->GetMS());
//
//            auto it_pair = iter + 1;
//            while (it_pair != _vActiveSetUpdate.end() && vPairMS.size() < m_iMUNum) {
//                if (find(MUPairMS.begin(), MUPairMS.end(), *it_pair) != MUPairMS.end()) {
//                    it_pair++;
//                    continue;
//                } else {
//                    double PLofPairMS = lm.GetCouplingLossDB(MainBTS, it_pair->GetMS());
//                    //如果两用户PL差值小于门限值且。。。，则认为这两用户可以配对
//                    if (abs(PLofPairMS - PLofMainMS) <= PLWindowForPairing &&
//                        lm.GetStrongestBSBeamIndex(MainBTS, iter->GetMS()) ==
//                        lm.GetStrongestBSBeamIndex(MainBTS, it_pair->GetMS())) {
//                        vPairMS.emplace_back(*it_pair);
//                        MUPairMS.emplace_back(*it_pair);
//                    }
//                    it_pair++;
//                }
//            }
//            //通过竞争体保存MU配对用户信息
//            vCompetors.emplace_back(vPairMS);
//            iter++;
//        }
//    }
}


void UL_SchedulingRenewal::UpdateQoS(vector<MSID>& _vActiveSetUpdate, vector<MSID> _vReMSID ) {

    BTS& MainBTS = m_MainServBTSID.GetBTS();
    vector<MSID> _vActiveSetNoBuffer;
    //剔除没有BSR-Buffer的用户
    for(auto ID : _vActiveSetUpdate)
    {
        double dBufferSize = 0.0;
        if(MainBTS.m_clsMac.checkLCGBuffer(ID.ToInt(), dBufferSize) != -1)//dBufferSize useless
        {

            _vActiveSetNoBuffer.emplace_back(ID);
        }
    }
    swap(_vActiveSetUpdate, _vActiveSetNoBuffer);

    //MU配对时，只将高优先级UE保留在QoS队列中，其他配对UE从QoS队列中剔除
    //此步骤于MU配对时完成

    //重传UE从QoS队列中剔除（ReMSID）
    for (auto iter = _vActiveSetUpdate.begin(); iter!= _vActiveSetUpdate.end();)
    {
        if(find( _vReMSID.begin(), _vReMSID.end(), *iter ) != _vReMSID.end())
        {
            iter = _vActiveSetUpdate.erase(iter);
        }
        else
        {
            iter++;
        }
    }


    //BJ令牌桶中无法调度的UE从QoS队列中剔除
    //_ID.GetMS().m_clsMacMs.IsBucketAvailable()只会返回false
    //在m_clsMacMs（MAC_MS)中的成员变量LCs可能为空，导致无法判断逻辑信道的令牌桶大小。
//    vector<MSID> _vActiveSet_BJBucket;
//    for(MSID _ID : _vActiveSetUpdate){
//        if(_ID.GetMS().m_clsMacMs.IsBucketAvailable() == true)
//        {
//            _vActiveSet_BJBucket.emplace_back(_ID);
//        }
//    }
//    swap(_vActiveSetUpdate, _vActiveSet_BJBucket);

}

void UL_SchedulingRenewal::CalculatePUSCHRBNum(vector<MSID>& _vActiveSetUpdate, std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer) {
    for(auto msid:_vActiveSetUpdate)
    {
        MSID2RBNeed[msid.ToInt()] = 0;
        unordered_map<int,double> LCG2Buffer = MSID2LCGBuffer[msid.ToInt()];

        //平均求MCS等级,实际上多流MCS等级应该是一致的,待修改(用SINR求平均）
        itpp::imat iCombinedMCS = MSID2MCS[msid.ToInt()];
        int iMCS = 0;
        for (int row = 0; row < iCombinedMCS.rows(); ++row) {
            for (int col = 0; col < iCombinedMCS.cols(); ++col) {
                iMCS += iCombinedMCS(row, col);
            }
        }//验证
        iMCS /= iCombinedMCS.rows();
        iMCS /= iCombinedMCS.cols();


        //通过MCS矩阵获取流数
        int iRank = iCombinedMCS.rows();
        iRank = iRank - 1;

        //每个RB上可用的RE数,按照要求应该是每次调度不一致,待修改
        int SingleRB = 1;
        int RENum = LinkLevelInterface::Instance_UL().GetRENum(SingleRB, 1);



        for(int LCGID = 0;LCGID < LCGNum; LCGID++){
            MSID2RBNeed[msid.ToInt()] += LinkLevelInterface::Instance_UL().GetRBNum_ByTable(iRank, RENum, iMCS, LCG2Buffer[LCGID]);
        }
        //考虑RBLimited的限制
        if (MSID2RBNeed[msid.ToInt()] > MSID2RBLimit[msid.ToInt()]) {
            MSID2RBNeed[msid.ToInt()] = MSID2RBLimit[msid.ToInt()];
        }
    }
    auto iter = _vActiveSetUpdate.begin();
    while(iter != _vActiveSetUpdate.end())
    {
        if(MSID2RBNeed[iter->ToInt()] == 0)
        {
            iter = _vActiveSetUpdate.erase(iter);
        }
        else
        {
            iter++;
        }
    }


    //////////
//    for(auto& Competor:vCompetors)
//    {
//        MSID msid = Competor[0];
//        unordered_map<int,double> LCG2Buffer = MSID2LCGBuffer[msid.ToInt()];
//
//        //平均求MCS等级,实际上多流MCS等级应该是一致的,待修改(用SINR求平均）
//        itpp::imat iCombinedMCS = MSID2MCS[msid.ToInt()];
//        int iMCS = 0;
//        for (int row = 0; row < iCombinedMCS.rows(); ++row) {
//            for (int col = 0; col < iCombinedMCS.cols(); ++col) {
//                iMCS += iCombinedMCS(row, col);
//            }
//        }//验证
//        iMCS /= iCombinedMCS.rows();
//        iMCS /= iCombinedMCS.cols();
//
//
//        //通过MCS矩阵获取流数
//        int iRank = iCombinedMCS.rows();
//        iRank = iRank - 1;
//
//        //每个RB上可用的RE数,按照要求应该是每次调度不一致,待修改
//        int SingleRB = 1;
//        int RENum = LinkLevelInterface::Instance_UL().GetRENum(SingleRB, 1);
//
//
//        //考虑RBLimited的限制
//        int RBLimit = 0;
//        if (Parameters::Instance().MSS_UL.UL.IsPhrOn)
//        {
//            RBLimit = m_MainServBTSID.GetBTS().UL.MSID2RBNumSupported[msid.ToInt()];
//        } else {
//            RBLimit = msid.GetMS().UL.GetRBNumSupport();
//        }
//
//        for(int LCGID = 0;LCGID < LCGNum; LCGID++){
//
//            int _RBNeed = LinkLevelInterface::Instance_UL().GetRBNum_ByTable(iRank, RENum, iMCS, LCG2Buffer[LCGID]);
//
//            if(_RBNeed <= RBLimit)
//            {
//                //是否应该用map
//                Competor.RBNeed[LCGID] = _RBNeed;
//                RBLimit -= _RBNeed;
//            }else
//            {
//                Competor.RBNeed[LCGID] = RBLimit;
//                RBLimit = 0;
//            }
//        }
//    }
//
//    //分配RB数目
//    assert(vReRBID.size() <= iRBNum);
//
//    for(auto& Competor:vCompetors)
//    {
//        int RBNeed = 0;
//        for(int LCGID = 0;LCGID < LCGNum; LCGID++)
//        {
//            RBNeed += Competor.RBNeed[LCGID];
//        }
//        Competor.RBNumAllocated = RBNeed;
//    }
//    auto iter = vCompetors.begin();
//    while(iter != vCompetors.end())
//    {
//        if(iter->RBNumAllocated == 0)
//        {
//            iter = vCompetors.erase(iter);
//        }
//        else
//        {
//            iter++;
//        }
//    }
}

void UL_SchedulingRenewal::AllocatePDCCH(vector<MSID>& _vActiveSet, vector<vector<bool>>& vRBUsedFlag) {

    for(auto iter = _vActiveSet.begin(); iter != _vActiveSet.end();)
    {
        if(m_PdcchAllocator.Allocate(*iter, vRBUsedFlag, MSID2CCELevel, MSID2CCESB, Parameters::UL) == false)
        {
            iter = _vActiveSet.erase(iter);
        }
        else
        {
            iter++;
        }
    }
//    //给竞争体中的每个用户分配PDCCH，如果都分不到就删除该竞争体
//    for(auto iter = vCompetors.begin(); iter != vCompetors.end();){
//        auto& competor = *iter;
//        for(auto iter_competer = competor.begin(); iter_competer != competor.end();){
//            if(m_PdcchAllocator.Allocate(*iter_competer, vRBUsedFlag, MSID2CCELevel, MSID2CCESB, Parameters::UL) == false){
//                iter_competer = competor.erase(iter_competer);
//            }
//            else{
//                iter_competer++;
//            }
//        }
//        if(competor.size() == 0){
//            iter = vCompetors.erase(iter);
//        }
//        else{
//            iter++;
//        }
//    }
}

void UL_SchedulingRenewal::PUCCH_Allocator() {
    return;
}

void UL_SchedulingRenewal::AllocateRBPosition(vector<MSID>& _vActiveSet, vector<vector<vector<bool>>>& vRBUsedFlag, vector<vector<bool>>& _vRBUsedFlagDL) {

    int iTimeSlot = Clock::Instance().GetTimeSlot();
    int iTimeInSFN;
    vector<vector<vector<bool>>> ResourceGrid =  vRBUsedFlag;
    vector<MSID> vMSSet = _vActiveSet;

    while(vMSSet.size() > 0)
    {
        auto iter = vMSSet.begin();
        MSID main_msid = (*iter);
        Competor competor;
        competor.push_back(main_msid);
        competor.RBNumAllocated = MSID2RBNeed[main_msid.ToInt()];
        bool success = false;
        int LastK2Slot = m_MainServBTSID.GetBTS().UL.MSID2LastK2Slot[main_msid.ToInt()];
        int k2 = 1;
        for(k2 = 1; k2 <= 8; k2++)
        {
            iTimeInSFN = (iTimeSlot - 1 + k2) % Parameters::Instance().BASIC.ISlotNumPerSFN;
            if( DownOrUpLink(iTimeSlot + k2) == 1 &&
                iTimeSlot + k2 > LastK2Slot &&
                AllocateRBPositionPerCompetor(ResourceGrid[iTimeInSFN], competor) == true )
            {
                m_MainServBTSID.GetBTS().UL.MSID2LastK2Slot[main_msid.ToInt()] = iTimeSlot + k2; //更新k2标识
                MSID2K2SlotAllocated[main_msid.ToInt()] = k2;
                success = true;
                break;
            }
        }

        iter = vMSSet.erase(iter);

        if(success == true)
        {
            while(iter != vMSSet.end() && competor.size() < m_iMUNum)
            {
                MSID pair_msid = *iter;
                if(MUAvaliableFlag[main_msid.ToInt()][pair_msid.ToInt()] == 1 && iTimeSlot + k2 > m_MainServBTSID.GetBTS().UL.MSID2LastK2Slot[pair_msid.ToInt()])
                {
                    m_MainServBTSID.GetBTS().UL.MSID2LastK2Slot[pair_msid.ToInt()] = iTimeSlot + k2; //更新配对用户k2标识
                    competor.push_back(pair_msid);
                    iter = vMSSet.erase(iter);
                }
                else
                {
                    iter++;
                }
            }
            vCompetors.push_back(competor);
        }
        else
        {
            //PDCCH资源回滚
            auto& vCCERB = MSID2CCESB[main_msid.ToInt()];
            ///PDCCH资源回滚，回退上传占据的CCE RB
            main_msid.GetMS().GetMainServBTS().GetBTS().UlCceOccupiedNum -= MSID2CCELevel[main_msid.ToInt()];
            for(auto rb : vCCERB)
            {
                for(int symbol = 0; symbol < Parameters::Instance().CoresetDuration; symbol++)
                {
                    _vRBUsedFlagDL[rb][symbol] = false;
                }
            }
        }
    }
//    auto iter = vCompetors.begin();
//    while(iter != vCompetors.end())
//    {
//        MSID msid = (*iter)[0];
//        bool success = false;
//        int LastK2Slot = m_MainServBTSID.GetBTS().UL.MSID2LastK2Slot[msid.ToInt()];
//        int k2 = 1;
//        for(k2 = 1; k2 <= 8; k2++)
//        {
//            iTimeInSFN = (iTimeSlot - 1 + k2) % Parameters::Instance().BASIC.ISlotNumPerSFN;
//            if( DownOrUpLink(iTimeSlot + k2) == 1 &&
//                iTimeSlot + k2 > LastK2Slot &&
//                AllocateRBPositionPerCompetor(ResourceGrid[iTimeInSFN], *iter) == true )
//            {
//                m_MainServBTSID.GetBTS().UL.MSID2LastK2Slot[msid.ToInt()] = iTimeSlot + k2; //更新k2标识
//                MSID2K2SlotAllocated[msid.ToInt()] = k2;
//                success = true;
//                break;
//            }
//        }
//
//        if(success == false)
//        {
//            iter = vCompetors.erase(iter);
//        }
//        else
//        {
//            iter++;
//        }
//    }
    vRBUsedFlag = ResourceGrid;
}

void UL_SchedulingRenewal::GenerateSchedulingMessage(std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer,
                                                     deque<std::shared_ptr<SchedulingMessageUL> >& _qSchM) {
    BTS &MainBTS = m_MainServBTSID.GetBTS();

    int iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);
    int iTime = Clock::Instance().GetTimeSlot();
    int iMUCount = 0;
    int iSUCount = 0;
    int iTotalRB = 0;
    for (auto &Competor: vCompetors) {
        if(Competor.size() == 1){
            iSUCount++;
        }else{
            iMUCount++;
        }
        iTotalRB += Competor.RBNumAllocated;
    }

    for (auto &Competor: vCompetors)
    {
        vector <RBID> vRBAllocated = Competor.vRBAllocated;//竞争体中有
        std::unordered_map<int, vector<MSID>> mRB2MSID;

        for(auto iter = vRBAllocated.begin();iter != vRBAllocated.end();iter++)
        {
            mRB2MSID[iter->ToInt()] = Competor;
        }

        for (int i = 0; i < Competor.size(); i++) {
            MSID msid = Competor[i];
            m_MSScheduleListRR.push_back(msid);

            vector <mat> vSINR;


            RBID FirstRBID = *(vRBAllocated.begin());
            RBID LastRBID = *(vRBAllocated.rbegin());

            imat iMCSEstimated = MSID2MCS[Competor[i].ToInt()];
            for (int row = 0; row < iMCSEstimated.rows(); row++) {
                for (int col = 0; col < iMCSEstimated.cols(); col++) {
                    iMCSEstimated(row, col) = max(iMCSEstimated(row, col), 0);
                }
            }

            int iHARQID = mMSID2AvaiHARQID[msid.ToInt()];
            msid.GetMS().GetMainServBTS().GetBTS().m_MSID2HARQStateList_UL[msid.ToInt()][iHARQID] = true;
            if(iTime >= Parameters::Instance().BASIC.IWarmUpSlot)
            {
                Statistician::Instance().m_MSData_UL[msid.ToInt()].m_dAVE_preSINR += MSID2EstimatedSINR[msid.ToInt()];
                Statistician::Instance().m_MSData_UL[msid.ToInt()].m_dAVE_preSINR_Index += 1;
            }

            int iRank = iMCSEstimated.rows();

            std::shared_ptr<SchedulingMessageUL> pSchM(new SchedulingMessageUL(msid, iHARQID));
            pSchM->SetMCSmat(iMCSEstimated);
            pSchM->SetRB2vMSID(mRB2MSID);
            pSchM->SetRank(iRank);
            pSchM->SetRBUsed(vRBAllocated);
            pSchM->SetSINR(MSID2EstimatedSINR[msid.ToInt()]);

            pSchM->SetCCE(MSID2CCELevel[msid.ToInt()], MSID2CCESB[msid.ToInt()]);

            assert(DownOrUpLink(MSID2K2SlotAllocated[msid.ToInt()] + iTime) == 1);
            pSchM->m_iK2Slot = MSID2K2SlotAllocated[msid.ToInt()];

            //在ComputeEstimateSINR中输入
//            pSchM->SetvVmimoGroup(Competor);

            double dTotalBuffer = MainBTS.m_clsMac.GetSumBuffer(msid.ToInt());
            pSchM->SetTBSizeKbitWithBuffer(dTotalBuffer);//m_mRB2vMSID流数需要


            //在该函数中更新历史吞吐量void MAC_BTS::ReduceBuffer(const int& msid, double datasize)
            //ACK\NACK反馈在哪里,直接减少Buffer对不对???
            //如果不对,应该改造ReduceBuffer,同时可以利用现有ReducedBuffer计算历史吞吐量
//            cout << "test " << endl;
//            cout << (MainBTS.m_clsMac.GetLGCbuffer())[msid.ToInt()][0] << endl;
            MainBTS.m_clsMac.ReduceBuffer(msid.ToInt(), pSchM->GetTBSizeKbit());
//            cout << (MainBTS.m_clsMac.GetLGCbuffer())[msid.ToInt()][0] << endl;
//            m_MSID.GetMS().m_clsMacMs.ReduceBuffer(dCorrectKbit);
//减少用户的Buffer
            double dTBSize = pSchM->GetTBSizeKbit();
            msid.GetMS().m_clsMacMs.ReduceBuffer(dTBSize);

            //Hyl 冗余
//            if (MainBTS.m_clsMac.GetSumBuffer(msid.ToInt()) < 0.00001) {
//                LogInfoLock.lock();
//                Observer::Print("SRlog") << iTime
//                                         << setw(20) << MainBTS.GetTxID()
//                                         << setw(20) << (msid)
//                                         << setw(20) << "removed" << endl;
//                LogInfoLock.unlock();
//            }


            ///小区级指标新增
            int BTS2TxID = m_MainServBTSID.GetBTS().GetTxID();
            vector<MSID>& v_Statistics_MSID = Statistics::Instance().m_BS_UL_Active_vMSID[BTS2TxID];
            if (find(v_Statistics_MSID.begin(), v_Statistics_MSID.end(), msid) == v_Statistics_MSID.end()) {
                v_Statistics_MSID.push_back(msid);
            }


            {
                Scheduling_Record_lock.lock();
                auto PDSCH_RB = pSchM->GetRBUsed();
                auto PDCCH_RB = pSchM->GetCCESB();
                Observer::Print("Scheduling_Record") << iTime
                                                     << setw(20) << m_MainServBTSID
                                                     << setw(20) << msid.ToInt()
                                                     << setw(20) << "UL"
                                                     << setw(20) << pSchM->GetHARQID()
                                                     << setw(20) << pSchM->GetRank()
                                                     << setw(20) << (pSchM->GetMCSmat())(0,0)
                                                     << setw(20) << pSchM->GetSendNum() + 1
                                                     << setw(20) << pSchM->GetTBSizeKbit()
                                                     << setw(20) << iTime + MSID2K2SlotAllocated[msid.ToInt()]
                                                     << setw(20) << '[' << PDSCH_RB[0].ToInt() << ','
                                                     << PDSCH_RB[PDSCH_RB.size() - 1].ToInt() << ']'
                                                     << setw(20);
                for (int j = 0; j < PDCCH_RB.size(); j++) {
                    Observer::Print("Scheduling_Record") << PDCCH_RB[j] << "\\";
                }
                Observer::Print("Scheduling_Record") << endl;
                Scheduling_Record_lock.unlock();
            }



            _qSchM.push_back(pSchM);
        }
    }
}

bool UL_SchedulingRenewal::AllocateRBPositionPerCompetor(vector<vector<bool>>& _vRBUsedFlag, Competor& _competor)
{
    int iRBNum = Parameters::Instance().BASIC.IRBNum;
    vector<vector<bool>> ResourceGrid =  _vRBUsedFlag;
    vector<bool> vRBAvaliableFlag(iRBNum);
    //先取出所有可用的RB
    for(int i = 0; i < iRBNum; i++)
    {
        bool Avaliable = true;

        //如果一个SB上有一个符号已被占用，那么这个SB就认为不可用
        //CoresetDuration之前的符号是PDCCH专用，不考虑
        for(int j=Parameters::Instance().CoresetDuration; j<Parameters::Instance().SymbolNumPerSlot;j++)
        {
            if(ResourceGrid[i][j] == true)
            {
                Avaliable = false;
                break;
            }
        }
        vRBAvaliableFlag[i] = Avaliable;
    }

    int iRBNeed = _competor.RBNumAllocated;
    vector<int> RBAllocated;
    vector<int> RBAllocated_temp;
    int index = 0, len = iRBNum;
    while(index < len)
    {
        if(vRBAvaliableFlag[index] == false)
        {
            if(RBAllocated_temp.size() > RBAllocated.size())
            {
                RBAllocated = RBAllocated_temp;
            }
            RBAllocated_temp.clear();
        }
        else
        {
            RBAllocated_temp.push_back(index);
            if(RBAllocated_temp.size() == iRBNeed){
                RBAllocated = RBAllocated_temp;
                break;
            }
        }
        index++;
    }

    if(RBAllocated.size() == 0)
    {
        return false;
    }
    else
    {
        for(auto i:RBAllocated)
        {
            _competor.vRBAllocated.emplace_back(RBID(i));
            for(int j = Parameters::Instance().CoresetDuration; j < Parameters::Instance().SymbolNumPerSlot; j++){
                ResourceGrid[i][j] = true;
            }
        }
    }
    _vRBUsedFlag = ResourceGrid;
    return true;
}
bool UL_SchedulingRenewal::AllocateRBPositionPerCompetorReTx(vector<vector<bool>>& _vRBUsedFlag, vector<RBID>& _vReTxRBID)
{
    int iRBNum = Parameters::Instance().BASIC.IRBNum;
    vector<vector<bool>> ResourceGrid =  _vRBUsedFlag;
    vector<bool> vRBAvaliableFlag(iRBNum);
    //先取出所有可用的RB
    for(int i = 0; i < iRBNum; i++)
    {
        bool Avaliable = true;

        //如果一个SB上有一个符号已被占用，那么这个SB就认为不可用
        //CoresetDuration之前的符号是PDCCH专用，不考虑
        for(int j=Parameters::Instance().CoresetDuration; j<Parameters::Instance().SymbolNumPerSlot;j++)
        {
            if(ResourceGrid[i][j] == true)
            {
                Avaliable = false;
                break;
            }
        }
        vRBAvaliableFlag[i] = Avaliable;
    }

    bool flag = true;
    for(auto& rbid : _vReTxRBID)
    {
        if(vRBAvaliableFlag[rbid.ToInt()] == false)
        {
            flag = false;
            break;
        }
    }
    if(flag == false)
    {
        return false;
    }


    for(auto& rbid : _vReTxRBID)
    {
        for(int j = Parameters::Instance().CoresetDuration; j < Parameters::Instance().SymbolNumPerSlot; j++)
        {
            ResourceGrid[rbid.ToInt()][j] = true;
        }
    }

    _vRBUsedFlag = ResourceGrid;
    return true;
}