///@file IRCAlgorithm.h
///@brief  IRC检测算法的声明
///@author wangfei

#pragma once
#include "../Utility/Include.h"
class Detector_DL;
using namespace std;
using namespace itpp;

/// @brief IRC检测算法
class IRC : public Detector_DL {
public:
    /// @brief 采用IRC算法检测SINR
    /// @see Detector::CalculateSINR
    mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);
public:
    /// @brief 构造函数
    IRC() = default;
    /// @brief 析构函数
    ~IRC() = default;
};