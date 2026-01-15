/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Detector_Common.cpp
 * Author: cyh
 * 
 * Created on 2021年1月18日, 下午4:42
 */

#include "Detector_Common.h"
#include <itpp/stat/misc_stat.h>

//Detector_Common::Detector_Common() {
//}
//
//Detector_Common::Detector_Common(const Detector_Common& orig) {
//}
//
//Detector_Common::~Detector_Common() {
//}

mat Detector_Common::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL,
        double _dSCNoisePowerMw, int _IMMSEOption) {

    vector<int> vSUPrecodeword_Pos(_mP.cols(), 1);

    return Detector_Common::CalculateSINR(_mS, _mP, _mH, vSUPrecodeword_Pos,
            _vSIS, _vSIH, _vSIP,
            _vWIS, _vWIL,
            _dSCNoisePowerMw, _IMMSEOption);

    //        int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    //        int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    //        int iBSAntNum = _mH.cols(); //BS天线数
    //        int iMSAntNum = _mH.rows(); //MS天线数
    //        int iRank = _mP.cols(); //流数
    //
    //        //输入参数校验
    //        assert(_mS.rows() == iRank);
    //        assert(_mP.rows() == iBSAntNum);
    //        assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    //        assert(_vWIS.size() == _vWIL.size());
    //        for (int i = 0; i < iSizeI; ++i) {
    //            assert(_vSIS[i].rows() == _vSIP[i].cols());
    //            assert(_vSIP[i].rows() == _vSIH[i].cols());
    //            assert(_vSIH[i].rows() == iMSAntNum);
    //        }
    //
    //        // step1: 计算干扰相关矩阵
    //        itpp::cmat R(iMSAntNum, iMSAntNum);
    //        R.zeros();
    //        // 根据强干扰累计干扰相关矩阵
    //        for (int i = 0; i < iSizeI; ++i) {
    //            itpp::cmat IH = _vSIH[i] * _vSIP[i];
    //            int iSIRank = _vSIP[i].cols();
    //            R += (real((_vSIS[i].H() * _vSIS[i]) (0, 0)) / iSIRank) * IH * IH.H();
    //        }
    //        // 根据弱干扰累计干扰相关矩阵
    //        for (int i = 0; i < iSizeW; ++i) {
    //            itpp::cmat mWIS = _vWIS[i];
    //            double dLinkLoss = _vWIL[i];
    //            double dWeakI = real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
    //            R += dWeakI * itpp::eye_c(R.rows());
    //        }
    //        R += _dSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    //        itpp::cmat invR = itpp::inv(R);
    //
    //        // step2: 计算信号的功率
    //        itpp::cmat SH = _mH * _mP; //计算预编码后的等效信道矩阵
    //        itpp::cmat G; // 计算均衡矩阵
    //        switch (_IMMSEOption) {
    //            case 0:
    //                G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRank) * SH * SH.H() + R);
    //                break;
    //            case 1:
    //                R = itpp::diag(itpp::diag(R));
    //                G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRank) * SH * SH.H() + R);
    //                break;
    //            case 2:
    //                R = itpp::diag(itpp::diag(R));
    //                G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRank) * SH * SH.H() + R);
    //                break;
    //            default:
    //                G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (real((_mS.H() * _mS) (0, 0)) / iRank)) * SH.H() * invR;
    //        }
    //
    //        itpp::cmat W = G * SH; //计算均衡后的等效信道矩阵
    //        itpp::cmat symbol_de = itpp::diag(itpp::diag(W)) * _mS; //计算均衡后的接收符号
    //        itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率
    //
    //        // step3: 计算干扰功率
    //        itpp::mat I;
    //        I.zeros();
    //        double dCarrierNoiseMW = _dSCNoisePowerMw;
    //
    //        // 计算两个流之间的干扰，如果是单流的情况计算结果为0
    //        itpp::cmat is_symbol = (W - itpp::diag(itpp::diag(W))) * _mS; //
    //        I += real(itpp::elem_mult(conj(is_symbol), is_symbol)); //
    //
    //        // 计算来自其他强干扰源的干扰
    //        for (int i = 0; i < iSizeI; ++i) {
    //            itpp::cmat IH = _vSIH[i] * _vSIP[i]; // 干扰加预编码后的等效信道矩阵
    //            itpp::cmat IW = G * IH; // 经过均衡后的等效信道矩阵
    //            itpp::cmat other_rx = IW * itpp::diag(_vSIS[i].get_col(0)); //
    //            I += real(elem_mult(conj(other_rx), other_rx)) * itpp::ones(_vSIP[i].cols(), 1);
    //        }
    //
    //        // 将弱干扰的功率累计到噪声中
    //        for (int i = 0; i < iSizeW; ++i) {
    //            double dLinkLoss = _vWIL[i];
    //            dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i]) (0, 0)) * dLinkLoss;
    //        }
    //
    //        itpp::mat mSINR = itpp::elem_div(S, dCarrierNoiseMW * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)) + I);
    //        // EVM误差
    //        //    mSINR = itpp::elem_div(DB2L(Parameters::Instance().ERROR.DEVMGateDb) * mSINR, DB2L(Parameters::Instance().ERROR.DEVMGateDb) * itpp::ones(mSINR.rows(), mSINR.cols()) + mSINR);
    //
    //        return mSINR;


}

