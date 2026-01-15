///@file LinkLevelInterface.cpp
///@brief 链路级接口函数
///@author dushaofeng
#include "LinkLevelNR.h"
#include "LinkLevelInterface.h"


unordered_map<int, vector<double> > LinkLevelInterface::m_SINR2SI;
vector<double> LinkLevelInterface::m_vMIESMSINR;
vector<double> LinkLevelInterface::m_vMIESMSINR_256QAM;

LinkLevelInterface* LinkLevelInterface::m_pLinkLevelInterface_DL = nullptr;
LinkLevelInterface* LinkLevelInterface::m_pLinkLevelInterface_UL = nullptr;

///LinkLevelInterface构造函数的实现

LinkLevelInterface::LinkLevelInterface() {
    ifstream fMIESMSINR("./inputfiles/MIESM_SINR.txt");
    ifstream fMIESMSINR_256QAM("./inputfiles/MIESM_SINR_256QAM.txt");
    ifstream fSINR2SIQPSK("./inputfiles/SINR2SIQPSK.txt");
    ifstream fSINR2SI16QAM("./inputfiles/SINR2SI16QAM.txt");
    ifstream fSINR2SI64QAM("./inputfiles/SINR2SI64QAM.txt");
    ifstream fSINR2SI256QAM("./inputfiles/SINR2SI256QAM.txt");

    vector<double> vSINR2SIQPSK;
    vector<double> vSINR2SI16QAM;
    vector<double> vSINR2SI64QAM;
    vector<double> vSINR2SI256QAM;
    vector<double> vMIESMSINR;
    vector<double> vMIESMSINR_256QAM;

    copy(istream_iterator<double>(fMIESMSINR), istream_iterator<double>(), back_inserter(vMIESMSINR));
    copy(istream_iterator<double>(fMIESMSINR_256QAM), istream_iterator<double>(), back_inserter(vMIESMSINR_256QAM));
    // 用copy读入SINRQPSK.txt，将QPSK下的SI的值读入到vector类型的m_SINR2SI[2]中
    copy(istream_iterator<double>(fSINR2SIQPSK), istream_iterator<double>(), back_inserter(vSINR2SIQPSK));
    // 用copy读入SINR16QAM.txt，将16QAM下的SI的值读入到vector类型的m_SINR2SI[4]中
    copy(istream_iterator<double>(fSINR2SI16QAM), istream_iterator<double>(), back_inserter(vSINR2SI16QAM));
    // 用copy读入SINR64QAM.txt，将64QAM下SI的值读入到vector类型的m_SINR2SI[6]中
    copy(istream_iterator<double>(fSINR2SI64QAM), istream_iterator<double>(), back_inserter(vSINR2SI64QAM));
    // 用copy读入SINR256QAM.txt，将256QAM下SI的值读入到vector类型的m_SINR2SI[8]中
    copy(istream_iterator<double>(fSINR2SI256QAM), istream_iterator<double>(), back_inserter(vSINR2SI256QAM));

    m_vMIESMSINR = vMIESMSINR;
    m_vMIESMSINR_256QAM = vMIESMSINR_256QAM;
    m_SINR2SI[2] = vSINR2SIQPSK;
    m_SINR2SI[4] = vSINR2SI16QAM;
    m_SINR2SI[6] = vSINR2SI64QAM;
    m_SINR2SI[8] = vSINR2SI256QAM;
}

///@brief 返回LinkLevelInterface对象的引用
///
///选择返回LTE或者16m对象的引用

LinkLevelInterface& LinkLevelInterface::Instance_DL() {
    if (!m_pLinkLevelInterface_DL) {
        //        m_pLinkLevelInterface = new LinkLevelLTE;
        m_pLinkLevelInterface_DL = new LinkLevelNR(0);
    }

    return *m_pLinkLevelInterface_DL;
}

LinkLevelInterface& LinkLevelInterface::Instance_UL() {
    if (!m_pLinkLevelInterface_UL) {
        m_pLinkLevelInterface_UL = new LinkLevelNR(1);
    }
    return *m_pLinkLevelInterface_UL;
}

///@brief 给出从信噪比SINR到互信息SI的映射
///@param _dSINRdB SINR值
///@param _iModulation 调制阶数
///@return 互信息SI值

double LinkLevelInterface::SINRDB2SI(double _dSINRdB, int _iModulation) {
    std::vector<double>::iterator right_pos, left_pos;
    vector<double> MIESMSINR = m_vMIESMSINR;
    if (_iModulation == 8)
        MIESMSINR = m_vMIESMSINR_256QAM;

    right_pos = find_if(MIESMSINR.begin(), MIESMSINR.end(), bind2nd(greater<double>(), _dSINRdB));
    if (right_pos == MIESMSINR.begin()) {
        return *m_SINR2SI[_iModulation].begin();
    }

    if (right_pos == MIESMSINR.end()) {
        return *m_SINR2SI[_iModulation].rbegin();
    }
    left_pos = right_pos - 1;
    int ileft = static_cast<int> (left_pos - MIESMSINR.begin());
    int iright = ileft + 1;

    double dSINRleft = MIESMSINR[ileft];
    double dSINRright = MIESMSINR[iright];
    double dSIleft = m_SINR2SI[_iModulation][ileft];
    double dSIright = m_SINR2SI[_iModulation][iright];
    double k = (dSIright - dSIleft) / (dSINRright - dSINRleft);
    double dSI = dSIleft + k * (_dSINRdB - dSINRleft);
    return dSI;
}

