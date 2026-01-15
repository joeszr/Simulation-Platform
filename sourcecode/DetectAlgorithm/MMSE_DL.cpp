///@author zhangyong

#include "MMSE_DL.h"
#include <itpp/stat/misc_stat.h>

///@author zhangyong
//
//#include "MMSE.h"
//#include "../Statistician/Observer.h"
//#include "../NetworkDrive/Clock.h"
//#include <itpp/stat/misc_stat.h>

itpp::mat MMSE_DL::calcEVM_ERROR(itpp::mat mSINR_wo_EVM) {
   int temp=0;//floor((G_ICurDrop-1)/4.0);
   double DEVMGate_percent =0;//  Parameters::Instance().ERROR.DEVMGate_percent/100+temp*0.005;
   double x = random.xNormal_Other(0, DEVMGate_percent);

   itpp::mat mSINR = itpp::elem_div(
            mSINR_wo_EVM, 
            itpp::ones(mSINR_wo_EVM.rows(), mSINR_wo_EVM.cols()) + pow(x,2)* mSINR_wo_EVM);
   
/*
    itpp::mat mSINR = itpp::elem_div(
            DB2L(Parameters::Instance().ERROR.DEVMGateDb) * mSINR_wo_EVM, 
            DB2L(Parameters::Instance().ERROR.DEVMGateDb) * 
                itpp::ones(mSINR_wo_EVM.rows(), mSINR_wo_EVM.cols()) + mSINR_wo_EVM);
*/
    return mSINR;
}

double MMSE_DL::calcEVM_ERROR(double dSINR_wo_EVM) {
   int temp=0;//floor((G_ICurDrop-1)/4.0);
   double DEVMGate_percent = 0;// Parameters::Instance().ERROR.DEVMGate_percent/100+temp*0.005;
   //double DEVMGate_percent =  Parameters::Instance().ERROR.DEVMGate_percent/100;
   double x =0;// xNormal_Other(0, DEVMGate_percent);
   
   double dSINR = dSINR_wo_EVM  / (1 + pow(x,2)   * dSINR_wo_EVM);
   
/*
   double dSINR = DB2L(Parameters::Instance().ERROR.DEVMGateDb) * dSINR_wo_EVM 
    / (DB2L(Parameters::Instance().ERROR.DEVMGateDb) + dSINR_wo_EVM);
*/
   return dSINR;
}

///用于SUMIMO的SINR，采用瞬时干扰

mat MMSE_DL::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
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
        assert(_vSIP[i].rows() == _vSIH[i].cols());
        assert(_vSIH[i].rows() == iMSAntNum);
    }

    // step1: 计算干扰相关矩阵
    itpp::cmat R(iMSAntNum, iMSAntNum);
    R.zeros();
    // 根据强干扰累计干扰相关矩阵
    for (int i = 0; i < iSizeI; ++i) {
        itpp::cmat IH = _vSIH[i] * _vSIP[i];
        int iSIRank = _vSIP[i].cols();
        R += (real((_vSIS[i].H() * _vSIS[i]) (0, 0)) / iSIRank) * IH * IH.H();
    }
    // 根据弱干扰累计干扰相关矩阵
    for (int i = 0; i < iSizeW; ++i) {
        const itpp::cmat& mWIS = _vWIS[i];
        double dLinkLoss = _vWIL[i];
        double dWeakI = real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
        R += dWeakI * itpp::eye_c(R.rows());
    }
    R += Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    itpp::cmat invR = itpp::inv(R);

    // step2: 计算信号的功率
    itpp::cmat SH = _mH * _mP; //计算预编码后的等效信道矩阵
    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (real((_mS.H() * _mS) (0, 0)) / iRank)) * SH.H() * invR; // 计算均衡矩阵
    if (Parameters::Instance().MIMO_CTRL.IMMSEOption == 0) {
        G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRank) * SH * SH.H() + R);
    } else if (Parameters::Instance().MIMO_CTRL.IMMSEOption == 1 || Parameters::Instance().MIMO_CTRL.IMMSEOption == 2) {
        R = itpp::diag(itpp::diag(R));
        G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRank) * SH * SH.H() + R);
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
        dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i]) (0, 0)) * dLinkLoss;
    }

    itpp::mat mSINR = itpp::elem_div(S, dCarrierNoiseMW * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)) + I);
    // EVM误差
    // mSINR = itpp::elem_div(DB2L(Parameters::Instance().ERROR.DEVMGateDb) * mSINR, DB2L(Parameters::Instance().ERROR.DEVMGateDb) * itpp::ones(mSINR.rows(), mSINR.cols()) + mSINR);

    mSINR = calcEVM_ERROR(mSINR);
    
    return mSINR;
}
///用于SUMIMO的SINR，采用平均干扰