mat Detector_Common::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL,
        double _dSCNoisePowerMw, int _IMMSEOption) {

    vector<int> vSUPrecodeword_Pos = Detector_Common::GenSUPrecodeword_Pos(_mPA, _mP);

    return Detector_Common::CalculateSINR(_mS, _mPA,
            _mH, vSUPrecodeword_Pos,
            _vSIS, _vSIH, _vSIP,
            _vWIS, _vWIL,
            _dSCNoisePowerMw, _IMMSEOption);
}

mat Detector_Common::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const cmat& _mCovR, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL,
        double _dSCNoisePowerMw, int _IMMSEOption) {

    vector<int> vSUPrecodeword_Pos(_mP.cols(), 1);

    int iMSAntNum = _mH.rows(); //MS天线数
    cmat mWeakR = Detector_Common::CalcWeakInterference(_vWIS, _vWIL, iMSAntNum);
    return Detector_Common::CalculateSINR(_mS, _mP,
            _mH, vSUPrecodeword_Pos,
            _mCovR, mWeakR,
            _dSCNoisePowerMw, _IMMSEOption);
}

mat Detector_Common::CalculateSINR(const cmat& _mSA, const cmat& _mPA,
        const cmat& _mH_SU, vector<int> _vSUPrecodeword_Pos,
        const cmat& _mStrongR, const cmat& _mWeakR,
        double _dSCNoisePowerMw, int _IMMSEOption) {

    //    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    //    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH_SU.cols(); //BS天线数
    int iMSAntNum = _mH_SU.rows(); //MS天线数
    //int iRank = _mP.cols(); //本（单）用户的流数
    //    int iRank = _vPrecodeword_Pos.size(); //本（单）用户的流数
    int iRankA = _mPA.cols(); //多用户数（多用户流数）

    //输入参数校验
    //    assert(iRank <= 2);
    assert(_mSA.rows() == iRankA);
    //    assert(_mP.rows() == iBSAntNum);
    assert(_mPA.rows() == iBSAntNum);
    //    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    //    assert(_vWIS.size() == _vWIL.size());
    //    assert(iRank <= iRankA);
    //    for (int i = 0; i < iSizeI; ++i) {
    //        assert(_vSIS[i].rows() == _vSIP[i].cols());
    //        assert(_vSIP[i].rows() == iBSAntNum);
    //        assert(_vSIH[i].rows() == iMSAntNum);
    //        assert(_vSIH[i].cols() == iBSAntNum);
    //    }

    itpp::cmat mP_SU, mP_otherUEs, mS_SU, mS_otherUEs;

    int iRank = 0;
    for (int i = 0; i < _mPA.cols(); ++i) {
        if (_vSUPrecodeword_Pos[i] == 0) {
            mP_otherUEs.append_col(_mPA.get_col(i));
            mS_otherUEs.append_col(_mSA.get_row(i));
        } else if (_vSUPrecodeword_Pos[i] == 1) {
            mP_SU.append_col(_mPA.get_col(i));
            mS_SU.append_col(_mSA.get_row(i));
            iRank++;
        } else {
            assert(false);
        }
    }

    itpp::cmat R(iMSAntNum, iMSAntNum);
    R.zeros();

    // 根据强干扰累计干扰相关矩阵
    R += _mStrongR;

    R += _mWeakR;

    R += _dSCNoisePowerMw * itpp::eye_c(iMSAntNum);

    itpp::cmat invR = itpp::inv(R);


    // other UEs interference干扰相关矩阵
    itpp::cmat Rc(iMSAntNum, iMSAntNum);
    Rc.zeros();
    if (iRankA > iRank) {
        itpp::cmat mTH = _mH_SU * mP_otherUEs;
        ///这种计算方法只能适用于每个流之间等功率分配的情况
        Rc += (real((mS_otherUEs.H() * mS_otherUEs)(0, 0)) / (iRankA - iRank)) * mTH * mTH.H();
    }


    //计算信号的功率
    itpp::cmat HP = _mH_SU * mP_SU; // 计算预编码后的等效信道矩阵

    switch (_IMMSEOption) {
        case 0:
            break;
        case 1:
            R = itpp::diag(itpp::diag(R));
            Rc = itpp::diag(itpp::diag(Rc));
            break;
        case 2:
            R = itpp::diag(itpp::diag(R));
            break;
        default:
            assert(false);
            break;
    }
    // 计算均衡矩阵
    itpp::cmat G = HP.H() * itpp::inv((real((_mSA.H() * _mSA) (0, 0)) / iRankA) * HP * HP.H() + Rc + R);

    itpp::cmat W_SU = G * _mH_SU * mP_SU;
    itpp::mat S = real(((mS_SU.H() * mS_SU) (0, 0)) / iRank * itpp::diag(itpp::diag(W_SU * W_SU.H())) * itpp::ones(G.rows(), 1));

    // SU other Ranks interference
    itpp::cmat W_SU_otherRanks = W_SU - itpp::diag(itpp::diag(W_SU));
    itpp::mat I = real(((mS_SU.H() * mS_SU) (0, 0)) / iRank * itpp::diag(itpp::diag(W_SU_otherRanks * W_SU_otherRanks.H())) * itpp::ones(G.rows(), 1));


    //    itpp::cmat symbol_SU = itpp::diag(itpp::diag(W_SU)) * mS_SU; //计算均衡后的接收符号
    //
    //    itpp::mat S = real(itpp::elem_mult(conj(symbol_SU), symbol_SU)); //计算信号的功率

    // SU other Ranks interference
    //    itpp::cmat symbol_SU_otherRanks = (W_SU - itpp::diag(itpp::diag(W_SU))) * mS_SU;
    //    itpp::mat I = real(itpp::elem_mult(conj(symbol_SU_otherRanks), symbol_SU_otherRanks));


    // MU other UEs interference
    if (iRankA > iRank) {
        itpp::cmat W_otherUEs = G * _mH_SU * mP_otherUEs;
        I += real(((mS_otherUEs.H() * mS_otherUEs) (0, 0)) / (iRankA - iRank) * itpp::diag(itpp::diag(W_otherUEs * W_otherUEs.H())) * itpp::ones(G.rows(), 1));
        //        itpp::cmat symbol_otherUEs = itpp::diag(itpp::diag(W_otherUEs)) * mS_otherUEs;
        //        I += real(itpp::elem_mult(conj(symbol_otherUEs), symbol_otherUEs));
    }
    // 计算来自其他强干扰源的干扰
    itpp::cmat other_rx = G * _mStrongR * G.H();
    I += real(itpp::diag(itpp::diag(other_rx)) * itpp::ones(G.rows(), 1));

    //    for (int i = 0; i < iSizeI; ++i) {
    //        itpp::cmat IHP = _vSIH[i] * _vSIP[i]; // 干扰加预编码后的等效信道矩阵
    //        itpp::cmat IW = G * IHP; // 经过均衡后的等效信道矩阵
    //        itpp::cmat other_rx = IW * itpp::diag(_vSIS[i].get_col(0)); //
    //        itpp::mat Interf = real(elem_mult(conj(other_rx), other_rx)) * itpp::ones(_vSIP[i].cols(), 1);
    //        assert((Interf.rows() == iRank) && (Interf.cols() == 1));
    //        I += Interf;
    //    }
    // 将弱干扰的功率累计到噪声中

    I += real(_mWeakR * itpp::ones(G.cols(), 1));
    double dCarrierNoiseMW = _dSCNoisePowerMw;
    //    for (int i = 0; i < iSizeW; ++i) {
    //        double dLinkLoss = _vWIL[i];
    //        dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i]) (0, 0)) * dLinkLoss;
    //    }
    itpp::mat mNoise = dCarrierNoiseMW * real(diag(diag(G * G.H())));

    itpp::mat mSINR = itpp::elem_div(S, I + mNoise);


    assert((mNoise.rows() == iRank) && (mNoise.cols() == iRank));
    assert(S.rows() == iRank);
    assert(I.rows() == iRank);

    return mSINR;

}

