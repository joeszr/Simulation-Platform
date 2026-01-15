///@file IRCAlgorithm.cpp
///@brief  IRC检测算法的实现
///@author wangfei

#include "Detector_DL.h"
#include "IRC.h"

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
mat IRC::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    ///计算干扰相关阵
    int iNr = _mH.rows();
    cmat mRe(iNr,iNr);
    mRe.zeros();
    for(int i=0;i<static_cast<int>(_vSIS.size());++i){
        mRe += (_vSIH[i]*_vSIP[i]*_vSIS[i])*(_vSIH[i]*_vSIP[i]*_vSIS[i]).H();
    }
    for(int i=0;i<static_cast<int>(_vWIS.size());++i){
        mRe += _vWIL[i]*(_vWIS[i].H()*_vWIS[i])(0,0)*eye_c(iNr);
    }
    //DownOrUpLink(Clock::Instance().GetTimeSlot());
    //double dNoisePSD = Parameters::Instance().BASIC.IDLORUL==1 ? (Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw):(Parameters::Instance().Macro.UL.DSCNoisePowerMw);
    //double dNoisePSD = DownOrUpLink(Clock::Instance().GetTimeSlot())==Parameters::DL ? (Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw):(Parameters::Instance().Macro.UL.DSCNoisePowerMw);
    double dNoisePSD = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
    mRe += dNoisePSD*eye_c(iNr);
    ///计算SINR
    cmat mSINRtemp = (_mH*_mP).H()*inv(mRe)*(_mH*_mP)*(_mS.H()*_mS);
    mat mSINR(1,1);
    mSINR(0,0) = abs(mSINRtemp(0,0));
    return mSINR;
}