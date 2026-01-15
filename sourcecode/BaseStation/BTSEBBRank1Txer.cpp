///@file BTSEBBRankOneTXer.cpp
///@brief  BTS EBBRankOne发射机类函数实现
///@author wangxiaozhou

#include "../Statistician/Observer.h"
#include "../NetworkDrive/Clock.h"
#include "../Utility/IDType.h"
#include "../Utility/RBID.h"
#include "../Utility/SBID.h"
#include "../Utility/SCID.h"
#include "BTSID.h"
#include "../MobileStation/MSID.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../ChannelModel/AntennaOrientGain.h"
#include "../ChannelModel/Rx.h"
#include "../ChannelModel/WrapAround.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/BS.h"
#include "../BaseStation/BTS.h"
#include "../NetworkDrive/BSManager.h"
#include "../Statistician/Statistician.h"
#include "../MobileStation/MS.h"
#include "../MobileStation/SoundingMessage.h"
#include "../BaseStation/CodeBook.h"
#include "BTSTxer.h"

#include "BTSEBBRank1Txer.h"

BTSEBBRank1Txer::BTSEBBRank1Txer(const BTSID& _btsid)
: BTSTxer(_btsid),
  m_mSBID2CodeWord(Parameters::Instance().BASIC.ISBNum),
  m_mSBID2vMSID(Parameters::Instance().BASIC.ISBNum)
{
    for (SBID sbid = SBID::Begin(); sbid <= SBID::End(); ++sbid) {
        m_mSBID2CodeWord[sbid.ToInt()] = sqrt(1.0 / Parameters::Instance().Macro.ITotalAntNum) * itpp::ones_c(Parameters::Instance().Macro.ITotalAntNum, 1);
    }
    if (m_MSID2PanelAndvBeamIndex.empty()) {
        vector<int> ls;
        int iRandomIndex = 0; //xUniformInt(0,Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHBSBeamNum*Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iVBSBeamNum-1);
        ls.push_back(iRandomIndex);
        m_vUEPanelAndVBSBeam.emplace_back(make_pair(2, ls));
    }
}

/// @brief 取子载波上发送的数据符号
cmat BTSEBBRank1Txer::GetTxDataSymbol(SCID _scid) {
    int iRank = m_mSBID2CodeWord[_scid.GetSBID().ToInt()].cols();
    ///@xlong   根据m_BTSID.m_iBTSID（BTS的id）确定是BTS还是Pico，得到DSCTxPowerMw
    double dTxPower;
    if (BSManager::IsMacro(m_BTSID)) {
        dTxPower = Parameters::Instance().Macro.DL.DSCTxPowerMw;
    } else {
        dTxPower = Parameters::Instance().SmallCell.LINK.DPicoOutSCTxPowerMw;
    }
    cmat mResult = sqrt(dTxPower / iRank) * itpp::ones_c(iRank, 1);
    return mResult;
}
/// @brief 取发送的参考符号

cmat BTSEBBRank1Txer::GetTxCRS(int _iPortNum) {
    ///@xlong   根据m_BTSID.m_iBTSID（BTS的id）确定是BTS还是Pico，得到DSCTxPowerMw
    double dTxPower;
    if (BSManager::IsMacro(m_BTSID)) {
        dTxPower = Parameters::Instance().Macro.DL.DSCTxPowerMw;
    } else {
        dTxPower = Parameters::Instance().SmallCell.LINK.DPicoOutSCTxPowerMw;
    }
    return sqrt(dTxPower / _iPortNum) * itpp::ones_c(_iPortNum, 1);
}

/// @brief 取SB上的预编码码字

cmat BTSEBBRank1Txer::GetPrecodeCodeWord(SBID _sbid) {
    return m_mSBID2CodeWord[_sbid.ToInt()];
}

/// @brief 获得对应RB上的被调度的MSID数组

vector<MSID> BTSEBBRank1Txer::GetSchedvMSID(SBID _sbid) {
    return m_mSBID2vMSID[_sbid.ToInt()];
}
/// @brief 时间驱动接口，完成一个TTI的操作

void BTSEBBRank1Txer::WorkSlot() {
    assert(false);
    // 接收Sounding信息
    //    ReceiveSoundingMessage();
    //    // 为每个RB设置波束赋形向量
    //    for (deque<std::shared_ptr<SchedulingMessageDL> >::iterator it = m_pScheduleMessageQueue->begin(); it != m_pScheduleMessageQueue->end(); ++it) {
    //        MSID msid = (*it)->GetMSID();
    //        vector<SBID> vSBUsed = (*it)->GetSBUsed();
    //        for (int i = 0; i < static_cast<int> (vSBUsed.size()); ++i) {
    //            SBID sbid = vSBUsed[i];
    //            cmat mCodeWord = (*it)->GetCodeWord(sbid);
    //            m_mSBID2CodeWord[sbid] = mCodeWord;
    //            m_mSBID2vMSID[sbid].push_back(msid);
    //        }
    //    }
}

/// @brief 重置操作

void BTSEBBRank1Txer::Reset() {
    m_pScheduleMessageQueue->clear();
    m_mSBID2CodeWord.clear();
    m_mSBID2vMSID.clear();
    m_qSoundingMessage.clear();
}

/// @brief 压入Sounding消息

void BTSEBBRank1Txer::PushSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage) {
    m_qSoundingMessage.push_back(_pSoundingMessage);
}

/// @brief SetApSounding消息

void BTSEBBRank1Txer::SetApSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage) {
    m_pApSoundingMessage = _pSoundingMessage;
}

/// @brief 接收Sounding消息