mat Detector_Common::CalculateSINR(const cmat& _mSA, const cmat& _mPA,
        const cmat& _mH_SU, vector<int> _vSUPrecodeword_Pos,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH, const vector<cmat>& _vSIP,
        const vector<cmat>& _vWIS, const vector<double>& _vWIL,
        double _dSCNoisePowerMw, int _IMMSEOption) {

    int iMSAntNum = _mH_SU.rows();

    cmat mStrongR = Detector_Common::CalcStrongInterference(_vSIS, _vSIH, _vSIP, iMSAntNum);
    cmat mWeakR = Detector_Common::CalcWeakInterference(_vWIS, _vWIL, iMSAntNum);

    return Detector_Common::CalculateSINR(_mSA, _mPA,
            _mH_SU, _vSUPrecodeword_Pos,
            mStrongR, mWeakR,
            _dSCNoisePowerMw, _IMMSEOption);
    //        int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    //        int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    //        int iBSAntNum = _mH_SU.cols(); //BS天线数
    //        int iMSAntNum = _mH_SU.rows(); //MS天线数
    //        //int iRank = _mP.cols(); //本（单）用户的流数
    //        //    int iRank = _vPrecodeword_Pos.size(); //本（单）用户的流数
    //        int iRankA = _mPA.cols(); //多用户数（多用户流数）
    //
    //        //输入参数校验
    //        //    assert(iRank <= 2);
    //        assert(_mSA.rows() == iRankA);
    //        //    assert(_mP.rows() == iBSAntNum);
    //        assert(_mPA.rows() == iBSAntNum);
    //        assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    //        assert(_vWIS.size() == _vWIL.size());
    //        assert(iRank <= iRankA);
    //        for (int i = 0; i < iSizeI; ++i) {
    //            assert(_vSIS[i].rows() == _vSIP[i].cols());
    //            assert(_vSIP[i].rows() == iBSAntNum);
    //            assert(_vSIH[i].rows() == iMSAntNum);
    //            assert(_vSIH[i].cols() == iBSAntNum);
    //        }
    //
    //        itpp::cmat mP_SU, mP_otherUEs, mS_SU, mS_otherUEs;
    //
    //        int iRank = 0;
    //        for (int i = 0; i < _mPA.cols(); ++i) {
    //            if (_vSUPrecodeword_Pos[i] == 0) {
    //                mP_otherUEs.append_col(_mPA.get_col(i));
    //                mS_otherUEs.append_col(_mSA.get_col(i));
    //            } else if (_vSUPrecodeword_Pos[i] == 1) {
    //                mP_SU.append_col(_mPA.get_col(i));
    //                mS_SU.append_col(_mSA.get_col(i));
    //                iRank++;
    //            } else {
    //                assert(false);
    //            }
    //        }
    //
    //        itpp::cmat R(iMSAntNum, iMSAntNum);
    //        R.zeros();
    //
    //        // 根据强干扰累计干扰相关矩阵
    //        R += Detector_Common::CalcStrongInterference(_vSIS, _vSIH, _vSIP);
    //
    //        R += Detector_Common::CalcWeakInterference(_vWIS, _vWIL, iMSAntNum);
    //        //    for (int i = 0; i < iSizeI; ++i) {
    //        //        int iRankI = _vSIS[i].rows();
    //        //        itpp::cmat IH = _vSIH[i] * _vSIP[i];
    //        //        R += (real((_vSIS[i].H() * _vSIS[i]) (0, 0)) / iRankI) * IH * IH.H();
    //        //    }
    //
    //        // 根据弱干扰累计干扰相关矩阵
    //        //    for (int i = 0; i < iSizeW; ++i) {
    //        //        itpp::cmat mWIS = _vWIS[i];
    //        //        double dLinkLoss = _vWIL[i];
    //        //        double dWeakI = real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
    //        //        R += dWeakI * itpp::eye_c(R.rows());
    //        //    }
    //        R += _dSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    //
    //        itpp::cmat invR = itpp::inv(R);
    //
    //
    //        // other UEs interference干扰相关矩阵
    //        itpp::cmat Rc(iMSAntNum, iMSAntNum);
    //        Rc.zeros();
    //        if (iRankA > iRank) {
    //            itpp::cmat mTH = _mH_SU * mP_otherUEs;
    //            ///这种计算方法只能适用于每个流之间等功率分配的情况
    //            Rc += (real((mS_otherUEs.H() * mS_otherUEs)(0, 0)) / (iRankA - iRank)) * mTH * mTH.H();
    //        }
    //
    //
    //        //计算信号的功率
    //        itpp::cmat HP = _mH_SU * mP_SU; // 计算预编码后的等效信道矩阵
    //
    //        switch (_IMMSEOption) {
    //            case 0:
    //                break;
    //            case 1:
    //                R = itpp::diag(itpp::diag(R));
    //                Rc = itpp::diag(itpp::diag(Rc));
    //                break;
    //            case 2:
    //                R = itpp::diag(itpp::diag(R));
    //                break;
    //            default:
    //                assert(fasle);
    //                break;
    //        }
    //        // 计算均衡矩阵
    //        itpp::cmat G = HP.H() * itpp::inv((real((_mSA.H() * _mSA) (0, 0)) / iRankA) * HP * HP.H() + Rc + R);
    //
    //        itpp::cmat W_SU = G * _mH_SU * mP_SU;
    //        itpp::cmat symbol_SU = itpp::diag(itpp::diag(W_SU)) * mS_SU; //计算均衡后的接收符号
    //
    //        itpp::mat S = real(itpp::elem_mult(conj(symbol_SU), symbol_SU)); //计算信号的功率
    //
    //        // SU other Ranks interference
    //        itpp::cmat symbol_SU_otherRanks = (W_SU - itpp::diag(itpp::diag(W_SU))) * mS_SU;
    //        itpp::mat I = real(itpp::elem_mult(conj(symbol_SU_otherRanks), symbol_SU_otherRanks));
    //
    //        // MU other UEs interference
    //        if (iRankA > iRank) {
    //            itpp::cmat W_otherUEs = G * _mH_SU * mP_otherUEs;
    //            itpp::cmat symbol_otherUEs = itpp::diag(itpp::diag(W_otherUEs)) * mS_otherUEs;
    //            I += real(itpp::elem_mult(conj(symbol_otherUEs), symbol_otherUEs));
    //        }
    //        // 计算来自其他强干扰源的干扰
    //        for (int i = 0; i < iSizeI; ++i) {
    //            itpp::cmat IHP = _vSIH[i] * _vSIP[i]; // 干扰加预编码后的等效信道矩阵
    //            itpp::cmat IW = G * IHP; // 经过均衡后的等效信道矩阵
    //            itpp::cmat other_rx = IW * itpp::diag(_vSIS[i].get_col(0)); //
    //            itpp::mat Interf = real(elem_mult(conj(other_rx), other_rx)) * itpp::ones(_vSIP[i].cols(), 1);
    //            assert((Interf.rows() == iRank) && (Interf.cols() == 1));
    //            I += Interf;
    //        }
    //        // 将弱干扰的功率累计到噪声中
    //        double dCarrierNoiseMW = _dSCNoisePowerMw;
    //        for (int i = 0; i < iSizeW; ++i) {
    //            double dLinkLoss = _vWIL[i];
    //            dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i]) (0, 0)) * dLinkLoss;
    //        }
    //        itpp::mat mNoise = dCarrierNoiseMW * real(diag(diag(G * G.H())));
    //
    //        S = itpp::diag(itpp::diag(S));
    //        I = itpp::diag(itpp::diag(I));
    //        mNoise = itpp::diag(itpp::diag(mNoise));
    //        itpp::mat mSINR = itpp::elem_div(S * itpp::ones(S.cols(), 1), I * itpp::ones(I.cols(), 1) + mNoise * itpp::ones(mNoise.cols(), 1));
    //
    //
    //        assert((mNoise.rows() == iRank) && (mNoise.cols() == iRank));
    //        assert((S.rows() == iRank) && (S.cols() == iRank));
    //        assert((I.rows() == iRank) && (I.cols() == iRank));
    //
    //        return mSINR;
}

