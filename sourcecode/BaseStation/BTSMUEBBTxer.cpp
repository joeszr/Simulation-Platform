#include "../Utility/SBID.h"
#include "../Utility/SCID.h"
#include "../Utility/RBID.h"
#include "../MobileStation/MSID.h"
#include "../Scheduler/SchedulingMessageDL.h"
#include "../MobileStation/CQIMessage.h"
#include "../BaseStation/BTSID.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../BaseStation/CodeBook.h"
#include "../Scheduler/CompetorRankA.h"
#include "../BaseStation/BTSTxer.h"
#include "../NetworkDrive/Clock.h"
#include "../MobileStation/SoundingMessage.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/BTS.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../BaseStation/BTSEBBRank1Txer.h"
#include "BTSMUEBBTxer.h"
#include <itpp/stat/misc_stat.h>

BTSMUEBBTxer::BTSMUEBBTxer(const BTSID& _btsid) : BTSEBBRank1Txer(_btsid) {
}


/// @brief 时间驱动接口，完成一个TTI的操作

void BTSMUEBBTxer::WorkSlot() {
    // 接收Sounding信息
    //    ReceiveSoundingMessage();

    m_mSBID2vMSID.clear();
    std::vector<SBID> vUsedSBID;
    deque<std::shared_ptr<SchedulingMessageDL> >::iterator it;
    for (it = m_pScheduleMessageQueue->begin(); it != m_pScheduleMessageQueue->end(); ++it) {
        //根据调度结果对SchedulingMessage做进一步处理，并设置BTSTxer的信息
        MSID msid = (*it)->GetMSID(); //被调度的移动台ID
        vector<SBID> vSBUsed = (*it)->GetSBUsed(); //被分配给移动台的RB
        for (auto& sbid: vSBUsed) {
            if (find(vUsedSBID.begin(), vUsedSBID.end(), sbid) == vUsedSBID.end()) {
                m_mSBID2CodeWord[sbid.ToInt()] = (*it)->GetCodeWord(sbid);
                vUsedSBID.push_back(sbid);
            } else {
                for (int j = 0; j < (*it)->GetCodeWord(sbid).cols(); ++j) {
                    m_mSBID2CodeWord[sbid.ToInt()].append_col((*it)->GetCodeWord(sbid).get_col(j));
                }
            }
            m_mSBID2vMSID[sbid.ToInt()].push_back(msid);
        }
    }
}

//std::shared_ptr<SchedulingMessageDL> BTSMUEBBTxer::GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, vector<SBID> _vSBUsed, std::map<SBID, Competor> _mSBID2Competor) {
//    assert(false);
//    //    std::shared_ptr<SchedulingMessageDL> pSchedulingMes(new SchedulingMessageDL(_msid, _iHARQID, _iRank));
//    //    pSchedulingMes->SetSBUsed(_vSBUsed); //设置使用的SB集合
//    //    vector<double> vSINR;
//    //    for (int i = 0; i<static_cast<int> (_vSBUsed.size()); ++i) {
//    //        SBID sbid = _vSBUsed[i];
//    //        Competor& c = _mSBID2Competor[sbid]; //在本RB上被选中的Competor
//    //        vector<MSID>::iterator it = find(c.begin(), c.end(), _msid);
//    //        assert(it != c.end());
//    //        int iCol = it - c.begin();
//    //        itpp::cmat mCodeWord = c.m_mSBID2CodeWord[sbid].get_cols(iCol, iCol);
//    //        pSchedulingMes->SetCodeWord(sbid, mCodeWord); //设置每个RB上使用的码字
//    //        double dSINR = c.m_mMSIDSBID2SINR[make_pair(_msid, sbid)];
//    //        vSINR.insert(vSINR.end(), Parameters::Instance().BASIC.ISBSize * Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace, dSINR);
//    //    }
//    //
//    //    int iFinalMCS = LinkLevelInterface::Instance_DL().SINR2MCS(vSINR);
//    //    pSchedulingMes->SetMCS(iFinalMCS * itpp::ones_i(1));
//    //    return pSchedulingMes;
//}

//20180404 未使用

