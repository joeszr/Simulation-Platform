/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file SimpleMMSE_UL.h
///@brief SimpleMMSE检测算法的声明
///
///包括构造函数，析构函数，SimpleMMSE算法计算SINR的函数
///
///@author wangfei
#pragma once
#include "../Utility/Include.h"
class Detector_UL;
using namespace std;
using namespace itpp;

/// @brief 简单MMSE检测算法
class SimpleMMSE_UL : public Detector_UL {
public:
    /// @brief 采用简单MMSE检测SINR
    /// @see Detector::CalculateSINR
    mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);
    
    //上行添加_begin
    mat CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL); //add by zhengyi

    itpp::cmat CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL); //add by zhengyi
    //上行添加_end
    
public:
    /// @brief 构造函数
    SimpleMMSE_UL() = default;
    /// @brief 析构函数
    ~SimpleMMSE_UL() override = default;
};