mat MMSE_DL::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const cmat& _mCovR, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH.cols(); //BS天线数
    int iMSAntNum = _mH.rows(); //MS天线数
    int iRank = _mP.cols(); //流数

    //输入参数校验
    assert(_mS.rows() == iRank);
    int a = _mP.rows();
    assert(_mP.rows() == iBSAntNum);
    assert(_vWIS.size() == _vWIL.size());
    assert(_mCovR.rows() == _mCovR.cols());
    assert(_mCovR.rows() == iMSAntNum);

    // step1: 计算干扰相关矩阵
    itpp::cmat R(iMSAntNum, iMSAntNum);
    R.zeros();
    // 根据强干扰累计干扰相关矩阵
    R += _mCovR;
    // 根据弱干扰累计干扰相关矩阵
    for (int i = 0; i < iSizeW; ++i) {
        const itpp::cmat& mWIS = _vWIS[i];
        double dLinkLoss = _vWIL[i];
        double dWeakI = real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
        R += dWeakI * itpp::eye_c(R.rows());
    }
    R += Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    itpp::cmat invR = itpp::inv(R);

    // step2: 计算信号的功率
    itpp::cmat SH = _mH * _mP; //计算预编码后的等效信道矩阵
    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / 
            (real((_mS.H() * _mS) (0, 0)) / iRank)) * SH.H() * invR; // 计算均衡矩阵
    if (Parameters::Instance().MIMO_CTRL.IMMSEOption == 0) {
        G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRank) * SH * SH.H() + R);
    } else if (Parameters::Instance().MIMO_CTRL.IMMSEOption == 1 || Parameters::Instance().MIMO_CTRL.IMMSEOption == 2) {
        R = itpp::diag(itpp::diag(R));
        G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRank) * SH * SH.H() + R);
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
    I += real(itpp::diag(itpp::diag(G * _mCovR * G.H()))) * itpp::ones(G.rows(), 1);

    // 将弱干扰的功率累计到噪声中
    for (int i = 0; i < iSizeW; ++i) {
        double dLinkLoss = _vWIL[i];
        dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i]) (0, 0)) * dLinkLoss;
    }
    // 计算信干噪比
    //    if(Clock::Instance().GetTimeSlot()>Parameters::Instance().BASIC.IWarmUpSlot){
    //            Observer::Print("test") <<setw(20)<<S<<setw(20)<<(dCarrierNoiseMW * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)))<<setw(20)<<(I);
    //    }
    itpp::mat mSINR = itpp::elem_div(S, dCarrierNoiseMW * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)) + I);

    if (Parameters::Instance().MIMO_CTRL.IMIMOMode == 10) {
        double S = (real((_mS.H() * _mS) (0, 0)) / (_mS.rows())) * real((SH.H() * SH) (0, 0));
        double I = real(_mCovR(0, 0)) + real(_mCovR(1, 1));
        mSINR(0, 0) = S / (I + dCarrierNoiseMW);
    }
    // EVM误差
    // mSINR = itpp::elem_div(DB2L(Parameters::Instance().ERROR.DEVMGateDb) * mSINR, DB2L(Parameters::Instance().ERROR.DEVMGateDb) * itpp::ones(mSINR.rows(), mSINR.cols()) + mSINR);
    
    mSINR = calcEVM_ERROR(mSINR);
    return mSINR;
}
///用于SUMIMO的SINR，采用平均干扰

