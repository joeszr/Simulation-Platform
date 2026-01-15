///@file BTS.cpp
///@brief  BTS类函数类实现
///@author wangxiaozhou

#include <set>

#include "../NetworkDrive/Clock.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "../Utility/IDType.h"
#include "../Utility/SCID.h"
#include "../Utility/RBID.h"
#include "../Utility/SBID.h"
#include "../MobileStation/MSID.h"
#include "BTSID.h"
#include "../NetworkDrive/BSManager.h"
#include "../ChannelModel/AntennaOrientGain.h"
#include "../ChannelModel/AOGOmni.h"
#include "../ChannelModel/AOGSector.h"
#include "../ChannelModel/AOGPico.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "BTSRxer.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "ACKNAKMessageUL.h"
#include "../Statistician/Statistics.h"
#include "../Statistician/Statistician.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../BaseStation/CodeBook.h"
#include "BTSTxer.h"
#include "BTSEBBRank1Txer.h"
#include "../MobileStation/MS.h"
#include "../MobileStation/CQIMessage.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../Scheduler/Mixed_DL_Scheduler.h"
#include "BTSMUEBBTxer.h"
#include "BTS.h"
#include"../MobileStation/SRSTxer.h"
#include "../Utility/Random.h"
//@threads
#include"../NetworkDrive/Thread_control.h"
#include <condition_variable>

#include "RIS/RIS.h"
//boost::mutex FTP_BTS_mutex;
extern int G_ICurDrop;
using namespace itpp;
std::mutex coutmutex;
std::mutex BTSmutex;

void BtsTaskFunc(void* arg)
{
    BTS* bts = (BTS*)arg;
    bts->runThreadTask();
}

BTS::BTS(const BSID& _bsid, const int& _iID)
{
    m_HARQTxStateBTS.setBTSID(_bsid.ToInt(),_iID);
    //初始化Tx内的参数
    iFTPPacketLostNum = 0;
    m_ID = BTSID(_bsid, _iID);
    if (BSManager::IsMacro(m_ID))
        m_dTxHeight = Parameters::Instance().Macro.DAntennaHeightM;
    else
        m_dTxHeight = Parameters::Instance().SmallCell.LINK.DPicoAntennaHeightM;

    //初始化天线参数
    m_iAntNum = Parameters::Instance().Macro.ITotalAntNum;
    if (BSManager::IsMacro(m_ID)) {
        m_dTxAntGainDB = Parameters::Instance().Macro.DAntennaGainDb;
        switch (Parameters::Instance().Macro.IAntennaPatternMode) {
            case Parameters::IAntennaPattern_OmniAntenna:
                m_pTxAOG = std::shared_ptr<cm::AntennaOrientGain > (new cm::AOGOmni());
                break;
            case Parameters::IAntennaPattern_SectorAntenna:
                m_pTxAOG = std::shared_ptr<cm::AntennaOrientGain > (
                        new cm::AOGSector(
                        Parameters::Instance().Macro.DH3DBBeamWidthDeg,
                        Parameters::Instance().Macro.DV3DBBeamWidthDeg,
                        Parameters::Instance().Macro.DHBackLossDB,
                        Parameters::Instance().Macro.DVBackLossDB,
                        Parameters::Instance().Macro.DHBackLossDB,
                        Parameters::Instance().Macro.DMechanicalTiltDeg));
                break;
            default:
                break;
        }
    } else {
        m_dTxAntGainDB = Parameters::Instance().SmallCell.LINK.DPicoAntennaGainDbi;
        m_pTxAOG = std::shared_ptr<cm::AntennaOrientGain > (new cm::AOGPico());
    }

    vSBUsedFlag.resize(Parameters::Instance().BASIC.ISBNum, vector<bool>(Parameters::Instance().SymbolNumPerSlot, false));
    vSBUsedFlagPerSlot.resize(Parameters::Instance().BASIC.ISlotNumPerSFN, vSBUsedFlag);

    //初始化下行调度器
    m_SchDL = std::shared_ptr<SchedulerDL > (new Mixed_DL_Scheduler(m_ID));

    m_iAntNum = Parameters::Instance().Macro.ITotalAntNum;
    if (BSManager::IsMacro(m_ID)) {
        m_dTxAntGainDB = Parameters::Instance().Macro.DAntennaGainDb;
        switch (Parameters::Instance().Macro.IAntennaPatternMode) {
            case Parameters::IAntennaPattern_OmniAntenna:
                //            case Parameters::IAntennaPattern_Sector_UEAntenna:
                m_pTxAOG = std::shared_ptr<cm::AntennaOrientGain > (new cm::AOGOmni());
                // m_pTxAOG = std::shared_ptr<cm::AntennaOrientGain > (new cm::AOGSector_UE());
                break;
            case Parameters::IAntennaPattern_SectorAntenna:
                m_pTxAOG = std::shared_ptr<cm::AntennaOrientGain > (
                        new cm::AOGSector(
                        Parameters::Instance().Macro.DH3DBBeamWidthDeg,
                        Parameters::Instance().Macro.DV3DBBeamWidthDeg,
                        Parameters::Instance().Macro.DHBackLossDB,
                        Parameters::Instance().Macro.DVBackLossDB,
                        Parameters::Instance().Macro.DHBackLossDB,
                        Parameters::Instance().Macro.DMechanicalTiltDeg));
                break;
            default:
                break;
        }
    } else {
        m_dTxAntGainDB = Parameters::Instance().SmallCell.LINK.DPicoAntennaGainDbi;
        m_pTxAOG = std::shared_ptr<cm::AntennaOrientGain > (new cm::AOGPico());
    }
    //初始化上行调度器
    UL.Construct(m_ID);
    //@threads
//    m_qSchedulingMessageQueue.clear();
    m_qACKNAKMesQueue.clear();
    connectmutex=std::shared_ptr<std::mutex>(new std::mutex);
}




