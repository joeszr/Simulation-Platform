///@file Detector.cpp
///@brief  检测算法的基类实现
///@author wangfei

#include "Detector_DL.h"
mat Detector_DL::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    return itpp::zeros(0, 0);
}

mat Detector_DL::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const cmat& _mCovR, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL)  {
    assert(false);
    cout << "Error in Detector's CalculateSINR()!" << endl;
    return itpp::zeros(0, 0);
}

mat Detector_DL::CalculateTxDSINR(const cmat& _mS, const cmat& _mH,
    const cmat& _mCovR, const vector<cmat>& _vWIS,
    const vector<double>& _vWIL) {
    assert(false);
    cout << "Error in Detector's CalculateTxDSINR()!" << endl;
    return itpp::zeros(0, 0);
}


mat Detector_DL::CalculateSINRWithDMRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mHe,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    cout << "Error in Detector's CalculateSINRWithDMRSError()!" << endl;
    return itpp::zeros(0, 0);
}

mat Detector_DL::CalculateSINRWithDMRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA, const cmat& _mHe,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    cout << "Error in Detector's CalculateSINRWithDMRSError()!" << endl;
    return itpp::zeros(0, 0);
}

mat Detector_DL::CalculateCQIwithSFBC(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    cout << "Error in Detector's CalculateCQIwithSFBC()!" << endl;
    return itpp::zeros(0, 0);
}

double Detector_DL::CalculateCRSSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    cout << "Error in Detector's CalculateCRSSINR()!" << endl;
    return 0;
}

mat Detector_DL::CalculateCQIwithSFBC_TM3(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    assert(false);
    cout << "Error in Detector's CalculateCQIwithSFBC_TM3()!" << endl;
    return itpp::zeros(0, 0);
}

