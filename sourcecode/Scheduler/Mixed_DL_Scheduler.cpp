#include "../MobileStation/HARQRxStateMS.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../BaseStation/BTSID.h"

#include "../MobileStation/MSID.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "SchedulerDL.h"
#include "SchedulingMessageDL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/CodeBook.h"

#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../NetworkDrive/Clock.h"
#include "../BaseStation/BTSTxer.h"
#include "Mixed_DL_Scheduler.h"
#include "../MobileStation/MS.h"
#include "../MobileStation/CQIMessage.h"
#include "../BaseStation/BTS.h"
#include "../BaseStation/CodeBookFactory.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../Statistician/Statistics.h"
#include "../Statistician/Statistician.h"

std::mutex Mixed_DL_Scheduler_mutex;
Mixed_DL_Scheduler::Mixed_DL_Scheduler(const BTSID& _MainServBTSID)
        : SchedulerDL(_MainServBTSID), m_PdcchAllocator(_MainServBTSID) {
    RR_Index = 0;
    m_iMUNum = Parameters::Instance().MIMO_CTRL.IMUMIMONum;
    PLWindowForPairing = 5;
    LCNum = 4;
}
//@

std::mutex QoSLock;
void Mixed_DL_Scheduler::Scheduling(BTSTxer* _pBTSTxer, vector<MSID>& _vActiveSet, HARQTxStateBTS& _HARQTxState, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer, SafeDeque<std::shared_ptr<SchedulingMessageDL> >& _qScheduleMesQueue, vector<vector<bool>>& _vSBUsedFlag) {
    MSID2CCELevel.clear();
    MSID2CCESB.clear();
    //若没有服务的用户，直接返回
    if (_vActiveSet.empty()) {
        return;
    }
    int iTime = Clock::Instance().GetTimeSlot();
    vector<SBID> vSBSet; //存储参与调度分配的子带资源
    vector<vector<bool>> vSBSetUsedFlag = _vSBUsedFlag;
    for (SBID sbid = SBID::Begin(); sbid <= SBID::End(); ++sbid) {
        vSBSet.push_back(sbid);
//        vSBSetUsedFlag.push_back(false);
    }
    vector<MSID> vMSSet;
    if(iTime == 1)
    {
        m_MSScheduleListRR.clear();//清除上个drop的队列
    }
    if(m_MSScheduleListRR.empty())
    {
        vMSSet = _vActiveSet; //存储参与调度竞争的移动台ID
    }
    else
    {
        while(!m_MSScheduleListRR.empty())
        {
            vMSSet.push_back(m_MSScheduleListRR.front());
            m_MSScheduleListRR.pop_front();
        }
    }
    vector<MSID> vMSSetCopy = vMSSet;


    unordered_map<int, vector<double>>MSID2Priority;
    unordered_map<int, int> MSID2MCS;

    std::unordered_map<int, vector<bool> >& mMSID2HARQStateList = m_MainServBTSID.GetBTS().GetMSID2HARQStateList();
    std::unordered_map<int, vector<MSID> > mReTxSBID2vMSID;
    vector<MSID> mReTxMSID;
    std::unordered_map<int, int> mMSID2HARQID;
    vector<MSID> UnAvaliableMSID;
    std::unordered_map<int, int> mMSID2AvaiHARQID;

    ///小区级指标新增
    int BTS2TxID = m_MainServBTSID.GetBTS().GetTxID();
    Statistics::Instance().m_BS_DL_PRB_Rate_Total[BTS2TxID] += vSBSet.size();

    // CQI消息为空的用户，去除后存在UnAvaliableMSID中
    GetUnAvaliableMS_CQI(vMSSet, UnAvaliableMSID, _mMSTxBuffer);

    //去除DRX非激活的用户，去除后存在UnAvaliableMSID中   路损小于-130的也去除了
    GetUnAvaliableMS_DRX(vMSSet, UnAvaliableMSID);


    //HYL 冗余输出
//    for(auto msid:vMSSet){
//        QoSLock.lock();
//        Observer::Print("QoS_Record_before")<<iTime
//                            <<setw(20)<<msid
//                            <<setw(20)<<m_MainServBTSID;
//        for(int i=0;i<LCNum;i++){
//            Observer::Print("QoS_Record_before")<<setw(20)<<MSID2Priority[msid.ToInt()][i];
//        }
//        Observer::Print("QoS_Record_before")<<endl;
//        QoSLock.unlock();
//    }

    //Time out Process
    TimeOutProcess(_vActiveSet, _mMSTxBuffer, _HARQTxState, mMSID2HARQStateList);

    //计算每个用户在每个逻辑信道上优先级，并记录MCS
    Calc_UE_Priority(vMSSet, _mMSTxBuffer, MSID2Priority, MSID2MCS, UnAvaliableMSID);
    //重传操作
    //重传用户会从vMSSet中删掉，存放在mReTxMSID中
    HARQ(vMSSet, _HARQTxState, mMSID2HARQID, mReTxMSID, mReTxSBID2vMSID, _qScheduleMesQueue, vSBSetUsedFlag);
    //分配HARQID，去除分不到的用户，存放在UnAvaliableMSID中
    AllocateHARQID(vMSSet, UnAvaliableMSID, mMSID2HARQStateList, mMSID2AvaiHARQID);

    //用户集合为空，返回
    if (vMSSet.empty()){
        return;
    }
    vector<NewCompetor> vCompetors;
    //MU配对,生成vCompetors保存了各个主用户和配对用户
    MUPairing(vMSSet, vCompetors, _mMSTxBuffer);
    assert(vMSSet.size() + mReTxMSID.size() + UnAvaliableMSID.size() == _vActiveSet.size());

//hyl 冗余输出
//    for(auto competor:vCompetors){
//        QoSLock.lock();
//        Observer::Print("QoS_Record_after")<<iTime
//                                     <<setw(20)<<competor[0]
//                                     <<setw(20)<<m_MainServBTSID;
//        for(int i=0;i<LCNum;i++){
//            Observer::Print("QoS_Record_after")<<setw(20)<<MSID2Priority[competor[0].ToInt()][i];
//        }
//        Observer::Print("QoS_Record_after")<<endl;
//        QoSLock.unlock();
//    }

    //给竞争体分配RB数
    PDSCH_RBNum_Allocator(vCompetors, _mMSTxBuffer, MSID2MCS);
    //给竞争体中的每个用户分配PDCCH，如果都分不到就删除该竞争体
    PDCCH_Allocate(vCompetors, vSBSetUsedFlag, MSID2CCELevel, MSID2CCESB);
    //目前为空
    PUCCH_Allocator();
    //PDSCH RB位置分配，确定竞争体具体分到的资源, 目前没有考虑k0!!!!
    PDSCH_RBPos_Allocator(vCompetors, vSBSet, vSBSetUsedFlag);
//#endif
    //生成调度信息
    GenerateSchedulingMessage(vCompetors, _mMSTxBuffer, mMSID2AvaiHARQID, MSID2MCS, _qScheduleMesQueue);
    //更新历史吞吐量，所有用户都要更新
    UpdateHistoryThrouthput(_vActiveSet, _mMSTxBuffer);
    //更新小区资源位图
    _vSBUsedFlag = vSBSetUsedFlag;
    int len = vMSSetCopy.size();
    for(int idx = len-1; idx >= 0; idx--)
    {
        if(find(m_MSScheduleListRR.begin(), m_MSScheduleListRR.end(), vMSSetCopy[idx]) == m_MSScheduleListRR.end())
        {
            m_MSScheduleListRR.push_front(vMSSetCopy[idx]);
        }
    }

    ///小区级指标新增
    int i_Statistics_PRBNum = 0;
    for(auto& Competor:vCompetors){
        i_Statistics_PRBNum += Competor.vSBAllocated.size();
        if(Competor.size() > 1){
            Statistics::Instance().m_BS_DL_MIMO_Layer[BTS2TxID] += Competor.size();
            Statistics::Instance().m_BS_DL_MIMO_PRBNum[BTS2TxID] += Competor.vSBAllocated.size();
            Statistics::Instance().m_BS_DL_MIMO_PairTimes[BTS2TxID] += 1;
        }
    }
    Statistics::Instance().m_BS_DL_PRB_Rate_Use[BTS2TxID] += i_Statistics_PRBNum;

    if(iTime >= Parameters::Instance().BASIC.IWarmUpSlot)
    {
        for(auto& Competor:vCompetors)
        {
            Statistician::Instance().m_BTSData[m_MainServBTSID.GetTxID()].DL.m_iScheMSNum += Competor.size();
        }
        Statistician::Instance().m_BTSData[m_MainServBTSID.GetTxID()].DL.m_iScheTime += 1;
    }
}
void Mixed_DL_Scheduler::Calc_UE_Priority(vector<MSID>& _vMSSet, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer, unordered_map<int, vector<double>>& _MSID2Priority, unordered_map<int, int>& _MSID2MCS, vector<MSID>& _UnAvaliableMSID){
    vector<MSID> vMSSet;
#ifdef RR
    vMSSet = _vMSSet;
    for(auto msid: vMSSet)
    {
        MSTxBufferBTS& Txbuffer = _mMSTxBuffer[msid.ToInt()];
        std::shared_ptr<CQIMessage> pCQIMes = Txbuffer.GetCQIMessage();
        int iRank = pCQIMes->GetRank();
        double dSINR_dB = Txbuffer.GetCQISINR(iRank);
        double msolla = msid.GetMS().GetOllA(iRank);
        dSINR_dB += msolla;  //外环加内环
//        double ollaSINR = Txbuffer.GetOllaSINR();
//        dSINR_dB += ollaSINR;
        //外环加内环得到MCS
        int iMCS = LinkLevelInterface::Instance_DL().SINR2MCS_simplified(dSINR_dB);
        _MSID2MCS[msid.ToInt()] = iMCS;
    }
#else
    vMSSet = _vMSSet;
    auto iter = vMSSet.begin();
    while(iter != vMSSet.end()){
        MSID msid = (*iter);
        _MSID2Priority[msid.ToInt()] = vector<double>(LCNum, 0.0);
        MSTxBufferBTS& Txbuffer = _mMSTxBuffer[msid.ToInt()];
        std::shared_ptr<CQIMessage> pCQIMes = Txbuffer.GetCQIMessage();
        int iRank = pCQIMes->GetRank();
        double dSINR_dB = Txbuffer.GetCQISINR(iRank);
        double msolla = msid.GetMS().GetOllA(iRank);
        dSINR_dB += msolla;  //外环加内环
//        double ollaSINR = Txbuffer.GetOllaSINR();
//        dSINR_dB += ollaSINR;
        //外环加内环得到MCS
        int iMCS = LinkLevelInterface::Instance_DL().SINR2MCS_simplified(dSINR_dB);
        _MSID2MCS[msid.ToInt()] = iMCS;
        int RENum = Parameters::Instance().BASIC.ISBNum * LinkLevelInterface::Instance_DL().GetRENum(Parameters::Instance().BASIC.ISBSize, 1);
        double dTBSKbit = LinkLevelInterface::Instance_DL().MCS2TBSKBit2(iMCS, RENum,iRank+1);  //全带宽的容量

        auto LC2TxKbits = Txbuffer.GetLC2TxKBits();
//        auto LC2Buffer = Txbuffer.GetLC2Buffer();
//        cout<<"MSID="<<msid.ToInt()<<"  Buffer="<<LC2Buffer[0]<<endl;
        for(int lc=0; lc<LCNum;lc++){
            double TxKbit = LC2TxKbits[lc];
            if(TxKbit < 0.001){
                TxKbit = 0.001;
            }
            _MSID2Priority[msid.ToInt()][lc] = pow(dTBSKbit, Parameters::Instance().SIM.DL.DProportionFairFactor) / TxKbit;
        }
        iter++;
    }

    //将用户按最小逻辑信道和优先级排序
    unordered_map<int, int>MSID2LCIndex;
    auto it = vMSSet.begin();
    while(it != vMSSet.end())
    {
        MSID msid = *it;
        MSTxBufferBTS& TxBuffer = _mMSTxBuffer[msid.ToInt()];
        int LCIndex = TxBuffer.GetSmallestLCIndexWithData();
        if(LCIndex < 0){
            _UnAvaliableMSID.emplace_back(*it);
            it = vMSSet.erase(it);
            continue;
        }
        else{
            MSID2LCIndex[msid.ToInt()] = LCIndex;
            it++;
        }
    }
    sort(vMSSet.begin(), vMSSet.end(), [&](const MSID& msid1, const MSID& msid2){
        return MSID2LCIndex[msid1.ToInt()] < MSID2LCIndex[msid2.ToInt()] ||
               (MSID2LCIndex[msid1.ToInt()] == MSID2LCIndex[msid2.ToInt()] && _MSID2Priority[msid1.ToInt()][MSID2LCIndex[msid1.ToInt()]] > _MSID2Priority[msid2.ToInt()][MSID2LCIndex[msid2.ToInt()]]);
    });
#endif
    _vMSSet = vMSSet;

}
void Mixed_DL_Scheduler::TimeOutProcess(vector<MSID>& _vActiveSet, std::unordered_map<int, MSTxBufferBTS>& _MSTxBuffer, HARQTxStateBTS& _HARQTxState, std::unordered_map<int, vector<bool> >& _MSID2HARQStateList){
    int iTime = Clock::Instance().GetTimeSlot();
    if (!_HARQTxState.GetACKNAKMesQueue().empty()) {
        auto cur = _HARQTxState.GetACKNAKMesQueue().begin();
        while (cur != _HARQTxState.GetACKNAKMesQueue().end()) {
            std::shared_ptr<ACKNAKMessageDL> pACKMes = (*cur);
            std::shared_ptr<SchedulingMessageDL> pScheduleMes = pACKMes->GetScheduleMes();
            int iHARQID = pScheduleMes->GetHARQID();
            int iRank = pScheduleMes->GetRank();
            MSID msid = pScheduleMes->GetMSID();

            double dTBSize = pScheduleMes->GetdTBSize();
            auto& vpPacketAndSizeKbits = pScheduleMes->GetUniversal_PacketAndSizeKbits();
            int iPacketNum = static_cast<int> (vpPacketAndSizeKbits[0].size());
            auto iter = vpPacketAndSizeKbits[0].begin();
            while(iter!=vpPacketAndSizeKbits[0].end()){
                if(iter->first->IsTimeout(iTime)){
//                    dTBSize -= iter->second;
                    iter->first->DropPacket_DueToTimeout();
                    iter = vpPacketAndSizeKbits[0].erase(iter);
                }
                else{
                    iter++;
                }
            }
            if(vpPacketAndSizeKbits[0].size() == 0){
                _MSID2HARQStateList[msid.ToInt()][iHARQID] = false;
                msid.GetMS().RefreshHARQSINR(iHARQID, iRank);
                cur = _HARQTxState.GetACKNAKMesQueue().erase(cur);
            }
            else{
                pScheduleMes->SetdTBSize(dTBSize);
                cur++;
            }
        }
    }
    for (const auto& msid : _vActiveSet) {
        _MSTxBuffer[msid.ToInt()].PopTimeOutPacket(iTime);
    }
}
extern std::mutex Scheduling_Record_lock;
void Mixed_DL_Scheduler::HARQ(vector<MSID>& _vMSSet, HARQTxStateBTS& _HARQTxState,
                              std::unordered_map<int, int>& _MSID2HARQID, vector<MSID>& _ReTxMSID,
                              std::unordered_map<int, vector<MSID> >& _ReTxSBID2vMSID,
                              SafeDeque<std::shared_ptr<SchedulingMessageDL> >& _qScheduleMesQueue, vector<vector<bool>>& _vSBSetUsedFlag){
    int iTime = Clock::Instance().GetTimeSlot();
    int ik1_slot = Getk1_slot(iTime);

    if (!_HARQTxState.GetACKNAKMesQueue().empty()) {
        auto cur = _HARQTxState.GetACKNAKMesQueue().begin();
        while (cur != _HARQTxState.GetACKNAKMesQueue().end()) {
            std::shared_ptr<ACKNAKMessageDL> pACKMes = (*cur);
            //            std::shared_ptr<SchedulingMessageDL> pScheduleMes = dynamic_pointer_cast<SchedulingMessageDL> (pACKMes->GetScheduleMes());
            std::shared_ptr<SchedulingMessageDL> pScheduleMes = pACKMes->GetScheduleMes();
            //记录重传的MSID和占用的SB
            MSID msid = pScheduleMes->GetMSID();

            //取得重传需要占用的子带资源
            vector<SBID> vSBUsed = pScheduleMes->GetSBUsed();
            assert(vSBUsed.size() > 0);

            bool bCanReTx = false;
            //if time>k1+k3
            if (_HARQTxState.CanReTx(cur)) {
                //if this MS is retx firstly
                if (find(_vMSSet.begin(), _vMSSet.end(), msid) != _vMSSet.end()) {
                    int i = 0;
                    for (; i < vSBUsed.size(); ++i) {
                        if (_ReTxSBID2vMSID.count(vSBUsed[i].ToInt())) {
                            break;
                        }
                    }
                    if (i == vSBUsed.size()) {
                        bCanReTx = true;
                    }
                }
            }
            bCanReTx = bCanReTx && m_PdcchAllocator.Allocate(msid, _vSBSetUsedFlag, MSID2CCELevel, MSID2CCESB, Parameters::DL);
            if (bCanReTx) {
                pScheduleMes->SetCCE(MSID2CCELevel[msid.ToInt()], MSID2CCESB[msid.ToInt()]);
                pScheduleMes->Setk1_slot(ik1_slot);
                pScheduleMes->SetLatestTransTime(iTime);
                _qScheduleMesQueue.push_back(pScheduleMes);
                assert(pScheduleMes->GetiSendNum()>1);
                _MSID2HARQID[msid.ToInt()] = pScheduleMes->GetHARQID();
                int iRank = pScheduleMes->GetRank();
                _ReTxMSID.push_back(msid);
#ifndef NDEBUG
                {
                    Scheduling_Record_lock.lock();
                    auto PDSCH_RB = pScheduleMes->GetSBUsed();
                    auto PDCCH_RB = pScheduleMes->GetCCESB();
                    Observer::Print("Scheduling_Record") << iTime
                                                         << setw(20) << m_MainServBTSID
                                                         << setw(20) << msid.ToInt()
                                                         << setw(20) << "DL"
                                                         << setw(20) << pScheduleMes->GetHARQID()
                                                         << setw(20) << pScheduleMes->GetRank()+1
                                                         << setw(20) << pScheduleMes->GetiMCS()
                                                         << setw(20) << pScheduleMes->GetiSendNum()
                                                         << setw(20) << pScheduleMes->GetdTBSize()
                                                         << setw(20) << iTime
                                                         << setw(20) << '[' << PDSCH_RB[0].ToInt() << ','
                                                         << PDSCH_RB[PDSCH_RB.size() - 1].ToInt() << ']'
                                                         << setw(20);
                    for (int j = 0; j < PDCCH_RB.size(); j++) {
                        Observer::Print("Scheduling_Record") << PDCCH_RB[j] << "\\";
                    }
                    Observer::Print("Scheduling_Record") << endl;
                    Scheduling_Record_lock.unlock();
                }
#endif
                //从用户集合中去掉需要重传的用户
                _vMSSet.erase(std::remove(_vMSSet.begin(), _vMSSet.end(), msid), _vMSSet.end());

                //统计重传用户占用的资源
                for (int i = 0; i<static_cast<int> (vSBUsed.size()); ++i) {
                    _ReTxSBID2vMSID[vSBUsed[i].ToInt()].push_back(msid);
                    for(int j = Parameters::Instance().CoresetDuration; j< Parameters::Instance().SymbolNumPerSlot; ++j){
                        _vSBSetUsedFlag[vSBUsed[i].ToInt()][j] = true;
                    }
                }
                cur = _HARQTxState.GetACKNAKMesQueue().erase(cur);
            } else {
                cur++;
            }
        }
    }
}

