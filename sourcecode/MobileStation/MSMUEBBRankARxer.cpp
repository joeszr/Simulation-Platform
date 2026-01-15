#include "../BaseStation/BTSID.h"
#include "../MobileStation/MSID.h"
#include "MSRxer.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../NetworkDrive/Clock.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../Utility/SCID.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../Scheduler/SchedulingMessageDL.h"
#include "../MobileStation/MS.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/BTS.h"
#include "../DetectAlgorithm/Detector_DL.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../MobileStation/CQIMessage.h"
#include "../MobileStation/SoundingMessage.h"
#include "MSEBBRank1Rxer.h"
#include "MSMUEBBRankARxer.h"
#include "../BaseStation/CodeBookFactory.h"
#include "../Statistician/Statistician.h"
#include <cassert>

MSMUEBBRankARxer::MSMUEBBRankARxer(BTSID _MainservBTSID, MSID _msid, vector<mat> *_pSINR, HARQRxStateMS* _pHARQRxStateMS)
: MSEBBRank1Rxer(_MainservBTSID, _msid, _pSINR, _pHARQRxStateMS) {
    //m_iMaxRank = Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel - 1;
    
    int iTotalChannel = Parameters::Instance().MSS.FirstBand.Polarize_Num * Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel;
    int iTotalChannel_2 = Parameters::Instance().MSS.FirstBand.Polarize_Num_2 * Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel_2 * Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel_2;
    
    if(Parameters::Instance().BASIC.ifcoexist && _msid.GetMS().gettype() == 2){
            //如果RedCap用户最大支持一流，将最大流数修改为一流
        m_iMaxRank = min(Parameters::Instance().MIMO_CTRL.IMaxRankNum, iTotalChannel_2) - 1;
    }else{
        m_iMaxRank = min(Parameters::Instance().MIMO_CTRL.IMaxRankNum, iTotalChannel) - 1;
    }
    
    m_iBestRank = m_iMaxRank;
    m_pCodeBook = CodeBookFactory::Instance().GetCodeBook(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num);
}

void MSMUEBBRankARxer::ComputeEstimateSINR() {
    if (Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1) {
        ComputeEstimateSINR_1TxSRSEnhanceCQI_AverageI(m_iBestRank);
    } else {
        ComputeEstimateSINR_TxDCQI_AverageI(m_iBestRank);
//        ComputeEstimateSINR_1TxSRSEnhanceCQI_AverageI(m_iBestRank);
    }
}

void MSMUEBBRankARxer::ComputeEstimateSINR_1TxSRSEnhanceCQI_AverageI(int _iRank) {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度

    BTS& mainbts = m_MainServBTS.GetBTS();
    MS& ms = m_MSID.GetMS();
    //子载波Loop
    for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
        //获取有用信号
        cmat mS = mainbts.GetTxCRS(_iRank + 1);

        (*m_pSINR)[scid.ToInt() / iSpace].set_size(_iRank + 1, 1);
        (*m_pSINR)[scid.ToInt() / iSpace] = itpp::zeros(_iRank + 1, 1);
        cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                mainbts, ms, scid.ToInt());
        cmat mP = m_mSubBandPrecode[scid.GetSBID().ToInt()];
        //获取干扰信息
        vector<cmat> vWIS;
        vector<double> vWIL;

        CalWeakInterference(vWIS, vWIL, m_MainServBTS, m_MSID, scid);

        //@brief 未重计算时采用以下，要考虑欧拉
        mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, m_vCovR[scid.ToInt() / iSpace], vWIS, vWIL);
        (*m_pSINR)[scid.ToInt() / iSpace] = mSINR;
    }
}
void MSMUEBBRankARxer::ComputeEstimateSINR_TxDCQI_AverageI(int _iRank) {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
    BTS& mainbts = m_MainServBTS.GetBTS();
    MS& ms = m_MSID.GetMS();
    for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
        (*m_pSINR)[scid.ToInt() / iSpace].set_size(_iRank+1, 1);
        (*m_pSINR)[scid.ToInt() / iSpace] = itpp::zeros(_iRank+1, 1);
        //获取有用信号
        cmat mS = mainbts.GetTxCRS(_iRank+1);
        cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                mainbts, ms, scid.ToInt());