mat MMSE_DL::CalculateTxDSINR(const cmat& _mS, const cmat& _mH,
        const cmat& _mCovR, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    int irows = _mH.rows(); //MS天线数

    //输入参数校验
    assert(_vWIS.size() == _vWIL.size());
    assert(_mCovR.rows() == _mCovR.cols());
    // assert(_mCovR.rows() == irows);
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    cmat mSH = _mH * _mH.H();
    double Power = 0;
    for (int i = 0; i < irows; ++i) {
        Power += abs(mSH(i, i));
    }
    double S = (real((_mS.H() * _mS) (0, 0)) / (_mS.rows())) * Power;
    //20180712
    double I = 0.0;
    for(int i = 0; i<_mCovR.rows(); ++i){
        I += real(_mCovR(i,i));
    }
    
    double dCarrierNoiseMW = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
    itpp::mat mSINR = zeros(1, 1);
    mSINR(0, 0) = S / (I + dCarrierNoiseMW);
    // EVM误差
    //mSINR = itpp::elem_div(DB2L(Parameters::Instance().ERROR.DEVMGateDb) * mSINR, DB2L(Parameters::Instance().ERROR.DEVMGateDb) * itpp::ones(mSINR.rows(), mSINR.cols()) + mSINR);
    mSINR = calcEVM_ERROR(mSINR);
    return mSINR;
}

/////用于MUMIMO的SINR，采用瞬时干扰


///用于MUMIMO RankA的SINR，采用瞬时干扰