void Mixed_DL_Scheduler::GetUnAvaliableMS_CQI(vector<MSID>& _vMSSet, vector<MSID>& UnAvaliableMSID, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer){
    vector<MSID>vMSSet = _vMSSet;
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    BTS& bts = m_MainServBTSID.GetBTS();
    //去掉CQI消息为空的用户
    for(vector<MSID>::iterator it = vMSSet.begin(); it!=vMSSet.end();)
    {
        MSID msid = *it;
        if(!_mMSTxBuffer[msid.ToInt()].GetCQIMessage()){
            UnAvaliableMSID.emplace_back(msid);
            it = vMSSet.erase(it);
            continue;
        }
        else{
            it++;
        }
    }
    _vMSSet = vMSSet;
}
void Mixed_DL_Scheduler::GetUnAvaliableMS_DRX(vector<MSID>& _vMSSet, vector<MSID>& UnAvaliableMSID){
    vector<MSID>vMSSet = _vMSSet;
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    BTS& bts = m_MainServBTSID.GetBTS();
    //去掉DRX非激活状态的用户
    for(vector<MSID>::iterator it = vMSSet.begin(); it!=vMSSet.end();){
        MSID msid = *it;
        if( bts.MSID2DRX[msid.ToInt()].GetDRXState() == state_sleep){  // || lm.GetCouplingLossDB(bts, it->GetMS()) < -130
            UnAvaliableMSID.emplace_back(msid);
            it = vMSSet.erase(it);
            continue;
        }
        else{
            it++;
        }
    }
    _vMSSet = vMSSet;
}