//        //获取干扰信息
//        vector<cmat> vWIS;
//        vector<double> vWIL;
//
//        CalWeakInterference(vWIS, vWIL, m_MainServBTS, m_MSID, scid);
//
//        mat mSINR = m_pDetector->CalculateTxDSINR(mS, mH, m_vCovR[scid.ToInt() / iSpace], vWIS, vWIL);   //这个函数有问题，若干扰根本没用到，也没考虑流间干扰，返回的SINR是1x1的
//        (*m_pSINR)[scid.ToInt() / iSpace](0, 0) = mSINR(0,0);


        std::vector<itpp::cmat> vSIS, vSIH, vSIP, vWIS;
        std::vector<double> vWIL;

        //SVD分解
        cmat mU, mD;
        vec vS;
        svd(mH, mU, vS, mD);
        cmat mP = mD.get_cols(0, _iRank);
        {
            for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
                if (btsid == m_MainServBTS)
                    continue;
                ///按一定比例决定是否存在该干扰
                if (Parameters::Instance().BASIC.DInterfProb < 0.99) {
                    if (random.xUniform() > Parameters::Instance().BASIC.DInterfProb)
                        continue;
                }

                if (!btsid.GetBTS().IsExistTraffic())
                    continue;

                BTS& bts = btsid.GetBTS();
//                if (!bts.GetSBUsedFlag(scid.GetSBID()))
//                    continue;
                if (lm.IsStrong(bts, ms)) {
//                    vSIS.push_back(bts.GetTxDataSymbol(scid));
                    vSIS.push_back(bts.GetTxCRS(1));

                    vSIH.push_back(
                            lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs2(
                                    bts, ms, scid.ToInt(),
                                    btsid.GetBTS().GetvUEPanelAndvBSBeam()));
//                    vSIP.push_back(bts.GetPrecodeCodeWord(scid.GetSBID()));
                    vSIP.push_back(1 / sqrt(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num) * itpp::ones_c(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num, 1));
                } else {
//                    vWIS.push_back(bts.GetTxDataSymbol(scid));
                    vWIS.push_back(bts.GetTxCRS(1));
                    vWIL.push_back(DB2L(lm.GetCouplingLossDB(bts, ms)));
                }
            }
        }
//        CalInterference(vSIS, vSIH, vSIP, vWIS, vWIL,
//                        m_MainServBTS, m_MSID, scid);
        mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, mP, vSIS, vSIH, vSIP, vWIS, vWIL);

        double dSINR = 0.0;
        for(int i=0;i<mSINR.rows();++i){
            dSINR +=mSINR(i,0);
        }
        dSINR/=mSINR.rows();


        (*m_pSINR)[scid.ToInt() / iSpace](0, 0) = dSINR;
    }
}

