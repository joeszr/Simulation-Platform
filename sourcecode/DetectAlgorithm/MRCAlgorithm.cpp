///@file MRCAlgorithm.cpp
///@brief  MRC检测算法的实现
///@author wangfei

#include "../Statistician/Observer.h"
#include "Detector_UL.h"
#include "MRCAlgorithm.h"


///@brief MRC算法计算SINR
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

mat MRCAlgorithm::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    ///构造MRC的检测矩阵
    cmat mG = (_mH * _mP).H();
    ///计算有用信号分量,mSignal应该是一个1X1的矩阵
    cmat mSignal = (mG * _mH * _mP * _mS)*(mG * _mH * _mP * _mS).H();
    ///计算强干扰分量
    cmat mStrongI(1, 1);
    mStrongI.zeros();
    int iStrongINum = static_cast<int> (_vSIS.size());
    for (int i = 0; i < iStrongINum; ++i) {
        mStrongI += (mG * _vSIH[i] * _vSIP[i] * _vSIS[i])*(mG * _vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    }
    ///计算弱干扰分量
    cmat mWeakI(1, 1);
    mWeakI.zeros();
    int iWeakINum = static_cast<int> (_vWIS.size());
    for (int i = 0; i < iWeakINum; ++i) {
        mWeakI += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * mG * mG.H();
    }
    ///计算噪声分量
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    cmat mNoise = dNoisePSD * mG * mG.H();
    ///计算SINR
    mat mSINR(1, 1);
    mSINR(0, 0) = mSignal(0, 0).real() / (mStrongI(0, 0).real() + mWeakI(0, 0).real() + mNoise(0, 0).real());
    return mSINR;
}


//计算MRC算法下的IOT

itpp::mat MRCAlgorithm::CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {

    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    //    int iBSAntNum = _mH.cols(); //BS天线数
    //    int iMSAntNum = _mH.rows(); //MS天线数
    //    int iRank = _mP.cols(); //流数

    //    //输入参数校验
    //    assert(iRank == 1);
    //    assert(_mS.rows() == iRank);
    //    assert(_mP.rows() == iBSAntNum);
    //    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    //    assert(_vWIS.size() == _vWIL.size());
    //    for (int i = 0; i < iSizeI; ++i) {
    //        assert(_vSIS[i].rows() == _vSIP[i].cols());
    //        assert(_vSIP[i].rows() == iBSAntNum);
    //        assert(_vSIH[i].rows() == iMSAntNum);
    //        assert(_vSIH[i].cols() == iBSAntNum);
    //    }

    itpp::cmat mDH = _mH * _mP; //预编码加权后的等效信道

    itpp::mat I = itpp::zeros(1, 1); //干扰
    for (int i = 0; i < iSizeI; ++i) { //处理强干扰
        itpp::cmat mIDH = _vSIH[i] * _vSIP[i]; //干扰加权后的等效信道
        itpp::cmat mIRS = mIDH * _vSIS[i]; //干扰的接收符号
        itpp::cmat mIDS = mDH.H() * mIRS; //干扰的检测符号
        I += real(itpp::elem_mult(conj(mIDS), mIDS)); //累加干扰
    }

//    double dNoisePowerMw = P.BTS.UL.DSCNoisePowerMw; //处理热噪声
    double dNoisePowerMw = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    for (int i = 0; i < iSizeW; ++i) { //处理弱干扰
        double dWeakI = real((_vWIS[i].H() * _vWIS[i])(0, 0)) * _vWIL[i];
        dNoisePowerMw += dWeakI;
    }

    itpp::mat mIoT = itpp::elem_div(dNoisePowerMw * real(mDH.H() * mDH) + I, Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw * real(mDH.H() * mDH));
    return mIoT;

}

itpp::cmat MRCAlgorithm::CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {


    //强干扰的个数
    //    int iBSAntNum = _mH.cols(); //BS天线数
    //    int iMSAntNum = _mH.rows(); //MS天线数
    //    int iRank = _mP.cols(); //流数

    //    //输入参数校验
    //    assert(iRank == 1);
    //    assert(_mS.rows() == iRank);
    //    assert(_mP.rows() == iBSAntNum);
    //    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    //    assert(_vWIS.size() == _vWIL.size());
    //    for (int i = 0; i < iSizeI; ++i) {
    //        assert(_vSIS[i].rows() == _vSIP[i].cols());
    //        assert(_vSIP[i].rows() == iBSAntNum);
    //        assert(_vSIH[i].rows() == iMSAntNum);
    //        assert(_vSIH[i].cols() == iBSAntNum);
    //    }

    itpp::cmat mDH = _mH * _mP; //预编码加权后的等效信道
    return mDH.H();

}