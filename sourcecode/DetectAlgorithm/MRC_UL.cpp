/* 
 * File:   MRC.cpp
 * Author: zhangyong
 * 
 * Created on April 27, 2010, 1:20 PM
 */

#include "MRC_UL.h"

itpp::mat MRC_UL::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH.cols(); //BS天线数
    int iMSAntNum = _mH.rows(); //MS天线数
    int iRank = _mP.cols(); //流数

    //输入参数校验
    assert(iRank == 1);
    assert(_mS.rows() == iRank);
    assert(_mP.rows() == iBSAntNum);
    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    assert(_vWIS.size() == _vWIL.size());
    for (int i = 0; i < iSizeI; ++i) {
        assert(_vSIS[i].rows() == _vSIP[i].cols());
        assert(_vSIP[i].rows() == iBSAntNum);
        assert(_vSIH[i].rows() == iMSAntNum);
        assert(_vSIH[i].cols() == iBSAntNum);
    }

    itpp::cmat mDH = _mH * _mP; //预编码加权后的等效信道
    itpp::cmat mRS = mDH * _mS; //接收符号
    itpp::cmat mDS = mDH.H() * mRS; //MRC检测后的符号
    itpp::mat S = real(itpp::elem_mult(conj(mDS), mDS)); //信号功率


    itpp::mat I = itpp::zeros(1, 1); //干扰
    for (int i = 0; i < iSizeI; ++i) { //处理强干扰
        itpp::cmat mIDH = _vSIH[i] * _vSIP[i]; //干扰加权后的等效信道
        itpp::cmat mIRS = mIDH * _vSIS[i]; //干扰的接收符号
        itpp::cmat mIDS = mDH.H() * mIRS; //干扰的检测符号
        I += real(itpp::elem_mult(conj(mIDS), mIDS)); //累加干扰
    }
    
    double dNoisePowerMw =  Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw; //处理热噪声
    for (int i = 0; i < iSizeW; ++i) { //处理弱干扰
        double dWeakI = real((_vWIS[i].H() * _vWIS[i])(0, 0)) * _vWIL[i];
        dNoisePowerMw += dWeakI;
    }

    itpp::mat mSINR = itpp::elem_div( S, dNoisePowerMw * real( mDH.H() * mDH ) + I );
    return mSINR;
}

mat MRC_UL::CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL) {
    assert(false);
    return itpp::zeros(0, 0);
}

itpp::cmat MRC_UL::CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    return itpp::zeros_c(0, 0);
}
