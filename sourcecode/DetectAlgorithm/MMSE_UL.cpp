///@author zhangyong

#include "MMSE_UL.h"
#include "Detector_Common.h"

mat MMSE_UL::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    int IMMSEOption = -1;
    //    return Detector_Common::CalculateSINR(_mS, _mH, _mP, _vSIS, _vSIH, _vSIP, _vWIS, _vWIL, Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw, IMMSEOption);
    return Detector_Common::CalculateSINR(_mS, _mH, _mP, _vSIS, _vSIH, _vSIP, _vWIS, _vWIL, Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw, IMMSEOption);

    //
    //
    //    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    //    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    //    int iBSAntNum = _mH.cols(); //BS天线数
    //    int iMSAntNum = _mH.rows(); //MS天线数
    //    int iRank = _mP.cols(); //流数
    //
    //    //输入参数校验
    //    assert(_mS.rows() == iRank);
    //    assert(_mP.rows() == iBSAntNum);
    //    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    //    assert(_vWIS.size() == _vWIL.size());
    //    for (int i = 0; i < iSizeI; ++i) {
    //        assert(_vSIS[i].rows() == _vSIP[i].cols());
    //        assert(_vSIP[i].rows() == iBSAntNum);
    //        assert(_vSIH[i].rows() == iMSAntNum);
    //        assert(_vSIH[i].cols() == iBSAntNum);
    //    }
    //
    //    // step1: 计算干扰相关矩阵
    //    itpp::cmat R(iMSAntNum, iMSAntNum);
    //    R.zeros();
    //    // 根据强干扰累计干扰相关矩阵
    //    for (int i = 0; i < iSizeI; ++i) {
    //        itpp::cmat IH = _vSIH[i] * _vSIP[i];
    //        R += (real((_vSIS[i].H() * _vSIS[i]) (0, 0)) / iRank) * IH * IH.H();
    //    }
    //    // 根据弱干扰累计干扰相关矩阵
    //    for (int i = 0; i < iSizeW; ++i) {
    //        itpp::cmat mWIS = _vWIS[i];
    //        double dLinkLoss = _vWIL[i];
    //        double dWeakI = real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
    //        R += dWeakI * itpp::eye_c(R.rows());
    //    }
    //    R += Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    //    itpp::cmat invR = itpp::inv(R);
    //
    //    // step2: 计算信号的功率
    //    itpp::cmat SH = _mH * _mP; //计算预编码后的等效信道矩阵
    //    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (Parameters::Instance().BTS_UL.DL.DSCTxPowerMw / iRank)) * SH.H() * invR; // 计算均衡矩阵
    //    itpp::cmat W = G * SH; //计算均衡后的等效信道矩阵
    //
    //    itpp::cmat symbol_de = itpp::diag(itpp::diag(W)) * _mS; //计算均衡后的接收符号
    //    itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率
    //
    //    // step3: 计算干扰功率
    //    itpp::mat I;
    //    I.zeros();
    //    double dCarrierNoiseMW = Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw;
    //
    //    // 计算两个流之间的干扰，如果是单流的情况计算结果为0
    //    itpp::cmat is_symbol = (W - itpp::diag(itpp::diag(W))) * _mS; //
    //    I += real(itpp::elem_mult(conj(is_symbol), is_symbol)); //
    //
    //    // 计算来自其他强干扰源的干扰
    //    for (int i = 0; i < iSizeI; ++i) {
    //        itpp::cmat IH = _vSIH[i] * _vSIP[i]; // 干扰加预编码后的等效信道矩阵
    //        itpp::cmat IW = G * IH; // 经过均衡后的等效信道矩阵
    //        itpp::cmat other_rx = IW * itpp::diag(_vSIS[i].get_col(0)); //
    //        I += real(elem_mult(conj(other_rx), other_rx)) * itpp::ones(_vSIP[i].cols(), 1);
    //    }
    //
    //    // 将弱干扰的功率累计到噪声中
    //    for (int i = 0; i < iSizeW; ++i) {
    //        double dLinkLoss = _vWIL[i];
    //        dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i]) (0, 0)) * dLinkLoss;
    //    }
    //
    //    // 计算信干噪比
    //    itpp::mat mSINR = itpp::elem_div(S, dCarrierNoiseMW * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)) + I);
    //    return mSINR;
}