BTS::~BTS() {
    m_MSID2HARQStateList.clear();
    m_MSID2HARQStateList_UL.clear();
}

///BTS初始化

void BTS::Initialize() {
    dci.initialize(m_ID);
    m_qSchedulingMessageQueue = dci.GetSchMesQueDL();
    //下行发射机初始化
    if (Parameters::Instance().BASIC.DWorkingMode == Parameters::WorkingMode_Normal) {
        //初始化BTSTXer
        switch (Parameters::Instance().MIMO_CTRL.IMIMOMode) {
            case Parameters::IMIMOMode_MUBF:
            case Parameters::IMIMOMode_MUBFRankA:
                m_pTxer = std::shared_ptr<BTSTxer > (new BTSMUEBBTxer(m_ID));
                break;
            default:
                assert(false);
        }

        m_pTxer->Initialize(m_qSchedulingMessageQueue, &m_TxBufferBTS, &m_ActiveSet);
    }
    for(auto &msid:m_ActiveSet){
        MSID2DRX[msid.ToInt()].initialize(msid);
        MSID2NewPDCCH[msid.ToInt()] = false;
        UpdateCCELevel(msid);
    }

    iTotalUsedSBNum = 0;
    iTotalSBNum = 0;
    //上行模块接收机初始化
    UL.Initialize();


}

///重置BTS

void BTS::Reset()
{
    m_ActiveSet.clear();
    m_MSID2HARQStateList.clear();
    m_MSID2HARQStateList_UL.clear();
    if (Parameters::Instance().BASIC.DWorkingMode
            == Parameters::WorkingMode_Normal) {

        //        m_RxBufferBTS.clear();
        m_TxBufferBTS.clear();
//        m_qSchedulingMessageQueue->clear();
        m_HARQTxStateBTS.Reset();
        m_qACKNAKMesQueue.clear();
        dci.Reset();
        m_pTxer->Reset();
    }

    //上行模块Reset
    UL.Reset();

    int iTime = Clock::Instance().GetTimeSlot();
    if (iTime >= 20) {
        Observer::Print("TotalRU") << "(" << (this->m_ID.GetBSID().ToInt()) << "," << (this->m_ID.GetIndex()) << ")" << setw(20) << iTotalSBNum << setw(20) << iTotalUsedSBNum << endl;
        if (BSManager::Instance().IsMacro(m_ID)) {
            Observer::Print("MacroRU") << "(" << (this->m_ID.GetBSID().ToInt()) << "," << (this->m_ID.GetIndex()) << ")" << setw(20) << iTotalSBNum << setw(20) << iTotalUsedSBNum << endl;
        } else {
            Observer::Print("SmallcellRU") << "(" << (this->m_ID.GetBSID().ToInt()) << "," << (this->m_ID.GetIndex()) << ")" << setw(20) << iTotalSBNum << setw(20) << iTotalUsedSBNum << endl;
        }
    }
    iTotalUsedSBNum = 0;
    iTotalSBNum = 0;
}

