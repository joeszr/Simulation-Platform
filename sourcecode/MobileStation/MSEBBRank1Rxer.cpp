///@file MSEBBRankOneRxer.h
///@brief  EBBRankOne接收机的实现
///@author wangxiaozhou

#include "../Utility/IDType.h"
#include "../Utility/RBID.h"
#include "../Utility/SCID.h"
#include "../Utility/SBID.h"
#include "../Utility/functions.h"
#include "MSID.h"
#include "../BaseStation/BTSID.h"
#include "../NetworkDrive/Clock.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "ACKNAKMessageDL.h"
#include "SoundingMessage.h"
#include "../BaseStation/BTS.h"
#include "../Statistician/Statistician.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../DetectAlgorithm/Detector_DL.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../MobileStation/MS.h"
#include "../BaseStation/BS.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "CQIMessage.h"
#include "../ChannelModel/LinkMatrix.h"
#include "MSRxer.h"
#include "MSEBBRank1Rxer.h"
#include "SRSTxer.h"
#include"../BaseStation/CodeBookFactory.h"
#include<boost/thread.hpp>
#include <set>
#include "../LinklevelInterface/LinkLevelNR.h"
extern int G_ICurDrop;
boost::mutex CQImutex;
boost::mutex MSEBBRank1Rxer_mutex;

MSEBBRank1Rxer::MSEBBRank1Rxer(BTSID _MainservBTSID, MSID _msid, vector<mat> *_pSINR, HARQRxStateMS* _pHARQRxStateMS)
: MSRxer(_MainservBTSID, _msid, _pSINR, _pHARQRxStateMS),
m_mSubBandPMI(Parameters::Instance().BASIC.ISBNum),
m_mSubBandPrecode(Parameters::Instance().BASIC.ISBNum),
m_mSubBandPrecodeMat(Parameters::Instance().BASIC.ISBNum)
{
    m_iMaxRank = min(Parameters::Instance().MIMO_CTRL.IMaxRankNum, Parameters::Instance().MIMO_CTRL.Total_TXRU_Num) - 1; //最大支持一流
    m_iBestRank = m_iMaxRank;
    m_pCodeBook = CodeBookFactory::Instance().GetCodeBook(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num);
}

void MSEBBRank1Rxer::WorkSlot() {
    //    m_pHARQRxStateMS->WorkSlot();
    int iTime = Clock::Instance().GetTimeSlot();
    double dTimeSec = Clock::Instance().GetTimeSec();
//    cm::LinkMatrix::Instance().WorkSlot(m_MSID.GetMS(), dTimeSec);
    //在S时刻CQI测量,发送CQIMsessage

    if (iTime == 1) {
        m_MSID.GetMS().GetpSRSTxer()->Initialize();
        m_MSID.GetMS().GetpSRSTxer()->SRSPowerControl();
    }
    if (iTime % Parameters::Instance().ERROR.ICQI_PERIOD == 1) {
        UpdateAverageInterferenceCovR();
//        UpdateCovR();
        ComputeRIandPMI();
        ComputeEstimateSINR();
        SendCQIFeedback();
    }

    if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding != 1) {
        assert(Parameters::Instance().ERROR.ISRS_PERIOD %
               Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot == 0);
        int ISRS_OFFSET = Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot;
        if (iTime % Parameters::Instance().ERROR.ISRS_PERIOD == ISRS_OFFSET) {
            //Sounding的测量和反馈
            int iMultiplexID = m_MainServBTS.GetBTS().GetMultiplexID();
            m_MSID.GetMS().GetpSRSTxer()->SetMultiplexID(iMultiplexID);

            //两天线全带宽发，且不考虑非理想因素
            Sounding();

        }
    }
    //每个下行时刻检测数据包
    ReceiveProcess();
}

void MSEBBRank1Rxer::UpdateAverageInterferenceCovR() {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
    MS& ms = m_MSID.GetMS();

    for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {

        cmat mCovR = zeros_c(
                ms.GetAntennaPointer()->GetActiveTXRU_Num(),
                ms.GetAntennaPointer()->GetActiveTXRU_Num());

        vector<cmat> vSIS, vSIH, vSIP;
        CalStrongInterference(vSIS, vSIH, vSIP,
                m_MainServBTS, m_MSID, scid);
        for (int i = 0; i < vSIS.size(); ++i) {
            cmat mSIS = vSIS[i];
            cmat mSIH = vSIH[i];
            cmat mSIP = vSIP[i];
            mCovR += (real((mSIS.H() * mSIS) (0, 0)) / (mSIS.rows())) * mSIH * mSIP * mSIP.H() * mSIH.H();
        }

        double dAverageWindow = 1;
        if (Clock::Instance().GetTimeSlot() <= (Parameters::Instance().ERROR.ICQI_PERIOD + 1)) {
            m_vCovR[scid.ToInt() / iSpace] = mCovR;
        } else {
            m_vCovR[scid.ToInt() / iSpace] = ((dAverageWindow - 1) / dAverageWindow) * m_vCovR[scid.ToInt() / iSpace] + (1 / dAverageWindow) * mCovR;
        }
    }
}

void MSEBBRank1Rxer::UpdateCovR() {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
    MS& ms = m_MSID.GetMS();
    MSID msid = ms.GetID();
    BTSID mainbtsid = ms.GetMainServBTS();
    BTS& mainbts = mainbtsid.GetBTS();
    //@threads
    std::unordered_map<int, MSTxBufferBTS>& mMSTxBuffer = mainbts.GetMSTxBufferBTS();
    MSTxBufferBTS& buffer = mMSTxBuffer[msid.ToInt()];

    cmat mCovR;
    if (Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1) {
        mCovR = itpp::zeros_c(Parameters::Instance().MIMO_CTRL.BF_CSIRS_PortNum, Parameters::Instance().MIMO_CTRL.BF_CSIRS_PortNum);
    } else {
        mCovR = zeros_c(
                mainbts.GetAntennaPointer()->GetActiveTXRU_Num(),
                mainbts.GetAntennaPointer()->GetActiveTXRU_Num());
    }

    int iNum = 0;
    if (Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1) {
        for (SBID sbid = SBID::Begin(); sbid <= SBID::End(); ++sbid) {
            cmat mTemp;
            if (Parameters::Instance().MIMO_CTRL.IHorCovR == 1) {
                mTemp = buffer.GetCovR(sbid);
            } else {
                mTemp = buffer.GetH(sbid);
            }
            vec vS;
            itpp::cmat mU, mD;
            cmat final_matrix = ChangeMatFull2Half(mTemp);
            svd(final_matrix, mU, vS, mD);
            cmat PrecodeMat = ProductPrecodeMat(mD);
            m_mSubBandPrecodeMat[sbid.ToInt()] = PrecodeMat;
        }
        for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
            iNum++;

            cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                    mainbts, ms, scid.ToInt());
            cmat _mH = mH * m_mSubBandPrecodeMat[scid.GetSBID().ToInt()];
            mCovR += _mH.H() * _mH; //维数出错
        }
    } else {
        for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
            iNum++;

            cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                    mainbts, ms, scid.ToInt());

            mCovR += mH.H() * mH; //维数出错
        }
    }
    mCovR = mCovR / iNum;
    double dAverageWindow = 10;
    if (Clock::Instance().GetTimeSlot() == 1) {
        m_WCovR = mCovR;
    } else {
        m_WCovR = ((dAverageWindow - 1) / dAverageWindow) * m_WCovR + (1 / dAverageWindow) * mCovR;
    }//if...else...区分增强非增强
}

