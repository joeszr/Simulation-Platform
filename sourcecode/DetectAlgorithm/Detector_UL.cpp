///@file Detector.cpp
///@brief  检测算法的基类实现
///@author wangfei

#include "Detector_UL.h"

mat Detector_UL::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    return itpp::zeros(0, 0);
}

mat Detector_UL::CalculateMUSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const cmat& _mEffectiveP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    if (Parameters::Instance().MSS_UL.DL.IDetectorMode != Parameters::IDetectorMode_MMSE) {
        assert(false);
    }
    assert(false);
    mat mSINR;
    return mSINR;
}

//没有定义纯虚函数
///包含了DMRS信道估计误差的H的sinr计算，Herror将直接加载等小信道H*P上

///包含了DMRS信道估计误差的H的sinr计算，Herror将直接加载等小信道H*P上

mat Detector_UL::CalculateSINR_UL(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL, const mat& HerrorStd,
        const cmat& Herror) {
    assert(false);
    mat mSINR;
    return mSINR;
}
mat Detector_UL::CalculateSINR_UL2(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    mat mSINR;
    return mSINR;
}

mat Detector_UL::CalculateSRSSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL, const mat& HerrorScale,
        const cmat& Herror) {
    assert(false);
    mat mSINR;
    return mSINR;
}

mat Detector_UL::CalculateSINR_DmrsError(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL,
        const mat& HerrorScale,
        const cmat& Herror) {
    assert(false);
    mat mSINR;
    return mSINR;
}
