///@file Detector.h
///@brief  检测算法的基类声明
///@author wangfei

#pragma once
#include "../Utility/Include.h"
using namespace std;
using namespace itpp;

///检测算法的基类

class Detector_UL {
public:
    ///@brief 实现单用户计算SINR的函数接口
    ///
    ///@param _mS 有用信号的符号
    ///@param _mH 有用信号经过的信道矩阵（包括大尺度和小尺度）
    ///@param _mP 有用信号的预编码码字
    ///@param _vSIS 所有强干扰的发送信号
    ///@param _vSIH 所有强干扰的发送信号经过的信道矩阵包括大尺度和小尺度）
    ///@param _vSIP 所有强干扰的PMI
    ///@param _vWIS 所有弱干扰的发送信号
    ///@param _vWIL 所有弱干的大尺度
    ///@return  返回一个SINR的矩阵
    virtual mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL) = 0;//same as DL
    ///@brief 实现多用户计算SINR的函数接口
    ///
    ///@param _mS 有用信号的符号
    ///@param _mH 有用信号经过的信道矩阵（包括大尺度和小尺度
    ///@param _mP 有用信号的预编码码字
    ///@param _mPA 多用户的预编码码字
    ///@param _vSIS 所有强干扰的发送信号
    ///@param _vSIH 所有强干扰的发送信号经过的信道矩阵包括大尺度和小尺度）
    ///@param _vSIP 所有强干扰的PMI
    ///@param _vWIS 所有弱干扰的发送信号
    ///@param _vWIL 所有弱干的大尺度
    ///@return  返回一个SINR的矩阵
    virtual mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);//same as DL
    
    ///包含了DMRS信道估计误差的H的sinr计算，Herror将直接加载等小信道H*P上
    virtual mat CalculateSINR_UL(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL, const mat& HerrorStd,
            const cmat& Herror);
    virtual mat CalculateSINR_UL2(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL);

    virtual mat CalculateMUSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const cmat& _mEffectiveP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    virtual mat CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL) = 0; //add by zhengyi

    virtual itpp::cmat CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL) = 0; //add by zhengyi

    

    ///包含了计算SRS信道估计误差的SINR计算
    virtual mat CalculateSRSSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL, const mat& HerrorScale,
            const cmat& Herror);

    ///sinr calculation considering the dmrs error
    ///only used in MMSEalgorithm
    virtual mat CalculateSINR_DmrsError(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL,
            const mat& HerrorScale,
            const cmat& Herror);
public:
    Detector_UL() = default;
    virtual ~Detector_UL() = default;
};