//std::shared_ptr<SchedulingMessageDL> BTSMUEBBTxer::GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed, std::map<SBID, Competor> _mSBID2Competor, vector<vector<pair<std::shared_ptr<Packet>, double> > > _vpPacketAndSizeKbits) {
//    assert(false);
//    //    std::shared_ptr<SchedulingMessageDL> pSchedulingMes(new SchedulingMessageDL(_msid, _iHARQID, _iRank, _vpPacketAndSizeKbits));
//    //    pSchedulingMes->SetSBUsed(_vSBUsed); //设置使用的SB集合
//    //    vector<double> vSINR;
//    //    for (int i = 0; i<static_cast<int> (_vSBUsed.size()); ++i) {
//    //        SBID sbid = _vSBUsed[i];
//    //        Competor& c = _mSBID2Competor[sbid]; //在本RB上被选中的Competor
//    //        vector<MSID>::iterator it = find(c.begin(), c.end(), _msid);
//    //        assert(it != c.end());
//    //        int iCol = it - c.begin();
//    //        itpp::cmat mCodeWord = c.m_mSBID2CodeWord[sbid].get_cols(iCol, iCol);
//    //        pSchedulingMes->SetCodeWord(sbid, mCodeWord); //设置每个RB上使用的码字
//    //        double dSINR = c.m_mMSIDSBID2SINR[make_pair(_msid, sbid)];
//    //        vSINR.insert(vSINR.end(), Parameters::Instance().BASIC.ISBSize * Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace, dSINR);
//    //    }
//    //
//    //    int iFinalMCS = LinkLevelInterface::Instance_DL().SINR2MCS(vSINR);
//    //    if (iFinalMCS != mMCS(0, 0)) cout << "BTSMUEBBTxer::GenerateSchedulingMessageDL Error!" << endl;
//    //    pSchedulingMes->SetMCS(iFinalMCS * itpp::ones_i(1));
//    //    return pSchedulingMes;
//}
//use
std::shared_ptr<SchedulingMessageDL> BTSMUEBBTxer::GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, int mMCS, const vector<SBID>& _vSBUsed, std::unordered_map<int, CompetorRankA> _mSBID2Competor, const vector<vector<pair<std::shared_ptr<Packet>, double> > >& _vpPacketAndSizeKbits) {

    std::shared_ptr<SchedulingMessageDL> pSchedulingMes(new SchedulingMessageDL(_msid, _iHARQID, _iRank, _vpPacketAndSizeKbits));
    pSchedulingMes->SetSBUsed(_vSBUsed); //设置使用的SB集合
    vector<double> vSINR;
    for (auto& sbid:_vSBUsed) {
        CompetorRankA& c = _mSBID2Competor[sbid.ToInt()]; //在本RB上被选中的Competor
        auto it = find(c.begin(), c.end(), _msid);
        assert(it != c.end());
        int iCol = it - c.begin();
        int iPrecodeStartIndex = 0;
        int iPrecodeEndIndex = 0;
        for (int j = 0; j < iCol; ++j) {
            MSID msid = c[j];
            iPrecodeStartIndex += (c.m_mMSID2Rank[msid.ToInt()] + 1);
        }
        MSID msid = c[iCol];
        iPrecodeEndIndex = iPrecodeStartIndex + c.m_mMSID2Rank[msid.ToInt()];
        itpp::cmat mCodeWord = c.m_CodeWord.get_cols(iPrecodeStartIndex, iPrecodeEndIndex);
        pSchedulingMes->SetCodeWord(sbid, mCodeWord); //设置每个RB上使用的码字

        int iLayerNum = 0;
        for(auto& ms : c){
            iLayerNum += (c.GetRank(ms)+1);
        }
        pSchedulingMes->SetLayerNum(sbid, iLayerNum);

        cmat mPA = c.m_CodeWord;
        int iRank = mCodeWord.cols();
        int iRankA = mPA.cols();
        vector<int> vRankID(iRank, -1);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            std::vector<int> vToBePastedRanks(iRankIndex,0);
            for (int k1 = 0; k1 < iRankIndex; k1++) {
                vToBePastedRanks[k1]=k1;
            }

            for (int icol = 0; icol < iRankA; ++icol) {
                if (!vToBePastedRanks.empty()) {
                    int k2 = 0;
                    for (; k2 <= static_cast<int>(vToBePastedRanks.size() - 1); k2++) {
                        if (icol == vToBePastedRanks[k2]) {
                            break;
                        }
                    }
                    if (k2 < vToBePastedRanks.size()) {
                        vToBePastedRanks.erase(vToBePastedRanks.begin() + k2);
                        continue;
                    }
                }
                const double tolent = 0.001;
                if (itpp::norm(mCodeWord.get_cols(iRankIndex, iRankIndex)
                        - mPA.get_cols(icol, icol)) < tolent) {
                    vRankID[iRankIndex] = icol;
                    break;
                }
            }
            assert(vRankID[iRankIndex] != -1);
            if (vRankID[iRankIndex] == -1) {
                cerr << "Can't find the correct rank id!";
                exit(0);
            }
        }


        vector<double> dSINR = c.m_vMSID2SINR[_msid.ToInt()];


        vSINR.insert(vSINR.end(), dSINR.begin(),dSINR.end());
    }