///@brief 返回从SI到RBIR的映射
///@param _dSI SI值
///@param _iModulation 调制阶数
///@return RBIR值

double LinkLevelInterface::SI2RBIR(double _dSI, int _iModulation) {
    return _dSI / _iModulation;
}

///对子载波对应的SINR进行MIESM合并
///@param _vSINR  SINR值
///@param  _iModulation 调制阶数
///@return 合并后RBIR值

double LinkLevelInterface::MIESM(const vector<double>& _vSINR, int _iModulation) {
    int iSize = _vSINR.size();
    double dRBIR = 0;
    //计算互信息的和
    for (int i = 0; i < iSize; ++i) {
        double dSINRDB = L2DB(_vSINR[i]);
        dRBIR += SI2RBIR(SINRDB2SI(dSINRDB, _iModulation), _iModulation);
    }
    //归一化得到RBIR
    dRBIR /= iSize;
    return dRBIR;
}
double LinkLevelInterface::MIESM(const vector<mat>& _vSINR, int _iModulation) {
    int iSize = _vSINR.size();
    double dRBIR = 0;
    //计算互信息的和
    for (int i = 0; i < iSize; ++i) {
        for(int j = 0 ; j < _vSINR[i].rows(); j++){
            double dSINRDB = L2DB(_vSINR[i](j,0));
            dRBIR += (SI2RBIR(SINRDB2SI(dSINRDB, _iModulation), _iModulation)/_vSINR[i].rows());
        }
    }
    //归一化得到RBIR
    dRBIR /= iSize;
    return dRBIR;
}
double LinkLevelInterface::SINRCombineUL2(const vector< mat >& _vSINR){
    assert(false);
    return 0.0;
}
///对子载波对应的SINR进行EESM合并 
///@param _vSINR  SINR值
///@param  _b  Beta值
///@return 合并后的SINR线性值

double LinkLevelInterface::EESM(const vector<double>& _vSINR, double _b) {
    double result = 1e-30;
    if (_vSINR.empty())
        return 0;
    for (const auto& sinr : _vSINR)
        result += pow(M_E, -1 * sinr / _b);
    result = -1 * _b * log(result / static_cast<int> (_vSINR.size())) / log(M_E);
    return result;
}

//double LinkLevelInterface::MCS2TBSKBit_LTE(itpp::imat _mMCS, int _iRBNum, int _iFlag) {
//    double dResult = 0.0;
//    for (int row = 0; row < _mMCS.rows(); ++row) {
//        for (int col = 0; col < _mMCS.cols(); ++col) {
//            dResult += MCS2TBSKBit(_mMCS(row, col), _iRBNum);
//        }
//    }
//    return dResult;
//}

double LinkLevelInterface::MCS2TBSKBit(itpp::imat _mMCS, int _iRENum_AllSB) {
    double dResult = 0.0;
    for (int row = 0; row < _mMCS.rows(); ++row) {
        for (int col = 0; col < _mMCS.cols(); ++col) {
            double dResulttemp = MCS2TBSKBit2(_mMCS(row, col), _iRENum_AllSB,1);
            dResult += dResulttemp;
        }
    }
    return dResult;
}

double LinkLevelInterface::SI2SINRDB(double _dSI, int _iModLevel) {
    std::vector<double>::iterator right_pos, left_pos;
    std::vector<double>& _v = m_SINR2SI[_iModLevel];
    right_pos = find_if(_v.begin(), _v.end(), bind2nd(greater<double>(), _dSI));

    vector<double> MIESMSINR = m_vMIESMSINR;
    if (_iModLevel == 8)
        MIESMSINR = m_vMIESMSINR_256QAM;

    if (right_pos == _v.begin()) {
        return *MIESMSINR.begin();
    }

    if (right_pos == _v.end()) {
        return *MIESMSINR.rbegin();
    }
    left_pos = right_pos - 1;
    int ileft = static_cast<int> (left_pos - _v.begin());
    int iright = ileft + 1;

    double dSILeft = _v[ileft];
    double dSIRight = _v[iright];
    double dSINRDBLeft = MIESMSINR[ileft];
    double dSINRDBRight = MIESMSINR[iright];
    double k = (dSINRDBRight - dSINRDBLeft) / (dSIRight - dSILeft);
    double dSINRDB = dSINRDBLeft + k * (_dSI - dSILeft);
    return dSINRDB;
}

double LinkLevelInterface::MCS2TBSKBit(int _iMCS, int _iRENum_AllSB) {
    assert(false);
    return -1;
}

double LinkLevelInterface::MCS2SINR(int _iMCS, int _iRBNum) {
    assert(false);
    return -1;
}

mat LinkLevelInterface::MCS2SINR(const imat& _iMCS, const int& _iRBNum) {
    assert(false);
}

double LinkLevelInterface::SINRCombineUL(const vector<double>& _vSINR) {
    assert(false);
    return 0.0;
}

int LinkLevelInterface::GetRENum(int _iRBnum_SingleSB, int _iLayerNum) {
    assert(false);
    return -1;
}
int LinkLevelInterface::GetRENumInRange(int RBbegin, int RBend) {
    assert(false);
    return -1;
}
//double LinkLevelInterface::BLER_UL(const vector<double>& _vSINR, int _iMCSIndex, int _iEESMorMIESM){
//    assert(false);
//    return 0.0;
//}