///BTS每个TTI要执行的内容

void BTS::WorkSlot() {
    if(!Parameters::Instance().CSI_data_set_on) {
        int iTime = Clock::Instance().GetTimeSlot();
        for (auto &drx: MSID2DRX) {
            drx.second.DRXstate_WorkSlot();
        }
        if( iTime == 1)
        {
            for(auto &msid:m_ActiveSet)
            {
                UpdateCCELevel(msid);
            }
        }
        ///天线端口计算，从BS::WorkSlot移到BTS::WorkSlot(有问题）
        //SRSMultiplexID();

//    cout<<Parameters::Instance().BASIC.BISMultiThread<<endl;
        //@threads
        if (DownOrUpLink(iTime) == 0 || DownOrUpLink(iTime) == 2) {
            WorkSlotDL();
        } else if (DownOrUpLink(iTime) == 1) {
            WorkSlotUL();
        }
        for (unordered_map<int, DRX>::iterator it = MSID2DRX.begin(); it != MSID2DRX.end(); it++) {
            it->second.InactivityTimer_WorkSlot(MSID2NewPDCCH[it->first]);
            MSID2NewPDCCH[it->first] = false;
            it->second.DRXRecord();
        }

        //    if (++m_iSwitch % 2 == 1) {
        //        if ((Parameters::Instance().BASIC.IDLORUL == Parameters::DL) && (DownOrUpLink(iTime) == 0 || DownOrUpLink(iTime) == 2))
        //            WorkSlotDL();
        //        else if ((Parameters::Instance().BASIC.IDLORUL == Parameters::UL) && (DownOrUpLink(iTime) == 1))
        //            WorkSlotUL();
        //    } else {
        //        for (int i = 0; i<static_cast<int> (m_ActiveSet.size()); ++i) {
        //            m_ActiveSet[i].GetMS().WorkSlot();
        //        }
        //    }
    }

}

//BTS每个下行TTI要执行的内容
std::mutex Scheduling_Record_lock;
void BTS::WorkSlotDL() {

    int iTimeInSF = Clock::Instance().GetTimeSlotInSF();
    int iTime = Clock::Instance().GetTimeSlot();

    CalcULCceRatio();

    ////上行调度//////
    if(Parameters::Instance().BASIC.IDLORUL != Parameters::DL)
    {
        UL.m_SchUL->Scheduling(m_ActiveSet, UL.m_HARQRxStateBTS, UL.m_RxBufferBTS, *UL.m_qSchedulingMessageQueueUL,
                               UL.vRBUsedFlagPerSlot, vSBUsedFlagPerSlot[iTimeInSF]);
    }
    ////////////////

    ///下行调度////
    if(Parameters::Instance().BASIC.IDLORUL != Parameters::UL)
    {
//        if (Parameters::Instance().BASIC.DWorkingMode == Parameters::WorkingMode_Normal) {
        if (iTime % Parameters::Instance().ERROR.ISRS_PERIOD == Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot+Parameters::Instance().ERROR.ISRS_DELAY)
        {
            m_pTxer->ReceiveSoundingMessage();
        }
        ReceiveCQIMessage();
        //        m_HARQTxStateBTS.WorkSlot();
        ProcessACKNAKMesQueue();
        ResetSBUsedFlag();
        m_SchDL->Scheduling(m_pTxer.get(), m_ActiveSet, m_HARQTxStateBTS, m_TxBufferBTS, *m_qSchedulingMessageQueue, vSBUsedFlagPerSlot[iTimeInSF]);
        m_pTxer->WorkSlot();
//        SendSchedulingMessage_afterk0();
        LogPacketErrorRate();

//        }
    }
    //////////////

    ///小区级指标新增
    int BTS2TxID = this->GetTxID();


    Statistics::Instance().m_BS_UL_PDCCHCCE_Rate_Use[BTS2TxID] += UlCceOccupiedNum;
    Statistics::Instance().m_BS_DL_PDCCHCCE_Rate_Use[BTS2TxID] += DlCceOccupiedNum;

    Statistics::Instance().m_BS_UL_PDCCHCCE[BTS2TxID] += UlCceOccupiedNum;
    Statistics::Instance().m_BS_DL_PDCCHCCE[BTS2TxID] += DlCceOccupiedNum;

    SetSBUsedFlag(vSBUsedFlagPerSlot[iTimeInSF]);
    dci.WorkSlot();
}