void MSEBBRank1Rxer::ComputeRIandPMI() {
    if (Parameters::Instance().ERROR.IRI_Meathod == 0) {
        if (Parameters::Instance().ERROR.ISRS_TxMode == 0)
            m_iMaxRank = 0;
        if (Parameters::Instance().ERROR.ISRS_TxMode == 0 && Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1)
            cout << "MSS.FirstBand.IPortbased_BF_CSIRS Wrong!" << endl;
        vector<double> vRank2Capacity(m_iMaxRank + 1, 0.0);
        for (int rank = 0; rank <= m_iMaxRank; ++rank) {
            ComputeEstimateSINR_EBBCQI_AverageI(rank);
            double dCapacity = EstimateWholeBandCapacity();
            if (rank == 0) {
                vRank2Capacity[rank] = dCapacity;
            } else {
                //assert(rank == 1);
                vRank2Capacity[rank] = 1.0 * dCapacity;
            }
        }

        //找到最优的rank和pmi
        m_iBestRank = -1;
        double dC = -1;
        for (int rank = 0; rank <= m_iMaxRank; ++rank) {
            if (vRank2Capacity[rank] > dC) {
                dC = vRank2Capacity[rank];
                m_iBestRank = rank;
            }
        }
    } else {
        if (Parameters::Instance().ERROR.ISRS_TxMode == 0 && Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 0) {
            m_iBestRank = 0;
        } else {
            if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding != 1) {
                SelectRankTDDNew();
//              vector<double> vRank2Capacity(m_iMaxRank + 1, 0.0);
//                m_iBestRank = -1;
//                double dC = -1;
//                const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
//                for (int rank = 0; rank <= m_iMaxRank; ++rank) {
//                    std::unordered_map<int, int> vSB2BestPMI(Parameters::Instance().BASIC.ISBNum);
//
//                    vector<double> _vSINR4SelectedRank;
//                    _vSINR4SelectedRank.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, 0.0);
//                    //                    ComputeEstimateSINR_ForSelectedRank_EBBCQI_AverageI(
//                    //                            rank, _mSINR4SelectedRank, vSB2BestPMI);
//                    ComputeEstimateSINR_ForSelectedRank_EBBCQI_AverageI_TDD(
//                            rank, _vSINR4SelectedRank);
//                    double dCapacity = EstimateWholeBandCapacity_For_mSINR(_vSINR4SelectedRank, rank + 1);
//                    vRank2Capacity[rank] = dCapacity;
//                    if (vRank2Capacity[rank] > dC) {
//                        dC = vRank2Capacity[rank];
//                        m_iBestRank = rank;
////                        for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
////                            (*m_pSINR)[scid.ToInt() / iSpace](0,0) = _vSINR4SelectedRank[scid.ToInt() / iSpace];
////                        }
//                    }
//                }
//                //            m_iBestRank = 0; //单流不注释、否则注释该行
////                m_iBestRank = std::min(m_iBestRank,
////                        Parameters::Instance().MIMO_CTRL.IMaxRankNum - 1);
//                m_iBestRank = std::min(m_iBestRank, m_iMaxRank);
            }//todo FDD用下面这个判断，TDD还得用上面的
            else if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1) {
                vector<double> vRank2Capacity(m_iMaxRank + 1, 0.0);
                m_iBestRank = -1;
                double dC = -1;
                const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
                for (int rank = 0; rank <= m_iMaxRank; ++rank) {
                    /*
                    map<SBID, int> vSB2BestPMI;
                    //map<SBID, int> m_mSubBandPMI;
                    //vSB2BestPMI.resize(Parameters::Instance().BASIC.ISBNum);
                    for (SBID sbid = sbid.Begin(); sbid <= sbid.End(); ++sbid) {
                        int iBestPMI = CalcBestPMI(rank, sbid);
                        vSB2BestPMI[sbid] = iBestPMI;
                    }
                    //ComputeEstimateSINR_EBBCQI_AverageI(rank);
                    vector<mat> _mSINR4SelectedRank;
                    _mSINR4SelectedRank.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, zeros(1, 1));
                    ComputeEstimateSINR_ForSelectedRank_EBBCQI_AverageI(
                            rank, _mSINR4SelectedRank, vSB2BestPMI);
//                    if(m_MSID.ToInt()==0){
//                        cout<<"****************Rank="<<rank<<endl;
//                        for(int i=0;i<_mSINR4SelectedRank.size();i++){
//                            cout<<_mSINR4SelectedRank[i]<<endl;
//                        }
//                    }
                    double dCapacity = EstimateWholeBandCapacity_For_mSINR(_mSINR4SelectedRank);
//                    cout<<"Rank="<<rank<<"  dCapacity="<<dCapacity<<endl;
//                    cout<<"MSID="<<m_MSID.ToInt()<<"  Rank="<<rank<<"  Capacity="<<dCapacity<<endl;
                    */
                   //new start
     //               cout<<"Rank "<<rank<<endl;
                    std::unordered_map<int, int> vSB2BestPMI;
                    vector<double> _mSINR4SelectedRank;
                    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
                    const static int ISBSize=Parameters::Instance().BASIC.ISBSize;
                    LinkLevelInterface& lli = LinkLevelInterface::Instance_DL();
                    _mSINR4SelectedRank.resize(Parameters::Instance().BASIC.ISCNum / iSpace, 0.0);
                    vector<double>sinr_temp(1,0.0);
                    for (SBID sbid = sbid.Begin(); sbid <= sbid.End(); ++sbid) {
                        vSB2BestPMI[sbid.ToInt()] = -1;
                    }
                    for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
                        if(vSB2BestPMI[scid.GetSBID().ToInt()]==-1){
                            SBID sbid=scid.GetSBID();
                            double max = -1, best = -1;
                            for (int j = 0; j < m_pCodeBook->GetCodeNum(rank); j++) {
                                sinr_temp[0] = CalcSINR_EBBCQI_AverageI_FDD(rank, scid, j);
                                int mMCS = lli.SINR2MCS2(sinr_temp,1);
                                //double dCapacityKbit = lli.GetTBSizeKBit_woDMRS(mMCS, ISBSize);
                                //competorranka method:

//                                int iRENumPerPRB;
//                                if(Parameters::Instance().BASIC.RRC_Config_On == 1){
//                                    iRENumPerPRB = _mCalcRENum.CalculateRENum();
//                                }else{
//                                    iRENumPerPRB = Parameters::Instance().BASIC.DDLRENumPerRBforData;
//                                }
                                int iRENumPerPRB = Parameters::Instance().BASIC.DDLRENumPerRBforData;
                                double dCapacityKbit = lli.MCS2TBSKBit(mMCS, ISBSize * iRENumPerPRB);
                                //cout<<dCapacityKbit1<<"  "<<dCapacityKbit<<"  "<<dCapacityKbit1-dCapacityKbit<<endl;
                                if (dCapacityKbit > max) {
                                    best = j;
                                    max = dCapacityKbit;
                                }
                            }
  //                          cout<<"SBID "<<sbid.ToInt()<<"  PMI  "<<best<<endl;
                            vSB2BestPMI[sbid.ToInt()] = best;
//                            if(best==-1){
//                                int aaa=0;
//                            }
                        }
                        _mSINR4SelectedRank[scid.ToInt() / iSpace] = CalcSINR_EBBCQI_AverageI_FDD(rank, scid, vSB2BestPMI[scid.GetSBID().ToInt()]);
                    }
                    double dCapacity = EstimateWholeBandCapacity_For_mSINR(_mSINR4SelectedRank,rank+1);
//new end

                        //子载波Loop
                    vRank2Capacity[rank] = dCapacity;

                    if (vRank2Capacity[rank] > dC) {
                        dC = vRank2Capacity[rank];
                        m_iBestRank = rank;
                        m_mSubBandPMI = vSB2BestPMI;
//                        for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
//                            (*m_pSINR)[scid.ToInt() / iSpace] = _mSINR4SelectedRank[scid.ToInt() / iSpace];
//                        }
                    }

                }

                //            m_iBestRank = 0; //单流不注释、否则注释该行
//                m_iBestRank = std::min(m_iBestRank,
//                        Parameters::Instance().MIMO_CTRL.IMaxRankNum - 1);
                m_iBestRank = std::min(m_iBestRank, m_iMaxRank);
//                cout<<"MSID="<<m_MSID.ToInt()<<"  Rank="<<m_iBestRank<<endl;
                //m_mMSID2Rank[m_MSID]=m_iBestRank;

                for (SBID sbid = sbid.Begin(); sbid <= sbid.End(); ++sbid) {
                    m_mSubBandPrecode[sbid.ToInt()] = m_pCodeBook->GetCodeWord(m_mSubBandPMI[sbid.ToInt()], m_iBestRank);
                }
                //找到最优的rank和pmi


            }

        }
    }
}