void Mixed_DL_Scheduler::AllocateHARQID(vector<MSID>& _vMSSet, vector<MSID>& _UnAvaliableMSID, std::unordered_map<int, vector<bool> >& _mMSID2HARQStateList, std::unordered_map<int, int>& _mMSID2AvaiHARQID){
    vector<MSID> vMSSet = _vMSSet;
    for(auto it = vMSSet.begin(); it!=vMSSet.end();){
        MSID msid = *it;
        int iAvaiHARQProcessID = -1;
        for (int i = 0; i < _mMSID2HARQStateList[msid.ToInt()].size(); i++) {
            if (!_mMSID2HARQStateList[msid.ToInt()][i]) {
                iAvaiHARQProcessID = i;
                break;
            }
        }
        if (iAvaiHARQProcessID == -1) {
            _UnAvaliableMSID.emplace_back(msid);
            it = vMSSet.erase(it);
            continue;
        } else {
            _mMSID2AvaiHARQID[msid.ToInt()] = iAvaiHARQProcessID;
            it++;
        }
    }
    _vMSSet = vMSSet;
}

void Mixed_DL_Scheduler::MUPairing(vector<MSID>& _vMSSet, vector<NewCompetor>& _vCompetors, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer){
    vector<MSID>vMSSet = _vMSSet;
    vector<MSID> _PairedMS;

    //MU配对,主用户放在vMSSet中，配对用户放在PairedMS中，同时在_MSID2PairedMS也保存了各个主用户和配对用户
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    BTS& bts = m_MainServBTSID.GetBTS();
    auto it = vMSSet.begin();
    while(it!=vMSSet.end()){
        if(find(_PairedMS.begin(), _PairedMS.end(), *it) != _PairedMS.end())
        {
            it++;
            continue;
        }
        else
        {
            vector<MSID>vPairMS;
            vPairMS.emplace_back(*it);
            double PLofMainMS = lm.GetCouplingLossDB(bts, it->GetMS());
            auto it_pair = it + 1;
            while(it_pair != vMSSet.end() && vPairMS.size() < m_iMUNum)
            {
                if(find(_PairedMS.begin(), _PairedMS.end(), *it_pair) != _PairedMS.end())
                {
                    it_pair++;
                    continue;
                }
                else
                {
                    double PLofPairMS = lm.GetCouplingLossDB(bts, it_pair->GetMS());
                    if(abs(PLofPairMS - PLofMainMS) <= PLWindowForPairing && lm.GetStrongestBSBeamIndex(bts, it->GetMS()) == lm.GetStrongestBSBeamIndex(bts, it_pair->GetMS()) ){
                        vPairMS.emplace_back(*it_pair);
                        _PairedMS.emplace_back(*it_pair);
                    }
                    it_pair++;
                }
            }
            _vCompetors.emplace_back(vPairMS);
            it++;
        }
    }
    _vMSSet = vMSSet;

}