void BTS::LogPacketErrorRate() {
    int iTime = Clock::Instance().GetTimeSlot();
    if (iTime % Parameters::Instance().XR.iLogPeriod_slot == 1) {
        for (auto& it : m_TxBufferBTS) {
            it.second.LogPacketErrorRate();
        }
    }
}

//BTS每个上行TTI要执行的内容

void BTS::WorkSlotUL() {
    ///在此添加上行代码！
    UL.WorkSlotUL();
}

//BTS链接一个MS

bool BTS::ConnectMS(const MSID& _msid) {
    MS& ms = _msid.GetMS();
    cm::Point ms_wrap;
    if (BSManager::IsMacro(m_ID))
        ms_wrap = cm::WrapAround::Instance().WrapRx(ms, *this);
    else
        ms_wrap = cm::WrapAround::Instance().WrapRx(ms, BSManager::GetMacroIDofPico(m_ID).GetBTS());
    // 20171204
    assert(Parameters::Instance().BASIC.IMsDistributeMode
            != Parameters::DistributeMS_CenterRetangle);

    ///xlong:根据基站类型选择最小限制距离
    double dMinDist = (BSManager::IsMacro(m_ID)) ? Parameters::Instance().Macro.LINK.DMinDistanceM : Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM;
    double tolerance = 0.1;
    //assert(cm::Distance(*this, ms_wrap) >= dMinDist - tolerance);
    bool bret=false;
    if (static_cast<int> (m_ActiveSet.size()) >= Parameters::Instance().BASIC.IMaxServeMSNum) {
        bret = false;
    } else {
        connectmutex->lock();
        m_ActiveSet.push_back(_msid);
        m_TxBufferBTS[_msid.ToInt()] = MSTxBufferBTS(m_ID, _msid);

        assert(m_MSID2HARQStateList.count(_msid.ToInt()) == 0);
        m_MSID2HARQStateList[_msid.ToInt()].resize(Parameters::Instance().SIM.DL.IHARQProcessNum, false);

        assert(m_MSID2HARQStateList_UL.count(_msid.ToInt()) == 0);
        m_MSID2HARQStateList_UL[_msid.ToInt()].resize(Parameters::Instance().SIM_UL.UL.IHARQProcessNum, false);
        bret = true;
        connectmutex->unlock();
    }
    return bret;
}

//BTS断开与一个MS的链接

//接收ACKNAK信息

void BTS::ReceiveACKNAKMes(const std::shared_ptr<ACKNAKMessageDL>& _pACKNAKMes) {
    BTSmutex.lock();
    m_qACKNAKMesQueue.emplace_back(_pACKNAKMes);
    BTSmutex.unlock();
}

void BTS::ProcessACKNAKMesQueue() {

    auto  cur = m_qACKNAKMesQueue.begin();
    while (cur != m_qACKNAKMesQueue.end()) {
        int iTime = Clock::Instance().GetTimeSlot();
        std::shared_ptr<SchedulingMessageDL> pScheMsgDL = (*cur)->GetScheduleMes();
        if (pScheMsgDL->CanHARQProcess(iTime)) {
            ProcessACKNAKMes(*cur);
            cur = m_qACKNAKMesQueue.erase(cur);
        } else {
            cur++;
        }
    }
}