void MSEBBRank1Rxer::ComputeEstimateSINR_EBBCQI_AverageI(int _iRank) {
    assert(false);
    //    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    //    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
    //
    //    BTS& mainbts = m_MainServBTS.GetBTS();
    //    MS& ms = m_MSID.GetMS();
    //
    //    //子载波Loop
    //    for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
    //        //获取有用信号
    //        cmat mS = mainbts.GetTxCRS(_iRank + 1);
    //
    //        (*m_pSINR)[scid.ToInt() / iSpace].set_size(_iRank + 1, 1);
    //        (*m_pSINR)[scid.ToInt() / iSpace] = itpp::zeros(_iRank + 1, 1);
    //        /*
    //                int iBSBeamIndex = lm.GetStrongestBSBeamIndex(mainbts, ms);
    //                double dBSetiltRAD = BSBeamIndex2EtiltRAD(iBSBeamIndex);
    //                double dBSescanRAD = BSBeamIndex2EscanRAD(iBSBeamIndex);
    //        //        cmat mPPortleft = ProductmPPortleftsub(dBSetiltRAD, dBSescanRAD, Parameters::Instance().Macro.IHAntNumPerPanel, Parameters::Instance().Macro.IVAntNumPerPanel,
    //        //                Parameters::Instance().Macro.IHAntNumPerPanel * Parameters::Instance().Macro.IHPanelNum, Parameters::Instance().Macro.IVAntNumPerPanel * Parameters::Instance().Macro.IVPanelNum);
    //                //20171206
    //                cmat mPPortleft= ProductmPPortleftsub_for_TXRU(
    //                        dBSetiltRAD, dBSescanRAD,
    //                        Parameters::Instance().Macro.IHAntNumPerPanel,
    //                        Parameters::Instance().Macro.IVAntNumPerPanel,
    //                        Parameters::Instance().Macro.IHPanelNum,
    //                        Parameters::Instance().Macro.IVPanelNum,
    //                        2);
    //
    //                int iUEBeamIndex = lm.GetStrongestUEBeamIndex(mainbts, ms);
    //                double dUEetiltRAD = UEBeamIndex2EtiltRAD(iUEBeamIndex);
    //                double dUEescanRAD = UEBeamIndex2EscanRAD(iUEBeamIndex);
    //                cmat mPPortleft_UE = ProductmPPortleft_UE(dUEetiltRAD, dUEescanRAD);
    //                int iStrongestUEPanelIndex = cm::LinkMatrix::Instance().GetStrongestUEPanelIndex(mainbts, ms);
    //         */
    //        /*
    //                cmat mH = mPPortleft_UE * lm.GetFadingMat(mainbts, ms, scid.ToInt(), iStrongestUEPanelIndex) * mPPortleft;
    //         */
    //
    //        cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
    //                mainbts, ms, scid.ToInt());
    //        cmat mU, mD;
    //        vec vS;
    //        svd(mH, mU, vS, mD);
    //        cmat mP = mD.get_cols(0, _iRank);
    //
    //        //获取干扰信息
    //        vector<cmat> vWIS;
    //        vector<double> vWIL;
    //        for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
    //            if (btsid == m_MainServBTS)
    //                continue;
    //            ///按一定比例决定是否存在该干扰
    //            if (Parameters::Instance().BASIC.DInterfProb < 0.99) {
    //                if (xUniform() > Parameters::Instance().BASIC.DInterfProb)
    //                    continue;
    //            }
    //
    //            if (!btsid.GetBTS().IsExistTraffic())
    //                continue;
    //
    //            BTS& bts = btsid.GetBTS();
    //            if (!bts.GetSBUsedFlag(scid.GetSBID()))
    //                continue;
    //            if (!lm.IsStrong(bts, ms)) {
    //                //获取弱干扰的信号
    //                cmat mWIS = bts.GetTxDataSymbol(scid);
    //                vWIS.push_back(mWIS);
    //                //获取弱干扰大尺度衰落
    //                double dPLI = DB2L(lm.GetCouplingLossDB(bts, ms));
    //                vWIL.push_back(dPLI);
    //            }
    //        }
    //        //@brief 未重计算时采用以下，要考虑欧拉
    //        mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, m_vCovR[scid.ToInt() / iSpace], vWIS, vWIL);
    //        (*m_pSINR)[scid.ToInt() / iSpace](0, 0) = mSINR(0, 0) * DB2L(m_dOLLAOffsetDB);
    //        //根据OLLA偏置和波束赋形增益调整估计的SINR
    //        if (_iRank == 1) {
    //            //assert(mSINR.rows()==2);
    //            (*m_pSINR)[scid.ToInt() / iSpace](1, 0) = mSINR(1, 0) * DB2L(m_dOLLAOffsetDB);
    //        }
    //    }
}

double MSEBBRank1Rxer::EstimateWholeBandCapacity() {
    LinkLevelInterface& lli = LinkLevelInterface::Instance_DL();
    itpp::imat mMCS = lli.SINR2MCS(*m_pSINR);
    double dCapacityKbit = lli.GetTBSizeKBit_woDMRS(mMCS, Parameters::Instance().BASIC.IRBNum);
    return dCapacityKbit;
}

void MSEBBRank1Rxer::ComputeEstimateSINR() {
    if (m_iBestRank == m_iMaxRank && Parameters::Instance().ERROR.IRI_Meathod == 0)
        return;
    ComputeEstimateSINR_EBBCQI_AverageI(m_iBestRank);
}
void MSEBBRank1Rxer::SendCQIFeedback() {

    //将CQI的结果转换为CQIMessage
    std::shared_ptr<CQIMessage> pCQIMes = std::make_shared<CQIMessage>();
    LinkLevelInterface &lli = LinkLevelInterface::Instance_DL();
    pCQIMes->SetRank(m_iBestRank);

    int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace;
    for (SBID sbid = SBID::Begin(); sbid <= SBID::End(); ++sbid) {
        vector<mat> vSINR_PerSB;
        for (SCID scid = sbid.GetFirstSCID(); scid <= sbid.GetLastSCID(); scid += iSpace) {
            vSINR_PerSB.push_back((*m_pSINR)[scid.ToInt() / iSpace]);
        }
        imat mSBMCS = lli.SINR2MCS(vSINR_PerSB);
        pCQIMes->SetSubBandMCS(sbid, mSBMCS);
        //反馈SubBandPMI
        if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1) {
            if (m_mSubBandPMI.find(sbid.ToInt()) != m_mSubBandPMI.end()) {
                pCQIMes->SetSubBandPMI(sbid, m_mSubBandPMI[sbid.ToInt()]);
            } else {
                cout << "Wrong" << endl;
            }
        }
        if (Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1) {
            if (m_mSubBandPMI.find(sbid.ToInt()) != m_mSubBandPMI.end()) {
                pCQIMes->SetSubBandPMI(sbid, m_mSubBandPMI[sbid.ToInt()]);
            } else {
                cout << "Wrong" << endl;
            }
        }
    }
    imat mWBMCS = lli.SINR2MCS(*m_pSINR);
    pCQIMes->SetWideBandMCS(mWBMCS);
    //将该CQIMessage发送
    m_MSID.GetMS().GetUci()->CollectCQIMes(pCQIMes);
//    BTS& mainbts = m_MainServBTS.GetBTS();
//    mainbts.PushCQIMessage(m_MSID, pCQIMes);
}

