///@file MMSEAlgorithm.cpp
///@brief  MMSE检测算法的实现
///@author wangfei

#include "../Statistician/Observer.h"
#include "Detector_UL.h"
#include "MMSEAlgorithm.h"
#include "../NetworkDrive/Clock.h"

///@brief MMSE算法计算SINR
///
///@param _mS 有用信号的符号
///@param _mH 有用信号经过的信道矩阵（包括大尺度和小尺度)
///@param _mP 有用信号的PMI
///@param _vSIS 所有强干扰的发送信号
///@param _vSIH 所有强干扰的发送信号经过的信道矩阵(包括大尺度和小尺度）
///@param _vSIP 所有强干扰的PMI
///@param _vWIS 所有弱干扰的发送信号
///@param _vWIL 所有弱干扰的大尺度
///@return  返回一个SINR的矩阵
///@see    
///@note  

mat MMSEAlgorithm::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {

    ///确定信号的流数和接收天线数
    int iNs = static_cast<int> (_mS.size());
    int iNr = _mH.rows();
    ///计算干扰相关阵
    cmat mRe(iNr, iNr);
    mRe.zeros();
    vector<cmat> vSHPtemp, vW;//every  _vSIH[i] * _vSIP[i]
    vSHPtemp.resize(_vSIS.size());
    vW.resize(_vWIS.size());//every _vWIL[i]*(_vWIS[i].H() * _vWIS[i])
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        cmat temp = _vSIH[i] * _vSIP[i];
        vSHPtemp[i] = temp;
        cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
        mCovSIS = diag(diag(mCovSIS)); //取对角线
        mRe += temp * mCovSIS * temp.H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        cmat w = _vWIL[i] * (_vWIS[i].H() * _vWIS[i]);
        vW[i] = w;
        mRe += w (0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
    }
    //    if (P.SIM.UL.BIsMMSEOptionOneOn) {
    //        mRe = diag(diag(mRe));
    //    }
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    mRe += dNoisePSD * eye_c(iNr);
    ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
    cmat mCovS = _mS * _mS.H();
    mCovS = diag(diag(mCovS)); //取对角线
    //mmse option-1
    if (Parameters::Instance().SIM_UL.UL.BIsMMSEOptionOneOn) {
        mRe = diag(diag(mRe));
    }

    ///计算等效矩阵
    cmat mHequivalent = _mH * _mP;
    ///DMRS信道误差建模
//    if (Parameters::Instance().ERROR.IDMRS_Error == 1) {
//    if (Parameters::Instance().ERROR.UL.IChannelMatrixError == 1) {
//上行的信道误差建模是错的！！！计算出来的SINR可能会偏大或者偏小？？？
        if(false){
        // step2: 计算信号的功率
        //itpp::cmat mHe = _mH*_mP;
        cmat mNewHe;
        mNewHe.set_size(mHequivalent.rows(), mHequivalent.cols());
        vector<cmat> vSIHe;
        int iSizeSI = static_cast<int> (_vSIS.size());
        int iSizeWI = static_cast<int> (_vWIS.size());
        for (int i = 0; i < iSizeSI; ++i) {
            vSIHe.emplace_back(_vSIH[i] * _vSIP[i]);
        }
        double dNoise = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
        for (int i = 0; i < mHequivalent.rows(); ++i) {
            for (int j = 0; j < mHequivalent.cols(); ++j) {
                double dSignal = pow(abs(mHequivalent(i, j)), 2) * pow(abs(_mS(j, 0)), 2);
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
                double dDeltaMSE = 9.0;                             // DeltaMSE = 7~9 dB
                double dE = 1 / (dSINR * DB2L(dDeltaMSE));
                double dAlfa = sqrt(dSINR * DB2L(dDeltaMSE) / (1 + dSINR * DB2L(dDeltaMSE)));

                complex<double> cOffset = complex<double>(random.xNormal_DMRSError(0, sqrt(dE)) * abs(mHequivalent(i, j)), random.xNormal_DMRSError(0, sqrt(dE)) * abs(mHequivalent(i, j)));
                mNewHe(i, j) = dAlfa * (mHequivalent(i, j) + cOffset);
            }
        }
        mHequivalent = mNewHe;
    }

    ///计算均衡矩阵(解调矩阵)
    //cmat mG = (inv((_mH * _mP) * mCovS * (_mH * _mP).H() + mRe)*(_mH * _mP)).H();
    cmat mG = (inv(mHequivalent * mCovS * mHequivalent.H() + mRe) * mHequivalent).H();
    ///定义两个中间变量mD,mIself
    cmat mD = diag(diag(mG * _mH * _mP));
    cmat mIself = mG * _mH * _mP - mD;
    ///计算有用信号分量
    cmat mSignal = mD*_mS;
    vector<double> vSignal;
    for (int i = 0; i < iNs; ++i) {
        double dSignal = pow(abs(mSignal(i, 0)), 2);
        vSignal.push_back(dSignal);
    }
    ///计算流间干扰分量
    cmat mIntraI = mIself*_mS;
    vector<double> vIntraI;
    for (int i = 0; i < iNs; ++i) {
        double dIntraI = pow(abs(mIntraI(i, 0)), 2);
        vIntraI.push_back(dIntraI);
    }
    ///计算小区间干扰分量
    cmat mInterI(iNs, iNs);
    mInterI.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        cmat tempresult=mG * vSHPtemp[i] * _vSIS[i];
        mInterI += tempresult*tempresult.H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        mInterI += abs(vW[i](0, 0)) * mG * mG.H();
    }
    vector<double> vInterI;
    for (int i = 0; i < iNs; ++i) {
        double dInterI = abs(mInterI(i, i));
        vInterI.push_back(dInterI);
    }
    ///计算噪声分量
    cmat mNoise(iNs, iNs);
    mNoise = dNoisePSD * mG * mG.H();
    vector<double> vNoise;
    for (int i = 0; i < iNs; ++i) {
        double dNoise = abs(mNoise(i, i));
        vNoise.push_back(dNoise);
    }
    mat mSINR(iNs, 1);
    for (int i = 0; i < iNs; ++i) {
        double dSINR = vSignal[i] / (vIntraI[i] + vInterI[i] + vNoise[i]);
        mSINR(i, 0) = dSINR;
    }
    return mSINR;
}