void BTS::ProcessACKNAKMes(const std::shared_ptr<ACKNAKMessageDL>& _pACKNAKMes) {

    bool bACKNAK = _pACKNAKMes->GetbACKNAK();
    //    std::shared_ptr<SchedulingMessageDL> pScheduleMes = dynamic_pointer_cast<SchedulingMessageDL> (_pACKNAKMes->GetScheduleMes());
    std::shared_ptr<SchedulingMessageDL> pScheduleMes = _pACKNAKMes->GetScheduleMes();
    auto msid= pScheduleMes->GetMSID();
    auto iHARQID=pScheduleMes->GetHARQID();
    int iSendNum = pScheduleMes->GetiSendNum();
    double dTBS = pScheduleMes->GetdTBSize();

//    m_TxBufferBTS[msid.ToInt()].OllaProcess(bACKNAK, iSendNum); //AMC外环

    if(bACKNAK){
        m_MSID2HARQStateList[msid.ToInt()][iHARQID] = false;
        auto& vpPacketAndSizeKbits = pScheduleMes->GetUniversal_PacketAndSizeKbits();
        double dCorrectKbit = 0;
        int iPacketNum = static_cast<int> (vpPacketAndSizeKbits[0].size());
        //assert(bACKNAK);
        //                if (bACKNAK) {
        for (int i = 0; i < iPacketNum; ++i) {
            vpPacketAndSizeKbits[0][i].first->RecordSuccessRecievedSizeKbits(
                    vpPacketAndSizeKbits[0][i].second, pScheduleMes->GetLatestTransTime());
            dCorrectKbit += vpPacketAndSizeKbits[0][i].second;
        }
        m_TxBufferBTS[msid.ToInt()].AccumulateSuccessRxKbit(dCorrectKbit);
        if(Clock::Instance().GetTimeSlot() >= Parameters::Instance().BASIC.IWarmUpSlot)
        {
            Statistician::Instance().m_MSData_DL[msid.ToInt()].DL.m_iCorrectBlockHit[0] += 1;
            Statistician::Instance().m_MSData_DL[msid.ToInt()].DL.m_iCorrectBlockHit[iSendNum] += 1;

            Statistician::Instance().m_MSData_DL[msid.ToInt()].DL.m_dAveRateKbps += dCorrectKbit;
            Statistician::Instance().m_BTSData[m_ID.ToInt()].DL.m_dThroughputKbps += dCorrectKbit;
        }

    }
    else
    {
        if (iSendNum < Parameters::Instance().SIM.DL.IHARQMaxTransNum) {
            pScheduleMes->IncreaseSendNum();
            m_HARQTxStateBTS.RegistReTxMessage(_pACKNAKMes);
        }
        else{
            m_MSID2HARQStateList[msid.ToInt()][iHARQID] = false;
        }

        if(Clock::Instance().GetTimeSlot() >= Parameters::Instance().BASIC.IWarmUpSlot)
        {
            Statistician::Instance().m_MSData_DL[msid.ToInt()].DL.m_iCorruptBlockHit[0] += 1;
            Statistician::Instance().m_MSData_DL[msid.ToInt()].DL.m_iCorruptBlockHit[iSendNum] += 1;
        }
    }
    m_TxBufferBTS[msid.ToInt()].PopFinishedPacket();

    if(iSendNum == 1)
    {
        Statistician::Instance().m_BTSData[m_iTxID].DL.m_iNewTxNum += 1;
        Statistician::Instance().m_BTSData[m_iTxID].DL.m_iNewTxErrorNum += bACKNAK ? 0:1;
    }

    ///小区级指标新增
    int BTS2TxID = this->GetTxID();

    int i_Statistics_MCS = pScheduleMes->GetiMCS();
    Statistics::Instance().m_BS_DL_MCS[BTS2TxID] += i_Statistics_MCS;
    Statistics::Instance().m_BS_DL_MCS_Times[BTS2TxID] += 1;
    ///平台采用MCS Index Table 2 PDSCH; MCS小于等于4时采用QPSK调制
    ///大话务支持MCS Index Table 1 PDSCH;MCS小于等于9时采用QPSK调制   Parameters::Instance().DaHuaWu.bDl256QamSwitch == false
    ///小区上行QPSK编码比例（上行QPSK调制的初始TB数和统计周期内传输的总上行初始TB数之比表示）
    int iMCS_Value;
    if(Parameters::Instance().DaHuaWu.bDl256QamSwitch == true) {
        iMCS_Value = 4;
    }else {
        iMCS_Value = 9;
    }
    Statistics::Instance().m_BS_DL_QPSK_Rate_Total[BTS2TxID] += 1;
    if(i_Statistics_MCS <= iMCS_Value){
        Statistics::Instance().m_BS_DL_QPSK_Rate_Used[BTS2TxID] += 1;
    }
    if (bACKNAK) {
        ///小区上行业务量
        Statistics::Instance().m_BS_DL_Traffic[BTS2TxID] += dTBS;
    } else {
        ///上行HARQ重传比例（UL-SCH上发生HARQ重传的TB数量与UL-SCH上传输的总的TB数量之比表示）
        Statistics::Instance().m_BS_DL_HARQ_Used[BTS2TxID] += 1;
    }
    Statistics::Instance().m_BS_DL_HARQ_Total[BTS2TxID] += 1;

}

//获得当前类型[字符串]