mat MMSE_DL::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH.cols(); //BS天线数
    int iMSAntNum = _mH.rows(); //MS天线数
    int iRank = _mP.cols(); //本（单）用户的流数
    int iRankA = _mPA.cols(); //多用户数（多用户流数）
    //cout<<endl<<_mS<<endl;
    //输入参数校验
    //    assert(iRank <= 2);
    assert(_mS.rows() == iRankA);
    assert(_mP.rows() == iBSAntNum);
    assert(_mPA.rows() == iBSAntNum);
    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    assert(_vWIS.size() == _vWIL.size());
    for (int i = 0; i < iSizeI; ++i) {
        assert(_vSIS[i].rows() == _vSIP[i].cols());
        assert(_vSIP[i].rows() == iBSAntNum);
        assert(_vSIH[i].rows() == iMSAntNum);
        assert(_vSIH[i].cols() == iBSAntNum);
    }

    //找到本用户是多用户流中的哪几流
    vector<int> vRankID(iRank, -1);
    for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
        std::vector<int> vToBePastedRanks(iRankIndex,0);
        for(int k1 = 0; k1 < iRankIndex; k1++) {
            vToBePastedRanks[k1] = k1;
        }

        for (int icol = 0; icol < iRankA; ++icol) {
            if (!vToBePastedRanks.empty()){
                int k2 = 0;
                for(; k2 <= static_cast<int>(vToBePastedRanks.size() - 1); k2++) {
                    if (icol == vToBePastedRanks[k2]) {
                        break;
                    }
                }
                if(k2 < vToBePastedRanks.size()) {
                    vToBePastedRanks.erase(vToBePastedRanks.begin()+k2);
                    continue;
                }
            }
            const double tolent = 0.001;
            if ( itpp::norm(_mP.get_cols(iRankIndex, iRankIndex) 
                    - _mPA.get_cols(icol, icol) ) < tolent ) {
                vRankID[iRankIndex] = icol;
                break;
            }
        }
        assert(vRankID[iRankIndex] != -1);
        if (vRankID[iRankIndex] == -1) {
            cout << "Error! Can't find the correct rank id! CalculateSINR" << endl;
        }
    }
    // step1: 计算干扰相关矩阵
    itpp::cmat R(iMSAntNum, iMSAntNum);
    R.zeros();
    itpp::cmat Rc(iMSAntNum, iMSAntNum);
    Rc.zeros();
    // 根据流间干扰累计干扰相关矩阵
    if (iRankA > iRank) {
        itpp::cmat t = itpp::eye_c(iRankA);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            t(vRankID[iRankIndex], vRankID[iRankIndex]) = 0;
        }
        itpp::cmat mP2 = _mPA * t;
        itpp::cmat mTH = _mH * mP2;
        itpp::cmat mTS = t.T() * _mS;
        ///这种计算方法只能适用于每个流之间等功率分配的情况
        Rc += (real((mTS.H() * mTS)(0, 0)) / (iRankA - iRank)) * mTH * mTH.H();
    }

    // 根据强干扰累计干扰相关矩阵
    for (int i = 0; i < iSizeI; ++i) {
        int iRankI = _vSIS[i].rows();
        itpp::cmat IH = _vSIH[i] * _vSIP[i];
        R += (real((_vSIS[i].H() * _vSIS[i]) (0, 0)) / iRankI) * IH * IH.H();
    }

    // 根据弱干扰累计干扰相关矩阵
    for (int i = 0; i < iSizeW; ++i) {
        const itpp::cmat& mWIS = _vWIS[i];
        double dLinkLoss = _vWIL[i];
        double dWeakI = real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
        R += dWeakI * itpp::eye_c(R.rows());
    }
    R += Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    itpp::cmat invR = itpp::inv(R);
    itpp::cmat SH = _mH * _mP; // 计算预编码后的等效信道矩阵
    //Channel Estimation Tag
    if (Parameters::Instance().ERROR.IChannelMatrixError == 1) {
    //    if (invert % 2 == 1){
        itpp::cmat mHe = _mH*_mP;
        cmat mNewHe;
        mNewHe.set_size(mHe.rows(), mHe.cols());
        vector<cmat> vSIHe;
        int iSizeSI = static_cast<int> (_vSIS.size());
        int iSizeWI = static_cast<int> (_vWIS.size());
        for (int i = 0; i < iSizeSI; ++i) {
            vSIHe.emplace_back(_vSIH[i] * _vSIP[i]);
        }
        double dNoise = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
        for (int i = 0; i < mHe.rows(); ++i) {
            for (int j = 0; j < mHe.cols(); ++j) {
                double dSignal = pow(abs(mHe(i, j)), 2) * pow(abs(_mS(j, 0)), 2);
                double dInterference = 0;
                for (int k = 0; k < iSizeSI; ++k) {
                    if (j >= _vSIS[k].rows()) continue;
                    dInterference += pow(abs(vSIHe[k](i, j)), 2) * pow(abs(_vSIS[k](j, 0)), 2);
                }
                for (int k = 0; k < iSizeWI; ++k) {
                    if (j >= _vWIS[k].rows()) continue;
                    dInterference += pow(abs(_vWIS[k](j, 0)), 2) * _vWIL[k];
                }
                double dSINR = dSignal / (dInterference + dNoise);
                double dSINRDB = L2DB(dSINR);
    //            double dA = 0.2711;
    //            double dB = 0.0933;
    //            double dSigmaStd = dA * exp(-1 * dB * dSINRDB);
    //            double dSigmaVar = pow(dSigmaStd, 2);
    //            double dMSESigmaVar = abs(min(0.49, dSigmaVar / (1 - 2 * dSigmaVar)));

                double dDeltaMSE = 9.0;                             // DeltaMSE = 7~9 dB
                double dE = 1 / (dSINR * DB2L(dDeltaMSE));
                double dAlfa = sqrt(dSINR * DB2L(dDeltaMSE) / (1 + dSINR * DB2L(dDeltaMSE)));

                complex<double> cOffset = complex<double>(random.xNormal_DMRSError(0, sqrt(dE)) * abs(mHe(i, j)), random.xNormal_DMRSError(0, sqrt(dE)) * abs(mHe(i, j)));
                mNewHe(i, j) = dAlfa * (mHe(i, j) + cOffset);

            }
        }
        SH=mNewHe;
    }
    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (real((_mS.H() * _mS) (0, 0)) / iRankA)) * SH.H() * invR; // 计算均衡矩阵