void MSEBBRank1Rxer::Sounding(int _iAntPort) {
    //step1:遍历所有的RB的信道快衰矩阵都存在SoundingMessage中
    //step2:将SoundingMessage发送到BTS端
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    //更新信道
    std::shared_ptr<SoundingMessage> pSoundingMes = std::shared_ptr<SoundingMessage > (new SoundingMessage(m_MSID));
    std::shared_ptr<SRSTxer> pSRSTxer = m_MSID.GetMS().GetpSRSTxer();
    MS& ms = m_MSID.GetMS();
    BTS& mainbts = ms.GetMainServBTS().GetBTS();

    for (RBID rbid = RBID::Begin(); rbid <= RBID::End(); ++rbid) {

        cmat mH = zeros_c(
                ms.GetAntennaPointer()->GetActiveTXRU_Num(),
                mainbts.GetAntennaPointer()->GetActiveTXRU_Num());

        for (SCID scid = rbid.GetFirstSCID(); scid <= rbid.GetLastSCID(); scid += Parameters::Instance().LINK_CTRL.IFrequencySpace) {

            mH += lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                    m_MainServBTS.GetBTS(), m_MSID.GetMS(), scid.ToInt());
        }
        mH /= (Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace);
        if (Parameters::Instance().ERROR.IAntennaCalibrationError == 1) {
            mH = AntennaCalibrationErrorModel(mH);
        }
        if (Parameters::Instance().ERROR.ISRS_Error == 1) {
            mH = SRSErrorModel(mH);
            //  mH = SRSErrorModel_Simple(mH);
        }
        pSoundingMes->SetH(rbid, mH, _iAntPort);
    }
    m_MainServBTS.GetBTS().PushSoundingMessage(pSoundingMes);
}

void MSEBBRank1Rxer::Sounding() {
    //step1:遍历所有的RB的信道快衰矩阵都存在SoundingMessage中
    //step2:将SoundingMessage发送到BTS端
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    std::shared_ptr<SoundingMessage> pSoundingMes = std::shared_ptr<SoundingMessage > (new SoundingMessage(m_MSID));
    MS& ms = m_MSID.GetMS();
    BTSID btsid = ms.GetMainServBTS();
    BTS& mainbts = btsid.GetBTS();

    for (RBID rbid = RBID::Begin(); rbid <= RBID::End(); ++rbid) {

        cmat mH = zeros_c(
                ms.GetAntennaPointer()->GetActiveTXRU_Num(),
                mainbts.GetAntennaPointer()->GetActiveTXRU_Num());

        for (SCID scid = rbid.GetFirstSCID(); scid <= rbid.GetLastSCID(); scid += Parameters::Instance().LINK_CTRL.IFrequencySpace) {

            mH += lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                    m_MainServBTS.GetBTS(), m_MSID.GetMS(), scid.ToInt());
        }
        mH /= (Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace);
        if (Parameters::Instance().ERROR.IAntennaCalibrationError == 1) {
            mH = AntennaCalibrationErrorModel(mH);
        }
        //Channel Estimation Tag
        if (Parameters::Instance().ERROR.ISRS_Error == 1) {
            mH = SRSErrorModel(mH);
            //mH = SRSErrorModel_Simple(mH);
        }
        pSoundingMes->SetH(rbid, mH);
    }
    m_MainServBTS.GetBTS().PushSoundingMessage(pSoundingMes);
}

void MSEBBRank1Rxer::ApSounding() {
    //step1:遍历所有的RB的信道快衰矩阵都存在SoundingMessage中
    //step2:将SoundingMessage发送到BTS端
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    std::shared_ptr<SoundingMessage> pSoundingMes = std::shared_ptr<SoundingMessage > (new SoundingMessage(m_MSID));
    MS& ms = m_MSID.GetMS();
    BTS& mainbts = ms.GetMainServBTS().GetBTS();

    for (RBID rbid = RBID::Begin(); rbid <= RBID::End(); ++rbid) {

        cmat mH = zeros_c(
                ms.GetAntennaPointer()->GetActiveTXRU_Num(),
                mainbts.GetAntennaPointer()->GetActiveTXRU_Num());
        for (SCID scid = rbid.GetFirstSCID(); scid <= rbid.GetLastSCID(); scid += Parameters::Instance().LINK_CTRL.IFrequencySpace) {

            mH += lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                    m_MainServBTS.GetBTS(), m_MSID.GetMS(), scid.ToInt());
        }
        mH /= (Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace);
        if (Parameters::Instance().ERROR.IAntennaCalibrationError == 1) {
            mH = AntennaCalibrationErrorModel(mH);
        }
        if (Parameters::Instance().ERROR.ISRS_Error == 1) {
            mH = SRSErrorModel(mH);
            //mH = SRSErrorModel_Simple(mH);
        }
        pSoundingMes->SetH(rbid, mH);
    }
    m_MainServBTS.GetBTS().SetApSoundingMessage(pSoundingMes);
}

cmat MSEBBRank1Rxer::SRSErrorModel(const cmat& _mH) {
    BTS& mainbts = m_MainServBTS.GetBTS();          //主服务基站
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();                //（链路矩阵）LinkMatrix
    double dLinkloss = DB2L(lm.GetCouplingLossDB(mainbts, m_MSID.GetMS()));         //获取该用户和基站的Linkloss
    cmat mHNew(_mH.rows(), _mH.cols());         //定义一个和 _mH矩阵同维度的 mHNew 矩阵
    std::shared_ptr<SRSTxer> pSRSTxer = m_MSID.GetMS().GetpSRSTxer();       //移动台SRS发射机的指针接口
    double dSignal = pSRSTxer->GetSRSTxPowerMW_per_SC() * dLinkloss;        //信号强度大小
    double dNoise = Parameters::Instance().Macro.UL.DSCNoisePowerMw;        //每个子载波上的噪声功率，mW
    double dInterference = 0;       //干扰

    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {        //遍历每个小区（BTSID）
        if (btsid == m_MainServBTS)
            continue;
        BTS& InterferenceBTS = btsid.GetBTS();          //定义一个干扰小区
        for (int imsindex = 0; imsindex < InterferenceBTS.GetConnectedNum(); ++imsindex) {      //遍历该小区的每个激活用户
            vector<MSID>& vInterferenceMSID = InterferenceBTS.GetActiveSet();       //该小区的激活用户集合
            MSID InterferenceMSID = vInterferenceMSID[imsindex];                    //定义一个干扰移动台ID类
            MS& InterferenceMS = InterferenceMSID.GetMS();                          //定义一个干扰移动台MS类
            if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP
                    || Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP2
                    || Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP3
                    || Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_XR
                    ||Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_XRmulti
                      ||Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::MixedTraffic) {
                if (!InterferenceMS.GetTrafficState())              //判断该用户是否有业务（？）有则开始下一个循环
                    continue;
            }
            std::shared_ptr<SRSTxer> pSRSTxerI = InterferenceMS.GetpSRSTxer();          //干扰移动台的SRS发射机的指针接口
            if (pSRSTxerI->GetMultiplexID() == pSRSTxer->GetMultiplexID()) {            //判断用户本身的SRS MultiplexID是否等于干扰用户的SRS MultiplexID
                double dPtxI = pSRSTxerI->GetSRSTxPowerMW_per_SC();             //干扰用户的SRSTxPowerMW_per_SC
                double dLinklossI = DB2L(lm.GetCouplingLossDB(mainbts, InterferenceMS));        //主服务基站到干扰用户的LinkLoss
                dInterference += dPtxI*dLinklossI;              //dInterference += dPtxI * dLinklossI;
            }
        }
    }

    //1226 ljq
    double dSINR = dSignal / (dInterference + dNoise);  //计算SINR
    double dDeltaMSE = 9.0;                             // DeltaMSE = 7~9 dB
    double dE = 1 / (dSINR * DB2L(dDeltaMSE));
    double dAlfa = sqrt(dSINR * DB2L(dDeltaMSE) / (1 + dSINR * DB2L(dDeltaMSE)));

    for (int i = 0; i < _mH.rows(); ++i) {
        for (int j = 0; j < _mH.cols(); ++j) {
            complex<double> cOffset = complex<double>(random.xNormal_SRSError(0, sqrt(dE)) * abs(_mH(i, j)), random.xNormal_SRSError(0, sqrt(dE)) * abs(_mH(i, j)));
            mHNew(i, j) = dAlfa * (_mH(i, j) + cOffset);
        }
    }
    return mHNew;
}

