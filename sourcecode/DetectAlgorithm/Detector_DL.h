///@file Detector.h
///@brief  检测算法的基类声明
///@author wangfei
#pragma once
#include "../Utility/Include.h"
#include "../Utility/Random.h"
using namespace std;
using namespace itpp;

///检测算法的基类

class Detector_DL {
public:
    Random random;
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
            const vector<double>& _vWIL) = 0;
    
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
            const vector<double>& _vWIL);

    virtual mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const cmat& _mCovR, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) ;

    virtual mat CalculateTxDSINR(const cmat& _mS, const cmat& _mH,
        const cmat& _mCovR, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) ;

    
    ///@brief 实现增加了DMRS误差后的计算SINR的函数接口
    ///
    ///@param _mS 有用信号的符号
    ///@param _mH 有用信号经过的信道矩阵（包括大尺度和小尺度）
    ///@param _mP 有用信号的预编码码字
    ///@param _mHe 有用信号的等效信道矩阵（可以是增加了DMRS误差或者没有增加DMRS误差的等效信道矩阵）
    ///@param _vSIS 所有强干扰的发送信号
    ///@param _vSIH 所有强干扰的发送信号经过的信道矩阵包括大尺度和小尺度）
    ///@param _vSIP 所有强干扰的PMI
    ///@param _vWIS 所有弱干扰的发送信号
    ///@param _vWIL 所有弱干的大尺度
    ///@return  返回一个SINR的矩阵
    virtual mat CalculateSINRWithDMRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mHe,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);
    ///@brief 实现增加了DMRS误差后的多用户计算SINR的函数接口
    ///
    ///@param _mS 有用信号的符号
    ///@param _mH 有用信号经过的信道矩阵（包括大尺度和小尺度）
    ///@param _mP 有用信号的预编码码字
    ///@param _mPA 多用户的预编码码字
    ///@param _mHe 有用信号的等效信道矩阵（可以是增加了DMRS误差或者没有增加DMRS误差的等效信道矩阵）
    ///@param _vSIS 所有强干扰的发送信号
    ///@param _vSIH 所有强干扰的发送信号经过的信道矩阵包括大尺度和小尺度）
    ///@param _vSIP 所有强干扰的PMI
    ///@param _vWIS 所有弱干扰的发送信号
    ///@param _vWIL 所有弱干的大尺度
    ///@return  返回一个SINR的矩阵
    virtual mat CalculateSINRWithDMRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA, const cmat& _mHe,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    virtual mat CalculateCQIwithSFBC(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    virtual double CalculateCRSSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    virtual mat CalculateCQIwithSFBC_TM3(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    //下行TDD选择流数时计算不同流数下SINR的函数
    void CalculateSINR4Ranks(const vector<cmat> &_vmS, const cmat &_mH,
                             const vector<cmat> &_vSIS, const vector<cmat> &_vSIH,
                             const vector<cmat> &_vSIP, const vector<cmat> &_vWIS,
                             const vector<double> &_vWIL, vector<vector<double>> &vRank_SC_SINR, int index);

    //从MMSE_DL复值的EVM函数
    itpp::mat calcEVM_ERROR(itpp::mat &mSINR_wo_EVM);
public:
    Detector_DL() = default;
    virtual ~Detector_DL() = default;
};