void BTSEBBRank1Txer::ReceiveSoundingMessage() {
    while (!m_qSoundingMessage.empty() ) {
        if(Clock::Instance().GetTimeSlot() - m_qSoundingMessage.front()->GetBornTime() < Parameters::Instance().ERROR.ISRS_DELAY){
            break;
        }
        std::shared_ptr<SoundingMessage> pSRS = m_qSoundingMessage.front();
        MSID msid = pSRS->GetMSID();
        MSTxBufferBTS& buffer = (*m_pTxBufferBTS)[msid.ToInt()];
        for (RBID rbid = RBID::Begin(); rbid <= RBID::End(); ++rbid) {
            int iAntID = pSRS->GetAntennaID();
            cmat mH = pSRS->GetH(rbid);
            if (iAntID == 2) {
                buffer.SetH(rbid, mH);
            } else {
                buffer.SetH(rbid, iAntID, mH);
            }

            assert(mH.cols() ==
                    m_BTSID.GetBTS().GetAntennaPointer()->GetTotalTXRU_Num());

        }

        buffer.ComputeCovRInfo();
        m_qSoundingMessage.pop_front();
    }
}

/// @brief 接收Sounding消息

void BTSEBBRank1Txer::ReceiveApSoundingMessage() {
    std::shared_ptr<SoundingMessage> pApSRS = m_pApSoundingMessage;
    MSID msid = pApSRS->GetMSID();
    MSTxBufferBTS& buffer = (*m_pTxBufferBTS)[msid.ToInt()];
    for (RBID rbid = RBID::Begin(); rbid <= RBID::End(); ++rbid) {
        int iAntID = pApSRS->GetAntennaID();
        cmat mH = pApSRS->GetH(rbid);
        if (iAntID == 2) {
            buffer.SetH(rbid, mH);
        } else {
            buffer.SetH(rbid, iAntID, mH);
        }
    }
    buffer.ComputeCovRInfo();
}


///@brief BTS EBB发射机初始化
void BTSEBBRank1Txer::Initialize(SafeDeque<std::shared_ptr<SchedulingMessageDL> > *_q, std::unordered_map<int, MSTxBufferBTS>* _pTxBufferBTS, vector<MSID>* _pActiveSet) {
    //调用基类的初始化函数
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    BTSTxer::Initialize(_q, _pTxBufferBTS, _pActiveSet);

    for (auto& msid:(*m_pActiveSet)) {
        //更新信道
        //MSID msid = (*m_pActiveSet)[i];
        MS& ms = msid.GetMS();
        BTS& mainbts = ms.GetMainServBTS().GetBTS();
        for (RBID rbid = RBID::Begin(); rbid <= RBID::End(); ++rbid) {
            itpp::cmat mHe = lm.
                    GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                    m_BTSID.GetBTS(),
                    msid.GetMS(),
                    rbid.GetFirstSCID().ToInt()
                    );
            assert(mHe.cols() ==
                    m_BTSID.GetBTS().GetAntennaPointer()->GetTotalTXRU_Num());

            (*_pTxBufferBTS)[msid.ToInt()].SetH(rbid, mHe);

        }
        if(Parameters::Instance().BASIC.IDLORUL != Parameters::UL)
        {
            (*_pTxBufferBTS)[msid.ToInt()].ComputeCovRInfo();
        }
    }
}

/// @brief 产生调度消息

std::shared_ptr<SchedulingMessageDL> BTSEBBRank1Txer::GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, int _mMCS, const vector<SBID>& _vSBUsed) {
    std::shared_ptr<SchedulingMessageDL> pScheduleMes(new SchedulingMessageDL(_msid, _iHARQID, _iRank));
    pScheduleMes->SetMCS(_mMCS);
    pScheduleMes->SetSBUsed(_vSBUsed);
    for (auto& sbid : _vSBUsed) {
        cmat mTemp, mPrecode;
        if (Parameters::Instance().MIMO_CTRL.IHorCovR) {
            mTemp = (*m_pTxBufferBTS)[_msid.ToInt()].GetCovR(sbid);
        } else {
            mTemp = (*m_pTxBufferBTS)[_msid.ToInt()].GetH(sbid);
        }
        cmat mU, mD;
        vec vS;
        svd(mTemp, mU, vS, mD);
        mPrecode = mD.get_cols(0, _iRank);
        pScheduleMes->SetCodeWord(sbid, mPrecode);
    }
    return pScheduleMes;
}

std::shared_ptr<SchedulingMessageDL> BTSEBBRank1Txer::GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, int _mMCS, const vector<SBID>& _vSBUsed, const vector<vector<pair<std::shared_ptr<Packet>, double> > >& _vpPacketAndSizeKbits) {
    std::shared_ptr<SchedulingMessageDL> pScheduleMes=std::make_shared<SchedulingMessageDL>(_msid, _iHARQID, _iRank, _vpPacketAndSizeKbits);
    pScheduleMes->SetMCS(_mMCS);
    pScheduleMes->SetSBUsed(_vSBUsed);
    for (auto& sbid : _vSBUsed) {
        cmat mTemp, mPrecode;
        if (Parameters::Instance().MIMO_CTRL.IHorCovR) {
            mTemp = (*m_pTxBufferBTS)[_msid.ToInt()].GetCovR(sbid);
        } else {
            mTemp = (*m_pTxBufferBTS)[_msid.ToInt()].GetH(sbid);
        }
        cmat mU, mD;
        vec vS;
        svd(mTemp, mU, vS, mD);
        mPrecode = mD.get_cols(0, _iRank);
        pScheduleMes->SetCodeWord(sbid, mPrecode);
    }
    return pScheduleMes;
}