cmat MSEBBRank1Rxer::DMRSErrorModel(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {
    BTS& mainbts = m_MainServBTS.GetBTS();
    MS& ms = m_MSID.GetMS();
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
            double dA = 0;
            double dB = 0;
            ///暂时只做了3 km/h的情况
            if (cm::LinkMatrix::Instance().IsLOS(mainbts, ms)) {
                dA = 0.2711;
                dB = 0.0933;
            } else {
                dA = 0.2663;
                dB = 0.09;
            }

            double dSigmaStd = dA * exp(-1 * dB * dSINRDB);
            double dSigmaVar = min(0.49, pow(dSigmaStd, 2));
            double dMSESigmaVar = dSigmaVar / (1 - 2 * dSigmaVar);
            if (dMSESigmaVar < 0) {
                cout << dMSESigmaVar << endl;
            }
            complex<double> cOffset = complex<double>(random.xNormal_DMRSError(0, sqrt(dMSESigmaVar)) * abs(mHe(i, j)), random.xNormal_DMRSError(0, sqrt(dMSESigmaVar)) * abs(mHe(i, j)));
            double dFactor = 1 / (1 + 2 * dMSESigmaVar);
            mNewHe(i, j) = dFactor * (mHe(i, j) + cOffset);
        }
    }
    return mNewHe;
}

cmat MSEBBRank1Rxer::AntennaCalibrationErrorModel(const cmat& _mH) {
    ///暂时只建模基站端的天线校准误差
    assert(Parameters::Instance().ERROR.IAntennaCalibrationError == 1);
    int icol = _mH.cols();
    itpp::Vec<complex<double> > vAntCalibError(icol);
    double dAmplitudeDbStd = 1; //幅度为均值为0标准差为dAmplitudeStd的高斯分布
    double dPhaseDegreeUni = 10; //相位为均匀分布;
    double dPhaseAngularUni = dPhaseDegreeUni / 180 * M_PI;
    for (int i = 0; i < icol; ++i) {
        double dAmplitude = DB2L(random.xNormal_AntCalibrationError(0, dAmplitudeDbStd));
        double dPhaseAngular = random.xUniform_AntCalibrationError(-1 * dPhaseAngularUni, dPhaseAngularUni);
        vAntCalibError[i] = complex<double>(dAmplitude * cos(dPhaseAngular), dAmplitude * sin(dPhaseAngular));
    }
    cmat mAntCalibError = itpp::diag(vAntCalibError);
    cmat mNewH = _mH*mAntCalibError;
    return mNewH;
}
void MSEBBRank1Rxer::ReceiveProcess() {
    if (m_pScheduleMesDL == 0)
        return;

    LinkLevelInterface& lli = LinkLevelInterface::Instance_DL();
    BTS& mainbts = m_MainServBTS.GetBTS();
    //计算接收SINR
    ComputeReceiveSINR();
    int iRank = m_pScheduleMesDL->GetRank();

    int iHARQID = m_pScheduleMesDL->GetHARQID();
    std::shared_ptr<ACKNAKMessageDL> pACKNAK = std::shared_ptr<ACKNAKMessageDL > (new ACKNAKMessageDL(m_pScheduleMesDL));
    int iMCS = m_pScheduleMesDL->GetiMCS();
    double dTBS = m_pScheduleMesDL->GetdTBSize();
    int iSendNum = m_pScheduleMesDL->GetiSendNum();
    std::vector<SBID> vSBUsed = m_pScheduleMesDL->GetSBUsed();
    int ik1_slot = m_pScheduleMesDL->Getk1_slot();
    int iLatestTransTime = m_pScheduleMesDL->GetLatestTransTime();
    int iSBNum = static_cast<int> (vSBUsed.size());

    int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace;
    int AllRight = true;

    int ccelevel = m_pScheduleMesDL->GetCCELevel();
    vector<int> CCESB = m_pScheduleMesDL->GetCCESB();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool bRight = false;
    std::vector<double> vSCSINR;
    for (int i = 0; i < iSBNum; ++i) {
        SBID sbid = vSBUsed[i];
        for (SCID scid = sbid.GetFirstSCID(); scid <= sbid.GetLastSCID(); scid += iSpace) {
            double temp = 0.0;
            for(int rank = 0;rank<=iRank;++rank){
                temp += (*m_pSINR)[scid.ToInt() / iSpace](rank, 0);
            }
            temp/=(iRank+1);
            vSCSINR.push_back(temp);
        }
    }
    double avesinr = 0.0;
    int count = 0;
    for(auto sinr:vSCSINR){
        avesinr += sinr;
        count++;
    }
    avesinr = L2DB(avesinr/count);

    // 根据HARQ合并SINR
    m_pHARQRxStateMS->CombineSINR(iHARQID, iRank, vSCSINR);
    const std::vector<double>& vCombinedSINR = m_pHARQRxStateMS->GetCombinedSINR(iHARQID, iRank);
    // 计算PostSINR
    double dPostSINRDB = lli.ComputePostSINRDB(vCombinedSINR, iMCS);
    if(Clock::Instance().GetTimeSlot() >= Parameters::Instance().BASIC.IWarmUpSlot)
    {
        if (iSendNum==1)
        { //第一次传输时统计
            Statistician::Instance().m_MSData_DL[m_MSID.ToInt()].DL.m_iRankHitNum[iRank] += 1;
        }

        Statistician::Instance().m_MSData_DL[m_MSID.ToInt()].DL.m_dAvePostSINR += DB2L(dPostSINRDB);
        Statistician::Instance().m_MSData_DL[m_MSID.ToInt()].DL.m_dLogAvePostSINR += dPostSINRDB;
        Statistician::Instance().m_MSData_DL[m_MSID.ToInt()].DL.m_iPostSINRHit += 1;
        /// 记录MCS
        Statistician::Instance().m_MSData_DL[m_MSID.ToInt()].DL.m_iMCSHit[iMCS] += 1;
    }
    // 计算该流的ACK
    double dBLER = lli.BLER(vCombinedSINR, iMCS, dTBS/(iRank+1));
    bRight = (random.xUniform_detection() > dBLER);
    pACKNAK->SetACKNAK( bRight);
    double dEstimateSINR = L2DB(m_pScheduleMesDL->EstimateSINR);
    double CQISINR = m_pScheduleMesDL->CQISINR;

    AllRight = (AllRight && bRight);
    {
        boost::mutex::scoped_lock lock(MSEBBRank1Rxer_mutex);
        int iTime = Clock::Instance().GetTimeSlot();
        //double dOlla = m_dOLLAOffsetDB;
        //double dOlla = v_dOLLAOffsetDB[iRankID];
        double dOlla = m_dOLLAOffsetDB(iRank, 0);
        Observer::SetIsEnable(true);

        double insert_rankID = -1; //暂时没有实现rankID的区分
        //hyl 冗余
//        Observer::Print("MSEBBRank1Decode") << iTime << setw(20) << iHARQID << setw(20) << m_MSID << setw(20) << m_MainServBTS
//                << setw(20) << iSBNum << setw(20) << iRank << setw(20) << iSendNum
//                << setw(20) << insert_rankID << setw(20) << iMCS << setw(20) << dOlla
//                << setw(20) << dPostSINRDB << setw(20) << dBLER << setw(20) << bRight
//                << setw(20) << dTBS << setw(20);

        for (int j = 0; j < iSBNum; ++j) {
            SBID sbid = vSBUsed[j];
//            Observer::Print("MSEBBRank1Decode") << "@" << sbid;
        }



     //   cout << Parameters::Instance().TRAFFIC.ITrafficModel << endl;
        if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP) {

            double Spectralefficiency;
            Spectralefficiency = dTBS / iSBNum * Parameters::Instance().BASIC.IRBNum / Parameters::Instance().BASIC.DSystemBandWidthKHz / Parameters::Instance().BASIC.DSlotDuration_ms * 1000;
            Spectralefficiency /=  (iRank + 1);

            //hyl 冗余
//            Observer::Print("MSEBBRank1Decode") << setw(20) << ik1_slot
//                        << setw(20) << iLatestTransTime
//                        << setw(20) << m_pScheduleMesDL->GetFirstPacket()->GetID()
//                        << setw(20) << m_pScheduleMesDL->GetCarriedPacket_SizeKbits() << setw(20) << Spectralefficiency;
            }else{

                double Spectralefficiency;
                Spectralefficiency = dTBS / iSBNum * Parameters::Instance().BASIC.IRBNum / Parameters::Instance().BASIC.DSystemBandWidthKHz / Parameters::Instance().BASIC.DSlotDuration_ms * 1000;
                Spectralefficiency /=  (iRank + 1);
            //hyl 冗余
//                Observer::Print("MSEBBRank1Decode") << setw(20) << "NULL"
//                << setw(20) << "NULL"
//                << setw(20) << "NULL"
//                << setw(20) << "NULL" << setw(20) << Spectralefficiency;
            }

        // cout << m_pScheduleMesDL->GetCarriedPacket_SizeKbits() << endl;
//        Observer::Print("MSEBBRank1Decode") << endl;

        //一个无线帧中有多少个时隙 10ms * slotperms
        //int iFrameNum = 10 / Parameters::Instance().BASIC.DSlotDuration_ms;
        int iFrameID = iTime / Parameters::Instance().BASIC.iFrameNum + 1;
		//string ModulationOder = LinkLevelNR::ModulationOrder(iMCS, Parameters::Instance().BASIC.IDLORUL);
		Observer::Print("SchMes") << m_MainServBTS << setw(20) << G_ICurDrop << setw(20) << iFrameID << setw(20) << iTime << setw(20) << DownOrUpLink(iTime) << setw(20) << m_MSID << setw(20)
			<< iHARQID << setw(20) << iRank + 1 << setw(20) << iSendNum << setw(20) << dPostSINRDB << setw(20) << dEstimateSINR << setw(20) << CQISINR << setw(20) << iMCS << setw(20) << LinkLevelNR::ModulationOrder(iMCS, Parameters::Instance().BASIC.IDLORUL) << setw(20)
			<< dBLER << setw(20) << bRight << setw(20) << dTBS << setw(20) << iSBNum << setw(20);
		for (int j = 0; j < iSBNum; ++j) {
			SBID sbid = vSBUsed[j];
			Observer::Print("SchMes") << sbid << "\\";
		}
//		Observer::Print("SchMes") << setw(20) << "N" << endl;
        Observer::Print("SchMes") << setw(20) << "N" << setw(20) <<ccelevel;// << setw(20);
//        for(int j=0;j<CCESB.size();j++){
//            Observer::Print("SchMes") << CCESB[j] << "\\";
//        }
        Observer::Print("SchMes") << endl;
        //刨除热启动时隙的调度信息
        int iTime_WarmUpslot = iTime - Parameters::Instance().BASIC.IWarmUpSlot;
        if(iTime_WarmUpslot > 0){
            int iFrameID_WarmUpslot = iTime_WarmUpslot / Parameters::Instance().BASIC.iFrameNum + 1;
            //string ModulationOder = LinkLevelNR::ModulationOrder(iMCS, Parameters::Instance().BASIC.IDLORUL);
//            Observer::Print("SchMes_WarmUpslot") << m_MainServBTS << setw(20) << iFrameID_WarmUpslot << setw(20) << iTime_WarmUpslot << setw(20) << DownOrUpLink(iTime) << setw(20) << m_MSID << setw(20)
//                                      << iHARQID << setw(20) << iRank + 1 << setw(20) << dPostSINRDB << setw(20) << iMCS << setw(20) << LinkLevelNR::ModulationOrder(iMCS, Parameters::Instance().BASIC.IDLORUL) << setw(20)
//                                      << dBLER << setw(20) << bRight << setw(20) << dTBS << setw(20) << iSBNum << setw(20);

            for (int j = 0; j < iSBNum; ++j) {
                SBID sbid = vSBUsed[j];
//                Observer::Print("SchMes_WarmUpslot") << sbid << "\\";
            }
//            Observer::Print("SchMes_WarmUpslot") << setw(20) << "N" << endl;

        }

    }
    if (iSendNum == 1) {
        static const double dDown = -0.5;
        static const double dUp = abs(dDown) * Parameters::Instance().SIM.DL.DBlerTarget / (1 - Parameters::Instance().SIM.DL.DBlerTarget);
        m_dOLLAOffsetDB(iRank, 0) += (AllRight ? dUp : dDown);
        m_dOLLAOffsetDB(iRank, 0) = std::min(std::max(-20.0, m_dOLLAOffsetDB(iRank, 0)), 20.0);
    }
    ///处理HARQ的CC合并
    if (AllRight || iSendNum == Parameters::Instance().SIM.DL.IHARQMaxTransNum) {
            m_pHARQRxStateMS->Refresh(iHARQID, iRank);
    }