mat MMSEAlgorithm::CalculateMUSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        //        const cmat& _mEffectiveP,
        const cmat& _mPA,
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
        assert(_vSIP[i].rows() == iBSAntNum); //??
        assert(_vSIP[i].rows() == _vSIH[i].cols());
        assert(_vSIH[i].rows() == iMSAntNum);
        assert(_vSIH[i].cols() == iBSAntNum); //??
    }

    //找到本用户是多用户流中的第几流
    int iRankID = -1;
    for (int icol = 0; icol < iRankA; ++icol) {
        if (_mP == _mPA.get_cols(icol, icol)) { //这个为什么是,为什么是get_cols ?
            iRankID = icol;
            break;
        }
    }
    //    for (int icol = 0; icol < iRankA; ++icol) {
    //        cout << "_mPA.get_cols(icol, icol) = " << _mPA.get_cols(icol, icol) << endl;
    //        if (_mP == _mPA.get_cols(icol, icol)) { //这个为什么是,为什么是get_cols ?
    //            cout<<"_mP = "<<_mP<<endl;
    //        }
    //    }

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
        itpp::cmat mP2 = _mPA * t;
        itpp::cmat mTH = _mH * mP2;
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
    double DSCNoisePowerMw = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    R += DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    itpp::cmat invR = itpp::inv(R);

    // step2: 计算信号的功率
    itpp::cmat SH = _mH * _mP; // 计算预编码后的等效信道矩阵
    //如果两个流的功率不相等，下面的公式需要更新，当前暂时用相等的情况计算
    //    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (P.BTS.DL.DSCTxPowerMw / iRankA)) * SH.H() * invR; // 计算均衡矩阵
    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (real(conj(_mS(iRankID)) * _mS(iRankID)) / iRank)) * SH.H() * invR; // 计算均衡矩阵
    itpp::cmat W = G * _mH * _mPA; // 计算均衡后的等效信道矩阵


    itpp::cmat symbol_de = itpp::elem_mult(W.T(), _mS); //计算均衡后的接收符号

    itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率

    itpp::mat SS = S.get_rows(iRankID, iRankID); //得到本用户的信号功率

    // step3: 计算干扰功率
    itpp::mat I;
    I.zeros();
    double dCarrierNoiseMW = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);

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


    itpp::mat mIoT = itpp::elem_div(dCarrierNoiseMW * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)) + I,
            Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)));

    return mSINR;

    //    ///确定信号的流数和接收天线数
    //    int iNs = static_cast<int> (_mS.size());
    //    int iNr = _mH.rows();
    //    if (1 == iNs) {
    //        assert(_mP == _mEffectiveP);
    //        ///判断有用信号是第几流
    //        cmat mEffectiveS = _mS;
    //
    //        ///计算干扰相关阵
    //        cmat mRe(iNr, iNr);
    //        mRe.zeros();
    //
    //        for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
    //            cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
    //            mCovSIS = diag(diag(mCovSIS)); //取对角线
    //            mRe += (_vSIH[i] * _vSIP[i]) * mCovSIS * (_vSIH[i] * _vSIP[i]).H();
    //        }
    //
    //        for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
    //            mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
    //        }
    //
    //        double dNoisePSD = P.BASIC.IDLORUL == 1 ? (P.MSS.DL.DSCNoisePowerMw) : (P.BTS.UL.DSCNoisePowerMw);
    //        mRe += dNoisePSD * eye_c(iNr);
    //
    //        ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
    //        cmat mCovS = mEffectiveS * mEffectiveS.H();
    //        mCovS = diag(diag(mCovS)); //取对角线
    //        ///计算均衡矩阵
    //        cmat mG = (inv((_mH * _mEffectiveP) * mCovS * (_mH * _mEffectiveP).H() + mRe)*(_mH * _mEffectiveP)).H();
    //        ///计算有用信号分量(仅有一流)
    //        cmat mSignal = (mG * _mH * _mEffectiveP * mEffectiveS)*(mG * _mH * _mEffectiveP * mEffectiveS).H();
    //        double dSignal = abs(mSignal(0, 0));
    //
    //        ///计算小区间干扰分量
    //        cmat mInterI(1, 1);
    //        mInterI.zeros();
    //        for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
    //            mInterI += (mG * _vSIH[i] * _vSIP[i] * _vSIS[i])*(mG * _vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    //        }
    //        for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
    //            cmat mTemp = _vWIL[i] * _vWIS[i].H() * _vWIS[i];
    //            mInterI += abs(mTemp(0, 0)) * mG * mG.H();
    //        }
    //        double dInterI = abs(mInterI(0, 0));
    //
    //        ///计算噪声分量
    //        cmat mNoise = dNoisePSD * mG * mG.H();
    //        double dNoise = abs(mNoise(0, 0));
    //
    //        ///计算SINR
    //        double dSINR = dSignal / (dInterI + dNoise);
    //
    //        mat mSINR(1, 1);
    //        mSINR(0, 0) = dSINR;
    //        return mSINR;
    //    } else if (2 == iNs) {
    //        assert(_mP.cols() == iNs && _mEffectiveP.cols() == 1);
    //        ///判断有用信号是第几流
    //        cmat mEffectiveS(1, 1);
    //        cmat mIntraS(iNs - 1, iNs - 1);
    //        cmat mIntraP(P.BTS.DL.IAntennaNum, iNs - 1);
    //
    //        if (_mEffectiveP == _mP.get_cols(0, 0)) {///第一流为有用信号  ///zhengyi  ???
    //            mEffectiveS = _mS.get_rows(0, 0);
    //            mIntraS = _mS.get_rows(1, iNs - 1);
    //            mIntraP = _mP.get_cols(1, iNs - 1);
    //        } else {///第二流为有用信号
    //            assert(_mEffectiveP == _mP.get_cols(1, 1));
    //            mEffectiveS = _mS.get_rows(1, iNs - 1);
    //            mIntraS = _mS.get_rows(0, 0);
    //            mIntraP = _mP.get_cols(0, 0);
    //        }
    //        ///计算干扰相关阵
    //        cmat mRe(iNr, iNr);
    //        mRe.zeros();
    //
    //        cmat mCovIntraS = mIntraS * mIntraS.H();
    //        mCovIntraS = diag(diag(mCovIntraS)); //取对角线
    //        mRe += (_mH * mIntraP) * mCovIntraS * (_mH * mIntraP).H();
    //
    //        for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
    //            cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
    //            mCovSIS = diag(diag(mCovSIS)); //取对角线
    //            mRe += (_vSIH[i] * _vSIP[i]) * mCovSIS * (_vSIH[i] * _vSIP[i]).H();
    //        }
    //
    //        for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
    //            mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
    //        }
    //
    //        double dNoisePSD = P.BASIC.IDLORUL == 1 ? (P.MSS.DL.DSCNoisePowerMw) : (P.BTS.UL.DSCNoisePowerMw);
    //        mRe += dNoisePSD * eye_c(iNr);
    //
    //        ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
    //        cmat mCovS = mEffectiveS * mEffectiveS.H();
    //        mCovS = diag(diag(mCovS)); //取对角线
    //        ///计算均衡矩阵
    //        cmat mG = (inv((_mH * _mEffectiveP) * mCovS * (_mH * _mEffectiveP).H() + mRe)*(_mH * _mEffectiveP)).H();
    //        ///计算有用信号分量(仅有一流)
    //        cmat mSignal = (mG * _mH * _mEffectiveP * mEffectiveS)*(mG * _mH * _mEffectiveP * mEffectiveS).H();
    //        double dSignal = abs(mSignal(0, 0));
    //
    //        ///计算流间干扰分量
    //        cmat mIntraI = (mG * _mH * mIntraP * mIntraS)*(mG * _mH * mIntraP * mIntraS).H();
    //        double dIntraI = abs(mIntraI(0, 0));
    //
    //        ///计算小区间干扰分量
    //        cmat mInterI(1, 1);
    //        mInterI.zeros();
    //        for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
    //            mInterI += (mG * _vSIH[i] * _vSIP[i] * _vSIS[i])*(mG * _vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    //        }
    //        for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
    //            cmat mTemp = _vWIL[i] * _vWIS[i].H() * _vWIS[i];
    //            mInterI += abs(mTemp(0, 0)) * mG * mG.H();
    //        }
    //        double dInterI = abs(mInterI(0, 0));
    //
    //        ///计算噪声分量
    //        cmat mNoise = dNoisePSD * mG * mG.H();
    //        double dNoise = abs(mNoise(0, 0));
    //        ///计算SINR
    //        double dSINR = dSignal / (dIntraI + dInterI + dNoise);
    //
    //        mat mSINR(1, 1);
    //        mSINR(0, 0) = dSINR;
    //        return mSINR;
    //    } else {
    //        assert(false);
    //    }
}