//    if (Parameters::Instance().MIMO_CTRL.IMMSEOption == 0) {
//        itpp::cmat temp = (real((_mS.H() * _mS) (0, 0)) / iRankA) * SH * SH.H();
//        itpp::cmat temp2 = temp + Rc + R;
//        itpp::cmat temp3 = itpp::inv(temp2);
//        G = SH.H() * temp3;
////        G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRankA) * SH * SH.H() + Rc + R);
//    } else if (Parameters::Instance().MIMO_CTRL.IMMSEOption == 1) {
//        R = itpp::diag(itpp::diag(R));
//        Rc = itpp::diag(itpp::diag(Rc));
//        G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRankA) * SH * SH.H() + Rc + R);
//    } else if (Parameters::Instance().MIMO_CTRL.IMMSEOption == 2) {
//        R = itpp::diag(itpp::diag(R));
//        G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRankA) * SH * SH.H() + Rc + R);
//    } else {
//        assert(false);
//    }

    itpp::cmat W = G * _mH * _mPA; // 计算均衡后的等效信道矩阵
   // cout<<G<<endl;
    itpp::cmat mSTemp = _mS;
    for (int iRankIndex = 1; iRankIndex < iRank; ++iRankIndex) {
        mSTemp = itpp::concat_horizontal(mSTemp, _mS);
    }
    itpp::cmat symbol_de = itpp::elem_mult(W.T(), mSTemp); //计算均衡后的接收符号
    itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率
    //得到本用户的信号功率
    itpp::mat SS = zeros(iRank, 1);
    for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
        SS(iRankIndex, 0) = S(vRankID[iRankIndex], iRankIndex);
    //    cout<<SS(iRankIndex, 0)<<endl;
    }

    // step3: 计算干扰功率
    // 计算多用户流之间的干扰，如果是单流的情况计算结果为0
    itpp::mat ISelfR = zeros(iRank, 1);
    for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
        ISelfR(iRankIndex, 0) = (itpp::ones(1, iRankA) * S.get_cols(iRankIndex, iRankIndex))(0, 0) - SS(iRankIndex, 0);
    }
    itpp::mat I = ISelfR;
    // 计算来自其他强干扰源的干扰
    for (int i = 0; i < iSizeI; ++i) {
        itpp::cmat IH = _vSIH[i] * _vSIP[i]; // 干扰加预编码后的等效信道矩阵
        itpp::cmat IW = G * IH; // 经过均衡后的等效信道矩阵
        itpp::cmat other_rx = IW * itpp::diag(_vSIS[i].get_col(0)); //
        itpp::mat Interf = real(elem_mult(conj(other_rx), other_rx)) * itpp::ones(_vSIP[i].cols(), 1);
        assert((Interf.rows() == iRank) && (Interf.cols() == 1));
        I += Interf;
    }
    // 将弱干扰的功率累计到噪声中
    double dCarrierNoiseMW = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
    for (int i = 0; i < iSizeW; ++i) {
        double dLinkLoss = _vWIL[i];
        dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i]) (0, 0)) * dLinkLoss;
    }
    itpp::mat mNoise = dCarrierNoiseMW * real(diag(diag(G * G.H())));
    assert((mNoise.rows() == iRank) && (mNoise.cols() == iRank));
    for (int i = 0; i < iRank; ++i) {
        I(i, 0) += mNoise(i, i);
    }
    //cout<<I;
    itpp::mat mSINR = itpp::elem_div(SS, I);
    // mSINR = itpp::elem_div(DB2L(Parameters::Instance().ERROR.DEVMGateDb) * mSINR, DB2L(Parameters::Instance().ERROR.DEVMGateDb) * itpp::ones(mSINR.rows(), mSINR.cols()) + mSINR);
    mSINR = calcEVM_ERROR(mSINR);
    return mSINR;
}



///用于有DMRS误差的SUMIMO的SINR，采用瞬时干扰

mat MMSE_DL::CalculateSINRWithDMRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mHe,
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
        assert(_vSIP[i].rows() == _vSIH[i].cols());
        assert(_vSIH[i].rows() == iMSAntNum);
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
    R += Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    itpp::cmat invR = itpp::inv(R);

    // step2: 计算信号的功率
    const itpp::cmat& SH = _mHe; //计算预编码后的等效信道矩阵
    assert(_mHe.rows() == _mH.rows() && _mHe.cols() == _mP.cols());
    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (real((_mS.H() * _mS) (0, 0)) / iRank)) * SH.H() * invR; // 计算均衡矩阵
    if (Parameters::Instance().MIMO_CTRL.IMMSEOption == 1) {
        R = itpp::diag(itpp::diag(R));
        G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRank) * SH * SH.H() + R);
    } else {
        G = SH.H() * itpp::inv((real((_mS.H() * _mS) (0, 0)) / iRank) * SH * SH.H() + R);
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
        dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i]) (0, 0)) * dLinkLoss;
    }

    // 计算信干噪比
    itpp::mat mSINR = itpp::elem_div(S, dCarrierNoiseMW * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)) + I);
    return mSINR;
}
///用于有DMRS误差的MUMIMO的SINR，采用瞬时