void MSMUEBBRankARxer::ComputeReceiveSINR() {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    //存在调度信息，执行检测流程
    if (m_pScheduleMesDL != 0) {
        std::vector<SBID> vSBUsed = m_pScheduleMesDL->GetSBUsed();
        std::sort(vSBUsed.begin(), vSBUsed.end());
        int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace;
        BTS& mainbts = m_MainServBTS.GetBTS();
        MS& ms = m_MSID.GetMS();
        int iRank = m_pScheduleMesDL->GetRank();
        //RB Loop
        vector<double> vSCSINR;
        for (int i = 0; i < static_cast<int> (vSBUsed.size()); ++i) {
            SBID sbid = vSBUsed[i];
            //SC Loop
            for (SCID scid = sbid.GetFirstSCID(); scid <= sbid.GetLastSCID(); scid += iSpace) {
                itpp::cmat mP = m_pScheduleMesDL->GetCodeWord(sbid);
                assert(mP.cols() == (iRank + 1));
                itpp::cmat mPA = mainbts.GetPrecodeCodeWord(scid.GetSBID());
                itpp::cmat mS = mainbts.GetTxDataSymbol(scid);
                itpp::cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                        mainbts, ms, scid.ToInt());
                std::vector<itpp::cmat> vSIS, vSIH, vSIP, vWIS;
                std::vector<double> vWIL;

                CalInterference(vSIS, vSIH, vSIP, vWIS, vWIL,
                        m_MainServBTS, m_MSID, scid);
                //Channel Estimation Tag : Error!
//                if (Parameters::Instance().ERROR.IDMRS_Error == 1) {
//                    vector<MSID> vMSID = mainbts.GetSchedvMSID(scid.GetSBID());
//                    cmat mHe = DMRSErrorModel(vMSID, mS, mH, mPA, vSIS, vSIH, vSIP, vWIS, vWIL);
//                    mat mSINR = m_pDetector->CalculateSINRWithDMRSError(mS, mH, mP, mPA, mHe, vSIS, vSIH, vSIP, vWIS, vWIL);
//                    (*m_pSINR)[scid.ToInt() / iSpace] = mSINR;
//                } else {
//                    mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, mPA, vSIS, vSIH, vSIP, vWIS, vWIL);
//                    (*m_pSINR)[scid.ToInt() / iSpace] = mSINR;
//                }
                    mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, mPA, vSIS, vSIH, vSIP, vWIS, vWIL);
                //    mSINR = m_pDetector->CalculateSINR_UL2Re(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL ,mSINR);

                    //mat mSINRe = m_pDetector->CalculateSINR(mS, mH, mP, mPA, vSIS, vSIH, vSIP, vWIS, vWIL);

                    (*m_pSINR)[scid.ToInt() / iSpace] = mSINR;

            }

            for (SCID scid = sbid.GetFirstSCID(); scid <= sbid.GetLastSCID(); scid += iSpace) {
                double ave = 0.0;
                for(int rank = 0; rank < iRank + 1; rank++){
                    ave += (*m_pSINR)[scid.ToInt() / iSpace](rank, 0);
                }
                ave /= (iRank+1);
//                vSCSINR.push_back((*m_pSINR)[scid.ToInt() / iSpace](0, 0));
                vSCSINR.push_back(ave);
            }
        }
        double dAveSINR = std::accumulate(vSCSINR.begin(), vSCSINR.end(), 0.0) / vSCSINR.size();
        if(Clock::Instance().GetTimeSlot() >= Parameters::Instance().BASIC.IWarmUpSlot)
        {
            Statistician::Instance().m_MSData_DL[m_MSID.ToInt()].DL.m_dAveSINR += dAveSINR;
            Statistician::Instance().m_MSData_DL[m_MSID.ToInt()].DL.m_iSINRHit += 1;
        }
    }
}

cmat MSMUEBBRankARxer::DMRSErrorModel(const vector<MSID>& _vMSID, const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
//    no use _vMSID
//    cout<<static_cast<int> (_vMSID.size());
//    cout<<_mS.rows()<<endl;
//    assert(static_cast<int> (_vMSID.size()) == _mS.rows());
    cmat mHe = _mH*_mP;
    cmat mNewHe;
    mNewHe.set_size(mHe.rows(), mHe.cols());
    vector<cmat> vSIHe;
    int iSizeSI = static_cast<int> (_vSIS.size());
    int iSizeWI = static_cast<int> (_vWIS.size());
    for (int i = 0; i < iSizeSI; ++i) {
        vSIHe.push_back(_vSIH[i] * _vSIP[i]);
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
//            double dA;
//            double dB;
//            ///暂时只做了3 km/h的情况
//            MSID msid = _vMSID[j];
//            MS& ms = msid.GetMS();
//            BTS& bts = ms.GetMainServBTS().GetBTS();
//            if (cm::LinkMatrix::Instance().IsLOS(bts, ms)) {
//                dA = 0.2711;
//                dB = 0.0933;
//            } else {
//                dA = 0.2663;
//                dB = 0.09;
//            }


            double dDeltaMSE = 9.0;                             // DeltaMSE = 7~9 dB
            double dE = 1 / (dSINR * DB2L(dDeltaMSE));
            double dAlfa = sqrt(dSINR * DB2L(dDeltaMSE) / (1 + dSINR * DB2L(dDeltaMSE)));

            complex<double> cOffset = complex<double>(random.xNormal_DMRSError(0, sqrt(dE)) * abs(mHe(i, j)), random.xNormal_DMRSError(0, sqrt(dE)) * abs(mHe(i, j)));
            mNewHe(i, j) = dAlfa * (mHe(i, j) + cOffset);
        }
    }
    return mNewHe;
}