mat MMSEAlgorithm::CalculateIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {

    ///确定信号的流数和接收天线数
    int iNs = static_cast<int> (_mS.size());
    int iNr = _mH.rows();
    ///计算干扰相关阵
    cmat mRe(iNr, iNr);
    mRe.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
        mCovSIS = diag(diag(mCovSIS)); //取对角线
        mRe += (_vSIH[i] * _vSIP[i]) * mCovSIS * (_vSIH[i] * _vSIP[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
    }
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    mRe += dNoisePSD * eye_c(iNr);
    ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
    cmat mCovS = _mS * _mS.H();
    mCovS = diag(diag(mCovS)); //取对角线
    ///计算均衡矩阵
    cmat mG = (inv((_mH * _mP) * mCovS * (_mH * _mP).H() + mRe)*(_mH * _mP)).H();
    ///定义两个中间变量mD,mIself
    cmat mD = diag(diag(mG * _mH * _mP));
    cmat mIself = mG * _mH * _mP - mD;
    ///计算有用信号分量
    cmat mSignal = mD*_mS;
    vector<double> vSignal;
    for (int i = 0; i < iNs; ++i) {
        double dSignal = pow(abs(mSignal(i, 0)), 2);
        vSignal.push_back(dSignal);
    }
    ///计算流间干扰分量
    cmat mIntraI = mIself*_mS;
    vector<double> vIntraI;
    for (int i = 0; i < iNs; ++i) {
        double dIntraI = pow(abs(mIntraI(i, 0)), 2);
        vIntraI.push_back(dIntraI);
    }
    ///计算小区间干扰分量
    cmat mInterI(iNs, iNs);
    mInterI.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        mInterI += (mG * _vSIH[i] * _vSIP[i] * _vSIS[i])*(mG * _vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        cmat mTemp = _vWIL[i] * _vWIS[i].H() * _vWIS[i];
        mInterI += abs(mTemp(0, 0)) * mG * mG.H();
    }
    vector<double> vInterI;
    for (int i = 0; i < iNs; ++i) {
        double dInterI = abs(mInterI(i, i));
        vInterI.push_back(dInterI);
    }
    ///计算噪声分量
    cmat mNoise(iNs, iNs);
    mNoise = dNoisePSD * mG * mG.H();
    vector<double> vNoise;
    for (int i = 0; i < iNs; ++i) {
        double dNoise = abs(mNoise(i, i));
        vNoise.push_back(dNoise);
    }
    mat mSINR(iNs, 1);
    mat mIOT(iNs, 1);
    for (int i = 0; i < iNs; ++i) {
        double dIOT = (vIntraI[i] + vInterI[i] + vNoise[i]) / vNoise[i];
        mIOT(i, 0) = dIOT;
    }
    return mIOT;
}

mat MMSEAlgorithm::CalculateMUIoT(const cmat& _mS, const cmat& _mH, const cmat& _mP,
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
    double DSCNoisePowerMw = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    R += DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    itpp::cmat invR = itpp::inv(R);

    // step2: 计算信号的功率
    itpp::cmat SH = _mH * _mP; //计算预编码后的等效信道矩阵
    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (real(_mS.H() * _mS)(0, 0) / iRank)) * SH.H() * invR; // 计算均衡矩阵
    itpp::cmat W = G * SH; //计算均衡后的等效信道矩阵

    itpp::cmat symbol_de = itpp::diag(itpp::diag(W)) * _mS; //计算均衡后的接收符号
    itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率

    // step3: 计算干扰功率
    itpp::mat I;
    I.zeros();
    double dCarrierNoiseMW = DSCNoisePowerMw;

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

    itpp::mat mIoT = itpp::elem_div(dCarrierNoiseMW * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)) + I, Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw * real(diag(diag(G * G.H())) * itpp::ones_c(iRank, 1)));
    return mIoT;
}