void Mixed_DL_Scheduler::PDSCH_RBNum_Allocator(vector<NewCompetor>& _vCompetors, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer, unordered_map<int, int>& _MSID2MCS){
    //先计算需要的RB数,每个用户每个逻辑信道分别计算
    for(auto& competor:_vCompetors){
        MSID msid = competor[0];

        MSTxBufferBTS& Txbuffer = _mMSTxBuffer[msid.ToInt()];
        auto LC2Buffer = Txbuffer.GetLC2Buffer();
        std::shared_ptr<CQIMessage> pCQIMes = Txbuffer.GetCQIMessage();
        int iRank = pCQIMes->GetRank();
        int iMCS = _MSID2MCS[msid.ToInt()];
        int RENum = LinkLevelInterface::Instance_DL().GetRENum(Parameters::Instance().BASIC.ISBSize, 1);
//        double dTBSKbit = LinkLevelInterface::Instance_DL().MCS2TBSKBit2(iMCS, RENum,iRank+1);  //一个SB上的容量
//        for(int lc=0;lc<LCNum;lc++){
//            //这个之后要改成查表的方式
//            competor.SBNumNeed[lc] = ceil(LC2Buffer[lc] / dTBSKbit);
//        }
        for(int lc=0;lc<LCNum;lc++){
            competor.SBNumNeed[lc] = LinkLevelInterface::Instance_DL().GetRBNum_ByTable(iRank, RENum, iMCS, LC2Buffer[lc]);
        }
    }
    for(auto& competor:_vCompetors){
        competor.SBNumAllocated = 0;
    }
    //RB数量分配
    for(int lc=0; lc<LCNum;lc++){
        for(auto& competor:_vCompetors){
            competor.SBNumAllocated += competor.SBNumNeed[lc];
        }
    }
    for(auto iter = _vCompetors.begin(); iter != _vCompetors.end();){
        if(iter->SBNumAllocated == 0){
            iter = _vCompetors.erase(iter);
            continue;
        }
        else{
            iter++;
        }
    }
}

