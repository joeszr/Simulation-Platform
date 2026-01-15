///@file IRCAlgorithm.h
///@brief  IRC检测算法的声明
///@author wangfei

#pragma once
#include "../Utility/Include.h"
class Detector_UL;
using namespace std;
using namespace itpp;

/// @brief IRC检测算法
class IRCAlgorithm : public Detector_UL {
public:
    /// @brief 采用IRC算法检测SINR
    /// @see Detector::CalculateSINR
    mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    mat CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL); //add by zhengyi
    
    itpp::cmat CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL); //add by zhengyi

public:
    /// @brief 构造函数
    IRCAlgorithm() = default;
    /// @brief 析构函数
    ~IRCAlgorithm() = default;
};