void Detector_DL::CalculateSINR4Ranks(const vector<cmat> &_vmS, const itpp::cmat &_mH,
                                      const vector<itpp::cmat> &_vSIS, const vector<itpp::cmat> &_vSIH,
                                      const vector<itpp::cmat> &_vSIP, const vector<itpp::cmat> &_vWIS,
                                      const vector<double> &_vWIL, vector<vector<double>> &vRank_SC_SINR, int index) {
    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH.cols(); //BS天线数
    int iMSAntNum = _mH.rows(); //MS天线数
    cmat _mP, mU, mD;
    vec vS;
    svd(_mH, mU, vS, mD);
    int maxrank = _vmS.size();
    // step1: 计算干扰相关矩阵
    itpp::cmat R(iMSAntNum, iMSAntNum);
    R.zeros();
    // 根据强干扰累计干扰相关矩阵
    for (int i = 0; i < iSizeI; ++i) {
        itpp::cmat IH = _vSIH[i] * _vSIP[i];
        int iSIRank = _vSIP[i].cols();
        R += (real((_vSIS[i].H() * _vSIS[i])(0, 0)) / iSIRank) * IH * IH.H();
    }
    // 根据弱干扰累计干扰相关矩阵
    for (int i = 0; i < iSizeW; ++i) {
        const itpp::cmat &mWIS = _vWIS[i];
        double dLinkLoss = _vWIL[i];
        double dWeakI = real((mWIS.H() * mWIS)(0, 0)) * dLinkLoss;
        R += dWeakI * itpp::eye_c(R.rows());
    }
    R += Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    itpp::cmat invR = itpp::inv(R);
    for (int idx = 0; idx < maxrank; idx++) {//idx为流数，从0开始
        itpp::cmat _mS = _vmS[idx];
        int iRank = idx + 1;//实际流数，从1开始
        _mP = mD.get_cols(0, idx);
        //输入参数校验
        assert(_mS.rows() == iRank);
        assert(_mP.rows() == iBSAntNum);
        assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
        assert(_vWIS.size() == _vWIL.size());
        for (int i = 0; i < iSizeI; ++i) {
            assert(_vSIS[i].rows() == _vSIP[i].cols());
            assert(_vSIP[i].rows() == _vSIH[i].cols());
            assert(_vSIH[i].rows() == iMSAntNum);
        }
        // step2: 计算信号的功率
        itpp::cmat SH = _mH * _mP; //计算预编码后的等效信道矩阵
        itpp::cmat G =
                itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (real((_mS.H() * _mS)(0, 0)) / iRank)) * SH.H() *
                invR; // 计算均衡矩阵
        if (Parameters::Instance().MIMO_CTRL.IMMSEOption == 0) {
            G = SH.H() * itpp::inv((real((_mS.H() * _mS)(0, 0)) / iRank) * SH * SH.H() + R);
        } else if (Parameters::Instance().MIMO_CTRL.IMMSEOption == 1 ||
                   Parameters::Instance().MIMO_CTRL.IMMSEOption == 2) {
            R = itpp::diag(itpp::diag(R));
            G = SH.H() * itpp::inv((real((_mS.H() * _mS)(0, 0)) / iRank) * SH * SH.H() + R);
        } else {
            assert(false);
        }
        itpp::cmat W = G * SH; //计算均衡后的等效信道矩阵
        itpp::cmat symbol_de = itpp::diag(itpp::diag(W)) * _mS; //计算均衡后的接收符号
        itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率

        // step3: 计算干扰功率
        itpp::mat I;
        I.zeros();
        double dCarrierNoiseMW = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;

        // 计算两个流之间的干扰，如果是单流的情况计算结果为0
        itpp::cmat is_symbol = (W - itpp::diag(itpp::diag(W))) * _mS; //
        I += real(itpp::elem_mult(conj(is_symbol), is_symbol)); //
        // 计算来自其他强干扰源的干扰
        for (int i = 0; i < iSizeI; ++i) {
            itpp::cmat IH = _vSIH[i] * _vSIP[i]; // 干扰加预编码后的等效信道矩阵
            itpp::cmat IW = G * IH; // 经过均衡后的等效信道矩阵
            itpp::cmat other_rx = IW * itpp::diag(_vSIS[i].get_col(0)); //
            I += real(elem_mult(conj(other_rx), other_rx)) * itpp::ones(_vSIP[i].cols(), 1);
        }

        // 将弱干扰的功率累计到噪声中
        for (int i = 0; i < iSizeW; ++i) {
            double dLinkLoss = _vWIL[i];
            dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i])(0, 0)) * dLinkLoss;
        }

        itpp::mat mSINR = itpp::elem_div(S, dCarrierNoiseMW * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)) + I);
        // EVM误差
        // mSINR = itpp::elem_div(DB2L(Parameters::Instance().ERROR.DEVMGateDb) * mSINR, DB2L(Parameters::Instance().ERROR.DEVMGateDb) * itpp::ones(mSINR.rows(), mSINR.cols()) + mSINR);
        mSINR = calcEVM_ERROR(mSINR);
        double dSINR = 0.0;
        for (int i = 0; i < mSINR.rows(); ++i) {
            dSINR += mSINR(i, 0);
        }
        dSINR /= mSINR.rows();
        vRank_SC_SINR[idx][index] = dSINR;
    }
}

itpp::mat Detector_DL::calcEVM_ERROR(itpp::mat &mSINR_wo_EVM) {
    int temp = 0;//floor((G_ICurDrop-1)/4.0);
    double DEVMGate_percent = 0;//  Parameters::Instance().ERROR.DEVMGate_percent/100+temp*0.005;
    double x = random.xNormal_Other(0, DEVMGate_percent);
    itpp::mat mSINR = itpp::elem_div(
            mSINR_wo_EVM,
            itpp::ones(mSINR_wo_EVM.rows(), mSINR_wo_EVM.cols()) + pow(x, 2) * mSINR_wo_EVM);
    return mSINR;
}