//    int mFinalMCS = LinkLevelInterface::Instance_DL().SINR2MCS2(vSINR,1);
    int mFinalMCS = LinkLevelInterface::Instance_DL().SINR2MCS_simplified(vSINR);
    if (mFinalMCS != mMCS) cout << "BTSMUEBBTxer::GenerateSchedulingMessageDL Error!" << endl;
    pSchedulingMes->SetMCS(mFinalMCS);
    pSchedulingMes->SetTBSizeKbit_TDD();
    double sinr = 0.0;
    for(int i=0;i<vSINR.size();i++){
        sinr +=vSINR[i];
    }
    sinr /= vSINR.size();
    pSchedulingMes->SetSINR(sinr);


    return pSchedulingMes;
}

std::shared_ptr<SchedulingMessageDL> BTSMUEBBTxer::GenerateSchedulingMessageDL(
        const MSID& _msid, int _iHARQID, int _iRank, int iMCS, const vector<SBID>& _vSBUsed, std::unordered_map<int, CompetorRankA> _mSBID2Competor) {
    std::shared_ptr<SchedulingMessageDL> pSchedulingMes(new SchedulingMessageDL(_msid, _iHARQID, _iRank));
    pSchedulingMes->SetSBUsed(_vSBUsed); //设置使用的SB集合
    vector<double> vSINR;
    for (auto& sbid : _vSBUsed) {
        CompetorRankA& c = _mSBID2Competor[sbid.ToInt()]; //在本RB上被选中的Competor
        auto it = find(c.begin(), c.end(), _msid);
        assert(it != c.end());
        int iCol = it - c.begin();
        int iPrecodeStartIndex = 0;
        int iPrecodeEndIndex = 0;
        for (int j = 0; j < iCol; ++j) {
            MSID msid = c[j];
            iPrecodeStartIndex += (c.m_mMSID2Rank[msid.ToInt()] + 1);
        }
        MSID msid = c[iCol];
        iPrecodeEndIndex = iPrecodeStartIndex + c.m_mMSID2Rank[msid.ToInt()];
        itpp::cmat mCodeWord = c.m_CodeWord.get_cols(iPrecodeStartIndex, iPrecodeEndIndex);
        pSchedulingMes->SetCodeWord(sbid, mCodeWord); //设置每个RB上使用的码字

        int iLayerNum = 0;
        int iMuNumber = static_cast<int> (c.size());
        for(auto& ms : c){
            iLayerNum += (c.GetRank(ms)+1);
        }
        pSchedulingMes->SetLayerNum(sbid, iLayerNum);

        cmat mPA = c.m_CodeWord;
        int iRank = mCodeWord.cols();
        int iRankA = mPA.cols();
        vector<int> vRankID(iRank, -1);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            std::vector<int> vToBePastedRanks(iRankIndex,0);
            for (int k1 = 0; k1 < iRankIndex; k1++) {
                vToBePastedRanks[k1]=k1;
            }

            for (int icol = 0; icol < iRankA; ++icol) {
                if (!vToBePastedRanks.empty()) {
                    int k2 = 0;
                    for (; k2 <= static_cast<int>(vToBePastedRanks.size() - 1); k2++) {
                        if (icol == vToBePastedRanks[k2]) {
                            break;
                        }
                    }
                    if (k2 < vToBePastedRanks.size()) {
                        vToBePastedRanks.erase(vToBePastedRanks.begin() + k2);
                        continue;
                    }
                }
                const double tolent = 0.001;
                if (itpp::norm(mCodeWord.get_cols(iRankIndex, iRankIndex)
                        - mPA.get_cols(icol, icol)) < tolent) {
                    vRankID[iRankIndex] = icol;
                    break;
                }
            }
            assert(vRankID[iRankIndex] != -1);
            if (vRankID[iRankIndex] == -1) {
                cerr << "Can't find the correct rank id!";
                exit(0);
            }
        }


        vector<double>tempSINR = c.m_vMSID2SINR[_msid.ToInt()];
        vSINR.insert(vSINR.end(),tempSINR.begin(),tempSINR.end());
//        mat dSINR = c.m_mMSID2SINR[_msid];
//        vSINR.insert(vSINR.end(), Parameters::Instance().BASIC.ISBSize * Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace, dSINR);
    }