void Mixed_DL_Scheduler::PDSCH_RBPos_Allocator(vector<NewCompetor>& _vCompetors, vector<SBID>& _vSBSet, vector<vector<bool>>& ResourceGrid){
    vector<vector<bool>> ResourceGrid_temp =  ResourceGrid;
    vector<bool> vSBAvaliableFlag(_vSBSet.size());
    //先取出所有可用的SB
    for(int i=0; i < _vSBSet.size(); i++){
        bool Avaliable = true;
        for(int j=Parameters::Instance().CoresetDuration; j<Parameters::Instance().SymbolNumPerSlot;j++){
            if(ResourceGrid_temp[i][j] == true){
                Avaliable = false;
                break;
            }
        }
        vSBAvaliableFlag[i] = Avaliable;
    }
    //从低频到高频分配，如果所有连续的RB段都不能满足，则分配一块最大的
    auto iter = _vCompetors.begin();
    while(iter != _vCompetors.end()){
        auto& competor = *iter;
        int iSBNeed = competor.SBNumAllocated;
        vector<int> SBAllocated;
        vector<int> SBAllocated_temp;
        int index = 0, len = _vSBSet.size();
        while(index < len){
            if(vSBAvaliableFlag[index] == false){
                //如果走到这里，说明这一段连续的RB已经遍历完了，此时RB数还是不满足需求，但是这一段RB的长度大于上一段RB，所以记录较大的
                if(SBAllocated_temp.size() > SBAllocated.size()){
                    SBAllocated = SBAllocated_temp;
                }
                SBAllocated_temp.clear();
            }
            else{
                SBAllocated_temp.push_back(index);
                if(SBAllocated_temp.size() == iSBNeed || (index == len-1 && SBAllocated_temp.size() > SBAllocated.size())){
                    SBAllocated = SBAllocated_temp;
                    break;
                }
            }
            index++;
        }
        if(SBAllocated.size() == 0){
            iter = _vCompetors.erase(iter);
        }
        else{
            for(auto i:SBAllocated){
                competor.vSBAllocated.push_back(_vSBSet[i]);
                vSBAvaliableFlag[i] = false;
                for(int j=Parameters::Instance().CoresetDuration; j<Parameters::Instance().SymbolNumPerSlot;j++){
                    ResourceGrid_temp[i][j] = true;
                }
            }
            iter++;
        }
    }
    ResourceGrid = ResourceGrid_temp;
}