mat MMSE_DL::CalculateSINRWithDMRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP, const cmat& _mPA, const cmat& _mHe,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH.cols(); //BS天线数
    int iMSAntNum = _mH.rows(); //MS天线数
    int iRank = _mP.cols(); //本（单）用户的流数
    int iRankA = _mPA.cols(); //多用户数（多用户流数）

    //输入参数校验
    assert(iRank == 1);
    assert(_mS.rows() == iRankA);
    assert(_mP.rows() == iBSAntNum);
    assert(_mPA.rows() == iBSAntNum);
    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    assert(_vWIS.size() == _vWIL.size());
    for (int i = 0; i < iSizeI; ++i) {
        assert(_vSIS[i].rows() == _vSIP[i].cols());
        assert(_vSIP[i].rows() == iBSAntNum);
        assert(_vSIH[i].rows() == iMSAntNum);
        assert(_vSIH[i].cols() == iBSAntNum);
    }

    //找到本用户是多用户流中的第几流
    int iRankID = -1;
    for (int icol = 0; icol < iRankA; ++icol) {
        if (_mP == _mPA.get_cols(icol, icol)) {
            iRankID = icol;
            break;
        }
    }

    assert(iRankID != -1);
    if (iRankID == -1) {
        cerr << "Can't find the correct rank id!";
        exit(0);
    }

    // step1: 计算干扰相关矩阵
    itpp::cmat R(iMSAntNum, iMSAntNum);
    R.zeros();
    // 根据流间干扰累计干扰相关矩阵
    if (iRankA > iRank) {
        itpp::cmat t = itpp::ones_c(iRankA, 1);
        t(iRankID, 0) = 0;
        itpp::cmat mTH = _mHe * t;
        itpp::cmat mTS = t.T() * _mS;
        R += real((mTS.H() * mTS)(0, 0)) * mTH * mTH.H();
    }

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
    R += Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    itpp::cmat invR = itpp::inv(R);

    // step2: 计算信号的功率
    itpp::cmat SH = _mHe.get_cols(iRankID, iRankID); // 计算预编码后的等效信道矩阵
    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (Parameters::Instance().Macro.DL.DSCTxPowerMw / iRankA)) * SH.H() * invR; // 计算均衡矩阵
    itpp::cmat W = G * _mHe; // 计算均衡后的等效信道矩阵

    itpp::cmat symbol_de = itpp::elem_mult(W.T(), _mS); //计算均衡后的接收符号
    itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率
    itpp::mat SS = S.get_rows(iRankID, iRankID); //得到本用户的信号功率

    // step3: 计算干扰功率
    itpp::mat I;
    I.zeros();
    double dCarrierNoiseMW = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;

    // 计算多用户流之间的干扰，如果是单流的情况计算结果为0
    itpp::mat ISelfR = itpp::ones(1, S.rows()) * S - SS;
    I += ISelfR;

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
        dCarrierNoiseMW += real((_vWIS[i].H() * _vWIS[i]) (0, 0)) * dLinkLoss;
    }

    itpp::mat mNoise = dCarrierNoiseMW * real(diag(diag(G * G.H())));
    itpp::mat mSINR = itpp::elem_div(SS, mNoise + I);
    return mSINR;
}
///用于TxD CQI的SINR，采用瞬时干扰