//接收MS反馈的CQI信息
void BTS::PushCQIMessage(const MSID& _msid, const std::shared_ptr<CQIMessage>& _pCQIMessage) {
    BTSmutex.lock();
    m_TxBufferBTS[_msid.ToInt()].PushCQIMessage(_pCQIMessage);
    BTSmutex.unlock();
}

void BTS::ReceiveCQIMessage() {
    for (int imsindex = 0; imsindex < GetConnectedNum(); ++imsindex) {
        m_TxBufferBTS[m_ActiveSet[imsindex].ToInt()].ReceiveCQIMessage();
    }
}

//获得发送端公共参考信号

cmat BTS::GetTxCRS(int _iPortNum) {
    return m_pTxer->GetTxCRS(_iPortNum);
}

//获得发送符号

cmat BTS::GetTxDataSymbol(const SCID& _scid) {
    return m_pTxer->GetTxDataSymbol(_scid);
}

//获得发送预编码码字

cmat BTS::GetPrecodeCodeWord(const SBID& _sbid) {
    return m_pTxer->GetPrecodeCodeWord(_sbid);
}

vector<MSID> BTS::GetSchedvMSID(const SBID& _sbid) {
    return m_pTxer->GetSchedvMSID(_sbid);
}
//

void BTS::SendSchedulingMessage() {
//    iTotalSBNum += Parameters::Instance().BASIC.ISBNum;
    while (!m_qSchedulingMessageQueue->empty()) {
        std::shared_ptr<SchedulingMessageDL> pSM = m_qSchedulingMessageQueue->front();
        vector<SBID> vSBUsed = pSM->GetSBUsed();
        for (auto& sb:vSBUsed) {
//                if (!GetSBUsedFlag(sb)) {
//                    SetSBUsedFlag(sb);
//                    iTotalUsedSBNum++;
//                }
            if (GetSBUsedFlag(sb)) {
                iTotalUsedSBNum++;   //这里如果是MU的话可能会加多次
            }
        }
        MS& ms = pSM->GetMSID().GetMS();
        if(pSM->GetiSendNum()==1){
            MSID2NewPDCCH[pSM->GetMSID().ToInt()] = true;
            assert(MSID2DRX[pSM->GetMSID().ToInt()].GetDRXState() == state_active);
        }
        ms.ReceiveSchedulingMessage(pSM);
        m_qSchedulingMessageQueue->pop_front();
    }
}

void BTS::SendSchedulingMessage_afterk0() {
    //    std::shared_ptr<SchedulingMessageDL> pSM = m_qSchedulingMessageQueue.front();
    //    int iSchedulingTime = pSM->GetBornTime();
    //    if (iTime >= iSchedulingTime + Parameters::Instance().SIM.DL.Ik0_slot) {
    SendSchedulingMessage();
    //    }
}

BTSID BTS::GetID() {
    return m_ID;
}

int BTS::GetConnectedNum() {
    return static_cast<int> (m_ActiveSet.size());
}

void BTS::PushSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage) {
    m_pTxer->PushSoundingMessage(_pSoundingMessage);
}

void BTS::SetApSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage) {
    m_pTxer->SetApSoundingMessage(_pSoundingMessage);
}

void BTS::ReceiveApSoundingMessage() {
    m_pTxer->ReceiveApSoundingMessage();
}

vector<int> BTS::GetUE2vBSBeam(const MSID& _msid) {
    return m_pTxer->m_MSID2PanelAndvBeamIndex[_msid.ToInt()].second;
}

int BTS::GetUE2PanelIndicator(const MSID& _msid) {
    return m_pTxer->m_MSID2PanelAndvBeamIndex[_msid.ToInt()].first;
}

std::unordered_map<int, pair<int, vector<int> > > BTS::GetMap_MSID2UEPanelAndvBSBeam() {
    return m_pTxer->m_MSID2PanelAndvBeamIndex;
}

vector< pair<int, vector<int> > > BTS::GetvUEPanelAndvBSBeam() {
    if (!m_pTxer->m_MSID2PanelAndvBeamIndex.empty()) {
        vector< std::pair<int, vector<int> > > _vUEPanelAndVBSBeam;
        for (auto it : m_pTxer->m_MSID2PanelAndvBeamIndex) {
            if (find(_vUEPanelAndVBSBeam.begin(), _vUEPanelAndVBSBeam.end(), it.second) == _vUEPanelAndVBSBeam.end()) {
                _vUEPanelAndVBSBeam.push_back(it.second);
            }
        }
        m_pTxer->m_vUEPanelAndVBSBeam = _vUEPanelAndVBSBeam;
    }
    return m_pTxer->m_vUEPanelAndVBSBeam;
}