itpp::cmat MMSEAlgorithm::CalculateDetecMatrix(const cmat& _mS, const cmat& _mH, const cmat& _mP,
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
    double DSCNoisePowerMw = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    R += DSCNoisePowerMw * itpp::eye_c(iMSAntNum);
    itpp::cmat invR = itpp::inv(R);

    // step2: 计算信号的功率
    itpp::cmat SH = _mH * _mP; //计算预编码后的等效信道矩阵
    itpp::cmat G = itpp::inv(SH.H() * invR * SH + itpp::eye(iRank) / (real(_mS.H() * _mS)(0, 0) / iRank)) * SH.H() * invR; // 计算均衡矩阵

    return G;
}

///加入误差的SINR算法，
///输出中增加信道估计误差Herror,直接作用在等小信道上 H*P

mat MMSEAlgorithm::CalculateSINR_UL(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL, const mat& HerrorStd,
        const cmat& Herror) {

    ///确定信号的流数和接收天线数
    int iNs = static_cast<int> (_mS.size());
    int iNr = _mH.rows();
    ///计算干扰相关阵
    cmat mRe(iNr, iNr);
    mRe.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
        mCovSIS = diag(diag(mCovSIS)); //取对角线
        mRe += (_vSIH[i] * _vSIP[i]) * mCovSIS * (_vSIH[i] * _vSIP[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
    }
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    mRe += dNoisePSD * eye_c(iNr);
    ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
    cmat mCovS = _mS * _mS.H();
    mCovS = diag(diag(mCovS)); //取对角线
    ///计算等效矩阵
    //    assert(false); //后面这个没有计算信道的模值
    //DMRS信道估计误差
    cmat mHequivalent = _mH * _mP;
    //    cout << mHequivalent << endl;
    //error should apply to the non-equivalent channel
    for (int i = 0; i < mHequivalent.rows(); ++i) {
        double elementStd = HerrorStd._elem(i, 0);
        complex<double> Helement = mHequivalent._elem(i, 0);
        complex<double> Error = Herror._elem(i, 0);
        Helement += abs(Helement) * Error;
        Helement *= 1 / (1 + 2 * pow(elementStd, 2));
        mHequivalent.set(i, 0, Helement);
    }
    //mmse option-1
    if (Parameters::Instance().SIM_UL.UL.BIsMMSEOptionOneOn) {
        mRe = diag(diag(mRe));
    }

    ///计算均衡矩阵
    //    cmat mG = (inv((_mH * _mP) * mCovS * (_mH * _mP).H() + mRe)*(_mH * _mP)).H();
    cmat mG = (inv(mHequivalent * mCovS * mHequivalent.H() + mRe) * mHequivalent).H();
    ///定义两个中间变量mD,mIself
    //    cmat mD = diag(diag(mG * _mH * _mP));
    cmat mD = diag(diag(mG * _mH * _mP));
    //    cmat mIself = mG * _mH * _mP - mD;
    cmat mIself = mG * _mH * _mP - mD;
    //    cmat mIself_test = mG * _mH * _mP - mD;
    ///计算有用信号分量
    cmat mSignal = mD*_mS;
    vector<double> vSignal;
    for (int i = 0; i < iNs; ++i) {
        double dSignal = pow(abs(mSignal(i, 0)), 2);
        vSignal.push_back(dSignal);
    }
    ///计算流间干扰分量
    cmat mIntraI = mIself*_mS;
    vector<double> vIntraI;
    for (int i = 0; i < iNs; ++i) {
        double dIntraI = pow(abs(mIntraI(i, 0)), 2);
        vIntraI.push_back(dIntraI);
    }
    ///计算小区间干扰分量
    cmat mInterI(iNs, iNs);
    mInterI.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        mInterI += (mG * _vSIH[i] * _vSIP[i] * _vSIS[i])*(mG * _vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        cmat mTemp = _vWIL[i] * _vWIS[i].H() * _vWIS[i];
        mInterI += abs(mTemp(0, 0)) * mG * mG.H();
    }
    vector<double> vInterI;
    for (int i = 0; i < iNs; ++i) {
        double dInterI = abs(mInterI(i, i));
        vInterI.push_back(dInterI);
    }
    ///计算噪声分量
    cmat mNoise(iNs, iNs);
    mNoise = dNoisePSD * mG * mG.H();
    vector<double> vNoise;
    for (int i = 0; i < iNs; ++i) {
        double dNoise = abs(mNoise(i, i));
        vNoise.push_back(dNoise);
    }
    mat mSINR(iNs, 1);
    for (int i = 0; i < iNs; ++i) {
        double dSINR = vSignal[i] / (vIntraI[i] + vInterI[i] + vNoise[i]);
        mSINR(i, 0) = dSINR;
    }
    return mSINR;
}
mat MMSEAlgorithm::CalculateSINR_UL2(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL){

    ///确定信号的流数和接收天线数
    int iNs = static_cast<int> (_mS.size());
    int iNr = _mH.rows();
    ///计算干扰相关阵
    cmat mRe(iNr, iNr);
    mRe.zeros();
    vector<cmat> vSHPtemp, vW;//every  _vSIH[i] * _vSIP[i]
    vSHPtemp.resize(_vSIS.size());
    vW.resize(_vWIS.size());//every _vWIL[i]*(_vWIS[i].H() * _vWIS[i])
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        cmat temp = _vSIH[i] * _vSIP[i];
        vSHPtemp[i] = temp;
        cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
        mCovSIS = diag(diag(mCovSIS)); //取对角线
        mRe += temp * mCovSIS * temp.H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        cmat w = _vWIL[i] * (_vWIS[i].H() * _vWIS[i]);
        vW[i] = w;
        mRe += w(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
    }
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    mRe += dNoisePSD * eye_c(iNr);
    ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
    cmat mCovS = _mS * _mS.H();
    mCovS = diag(diag(mCovS)); //取对角线
    ///计算等效矩阵
    //    assert(false); //后面这个没有计算信道的模值
    //DMRS信道估计误差
    cmat mHequivalent = _mH * _mP;
    //    cout << mHequivalent << endl;
    //error should apply to the non-equivalent channel
   /* for (int i = 0; i < mHequivalent.rows(); ++i) {
        double elementStd = HerrorStd._elem(i, 0);
        complex<double> Helement = mHequivalent._elem(i, 0);
        complex<double> Error = Herror._elem(i, 0);
        Helement += abs(Helement) * Error;
        Helement *= 1 / (1 + 2 * pow(elementStd, 2));
        mHequivalent.set(i, 0, Helement);
    }*/

    ///DMRS信道误差建模
    //if (Parameters::Instance().ERROR.IDMRS_Error == 1) {
//    if (Parameters::Instance().ERROR.UL.IChannelMatrixError == 1) {
//上行的信道误差建模是错的！！！计算出来的SINR可能会偏大或者偏小？？？
//SINR应按 照金婧.LTE-Advanced系统中多天线协作研究-4.4.1 实现
    if(false){
//    if (invert % 2 == 1){
        // step2: 计算信号的功率
        //itpp::cmat mHe = _mH*_mP;
        cmat mNewHe;
        mNewHe.set_size(mHequivalent.rows(), mHequivalent.cols());
        vector<cmat> vSIHe;
        int iSizeSI = static_cast<int> (_vSIS.size());
        int iSizeWI = static_cast<int> (_vWIS.size());
        for (int i = 0; i < iSizeSI; ++i) {
            vSIHe.emplace_back(_vSIH[i] * _vSIP[i]);
        }
//        double dNoise = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
        for (int i = 0; i < mHequivalent.rows(); ++i) {
            for (int j = 0; j < mHequivalent.cols(); ++j) {
                double dSignal = pow(abs(mHequivalent(i, j)), 2) * pow(abs(_mS(j, 0)), 2);
                double dInterference = 0;
                for (int k = 0; k < iSizeSI; ++k) {
                    if (j >= _vSIS[k].rows()) continue;
                    dInterference += pow(abs(vSIHe[k](i, j)), 2) * pow(abs(_vSIS[k](j, 0)), 2);
                }
                for (int k = 0; k < iSizeWI; ++k) {
                    if (j >= _vWIS[k].rows()) continue;
                    dInterference += pow(abs(_vWIS[k](j, 0)), 2) * _vWIL[k];
                }
                double dSINR = dSignal / (dInterference + dNoisePSD);
                //cout << dNoisePSD ;
                double dSINRDB = L2DB(dSINR);
                //dSINRDB = min(dSINRDB,1.0);
                double dDeltaMSE = 9.0;                             // DeltaMSE = 7~9 dB
                double dE = 1 / (dSINR * DB2L(dDeltaMSE));
                double dAlfa = sqrt(dSINR * DB2L(dDeltaMSE) / (1 + dSINR * DB2L(dDeltaMSE)));

                complex<double> cOffset = complex<double>(random.xNormal_DMRSError(0, sqrt(dE)) * abs(mHequivalent(i, j)), random.xNormal_DMRSError(0, sqrt(dE)) * abs(mHequivalent(i, j)));
                mNewHe(i, j) = dAlfa * (mHequivalent(i, j) + cOffset);
            }
        }
        mHequivalent = mNewHe;
    }

    //mmse option-1
    if (Parameters::Instance().SIM_UL.UL.BIsMMSEOptionOneOn) {
        mRe = diag(diag(mRe));
    }

    ///计算均衡矩阵
    //    cmat mG = (inv((_mH * _mP) * mCovS * (_mH * _mP).H() + mRe)*(_mH * _mP)).H();
    cmat mG = (inv(mHequivalent * mCovS * mHequivalent.H() + mRe) * mHequivalent).H();
    ///定义两个中间变量mD,mIself
    //    cmat mD = diag(diag(mG * _mH * _mP));
    cmat mD = diag(diag(mG * _mH * _mP));
    //    cmat mIself = mG * _mH * _mP - mD;
    cmat mIself = mG * _mH * _mP - mD;
    //    cmat mIself_test = mG * _mH * _mP - mD;
    ///计算有用信号分量
    cmat mSignal = mD*_mS;
    vector<double> vSignal;
    for (int i = 0; i < iNs; i++) {
        
        double dSignal = pow(abs(mSignal(i, 0)), 2);
        vSignal.push_back(dSignal);
    }
    ///计算流间干扰分量
    cmat mIntraI = mIself*_mS;
    vector<double> vIntraI;
    for (int i = 0; i < iNs; ++i) {
        double dIntraI = pow(abs(mIntraI(i, 0)), 2);
        vIntraI.push_back(dIntraI);
    }
    ///计算小区间干扰分量
    cmat mInterI(iNs, iNs);
    mInterI.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        cmat tempresult=mG * vSHPtemp[i] * _vSIS[i];
        mInterI += tempresult*tempresult.H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        mInterI += abs(vW[i](0, 0)) * mG * mG.H();
    }
    vector<double> vInterI;
    for (int i = 0; i < iNs; ++i) {
        double dInterI = abs(mInterI(i, i));
        vInterI.push_back(dInterI);
    }
    ///计算噪声分量
    cmat mNoise(iNs, iNs);
    mNoise = dNoisePSD * mG * mG.H();
    vector<double> vNoise;
    for (int i = 0; i < iNs; ++i) {
        double dNoise = abs(mNoise(i, i));
        vNoise.push_back(dNoise);
    }
    mat mSINR(iNs, 1);
    for (int i = 0; i < iNs; ++i) {
        double dSINR = vSignal[i] / (vIntraI[i] + vInterI[i] + vNoise[i]);
        mSINR(i, 0) = dSINR;
    }

//    int iTime = Clock::Instance().GetTimeSlot();
//    if (iTime >=200 && iTime <=220)
//    {
//        double dSINR = 0.0;
//        for(int i=0;i<mSINR.rows();++i){
//            dSINR +=mSINR(i,0);
//        }
//        dSINR/=mSINR.rows();
//        dSINR = L2DB(dSINR);
//
//        Observer::Print("LinkMatrixError_SINR") << dSINR << setw(20);
//        cout<<dSINR<<"  ";
//        if (invert % 2 == 1) {
//            Observer::Print("LinkMatrixError_SINR") << endl;
//            cout<<endl;
//        }
//    }
//
//    invert ++;

    return mSINR;
}