//    if (iRank > 0) {
//        for (int iRankID = 0; iRankID <= iRank; ++iRankID) {
//            if (mSendNum(0, 0) != mSendNum(iRank, 0)) {
//                cout << "MSEBBRank1Rxer::ReceiveProcess() error!" << endl;
//            }
//        }
//    }
    //发送ACKNAKMessage
    m_MSID.GetMS().GetUci()->CollectACKDL(pACKNAK);
//    mainbts.ReceiveACKNAKMes(pACKNAK);
    //解调完毕清空智能指针
    m_pScheduleMesDL.reset();
}

void MSEBBRank1Rxer::ComputeReceiveSINR() {
    assert(false);
    //    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    //    // 存在调度信息，执行检测流程
    //    if (m_pScheduleMesDL != 0) {
    //        std::vector<SBID> vSBUsed = m_pScheduleMesDL->GetSBUsed();
    //        std::sort(vSBUsed.begin(), vSBUsed.end());
    //        int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace;
    //        BTS& mainbts = m_MainServBTS.GetBTS();
    //        MS& ms = m_MSID.GetMS();
    //        /*
    //                cmat mPPortleft = ProductmPPortleftsub(m_pScheduleMesDL->GetMap_MSID2PanelAndvBSBeam()[m_MSID],
    //                        Parameters::Instance().Macro.IHAntNumPerPanel,
    //                        Parameters::Instance().Macro.IVAntNumPerPanel,
    //                        Parameters::Instance().Macro.IHAntNumPerPanel * Parameters::Instance().Macro.IHPanelNum,
    //                        Parameters::Instance().Macro.IVAntNumPerPanel * Parameters::Instance().Macro.IVPanelNum);
    //         */
    //        /*
    //                int iStrongestUEPanelIndex = cm::LinkMatrix::Instance().GetStrongestUEPanelIndex(mainbts, ms);
    //                int iUEBeamIndex = lm.GetStrongestUEBeamIndex(mainbts, ms);
    //                double dUEetiltRAD = UEBeamIndex2EtiltRAD(iUEBeamIndex);
    //                double dUEescanRAD = UEBeamIndex2EscanRAD(iUEBeamIndex);
    //                cmat mPPortleft_UE = ProductmPPortleft_UE(dUEetiltRAD, dUEescanRAD);
    //                map<BTSID, cmat> mBTSID2mPPortleft_Interf;
    //                for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
    //                    if (btsid == m_MainServBTS)
    //                        continue;
    //                    for (vector< pair<int, vector<int> > >::iterator it = btsid.GetBTS().GetvUEPanelAndvBSBeam().begin();
    //                            it != btsid.GetBTS().GetvUEPanelAndvBSBeam().end(); ++it) {
    //                        mBTSID2mPPortleft_Interf[btsid] += ProductmPPortleftsub(*it, /////该如何计算
    //                                Parameters::Instance().Macro.IHAntNumPerPanel,
    //                                Parameters::Instance().Macro.IVAntNumPerPanel,
    //                                Parameters::Instance().Macro.IHAntNumPerPanel * Parameters::Instance().Macro.IHPanelNum,
    //                                Parameters::Instance().Macro.IVAntNumPerPanel * Parameters::Instance().Macro.IVPanelNum);
    //                    }
    //                }
    //         */
    //
    //        vector<double> vSCSINR;
    //        for (int i = 0; i < static_cast<int> (vSBUsed.size()); ++i) {
    //            SBID sbid = vSBUsed[i];
    //            //SC Loop
    //            for (SCID scid = sbid.GetFirstSCID(); scid <= sbid.GetLastSCID(); scid += iSpace) {
    //                itpp::cmat mP = m_pScheduleMesDL->GetCodeWord(sbid);
    //                itpp::cmat mS = mainbts.GetTxDataSymbol(scid);
    //                itpp::cmat mH;
    //                /*
    //                                mH = mPPortleft_UE * lm.GetFadingMat(mainbts, ms, scid.ToInt(), iStrongestUEPanelIndex) * mPPortleft;
    //                 */
    //                mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
    //                        mainbts, ms, scid.ToInt());
    //
    //                std::vector<itpp::cmat> vSIS, vSIH, vSIP, vWIS;
    //                std::vector<double> vWIL;
    //                for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
    //                    if (btsid == m_MainServBTS)
    //                        continue;
    //                    ///按一定比例决定是否存在该干扰
    //                    if (Parameters::Instance().BASIC.DInterfProb < 0.99) {
    //                        if (xUniform() > Parameters::Instance().BASIC.DInterfProb)
    //                            continue;
    //                    }
    //
    //                    if (!btsid.GetBTS().IsExistTraffic())
    //                        continue;
    //
    //                    BTS& bts = btsid.GetBTS();
    //                    if (!bts.GetSBUsedFlag(scid.GetSBID()))
    //                        continue;
    //                    if (lm.IsStrong(bts, ms)) {
    //                        vSIS.push_back(bts.GetTxDataSymbol(scid));
    //                        /*
    //                                                vSIH.push_back( mPPortleft_UE * lm.GetFadingMat(bts, ms, scid.ToInt(), iStrongestUEPanelIndex) * mBTSID2mPPortleft_Interf[btsid]);
    //                         */
    //                        vSIH.push_back(
    //                                lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs2(
    //                                bts, ms, scid.ToInt(),
    //                                btsid.GetBTS().GetvUEPanelAndvBSBeam()));
    //
    //                        vSIP.push_back(bts.GetPrecodeCodeWord(scid.GetSBID()));
    //                    } else {
    //                        vWIS.push_back(bts.GetTxDataSymbol(scid));
    //                        vWIL.push_back(DB2L(lm.GetCouplingLossDB(bts, ms)));
    //                    }
    //                }
    //                if (Parameters::Instance().ERROR.IDMRS_Error == 1) {
    //                    cmat mHe = DMRSErrorModel(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);
    //                    mat mSINR = m_pDetector->CalculateSINRWithDMRSError(mS, mH, mP, mHe, vSIS, vSIH, vSIP, vWIS, vWIL);
    //                    (*m_pSINR)[scid.ToInt() / iSpace] = mSINR;
    //                } else {
    //                    mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);
    //                    (*m_pSINR)[scid.ToInt() / iSpace] = mSINR;
    //                }
    //                vSCSINR.push_back((*m_pSINR)[scid.ToInt() / iSpace](0, 0));
    //            }
    //        }
    //
    //        double dAveSINR = std::accumulate(vSCSINR.begin(), vSCSINR.end(), 0.0) / vSCSINR.size();
    //        Statistician::Instance().m_MSData_DL[m_MSID].DL.m_dAveSINR += dAveSINR;
    //        Statistician::Instance().m_MSData_DL[m_MSID].DL.m_iSINRHit += 1;
}