mat MMSE_DL::CalculateCQIwithSFBC(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH.cols(); //BS天线数
    int iMSAntNum = _mH.rows(); //MS天线数
    int iRank = _mP.cols(); //流数

    //输入参数校验
    assert(iRank == 2);
    assert(_mS.rows() == iRank);
    assert(_mP.rows() == iBSAntNum);
    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    assert(_vWIS.size() == _vWIL.size());
    for (int i = 0; i < iSizeI; ++i) {
        assert(_vSIS[i].rows() == _vSIP[i].cols());
        assert(_vSIP[i].rows() == _vSIH[i].cols());
        assert(_vSIH[i].rows() == iMSAntNum);
    }
    // step1: 计算有用信号功率
    itpp::cmat SH = _mH * _mP;
    double dGain = real(trace(SH * SH.H()));
    double dSignal = real((_mS.H() * _mS)(0, 0)) * pow(dGain, 2);
    // step2: 弱干扰和噪声
    double dNoise = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
    for (int i = 0; i < iSizeW; ++i) {
        const itpp::cmat& mWIS = _vWIS[i];
        double dLinkLoss = _vWIL[i];
        dNoise += real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
    }
    dNoise *= dGain;
    // 强干扰
    double dInterf = 0;
    /*
    for (int i = 0; i < iSizeI; ++i) {
        itpp::cmat IH = _vSIH[i] * _vSIP[i];
        double dIPower = real((_vSIS[i].H() * _vSIS[i]) (0, 0));
        for (int itx = 0; itx < static_cast<int> (_vSIS[i].rows()); ++itx) {
            complex<double> cInterf1 = 0;
            complex<double> cInterf2 = 0;
            for (int irx = 0; irx < iMSAntNum; ++irx) {
                cInterf1 += (conj(SH(irx, 0)) * IH(irx, itx));
                cInterf2 += (SH(irx, 1) * conj(IH(irx, itx)));
            }
            dInterf += (dIPower * (pow(abs(cInterf1), 2) + pow(abs(cInterf2), 2)));
        }
    }
     */
    for (int i = 0; i < iSizeI; ++i) {
        itpp::cmat IY = _vSIH[i] * _vSIP[i] * _vSIS[i];  
        dInterf += (dGain * real(IY.H() * IY)(0, 0));
    }

    itpp::mat mSINR = itpp::zeros(1, 1);
    mSINR(0, 0) = dSignal / (dNoise + dInterf);
    // EVM误差
    // mSINR = itpp::elem_div(DB2L(Parameters::Instance().ERROR.DEVMGateDb) * mSINR, DB2L(Parameters::Instance().ERROR.DEVMGateDb) * itpp::ones(mSINR.rows(), mSINR.cols()) + mSINR);
    mSINR = calcEVM_ERROR(mSINR);
    return mSINR;
}

double MMSE_DL::CalculateCRSSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH.cols(); //BS天线数
    int iMSAntNum = _mH.rows(); //MS天线数
    int iRank = _mP.cols(); //流数

    //输入参数校验
    assert(iRank == 2);
    assert(_mS.rows() == iRank);
    assert(_mP.rows() == iBSAntNum);
    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    assert(_vWIS.size() == _vWIL.size());
    for (int i = 0; i < iSizeI; ++i) {
        assert(_vSIS[i].rows() == _vSIP[i].cols());
        assert(_vSIP[i].rows() == _vSIH[i].cols());
        assert(_vSIH[i].rows() == iMSAntNum);
    }
    // step1: 计算有用信号功率
    itpp::cmat SH = _mH * _mP;
    double dTxPower = real((_mS.H() * _mS)(0, 0));
    double dSignal_Rx0_port0 = pow(abs(SH(0, 0)), 2) * dTxPower / 2;
    double dSignal_Rx0_port1 = pow(abs(SH(0, 1)), 2) * dTxPower / 2;
    double dSignal_Rx1_port0 = pow(abs(SH(1, 0)), 2) * dTxPower / 2;
    double dSignal_Rx1_port1 = pow(abs(SH(1, 1)), 2) * dTxPower / 2;

    // step2: 弱干扰和噪声
    double dNoise = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
    for (int i = 0; i < iSizeW; ++i) {
        const itpp::cmat& mWIS = _vWIS[i];
        double dLinkLoss = _vWIL[i];
        dNoise += real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
    }
    // 强干扰
    double dInterf_Rx0 = 0;
    double dInterf_Rx1 = 0;
    for (int i = 0; i < iSizeI; ++i) {
        itpp::cmat IH = _vSIH[i] * _vSIP[i];
        cmat H = IH * IH.H();
        double dPI = real((_vSIS[i].H() * _vSIS[i]) (0, 0));
        dInterf_Rx0 += real(H(0, 0)) * dPI / 2;
        dInterf_Rx1 += real(H(1, 1)) * dPI / 2;
    }

    double dSINR_Rx0_Port0 = dSignal_Rx0_port0 / (dNoise + dInterf_Rx0);
    double dSINR_Rx0_Port1 = dSignal_Rx0_port1 / (dNoise + dInterf_Rx0);
    double dSINR_Rx1_Port0 = dSignal_Rx1_port0 / (dNoise + dInterf_Rx1);
    double dSINR_Rx1_Port1 = dSignal_Rx1_port1 / (dNoise + dInterf_Rx1);

    double dSINR = (dSINR_Rx0_Port0 + dSINR_Rx0_Port1 + dSINR_Rx1_Port0 + dSINR_Rx1_Port1) / 2;
    // EVM误差
    // dSINR = DB2L(Parameters::Instance().ERROR.DEVMGateDb) * dSINR / (DB2L(Parameters::Instance().ERROR.DEVMGateDb) + dSINR);
    dSINR = calcEVM_ERROR(dSINR);
    
    return dSINR;
}

