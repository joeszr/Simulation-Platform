///@file IRCAlgorithm.cpp
///@brief  IRC检测算法的实现
///@author wangfei
#include "Detector_UL.h"
#include "IRCAlgorithm.h"

///@brief IRC算法计算SINR
///
///@param _mS 有用信号的符号
///@param _mH 有用信号经过的信道矩阵（包括大尺度和小尺度
///@param _mP 有用信号的PMI
///@param _vSIS 所有强干扰的发送信号
///@param _vSIH 所有强干扰的发送信号经过的信道矩阵包括大尺度和小尺度）
///@param _vSIP 所有强干扰的PMI
///@param _vWIS 所有弱干扰的发送信号
///@param _vWIL 所有弱干的大尺度
///@return  返回一个SINR的矩阵
///@see    
///@note    

mat IRCAlgorithm::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    ///计算干扰相关阵
    int iNr = _mH.rows();
    cmat mRe(iNr, iNr);
    mRe.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        mRe += (_vSIH[i] * _vSIP[i] * _vSIS[i])*(_vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr);
    }
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    //double dNoisePSD = Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw;
    mRe += dNoisePSD * eye_c(iNr);
    ///计算SINR
    cmat mSINRtemp = (_mH * _mP).H() * inv(mRe)*(_mH * _mP)*(_mS.H() * _mS);
    mat mSINR(1, 1);
    mSINR(0, 0) = abs(mSINRtemp(0, 0));
    return mSINR;
}

mat IRCAlgorithm::CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {

    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    //    int iBSAntNum = _mH.cols(); //BS天线数
    //    int iMSAntNum = _mH.rows(); //MS天线数
    //    int iRank = _mP.cols(); //流数

    ///计算干扰相关阵
    int iNr = _mH.rows();
    cmat mRe(iNr, iNr);
    mRe.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        mRe += (_vSIH[i] * _vSIP[i] * _vSIS[i])*(_vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr);
    }
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    //double dNoisePSD = Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw;
    mRe += dNoisePSD * eye_c(iNr);

    cmat mInvRe = inv(mRe);

    itpp::cmat mDH = _mH * _mP; //预编码加权后的等效信道 H0

    itpp::cmat mW = inv(mDH.H() * mInvRe * mDH) * mDH.H() * mInvRe; //计算均衡向量

    //    itpp::mat I = itpp::zeros(1, 1); //干扰
    itpp::mat I; //ZHENGYI 这么写行吗？
    I.zeros();
    for (int i = 0; i < iSizeI; ++i) { //处理强干扰
        itpp::cmat mIDH = _vSIH[i] * _vSIP[i]; //干扰加权后的等效信道
        itpp::cmat mIRS = mIDH * _vSIS[i]; //干扰的接收符号
        itpp::cmat mIDS = mW * mIRS; //干扰的检测符号
        I += real(itpp::elem_mult(conj(mIDS), mIDS)); //累加干扰
    }

//    double dNoisePowerMw = P.BTS.UL.DSCNoisePowerMw; //处理热噪声
    double dNoisePowerMw = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw); //处理热噪声
    //double dNoisePowerMw = Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw;
    for (int i = 0; i < iSizeW; ++i) { //处理弱干扰
        double dWeakI = real((_vWIS[i].H() * _vWIS[i])(0, 0)) * _vWIL[i];
        dNoisePowerMw += dWeakI;
    }

    itpp::mat mIoT = itpp::elem_div(dNoisePowerMw * real(mW * mW.H()) + I, Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw * real(mW * mW.H()));
    return mIoT;
}

itpp::cmat IRCAlgorithm::CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {

    //    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    //    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    //    int iBSAntNum = _mH.cols(); //BS天线数
    //    int iMSAntNum = _mH.rows(); //MS天线数
    //    int iRank = _mP.cols(); //流数

    ///计算干扰相关阵
    int iNr = _mH.rows();
    cmat mRe(iNr, iNr);
    mRe.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        mRe += (_vSIH[i] * _vSIP[i] * _vSIS[i])*(_vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr);
    }
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    //double dNoisePSD = Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw;
    mRe += dNoisePSD * eye_c(iNr);

    cmat mInvRe = inv(mRe);

    itpp::cmat mDH = _mH * _mP; //预编码加权后的等效信道 H0

    itpp::cmat mW = inv(mDH.H() * mInvRe * mDH) * mDH.H() * mInvRe; //计算均衡向量

    return mW;
}