int MSEBBRank1Rxer::CalcBestPMI(int _rank,const SBID& _sbid) {

    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    MS& ms = m_MSID.GetMS();
    BTSID btsid = ms.GetMainServBTS();
    BTS& mainbts = btsid.GetBTS();

    double dMaxres = 0.0;
    int iBestPMI = 0;
    double dMaxres1 = 0.0;
    int iBestPMI1 = 0;

    for (int j = 0; j < m_pCodeBook->GetCodeNum(_rank); j++) {
        double dresult = 0.0;
        double dresult1 = 0.0;
        cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                mainbts, ms, _sbid.GetFirstSCID().ToInt());
        cmat mHe = mH * m_pCodeBook->GetCodeWord(j, _rank);
        for (int k = 0; k < mHe.rows(); k++) {
            for (int l = 0; l < mHe.cols(); l++) {
                dresult = dresult + norm(mHe(k, l));
            }
        }
        if (dresult > dMaxres) {
            dMaxres = dresult;
            iBestPMI = j;
        }
//        double dresult = 0.0;
//        cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
//                mainbts, ms, _sbid.GetFirstSCID().ToInt());
//        cmat mHe = mH * m_pCodeBook->GetCodeWord(j, _rank);
//
        for (int k = 0; k < mHe.rows(); k++) {
            dresult1+=norm(sum(mHe.get_row(k)));
        }
        if (dresult1 > dMaxres1) {
            dMaxres1 = dresult;
            iBestPMI1 = j;
        }
    }

//    if(iBestPMI!=iBestPMI1) {
//        cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
//                mainbts, ms, _sbid.GetFirstSCID().ToInt());
//        cmat mHe = mH * m_pCodeBook->GetCodeWord(iBestPMI, _rank);
//        cmat mHe1 = mH * m_pCodeBook->GetCodeWord(iBestPMI1, _rank);
//        cout<<mHe<<endl<<mHe1<<endl;
//        int a=0;
//
//    }
    return iBestPMI;
}

mat MSEBBRank1Rxer::CalcSINR_EBBCQI_AverageI(int _iRank, SCID _scid, int _iBestPMI) {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
    BTS& mainbts = m_MainServBTS.GetBTS();
    MS& ms = m_MSID.GetMS();
    //获取有用信号
    cmat mS = mainbts.GetTxCRS(_iRank + 1);

    cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
            mainbts, ms, _scid.ToInt());

    cmat mP;
    // todo: if PMI_based_Precoding == true
    if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1) {
        mP = m_pCodeBook->GetCodeWord(_iBestPMI, _iRank);
    } else {
        cmat mU, mD;
        vec vS;
        svd(mH, mU, vS, mD);
//        cmat mD = lm.Get_mD_wABF_for_all_active_TXRU_Pairs(mainbts, ms, _scid.ToInt());
        mP = mD.get_cols(0, _iRank);
    }

    //获取干扰信息
    vector<cmat> vWIS;
    vector<double> vWIL;
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
        if (!bts.GetSBUsedFlag(_scid.GetSBID()))
            continue;
        if (!lm.IsStrong(bts, ms)) {
            //获取弱干扰的信号
            cmat mWIS = bts.GetTxDataSymbol(_scid);
            vWIS.push_back(mWIS);
            //获取弱干扰大尺度衰落
            double dPLI = DB2L(lm.GetCouplingLossDB(bts, ms));
            vWIL.push_back(dPLI);
        }
    }
    //@brief 未重计算时采用以下，要考虑欧拉
    mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, m_vCovR[_scid.ToInt() / iSpace], vWIS, vWIL);
    for (int i = 0; i < _iRank; i++) {
        double dOLLAOffsetDB = m_dOLLAOffsetDB(_iRank, i);
        mSINR(i, 0) = mSINR(i, 0) * DB2L(dOLLAOffsetDB);
    }
    return mSINR;
}
double MSEBBRank1Rxer::CalcSINR_EBBCQI_AverageI_FDD(int _iRank, SCID _scid, int _iBestPMI) {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
    BTS& mainbts = m_MainServBTS.GetBTS();
    MS& ms = m_MSID.GetMS();
    //获取有用信号
    cmat mS = mainbts.GetTxCRS(_iRank + 1);

    cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
            mainbts, ms, _scid.ToInt());

    cmat mP;
    // todo: if PMI_based_Precoding == true
    if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1) {
        mP = m_pCodeBook->GetCodeWord(_iBestPMI, _iRank);
    } else {
        cmat mU, mD;
        vec vS;
        svd(mH, mU, vS, mD);
//        cmat mD = lm.Get_mD_wABF_for_all_active_TXRU_Pairs(mainbts, ms, _scid.ToInt());
        mP = mD.get_cols(0, _iRank);
    }

    //获取干扰信息
    vector<cmat> vWIS;
    vector<double> vWIL;
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
        if (!bts.GetSBUsedFlag(_scid.GetSBID()))
            continue;
        if (!lm.IsStrong(bts, ms)) {
            //获取弱干扰的信号
            cmat mWIS = bts.GetTxDataSymbol(_scid);
            vWIS.push_back(mWIS);
            //获取弱干扰大尺度衰落
            double dPLI = DB2L(lm.GetCouplingLossDB(bts, ms));
            vWIL.push_back(dPLI);
        }
    }
    //@brief 未重计算时采用以下，要考虑欧拉
    mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, m_vCovR[_scid.ToInt() / iSpace], vWIS, vWIL);
    double dSINR = 0.0;
    for(int i=0;i<mSINR.rows();++i){
        dSINR +=mSINR(i,0);
    }
    dSINR/=mSINR.rows();
    double dOLLAOffsetDB = m_dOLLAOffsetDB(_iRank, 0);
    dSINR *= DB2L(dOLLAOffsetDB);
    return dSINR;
}