//mat MMSE_UL::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA,
//        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
//        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
//        const vector<double>& _vWIL) {
//    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
//    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
//    int iBSAntNum = _mH.cols(); //BS天线数
//    int iMSAntNum = _mH.rows(); //MS天线数
//    int iRank = _mP.cols(); //本（单）用户的流数
//    int iRankA = _mPA.cols(); //多用户数（多用户流数）
//
//    //输入参数校验
//    assert(iRank == 1);
//    assert(_mS.rows() == iRankA);
//    assert(_mP.rows() == iBSAntNum);
//    assert(_mPA.rows() == iBSAntNum);
//    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
//    assert(_vWIS.size() == _vWIL.size());
//    for (int i = 0; i < iSizeI; ++i) {
//        assert(_vSIS[i].rows() == _vSIP[i].cols());
//        assert(_vSIP[i].rows() == iBSAntNum);
//        assert(_vSIH[i].rows() == iMSAntNum);
//        assert(_vSIH[i].cols() == iBSAntNum);
//    }
//
//    //找到本用户是多用户流中的第几流
//    int iRankID = -1;
//    for (int icol = 0; icol < iRankA; ++icol) {
//        if (_mP == _mPA.get_cols(icol, icol)) {
//            iRankID = icol;
//            break;
//        }
//    }
//
//    assert(iRankID != -1);
//    if (iRankID == -1) {
//        cerr << "Can't find the correct rank id!";
//        exit(0);
//    }
//
//    // step1: 计算干扰相关矩阵
//    itpp::cmat R(iMSAntNum, iMSAntNum);
//    R.zeros();
//    // 根据流间干扰累计干扰相关矩阵
//    if (iRankA > iRank) {
//        itpp::cmat t = itpp::ones_c(iRankA, 1);
//        t(iRankID, 0) = 0;
//        itpp::cmat mP2 = _mPA * t;
//        itpp::cmat mTH = _mH * mP2;
//        itpp::cmat mTS = t.T() * _mS;
//        R += real((mTS.H() * mTS)(0, 0)) * mTH * mTH.H();
//    }
//
//    // 根据强干扰累计干扰相关矩阵
//    for (int i = 0; i < iSizeI; ++i) {
//        itpp::cmat IH = _vSIH[i] * _vSIP[i];
//        R += (real((_vSIS[i].H() * _vSIS[i]) (0, 0)) / iRank) * IH * IH.H();
//    }
//
//    // 根据弱干扰累计干扰相关矩阵
//    for (int i = 0; i < iSizeW; ++i) {
//        itpp::cmat mWIS = _vWIS[i];
//        double dLinkLoss = _vWIL[i];
//        double dWeakI = real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
//        R += dWeakI * itpp::eye_c(R.rows());
//    }
//    R += Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
//    itpp::cmat invR = itpp::inv(R);
//
//    // step2: 计算信号的功率
//    itpp::cmat SH = _mH * _mP; // 计算预编码后的等效信道矩阵
//    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (Parameters::Instance().BTS_UL.DL.DSCTxPowerMw / iRankA)) * SH.H() * invR; // 计算均衡矩阵
//    itpp::cmat W = G * _mH * _mPA; // 计算均衡后的等效信道矩阵
//
//    itpp::cmat symbol_de = itpp::elem_mult(W.T(), _mS); //计算均衡后的接收符号
//    itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率
//    itpp::mat SS = S.get_rows(iRankID, iRankID); //得到本用户的信号功率
//
//    // step3: 计算干扰功率
//    itpp::mat I;
//    I.zeros();
//    double dCarrierNoiseMW = Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw;
//
//    // 计算多用户流之间的干扰，如果是单流的情况计算结果为0
//    itpp::mat ISelfR = itpp::ones(1, S.rows()) * S - SS;
//    I += ISelfR;
//
//    // 计算来自其他强干扰源的干扰
//    for (int i = 0; i < iSizeI; ++i) {
//        itpp::cmat IH = _vSIH[i] * _vSIP[i]; // 干扰加预编码后的等效信道矩阵
//        itpp::cmat IW = G * IH; // 经过均衡后的等效信道矩阵
//        itpp::cmat other_rx = IW * itpp::diag(_vSIS[i].get_col(0)); //
//        I += real(elem_mult(conj(other_rx), other_rx)) * itpp::ones(_vSIP[i].cols(), 1);
//    }
//
//    // 将弱干扰的功率累计到噪声中
//    for (int i = 0; i < iSizeW; ++i) {
//        double dLinkLoss = _vWIL[i];
//        dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i]) (0, 0)) * dLinkLoss;
//    }
//
//    itpp::mat mNoise = dCarrierNoiseMW * real(diag(diag(G * G.H())));
//    itpp::mat mSINR = itpp::elem_div(SS, mNoise + I);
//    return mSINR;
//}

mat MMSE_UL::CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    return itpp::zeros(0, 0);
}

itpp::cmat MMSE_UL::CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {

    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH.cols(); //BS天线数
    int iMSAntNum = _mH.rows(); //MS天线数
    int iRank = _mP.cols(); //流数

    //输入参数校验
    assert(_mS.rows() == iRank);
    assert(_mP.rows() == iBSAntNum);
    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    assert(_vWIS.size() == _vWIL.size());
    for (int i = 0; i < iSizeI; ++i) {
        assert(_vSIS[i].rows() == _vSIP[i].cols());
        assert(_vSIP[i].rows() == iBSAntNum);
        assert(_vSIH[i].rows() == iMSAntNum);
        assert(_vSIH[i].cols() == iBSAntNum);
    }

    // step1: 计算干扰相关矩阵
    itpp::cmat R(iMSAntNum, iMSAntNum);
    R.zeros();
    // 根据强干扰累计干扰相关矩阵
    for (int i = 0; i < iSizeI; ++i) {
        itpp::cmat IH = _vSIH[i] * _vSIP[i];
        R += (real((_vSIS[i].H() * _vSIS[i]) (0, 0)) / iRank) * IH * IH.H();
    }
    // 根据弱干扰累计干扰相关矩阵
    for (int i = 0; i < iSizeW; ++i) {
        const itpp::cmat& mWIS = _vWIS[i];
        double dLinkLoss = _vWIL[i];
        double dWeakI = real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
        R += dWeakI * itpp::eye_c(R.rows());
    }
    //    R += Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    R += Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw * itpp::eye_c(iMSAntNum);

    itpp::cmat invR = itpp::inv(R);

    // step2: 计算信号的功率
    itpp::cmat SH = _mH * _mP; //计算预编码后的等效信道矩阵
    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (Parameters::Instance().Macro.DL.DSCTxPowerMw / iRank)) * SH.H() * invR; // 计算均衡矩阵

    return G;
}