//    int iFinalMCS = LinkLevelInterface::Instance_DL().SINR2MCS2(vSINR,1);
    int iFinalMCS = LinkLevelInterface::Instance_DL().SINR2MCS_simplified(vSINR);
    if (iFinalMCS != iMCS) {
        cout << "BTSMUEBBTxer::GenerateSchedulingMessageDL Error!" << endl;
    }

    pSchedulingMes->SetMCS(iFinalMCS);
    pSchedulingMes->SetTBSizeKbit_TDD();
    double sinr = 0.0;
    for(int i=0;i<vSINR.size();i++){
        sinr +=vSINR[i];
    }
    sinr /= vSINR.size();
    pSchedulingMes->SetSINR(sinr);
    return pSchedulingMes;
}
//used in MUProportionalFairRBRankAScheduler
std::shared_ptr<SchedulingMessageDL> BTSMUEBBTxer::GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, imat mMCS, const vector<SBID>& _vSBUsed, std::unordered_map<int, CompetorRankA> _mSBID2Competor, std::unordered_map<int, pair<int, vector<int> > > _mMSID2PanelAndvBSBeam) {
    std::shared_ptr<SchedulingMessageDL> pSchedulingMes(new SchedulingMessageDL(_msid, _iHARQID, _iRank));
    pSchedulingMes->SetSBUsed(_vSBUsed); //设置使用的SB集合
    vector<double> vSINR;
    for (auto& sbid : _vSBUsed) {
        CompetorRankA& c = _mSBID2Competor[sbid.ToInt()]; //在本RB上被选中的Competor
        auto it = find(c.begin(), c.end(), _msid);
        assert(it != c.end());
        int iCol = it - c.begin();
        int iPrecodeStartIndex = 0;
        int iPrecodeEndIndex = 0;
        for (int j = 0; j < iCol; ++j) {
            MSID msid = c[j];
            iPrecodeStartIndex += (c.m_mMSID2Rank[msid.ToInt()] + 1);
        }
        MSID msid = c[iCol];
        iPrecodeEndIndex = iPrecodeStartIndex + c.m_mMSID2Rank[msid.ToInt()];
        itpp::cmat mCodeWord = c.m_CodeWord.get_cols(iPrecodeStartIndex, iPrecodeEndIndex);
        pSchedulingMes->SetCodeWord(sbid, mCodeWord); //设置每个RB上使用的码字

        int iLayerNum = 0;
        int iMuNumber = static_cast<int> (c.size());
        for (int j = 0; j < iMuNumber; ++j) {
            iLayerNum += (c.GetRank(c[j]) + 1);
        }
        pSchedulingMes->SetLayerNum(sbid, iLayerNum);

        cmat mPA = c.m_CodeWord;
        int iRank = mCodeWord.cols();
        int iRankA = mPA.cols();
        vector<int> vRankID(iRank, -1);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            for (int icol = 0; icol < iRankA; ++icol) {
                if (iRankIndex == 1) {
                    if (icol == vRankID[0]) continue;
                }
                if (mCodeWord.get_cols(iRankIndex, iRankIndex) == mPA.get_cols(icol, icol)) {
                    vRankID[iRankIndex] = icol;
                    break;
                }
            }
            assert(vRankID[iRankIndex] != -1);
            if (vRankID[iRankIndex] == -1) {
                cerr << "Can't find the correct rank id!";
                exit(0);
            }
        }


        vector<double> dSINR = c.m_vMSID2SINR[_msid.ToInt()];
        vSINR.insert(vSINR.end(), dSINR.begin(),dSINR.end());
    }

//    int mFinalMCS = LinkLevelInterface::Instance_DL().SINR2MCS2(vSINR,1);
    int mFinalMCS = LinkLevelInterface::Instance_DL().SINR2MCS_simplified(vSINR);
//    if (mFinalMCS != mMCS) cout << "BTSMUEBBTxer::GenerateSchedulingMessageDL Error!" << endl;
    pSchedulingMes->SetMCS(mFinalMCS);
    pSchedulingMes->SetTBSizeKbit_TDD();
    double sinr = 0.0;
    for(int i=0;i<vSINR.size();i++){
        sinr +=vSINR[i];
    }
    sinr /= vSINR.size();
    pSchedulingMes->SetSINR(sinr);
    pSchedulingMes->SetMap_MSID2PanelAndBSBeam(_mMSID2PanelAndvBSBeam);
    return pSchedulingMes;
}