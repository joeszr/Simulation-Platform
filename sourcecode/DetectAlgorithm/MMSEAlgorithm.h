/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file MMSEAlgorithm.h
///@brief  MMSE检测算法的声明
///
///包括构造函数，析构函数，MMSE算法计算SINR的函数
///
///@author wangfei
#pragma once
#include "../Utility/Include.h"
#include "../Utility/Random.h"
class Detector_UL;
using namespace std;
using namespace itpp;

/// @brief MMSE检测算法

class MMSEAlgorithm : public Detector_UL {
public:
    Random random;
    /// @brief 采用MMSE算法检测SINR
    mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    mat CalculateMUSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const cmat& _mEffectiveP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    mat CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL); //add by zhengyi

    mat CalculateMUIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL); //ZHENGYI

    itpp::cmat CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL); //add by zhengyi

    ///包含了DMRS信道估计误差的H的sinr计算，Herror将直接加载等小信道H*P上
    mat CalculateSINR_UL(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL, const mat& HerrorStd,
            const cmat& Herror);
    mat CalculateSINR_UL2(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL);

    mat CalculateSRSSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL, const mat& HerrorScale,
            const cmat& Herror);

    mat CalculateSINR_DmrsError(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL,
            const mat& HerrorScale,
            const cmat& Herror);

public:
//    int invert = 0;
    /// @brief 构造函数
    MMSEAlgorithm() = default;
    /// @brief 析构函数
    ~MMSEAlgorithm() override = default;
};