void MSEBBRank1Rxer::ComputeEstimateSINR_ForSelectedRank_EBBCQI_AverageI(
        int _iRank, vector<mat>& _vmSINR4SelectedRank, map<SBID, int>& _vBestPMI) {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度

    BTS& mainbts = m_MainServBTS.GetBTS();
    MS& ms = m_MSID.GetMS();

    //子载波Loop

    for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {

        _vmSINR4SelectedRank[scid.ToInt() / iSpace].set_size(_iRank + 1, 1);
        _vmSINR4SelectedRank[scid.ToInt() / iSpace] = itpp::zeros(_iRank + 1, 1);


        _vmSINR4SelectedRank[scid.ToInt() / iSpace] = CalcSINR_EBBCQI_AverageI(_iRank, scid, _vBestPMI[scid.GetSBID()]);

    }
}
double MSEBBRank1Rxer::EstimateWholeBandCapacity_For_mSINR(vector<double>& _vmSINR4SelectedRank,int v) {
    LinkLevelInterface& lli = LinkLevelInterface::Instance_DL();
    int iMCS = lli.SINR2MCS2(_vmSINR4SelectedRank,v);
    double dCapacityKbit = lli.GetTBSizeKBit_woDMRS2(iMCS, Parameters::Instance().BASIC.IRBNum,v);
    return dCapacityKbit;
}
double MSEBBRank1Rxer::EstimateWholeBandCapacity_For_mSINR(vector<mat>& _vmSINR4SelectedRank) {
    LinkLevelInterface& lli = LinkLevelInterface::Instance_DL();
    itpp::imat mMCS = lli.SINR2MCS(_vmSINR4SelectedRank);
    double dCapacityKbit = lli.GetTBSizeKBit_woDMRS(mMCS, Parameters::Instance().BASIC.IRBNum);
    return dCapacityKbit;
}

void MSEBBRank1Rxer::ComputeEstimateSINR_ForSelectedRank_EBBCQI_AverageI_TDD(
        int _iRank, vector<double>& _vmSINR4SelectedRank) {
    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度

    BTS& mainbts = m_MainServBTS.GetBTS();
    MS& ms = m_MSID.GetMS();

    //子载波Loop

    for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
        _vmSINR4SelectedRank[scid.ToInt() / iSpace] = CalcSINR_EBBCQI_AverageI_TDD(_iRank, scid);
    }
}
double MSEBBRank1Rxer::CalcSINR_EBBCQI_AverageI_TDD(int _iRank, SCID _scid) {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    BTS& mainbts = m_MainServBTS.GetBTS();
    MS& ms = m_MSID.GetMS();
    //获取有用信号
    cmat mS = mainbts.GetTxCRS(_iRank + 1);

    cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
            mainbts, ms, _scid.ToInt());

    cmat mP;

    cmat mU, mD;
    vec vS;
    svd(mH, mU, vS, mD);
//    cmat mD = lm.Get_mD_wABF_for_all_active_TXRU_Pairs(mainbts, ms, _scid.ToInt());
    mP = mD.get_cols(0, _iRank);


    vector<cmat> vSIS, vSIH, vSIP;
        CalStrongInterference(vSIS, vSIH, vSIP,
                m_MainServBTS, m_MSID, _scid);
    //获取干扰信息
    vector<cmat> vWIS;
    vector<double> vWIL;
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
        if (!bts.GetSBUsedFlag(_scid.GetSBID()))
            continue;
        if (!lm.IsStrong(bts, ms)) {
            //获取弱干扰的信号
            cmat mWIS = bts.GetTxDataSymbol(_scid);
            vWIS.push_back(mWIS);
            //获取弱干扰大尺度衰落
            double dPLI = DB2L(lm.GetCouplingLossDB(bts, ms));
            vWIL.push_back(dPLI);
        }
    }
    //@brief 未重计算时采用以下，要考虑欧拉
    mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, mP, vSIS,vSIH,vSIP, vWIS, vWIL);
    double dSINR = 0.0;
    for(int i=0;i<mSINR.rows();++i){
        dSINR +=mSINR(i,0);
    }
    dSINR/=mSINR.rows();
    double dOLLAOffsetDB = m_dOLLAOffsetDB(_iRank, 0);
    dSINR *= DB2L(dOLLAOffsetDB);
    return dSINR;
}


void MSEBBRank1Rxer::RefreshHARQSINR(int harqID, int iRank){
    m_pHARQRxStateMS->Refresh(harqID, iRank);
}

void MSEBBRank1Rxer::SelectRankTDDNew() {
    vector<double> vRank2Capacity(m_iMaxRank + 1, 0.0);
    m_iBestRank = -1;
    double dC = -1;
    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
//    vector<vector<double>> vRank_SC_SINR;//[rank][scid]SINR
    vRank_SC_SINR.resize(m_iMaxRank + 1, vector<double>(
            Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, 0.0));
    vector<cmat> vmS;
    vmS.resize(m_iMaxRank + 1);//[rank]mS
    int totalscid = Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace;
    cm::LinkMatrix &lm = cm::LinkMatrix::Instance();
    BTS &mainbts = m_MainServBTS.GetBTS();
    MS &ms = m_MSID.GetMS();
    for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
        cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(mainbts, ms, scid.ToInt());
        vector<cmat> vSIS;
        vector<cmat> vSIH;
        vector<cmat> vSIP;
        CalStrongInterference(vSIS, vSIH, vSIP, m_MainServBTS, m_MSID, scid);
        vector<cmat> vWIS;
        vector<double> vWIL;
        assert(vWIS.size() == vWIL.size());

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

            BTS &bts = btsid.GetBTS();
            if (!bts.GetSBUsedFlag(scid.GetSBID()))
                continue;
            if (!lm.IsStrong(bts, ms)) {
                //获取弱干扰的信号
                cmat mWIS = bts.GetTxDataSymbol(scid);
//                cmat mWIS = bts.GetTxCRS(1);
                vWIS.push_back(mWIS);
                //获取弱干扰大尺度衰落
                double dPLI = DB2L(lm.GetCouplingLossDB(bts, ms));
                vWIL.push_back(dPLI);
                assert(vWIS.size() == vWIL.size());

            }
        }
        for (int rank = 0; rank <= m_iMaxRank; ++rank) {
            cmat mS = mainbts.GetTxCRS(rank + 1);
            vmS[rank] = mS;
        }
        int index = scid.ToInt() / iSpace;
        m_pDetector->CalculateSINR4Ranks(vmS, mH, vSIS, vSIH, vSIP, vWIS, vWIL, vRank_SC_SINR, index);

    }
    for (int rank = 0; rank <= m_iMaxRank; ++rank) {
        double dCapacity = EstimateWholeBandCapacity_For_mSINR(vRank_SC_SINR[rank], rank + 1);
        vRank2Capacity[rank] = dCapacity;
        if (vRank2Capacity[rank] > dC) {
            dC = vRank2Capacity[rank];
            m_iBestRank = rank;
        }
    }
    m_iBestRank = std::min(m_iBestRank, m_iMaxRank);
    return;
}