cmat Detector_Common::CalcStrongInterference(const vector<cmat>& _vSIS, const vector<cmat>& _vSIH, const vector<cmat>& _vSIP, int _iMSAntNum) {
    int iSizeI = _vSIS.size();
    itpp::cmat R(_iMSAntNum, _iMSAntNum);
    R.zeros();
    for (int i = 0; i < iSizeI; ++i) {

        int iRankI = _vSIS[i].rows();
        itpp::cmat IH = _vSIH[i] * _vSIP[i];
        R += (real((_vSIS[i].H() * _vSIS[i]) (0, 0)) / iRankI) * IH * IH.H();
    }
    return R;
}

cmat Detector_Common::CalcWeakInterference(const vector<cmat>& _vWIS, const vector<double>& _vWIL, int _iMSAntNum) {

    int iSizeW = _vWIS.size();
    itpp::cmat R(_iMSAntNum, _iMSAntNum);
    R.zeros();
    for (int i = 0; i < iSizeW; ++i) {

        itpp::cmat mWIS = _vWIS[i];
        double dLinkLoss = _vWIL[i];
        double dWeakI = real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
        R += dWeakI * itpp::eye_c(R.rows());
    }
    return R;
}

vector<int> Detector_Common::GenSUPrecodeword_Pos(const cmat& _mPA, const cmat& _mP) {

    int iRank = _mP.cols();
    int iRankA = _mPA.cols();
    //找到本用户是多用户流中的哪几流
    vector<int> vRankID(iRank, -1);
    for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
        std::vector<int> vToBePastedRanks(iRankIndex,0);
        for (int k1 = 0; k1 < iRankIndex; k1++) {
            vToBePastedRanks[k1]=k1;
        }

        for (int icol = 0; icol < iRankA; ++icol) {
            if (!vToBePastedRanks.empty()) {
                int k2 = 0;
                for (; k2 <= static_cast<int>(vToBePastedRanks.size() - 1); k2++) {
                    if (icol == vToBePastedRanks[k2]) {
                        break;
                    }
                }
                if (k2 < vToBePastedRanks.size()) {
                    vToBePastedRanks.erase(vToBePastedRanks.begin() + k2);
                    continue;
                }
            }
            const double tolent = 0.001;
            if (itpp::norm(_mP.get_cols(iRankIndex, iRankIndex)
                    - _mPA.get_cols(icol, icol)) < tolent) {
                vRankID[iRankIndex] = icol;
                break;
            }
        }

        if (vRankID[iRankIndex] == -1) {
            cout << "Error! Can't find the correct rank id! GenSUPrecodeword_Pos" << endl;
        }
        assert(vRankID[iRankIndex] != -1);
    }

    vector<int> vSUPrecodeword_Pos(_mPA.cols(), 0);
    for (int i = 0; i < iRank; ++i) {
        vSUPrecodeword_Pos[vRankID[i]] = 1;
    }
    return vSUPrecodeword_Pos;
}





