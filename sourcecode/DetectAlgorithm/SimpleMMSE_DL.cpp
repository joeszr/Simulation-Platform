///@file SimpleMMSE.cpp
///@brief  SimpleMMSE检测算法的实现
///@author wangfei

#include "Detector_DL.h"
#include "SimpleMMSE_DL.h"

mat SimpleMMSE_DL::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    ///确定信号的流数和接收天线数
    int iNs = static_cast<int> (_mS.size());
    int iNr = _mH.rows();
    ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
//    double dNoisePSD = DownOrUpLink(Clock::Instance().GetTimeSlot())==Parameters::DL ? 
//        (Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw) : (Parameters::Instance().Macro.UL.DSCNoisePowerMw);
    
    double dNoisePSD = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
    cmat mRe = dNoisePSD * eye_c(iNr);
    cmat mCovS = _mS*_mS.H();
    for(int irow = 0; irow < iNs; ++irow){
        for(int icol = 0; icol<iNs; ++icol){
            if(irow != icol) {
                mCovS(irow,icol) = complex<double>(0.0, 0.0);
            }
        }
    }
    cmat mG = (inv((_mH * _mP)*mCovS*(_mH * _mP).H() + mRe)*(_mH * _mP)).H();

    cmat mD(iNs, iNs);
    mD.zeros();
    cmat mIself(iNs, iNs);
    mIself.zeros();
    for (int i = 0; i < iNs; ++i) {
        for (int j = 0; j < iNs; ++j) {
            cmat mTemp = mG * _mH*_mP;
            if (i == j)
                mD(i, j) = mTemp(i, j);
            else
                mIself(i, j) = mTemp(i, j);
        }
    }
    ///计算有用信号分量
    cmat mSignal = mD*_mS;
    vector<double> vSignal;
    for (int i = 0; i < iNs; ++i) {
        double dSignal = pow(abs(mSignal(i, 0)), 2);
        vSignal.push_back(dSignal);
    }
    ///计算流间干扰分量
    cmat mIntraI = mIself*_mS;
    vector<double> vIntraI;
    for (int i = 0; i < iNs; ++i) {
        double dIntraI = pow(abs(mIntraI(i, 0)), 2);
        vIntraI.push_back(dIntraI);
    }
    ///计算小区间干扰分量
    cmat mInterI(iNs, iNs);
    mInterI.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        mInterI += (mG * _vSIH[i] * _vSIP[i] * _vSIS[i])*(mG * _vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        cmat mTemp = _vWIL[i] * _vWIS[i].H() * _vWIS[i];
        mInterI += abs(mTemp(0, 0)) * mG * mG.H();
    }
    vector<double> vInterI;
    for (int i = 0; i < iNs; ++i) {
        double dInterI = abs(mInterI(i, i));
        vInterI.push_back(dInterI);
    }
    ///计算噪声分量
    cmat mNoise(iNs, iNs);
    mNoise = dNoisePSD * mG * mG.H();
    vector<double> vNoise;
    for (int i = 0; i < iNs; ++i) {
        double dNoise = abs(mNoise(i, i));
        vNoise.push_back(dNoise);
    }
    mat mSINR(iNs, 1);
    for (int i = 0; i < iNs; ++i) {
        double dSINR = vSignal[i] / (vIntraI[i] + vInterI[i] + vNoise[i]);
        mSINR(i, 0) = dSINR;
    }
    return mSINR;
}

//上行添加_begin
mat SimpleMMSE_DL::CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    //return itpp::zeros(0, 0);
}

itpp::cmat SimpleMMSE_DL::CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    //return itpp::zeros_c(0, 0);

}
//上行添加_end