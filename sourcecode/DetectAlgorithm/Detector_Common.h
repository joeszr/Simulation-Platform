/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Detector_Common.h
 * Author: cyh
 *
 * Created on 2021年1月18日, 下午4:42
 */
#pragma once
#include "../Utility/Include.h"
using namespace std;
using namespace itpp;

class Detector_Common {
public:
    //    Detector_Common();
    //    
    //    Detector_Common(const Detector_Common& orig);
    //    
    //     ~Detector_Common();

    ///@brief 根据理论公式计算SINR
    ///
    ///当计算公共参考信号的SINR时采用下面的加权向量
    ///itpp::cmat v = std::sqrt(1.0 / iBSAntNum / iRank) * itpp::ones_c(iBSAntNum, iRank);
    ///@param _mS 有用信号的符号
    ///@param _mH 有用信号经过的信道矩阵（包括大尺度和小尺度）
    ///@param _mP 有用信号的PMI
    ///@param _vSIS 所有强干扰的发送信号
    ///@param _vSIH 所有强干扰的发送信号经过的信道矩阵包括大尺度和小尺度）
    ///@param _vSIP 所有强干扰的PMI
    ///@param _vWIS 所有弱干扰的发送信号
    ///@param _vWIL 所有弱干的大尺度
    ///@return  返回一个SINR的矩阵

    static mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL,
            double _dSCNoisePowerMw, int _IMMSEOption = -1);



    ///@param _mS MU有用信号的符号
    ///@param _mH SU有用信号经过的信道矩阵（包括大尺度和小尺度）
    ///@param _mPA MU的PMI
    ///@param _vSIS 所有强干扰的发送信号
    ///@param _vSIH 所有强干扰的发送信号经过的信道矩阵包括大尺度和小尺度）
    ///@param _vSIP 所有强干扰的PMI
    ///@param _vWIS 所有弱干扰的发送信号
    ///@param _vWIL 所有弱干的大尺度
    ///@param _vWIL 所有弱干的大尺度
    ///@paramvetor<int> _vSUPrecodeword_Pos bitmap 0110 1 means the crossponding cols of mPA belongs to SU 
    ///@return  返回一个SINR的矩阵

    static mat CalculateSINR(const cmat& _mSA, const cmat& _mPA,
            const cmat& _mH_SU, vector<int> _vSUPrecodeword_Pos,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH, const vector<cmat>& _vSIP,
            const vector<cmat>& _vWIS, const vector<double>& _vWIL,
            double _dSCNoisePowerMw, int _IMMSEOption);

    static mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL,
            double _dSCNoisePowerMw, int _IMMSEOption);

    static mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const cmat& _mCovR, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL,
            double _dSCNoisePowerMw, int _IMMSEOption);

    static mat CalculateSINR(const cmat& _mSA, const cmat& _mPA,
            const cmat& _mH_SU, vector<int> _vSUPrecodeword_Pos,
            const cmat& _mStrongR, const cmat& _mWeakR,
            double _dSCNoisePowerMw, int _IMMSEOption);

    static cmat CalcStrongInterference(const vector<cmat>& _vSIS, const vector<cmat>& _vSIH, const vector<cmat>& _vSIP, int _iMSAntNum);

    static cmat CalcWeakInterference(const vector<cmat>& _vWIS, const vector<double>& _vWIL, int _iMSAntNum);

    static vector<int> GenSUPrecodeword_Pos(const cmat& _mPA, const cmat& _mP);

};