void Mixed_DL_Scheduler::PDCCH_Allocate(vector<NewCompetor>& _vCompetors, vector<vector<bool>>& ResourceGrid, unordered_map<int, int>& _MSID2CCELevel, unordered_map<int, vector<int>>& _MSID2CCESB){
    for(auto it = _vCompetors.begin(); it != _vCompetors.end();){
        auto& competor = *it;
        for(auto iter = competor.begin(); iter != competor.end();){
            if(m_PdcchAllocator.Allocate(*iter, ResourceGrid, _MSID2CCELevel, _MSID2CCESB, Parameters::DL) == false){
                iter = competor.erase(iter);
                continue;
            }
            else{
                iter++;
            }
        }
        if(competor.size() == 0){
            it = _vCompetors.erase(it);
        }
        else{
            it++;
        }
    }
}

void Mixed_DL_Scheduler::PUCCH_Allocator(){

}

void Mixed_DL_Scheduler::GenerateSchedulingMessage(vector<NewCompetor>& _vCompetors, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer, std::unordered_map<int, int>& _mMSID2AvaiHARQID, unordered_map<int, int>& _MSID2MCS, SafeDeque<std::shared_ptr<SchedulingMessageDL> >& _qScheduleMesQueue){
    auto m_pCodeBook = CodeBookFactory::Instance().GetCodeBook(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num);
    int iTime = Clock::Instance().GetTimeSlot();
    int ik1_slot = Getk1_slot(iTime);


    //计算码本，沿用原来的方式
    for(auto& competor:_vCompetors) {
        unordered_map<int, vector<itpp::cmat>>SBID2vCodeWord;
        if(competor.size() == 1){
            for (auto sbid: competor.vSBAllocated) {
                itpp::cmat m_CodeWord;
                MSID msid = competor[0];
                MSTxBufferBTS &buffer = _mMSTxBuffer[msid.ToInt()];
                std::shared_ptr<CQIMessage> pCQIMes = buffer.GetCQIMessage();
                int iRank = pCQIMes->GetRank();
                if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1) {
                    int iPMI = pCQIMes->GetSubBandPMI(sbid);

                    m_CodeWord = m_pCodeBook->GetCodeWord(iPMI, iRank);
                } else {
                    itpp::cmat mTemp;
                    if (Parameters::Instance().MIMO_CTRL.IHorCovR == 1) {
                        mTemp = buffer.GetCovR(sbid);
                    } else {
                        mTemp = buffer.GetH(sbid);
                    }
                    vec vS;
                    cmat mU, mD;

                    svd(mTemp, mU, vS, mD);
                    cmat mV = mD.get_cols(0, iRank);
                    m_CodeWord = mV;
                }
                SBID2vCodeWord[sbid.ToInt()].push_back(m_CodeWord);
            }
        }
        else if(competor.size() > 1){
            for (auto sbid: competor.vSBAllocated) {
                vector<itpp::cmat> vChannelMat;
                vector<int> vRank;
                vector<int> vRank_real;
                int iTotalRank = 0;

                vector<itpp::vec> v_S;//AI模型使用
                vector<itpp::cmat> v_V;
                if (Parameters::Instance().MIMO_CTRL.IHorCovR) {
                    for (int i = 0; i < competor.size(); ++i) {
                        MSID msid = competor[i];
                        MSTxBufferBTS& buffer = _mMSTxBuffer[msid.ToInt()];
                        std::shared_ptr<CQIMessage> pCQIMes = buffer.GetCQIMessage();
                        int iRank = pCQIMes->GetRank();
                        itpp::cmat mTemp;
                        if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1) {
                            mTemp = buffer.GetH(sbid);
                        } else {
                            mTemp = buffer.GetCovR(sbid);
                        }
                        if (Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1) {
                            cmat final_matrix = ChangeMatFull2Half(mTemp);
                            vChannelMat.push_back(final_matrix);
                            vRank.push_back(0); //区分1Tx or 2Tx
                        } else {
                            vChannelMat.push_back(mTemp);
                            vRank.push_back(iRank);
                        }
                        vRank_real.push_back(iRank);
                        iTotalRank += (iRank + 1);
                    }
                    bool _bIsCovR = true;
                    if(Parameters::Instance().AI_model_on){
                        MET_RankA(v_V,v_S, SBID2vCodeWord[sbid.ToInt()], vRank, _bIsCovR);
                    }
                    else {
                        MET_RankA(vChannelMat, SBID2vCodeWord[sbid.ToInt()], vRank, _bIsCovR);
                    }
                } else {
                    for (int i = 0; i < competor.size(); ++i) {
                        MSID msid = competor[i];
                        MSTxBufferBTS& buffer = _mMSTxBuffer[msid.ToInt()];
                        std::shared_ptr<CQIMessage> pCQIMes = buffer.GetCQIMessage();
                        int iRank = pCQIMes->GetRank();
                        itpp::cmat mTemp = buffer.GetH(sbid); //把mTemp做svd分解后取第三个，为64*64，然后只取其奇数行和奇数列，得到32*32矩阵，上面那一快还要加一句取第一列
                        if (Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1) {
                            cmat final_matrix = ChangeMatFull2Half(mTemp);
                            vChannelMat.push_back(final_matrix);
                            vRank.push_back(0);
                        } else {
                            vChannelMat.push_back(mTemp);
                            vRank.push_back(iRank);
                        }
                        vRank_real.push_back(iRank);
                        iTotalRank += (iRank + 1);

                    }
                    bool _bIsCovR = false;
                    MET_RankA(vChannelMat, SBID2vCodeWord[sbid.ToInt()], vRank, _bIsCovR);
                }
            }
        }
        for (int i = 0; i < competor.size(); i++) {
            MSID msid = competor[i];
            m_MSScheduleListRR.push_back(msid);
            MSTxBufferBTS &buffer = _mMSTxBuffer[msid.ToInt()];
            std::shared_ptr<CQIMessage> pCQIMes = buffer.GetCQIMessage();

            int HARQID = _mMSID2AvaiHARQID[msid.ToInt()];
            int iRank = pCQIMes->GetRank();
            double BufferSize = buffer.GetSumOfBufferSize();
            std::shared_ptr<SchedulingMessageDL> pSchedulingMes(new SchedulingMessageDL(msid, HARQID, iRank));
            msid.GetMS().GetMainServBTS().GetBTS().m_MSID2HARQStateList[msid.ToInt()][HARQID] = true;
            pSchedulingMes->Setk1_slot(ik1_slot);
            pSchedulingMes->SetMCS(_MSID2MCS[msid.ToInt()]);
            pSchedulingMes->SetSBUsed(competor.vSBAllocated);
            pSchedulingMes->SetTBSizeKbit_TDD();
            pSchedulingMes->SetCCE(MSID2CCELevel[msid.ToInt()], MSID2CCESB[msid.ToInt()]);
            if(BufferSize < pSchedulingMes->GetdTBSize()){
                pSchedulingMes->SetdTBSize(BufferSize);
            }
            assert(competor.vSBAllocated.size() > 0);
            for(auto sbid:competor.vSBAllocated){
                pSchedulingMes->SetCodeWord(sbid, SBID2vCodeWord[sbid.ToInt()][i]);
            }
            double CQISINR = buffer.GetCQISINR(iRank);
            pSchedulingMes->CQISINR = CQISINR;
            double msolla = msid.GetMS().GetOllA(iRank); //原平台AMC
//            double msolla = buffer.GetOllaSINR();         //优化后AMC
            CQISINR += msolla;  //外环加内环
            pSchedulingMes->SetSINR(DB2L(CQISINR));
            auto vPacketAndSize = buffer.ReduceBuffer(pSchedulingMes->GetdTBSize());
            pSchedulingMes->SetUniversal_PacketAndSizeKbits(vPacketAndSize);

            ///小区级指标新增
            int BTS2TxID = m_MainServBTSID.GetBTS().GetTxID();
            vector<MSID>& v_Statistics_MSID = Statistics::Instance().m_BS_DL_Active_vMSID[BTS2TxID];
            if (find(v_Statistics_MSID.begin(), v_Statistics_MSID.end(), msid) == v_Statistics_MSID.end()) {
                v_Statistics_MSID.push_back(msid);
            }
#ifndef NDEBUG
            {
                Scheduling_Record_lock.lock();
                auto PDSCH_RB = pSchedulingMes->GetSBUsed();
                auto PDCCH_RB = pSchedulingMes->GetCCESB();
                Observer::Print("Scheduling_Record") << iTime
                                                     << setw(20) << m_MainServBTSID
                                                     << setw(20) << msid.ToInt()
                                                     << setw(20) << "DL"
                                                     << setw(20) << pSchedulingMes->GetHARQID()
                                                     << setw(20) << pSchedulingMes->GetRank()+1
                                                     << setw(20) << pSchedulingMes->GetiMCS()
                                                     << setw(20) << pSchedulingMes->GetiSendNum()
                                                     << setw(20) << pSchedulingMes->GetdTBSize()
                                                     << setw(20) << iTime
                                                     << setw(20) << '[' << PDSCH_RB[0].ToInt() << ','
                                                     << PDSCH_RB[PDSCH_RB.size() - 1].ToInt() << ']'
                                                     << setw(20);
                for (int j = 0; j < PDCCH_RB.size(); j++) {
                    Observer::Print("Scheduling_Record") << PDCCH_RB[j] << "\\";
                }
                Observer::Print("Scheduling_Record") << endl;
                Scheduling_Record_lock.unlock();
            }
#endif
            _qScheduleMesQueue.push_back(pSchedulingMes);
        }
    }
}
void Mixed_DL_Scheduler::UpdateHistoryThrouthput(vector<MSID>&_vActiveSet, std::unordered_map<int, MSTxBufferBTS>&_mMSTxBuffer){
    for(auto msid:_vActiveSet){
        MSTxBufferBTS& TxBuffer = _mMSTxBuffer[msid.ToInt()];
        TxBuffer.UpdateHistoryThrouthput();
    }
}
int Mixed_DL_Scheduler::Getk1_slot(int _iTime) {
    const int static OSNumberInSlot = 14;


    const int static DOWNLINK = 0;
    const int static UPLINK = 1;
    const int static Special = 2;
    const int static GP = -1;
    assert(DownOrUpLink(_iTime) != UPLINK);
    int iTime_OS = 0;
    if (DownOrUpLink(_iTime) == Special) {
        iTime_OS = (_iTime - 1) * OSNumberInSlot + Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLOS;
    } else {
        iTime_OS = _iTime*OSNumberInSlot;
    }

    int N1 = Parameters::Instance().SIM.DL.IN1_OS;

    while (true) {
        if (DownOrUpLink_OS(iTime_OS + N1) == UPLINK || DownOrUpLink_OS(iTime_OS + N1) == Special) {
            break;
        } else {
            N1++;
        }
    }
    int k1 = N1 / OSNumberInSlot + 1;
    return k1;
}