vector<MSID>& BTS::GetActiveSet() {
    return m_ActiveSet;
}

HARQTxStateBTS& BTS::GetBTSHARQTxState() {
    return m_HARQTxStateBTS;
}



///构造预编码矩阵以及发送信息


std::unordered_map<int, MSTxBufferBTS>& BTS::GetMSTxBufferBTS() {
    return m_TxBufferBTS;
}

void BTS::ReceivePacketDL(const MSID& _msid, const std::shared_ptr<Packet>& _pPacket) {
    m_TxBufferBTS[_msid.ToInt()].PushPacketDL(_pPacket);
}
void BTS::ReceivePacketDL(const MSID& _msid, const std::shared_ptr<Universal_Packet>& _pPacket) {
    m_TxBufferBTS[_msid.ToInt()].PushPacketDL(_pPacket);
}

bool BTS::IsExistTraffic() {
    if (m_ActiveSet.empty()) {
        return false;
    } else
        return true;
}

void BTS::ResetSBUsedFlag() {
    vSBUsedFlag.clear();
//    for (int i = 0; i < Parameters::Instance().BASIC.ISBNum; ++i) {
//        vSBUsedFlag.push_:back(false);
//    }
    vSBUsedFlag = vector<vector<bool>>(Parameters::Instance().BASIC.ISBNum, vector<bool>(Parameters::Instance().SymbolNumPerSlot, false));

}

void BTS::SetSBUsedFlag(const SBID& _sbid) {
    for(int i=0;i<Parameters::Instance().SymbolNumPerSlot;i++)
    vSBUsedFlag[_sbid.ToInt()][i] = true;
}
void BTS::SetSBUsedFlag(vector<vector<bool>>& SBUsedMap) {
    vSBUsedFlag = SBUsedMap;
    SBUsedMap = vector<vector<bool>>(Parameters::Instance().BASIC.ISBNum, vector<bool>(Parameters::Instance().SymbolNumPerSlot, false));
}

bool BTS::GetSBUsedFlag(const SBID& _sbid) {
    return vSBUsedFlag[_sbid.ToInt()][Parameters::Instance().CoresetDuration];
}

int BTS::GetFTPPacketLostNum() const{
    return iFTPPacketLostNum;
}

void BTS::SetFTPPacketLostNum(int _iNum) {
    iFTPPacketLostNum = _iNum;
}

std::unordered_map<int, vector<bool> >& BTS::GetMSID2HARQStateList() {
    return m_MSID2HARQStateList;
}

std::unordered_map<int, vector<bool> >& BTS::GetMSID2HARQStateList_UL() {
    return m_MSID2HARQStateList_UL;
}

void BTS::ReceiveSoundingMessage() {

    m_pTxer->ReceiveSoundingMessage();
}


void BTS::SRSMultiplexID(){
    /*
     * 目前还有问题，SRS天线端口号的在实际仿真中应该如何确定？对m_iMultiplexID的值需要进一步研究。
     */
    if (Parameters::Instance().ERROR.ISRS_Error == 1) {
        int iCrashNum = SRSTxer::GetCrashNum();
        int iTime = Clock::Instance().GetTimeSlot();
        assert(Parameters::Instance().ERROR.ISRS_PERIOD % Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot == 0);
        int ISRS_OFFSET = Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot;
        if (iTime % Parameters::Instance().ERROR.ISRS_PERIOD == ISRS_OFFSET) {
            m_vMultiplexID.clear();
            int iFoundRandNum = 0;
            const int MaxNum = 3;
            while (iFoundRandNum < MaxNum) {
                int iRandom = random.xUniformInt(0, iCrashNum - 1);
                if (find(m_vMultiplexID.begin(), m_vMultiplexID.end(), iRandom) == m_vMultiplexID.end()) {
                    m_vMultiplexID.push_back(iRandom);
                    iFoundRandNum++;
                }
            }

            for (int i = 0; i < Parameters::Instance().BASIC.IBTSPerBS; ++i) {
                this->m_iMultiplexID = m_vMultiplexID[i];
                //cout<<m_iMultiplexID;
            }
        }
    }
}