///用于计算TM3时单流SFBC的SINR

mat MMSE_DL::CalculateCQIwithSFBC_TM3(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH.cols(); //BS天线数
    int iMSAntNum = _mH.rows(); //MS天线数
    int iRank = _mP.cols(); //流数

    //输入参数校验
    assert(iRank == 2);
    assert(_mS.rows() == iRank);
    assert(_mP.rows() == iBSAntNum);
    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    assert(_vWIS.size() == _vWIL.size());
    for (int i = 0; i < iSizeI; ++i) {
        assert(_vSIS[i].rows() == _vSIP[i].cols());
        assert(_vSIP[i].rows() == _vSIH[i].cols());
        assert(_vSIH[i].rows() == iMSAntNum);
    }
    // step1: 计算有用信号功率
    itpp::cmat SH = _mH * _mP;
    double dGain = real(trace(SH * SH.H()));
    double dSignal = real((_mS.H() * _mS)(0, 0)) * pow(dGain, 2);
    // step2: 弱干扰和噪声
    double dNoise = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
    for (int i = 0; i < iSizeW; ++i) {
        const itpp::cmat& mWIS = _vWIS[i];
        double dLinkLoss = _vWIL[i];
        dNoise += real((mWIS.H() * mWIS) (0, 0)) * dLinkLoss;
    }
    dNoise *= dGain;
    // 强干扰
    double dInterf = 0;

    for (int i = 0; i < iSizeI; ++i) {
        itpp::cmat IH = _vSIH[i] * _vSIP[i];
        double dIPower = real((_vSIS[i].H() * _vSIS[i]) (0, 0));
        for (int itx = 0; itx < static_cast<int> (_vSIS[i].rows()); ++itx) {
            complex<double> cInterf1 = 0;
            complex<double> cInterf2 = 0;
            for (int irx = 0; irx < iMSAntNum; ++irx) {
                cInterf1 += (conj(SH(irx, 0)) * IH(irx, itx));
                cInterf2 += (SH(irx, 1) * conj(IH(irx, itx)));
            }
            dInterf += (dIPower * (pow(abs(cInterf1), 2) + pow(abs(cInterf2), 2)));
        }
    }

    itpp::mat mSINR = itpp::zeros(1, 1);
    mSINR(0, 0) = dSignal / (dNoise + dInterf);
    // EVM误差
    // mSINR = itpp::elem_div(DB2L(Parameters::Instance().ERROR.DEVMGateDb) * mSINR, DB2L(Parameters::Instance().ERROR.DEVMGateDb) * itpp::ones(mSINR.rows(), mSINR.cols()) + mSINR);
    mSINR = calcEVM_ERROR(mSINR);
    return mSINR;
}


//unicom xr-sinr-check202007