int Mixed_DL_Scheduler::DownOrUpLink_OS(int _iTime_OS) {
    const int static DOWNLINK = 0;
    const int static UPLINK = 1;
    const int static Special = 2;
    const int static GP = -1;

    const int static OSNumberInSlot = 14;

    if(Parameters::Instance().SIM.FrameStructure4Sim.IIsSinglePeriod==1){
        int iTime_OS = ((_iTime_OS - 1) % (Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot * OSNumberInSlot)) + 1;

        if (iTime_OS <= Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot * OSNumberInSlot) {
            return DOWNLINK;
        } else if (iTime_OS >= (Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot - Parameters::Instance().SIM.FrameStructure4Sim.P1.IULSlot) * OSNumberInSlot + 1) {
            return UPLINK;
        }else{
            return Special;
        }
//        } else if (iTime_OS == (Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot + Parameters::Instance().SIM.FrameStructure4Sim.P1.ISpecialSlot) * OSNumberInSlot) {
//            return Special;
//        } else {
//            return GP;
//        }
    }
    else{
        int iTime_OS = ((_iTime_OS - 1) % ((Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot + Parameters::Instance().SIM.FrameStructure4Sim.P2.IPeriodSlot) * OSNumberInSlot)) + 1;
        if(iTime_OS <= Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot * OSNumberInSlot){
            if (iTime_OS <= Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot * OSNumberInSlot) {
                return DOWNLINK;
            } else if (iTime_OS >= (Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot - Parameters::Instance().SIM.FrameStructure4Sim.P1.IULSlot) * OSNumberInSlot + 1) {
                return UPLINK;
            }else{
                return Special;
            }
        }
        else{
            iTime_OS -= Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot * OSNumberInSlot;
            if (iTime_OS <= Parameters::Instance().SIM.FrameStructure4Sim.P2.IDLSlot * OSNumberInSlot) {
                return DOWNLINK;
            } else if (iTime_OS >= (Parameters::Instance().SIM.FrameStructure4Sim.P2.IPeriodSlot - Parameters::Instance().SIM.FrameStructure4Sim.P2.IULSlot) * OSNumberInSlot + 1) {
                return UPLINK;
            }else{
                return Special;
            }
        }
    }

}