void BTS::UpdateCCELevel(MSID& msid)
{
    int CceLevel;
    if(Parameters::Instance().DaHuaWu.bPdcchAlgoEnhSwitch == true)
    {
        if(Parameters::Instance().DaHuaWu.bPdcchAggLvlAdaptPol == true)
        {
            CceLevel = PL2CCELevel(msid);
        }
        else
        {
            if(G_ICurDrop == 1 || MSID2CCELevel.find(msid.ToInt()) == MSID2CCELevel.end())
            {
                CceLevel = PL2CCELevel(msid);
            }
            else
            {
                assert(MSID2CCELevel[msid.ToInt()] != 0);
                CceLevel = MSID2CCELevel[msid.ToInt()];
            }

        }
    }
    else
    {
        if(Parameters::Instance().DaHuaWu.bPdcchAggLvlAdaptPol == true)
        {
            if(G_ICurDrop == 1)
            {
                CceLevel = 4;
            }
            else
            {
                CceLevel = PL2CCELevel(msid);
            }
        }
        else
        {
            CceLevel = 4;
        }
    }

    MSID2CCELevel[msid.ToInt()] = CceLevel;
}
int BTS::PL2CCELevel(MSID& msid)
{
    double PL = cm::LinkMatrix::Instance().GetCouplingLossDB(m_ID.GetBTS(), msid.GetMS());
    int CceLevel;
    if(PL >= -80)
    {
        CceLevel = 1;
    }
    else if( PL >= -90)
    {
        CceLevel = 2;
    }
    else if( PL >= -100)
    {
        CceLevel = 4;
    }
    else if( PL >= -110)
    {
        CceLevel = 8;
    }
    else
    {
        CceLevel = 16;
    }
    return CceLevel;
}
void BTS::CalcULCceRatio()
{
    ///上行调度使用的CCE数,过了调度读取这个值
    UlCceOccupiedNum = 0;
    DlCceOccupiedNum = 0;
    double UlCceNeedNum = 0, DlCceNeedNum = 0;
    int coresetRbLen = Parameters::Instance().BASIC.IRBNum/6*6;
    int regMaxNum = coresetRbLen * Parameters::Instance().CoresetDuration;
    ///CCE总数
    int CceMaxNum = regMaxNum / 6;

    if(Parameters::Instance().BASIC.IDLORUL != Parameters::ULandDL)
    {
        ///上行可用的PDCCH的CCE总数
        UlCceMaxNum = CceMaxNum;
        return;
    }

    switch(Parameters::Instance().DaHuaWu.HeavyLoadUlCceAdjPolicy)
    {
        case DaHuaWu_Para::RATIO_FIXED:
            ///上行可用的PDCCH的CCE总数
            UlCceMaxNum = CceMaxNum;
            break;
        case DaHuaWu_Para::RATIO_OPT:
            for(auto& msid:m_ActiveSet)
            {
                if(UL.MSID2SRflag[msid.ToInt()] > 0 || m_clsMac.HaveAnyData(msid.ToInt()) == true)
                {
                    UlCceNeedNum += MSID2CCELevel[msid.ToInt()];
                }
                if(m_TxBufferBTS[msid.ToInt()].HaveAnyData())
                {
                    DlCceNeedNum += MSID2CCELevel[msid.ToInt()];
                }
            }
            ///上行可用的PDCCH的CCE总数
            UlCceMaxNum = CceMaxNum * UlCceNeedNum / (UlCceNeedNum + DlCceNeedNum);
            break;
        default:
            UlCceMaxNum = CceMaxNum * Parameters::Instance().DaHuaWu.HeavyLoadUlCceAdjPolicy / 100;
    }

    ///小区级指标新增
    int BTS2TxID = this->GetTxID();
    Statistics::Instance().m_BS_UL_PDCCHCCE_Rate_Total[BTS2TxID] += UlCceMaxNum;
    Statistics::Instance().m_BS_DL_PDCCHCCE_Rate_Total[BTS2TxID] += CceMaxNum;

}
void BTS::runThreadTask()
{
    m_Thread_control = &Thread_control::Instance();

    WorkSlot();

    return;
}

//20260115
void BTS::AddRISs() {
    m_vpRIS.clear();
    for (int i = 0; i < Parameters::Instance().BASIC.IRISPerBTS; ++i)
        m_vpRIS.push_back(std::shared_ptr<RIS>(new RIS(m_ID, i)));
}

//20251107
RIS& BTS::GetRIS(int _iID) {
    return *m_vpRIS[_iID];
}

int BTS::GetRISNum() {
    return m_vpRIS.size();
}