mat MMSEAlgorithm::CalculateSRSSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL,
        const mat& HerrorScale,
        const cmat& Herror) {//包含误差的SRS SINR的计算

    ///确定信号的流数和接收天线数
    int iNs = static_cast<int> (_mS.size());
    int iNr = _mH.rows();
    ///计算干扰相关阵
    cmat mRe(iNr, iNr);
    mRe.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
        mCovSIS = diag(diag(mCovSIS)); //取对角线
        mRe += (_vSIH[i] * _vSIP[i]) * mCovSIS * (_vSIH[i] * _vSIP[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
    }
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    mRe += dNoisePSD * eye_c(iNr);
    ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
    cmat mCovS = _mS * _mS.H();
    mCovS = diag(diag(mCovS)); //取对角线
    ///计算等效矩阵
    //    assert(false); //后面这个没有计算信道的模值
    //SRS信道估计误差
    //SRS的误差应该不是作用在等效信道上
    assert(_mH.cols() == 1); //表示是单发的天线，如果是两发，则天线应该是两列
    cmat mHestimate = _mH;
    for (int i = 0; i < mHestimate.rows(); ++i) {
        double dalpha = HerrorScale._elem(i, 0);
        complex<double> Helement = mHestimate._elem(i, 0);
        complex<double> Error = Herror._elem(i, 0);
        Helement += abs(Helement) * Error;
        Helement *= dalpha;
        mHestimate.set(i, 0, Helement);
    }
    mHestimate *= _mP;
    ///计算均衡矩阵
    //    cmat mG = (inv((_mH * _mP) * mCovS * (_mH * _mP).H() + mRe)*(_mH * _mP)).H();
    cmat mG = (inv(mHestimate * mCovS * mHestimate.H() + mRe) * mHestimate).H();
    ///定义两个中间变量mD,mIself
    //    cmat mD = diag(diag(mG * _mH * _mP));
    cmat mD = diag(diag(mG * _mH * _mP));
    //    cmat mIself = mG * _mH * _mP - mD;
    cmat mIself = mG * _mH * _mP - mD;
    ///计算有用信号分量
    cmat mSignal = mD*_mS;
    vector<double> vSignal;
    for (int i = 0; i < iNs; ++i) {
        double dSignal = pow(abs(mSignal(i, 0)), 2);
        vSignal.push_back(dSignal);
    }
    ///计算流间干扰分量
    cmat mIntraI = mIself*_mS;
    vector<double> vIntraI;
    for (int i = 0; i < iNs; ++i) {
        double dIntraI = pow(abs(mIntraI(i, 0)), 2);
        vIntraI.push_back(dIntraI);
    }
    ///计算小区间干扰分量
    cmat mInterI(iNs, iNs);
    mInterI.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        mInterI += (mG * _vSIH[i] * _vSIP[i] * _vSIS[i])*(mG * _vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        cmat mTemp = _vWIL[i] * _vWIS[i].H() * _vWIS[i];
        mInterI += abs(mTemp(0, 0)) * mG * mG.H();
    }
    vector<double> vInterI;
    for (int i = 0; i < iNs; ++i) {
        double dInterI = abs(mInterI(i, i));
        vInterI.push_back(dInterI);
    }
    ///计算噪声分量
    cmat mNoise(iNs, iNs);
    mNoise = dNoisePSD * mG * mG.H();
    vector<double> vNoise;
    for (int i = 0; i < iNs; ++i) {
        double dNoise = abs(mNoise(i, i));
        vNoise.push_back(dNoise);
    }
    mat mSINR(iNs, 1);
    for (int i = 0; i < iNs; ++i) {
        double dSINR = vSignal[i] / (vIntraI[i] + vInterI[i] + vNoise[i]);
        mSINR(i, 0) = dSINR;
    }
    return mSINR;
}

mat MMSEAlgorithm::CalculateSINR_DmrsError(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL,
        const mat& HerrorScale,
        const cmat& Herror) {//包含误差的SRS SINR的计算

    ///确定信号的流数和接收天线数
    int iNs = static_cast<int> (_mS.size());
    int iNr = _mH.rows();
    ///计算干扰相关阵
    cmat mRe(iNr, iNr);
    mRe.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
        mCovSIS = diag(diag(mCovSIS)); //取对角线
        mRe += (_vSIH[i] * _vSIP[i]) * mCovSIS * (_vSIH[i] * _vSIP[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
    }
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    mRe += dNoisePSD * eye_c(iNr);
    ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
    cmat mCovS = _mS * _mS.H();
    mCovS = diag(diag(mCovS)); //取对角线
    ///计算等效矩阵
    // dmrs error works on equivalent channel
    cmat mHestimate = _mH * _mP;
    //    cout << _mH << endl;
    //    cout << _mP << endl;
    //    cout << mHestimate << endl;
    //consider multi-layer
    for (int i = 0; i < iNr; ++i) {
        for (int j = 0; j < iNs; ++j) {
            double dalpha = HerrorScale._elem(i, j);
            complex<double> Helement = mHestimate._elem(i, j);
            complex<double> Error = Herror._elem(i, j);

            Helement += abs(Helement) * Error;
            Helement *= dalpha;
            mHestimate.set(i, j, Helement);
        }
    }
    //    cout << mHestimate - _mH * _mP << endl;

    //mmse option-1
    if (Parameters::Instance().SIM_UL.UL.BIsMMSEOptionOneOn) {
        mRe = diag(diag(mRe));
    }
    ///计算均衡矩阵
    //    cmat mG = (inv((_mH * _mP) * mCovS * (_mH * _mP).H() + mRe)*(_mH * _mP)).H();
    cmat mG = (inv(mHestimate * mCovS * mHestimate.H() + mRe) * mHestimate).H();
    ///定义两个中间变量mD,mIself
    //    cmat mD = diag(diag(mG * _mH * _mP));
    cmat mD = diag(diag(mG * _mH * _mP));
    //    cmat mIself = mG * _mH * _mP - mD;
    cmat mIself = mG * _mH * _mP - mD;
    ///计算有用信号分量
    cmat mSignal = mD*_mS;
    vector<double> vSignal;
    for (int i = 0; i < iNs; ++i) {
        double dSignal = pow(abs(mSignal(i, 0)), 2);
        vSignal.push_back(dSignal);
    }
    ///计算流间干扰分量
    cmat mIntraI = mIself*_mS;
    vector<double> vIntraI;
    for (int i = 0; i < iNs; ++i) {
        double dIntraI = pow(abs(mIntraI(i, 0)), 2);
        vIntraI.push_back(dIntraI);
    }
    ///计算小区间干扰分量
    cmat mInterI(iNs, iNs);
    mInterI.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        mInterI += (mG * _vSIH[i] * _vSIP[i] * _vSIS[i])*(mG * _vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    }
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        cmat mTemp = _vWIL[i] * _vWIS[i].H() * _vWIS[i];
        mInterI += abs(mTemp(0, 0)) * mG * mG.H();
    }
    vector<double> vInterI;
    for (int i = 0; i < iNs; ++i) {
        double dInterI = abs(mInterI(i, i));
        vInterI.push_back(dInterI);
    }
    ///计算噪声分量
    cmat mNoise(iNs, iNs);
    mNoise = dNoisePSD * mG * mG.H();
    vector<double> vNoise;
    for (int i = 0; i < iNs; ++i) {
        double dNoise = abs(mNoise(i, i));
        vNoise.push_back(dNoise);
    }
    mat mSINR(iNs, 1);
    for (int i = 0; i < iNs; ++i) {
        double dSINR = vSignal[i] / (vIntraI[i] + vInterI[i] + vNoise[i]);
        mSINR(i, 0) = dSINR;
    }
    return mSINR;
}