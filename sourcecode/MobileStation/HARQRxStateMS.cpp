///@file HARQRxStateMS.cpp
///@brief  HARQRxStateMS类实现
///@author wangfei

#include "../NetworkDrive/Clock.h"
#include "HARQRxStateMS.h"

HARQRxStateMS::HARQRxStateMS():m_mHarqRank2SINR(100,pair_intHash) {
    //    m_iHARQID = 0;
}

void HARQRxStateMS::CombineSINR(int _iHARQID, int _iRankID, const vector<double>& _vSINR) {
    if (m_mHarqRank2SINR.find(make_pair(_iHARQID, _iRankID)) != m_mHarqRank2SINR.end()) {
        assert(_vSINR.size() == m_mHarqRank2SINR[make_pair(_iHARQID, _iRankID)].size());
        for (int i = 0; i<static_cast<int> (_vSINR.size()); ++i) {
            m_mHarqRank2SINR[make_pair(_iHARQID, _iRankID)][i] += _vSINR[i];
        }
    } else {
        m_mHarqRank2SINR[make_pair(_iHARQID, _iRankID)] = _vSINR;
    }
}

vector<double>& HARQRxStateMS::GetCombinedSINR(int _iHARQID, int _iRankID) {
    if (m_mHarqRank2SINR.find(make_pair(_iHARQID, _iRankID)) != m_mHarqRank2SINR.end()) {
        return m_mHarqRank2SINR[make_pair(_iHARQID, _iRankID)];
    } else {
        assert(false);
    }
}

//int HARQRxStateMS::GetHARQID() {
//    return m_iHARQID;
//}

void HARQRxStateMS::Refresh(int _iHARQID, int _iRankID) {
    if (m_mHarqRank2SINR.find(make_pair(_iHARQID, _iRankID)) != m_mHarqRank2SINR.end()) {
        m_mHarqRank2SINR.erase(make_pair(_iHARQID, _iRankID));
    } else {
        cout<<_iRankID<<endl;
        cout << "HARQRxStateMS can not erase data!" << endl;
        assert(false);
    }
}

//void HARQRxStateMS::WorkSlot() {
//    m_iHARQID = (++m_iHARQID) % Parameters::Instance().SIM.DL.IHARQProcessNum;
//}

void HARQRxStateMS::Reset() {
    //    m_iHARQID = 0;
    m_mHarqRank2SINR.clear();
}