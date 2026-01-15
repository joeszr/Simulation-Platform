#include "../BaseStation/BTSID.h"
#include "../MobileStation/MSID.h"
#include "../Utility/SBID.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "../BaseStation/BTS.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../MobileStation/CQIMessage.h"
#include "../BaseStation/CodeBook.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../Utility/RBID.h"
#include "../Utility/SCID.h"
#include "../NetworkDrive/Clock.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../MobileStation/MS.h"
#include "CompetorRankA.h"
#include <itpp/stat/misc_stat.h>
#include "../BaseStation/CodeBookFactory.h"
#include "../DetectAlgorithm/Detector_Common.h"

CompetorRankA::CompetorRankA():m_mMSID2Rank(Parameters::Instance().BASIC.DNumMSPerBTS),
m_vMSID2SINR(Parameters::Instance().BASIC.DNumMSPerBTS),
m_mMSID2SINR(Parameters::Instance().BASIC.DNumMSPerBTS),
m_dMSID2SINR(Parameters::Instance().BASIC.DNumMSPerBTS),
m_mMSID2MCS(Parameters::Instance().BASIC.DNumMSPerBTS),
m_iMSID2MCS(Parameters::Instance().BASIC.DNumMSPerBTS),
m_mSBID2Capacity(Parameters::Instance().BASIC.ISBNum),
m_mMSID2Capacity(Parameters::Instance().BASIC.DNumMSPerBTS)
{
    /*
    
        if (Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel == 2) {
            m_pCodeBook = std::shared_ptr<CodeBook > (new CodeBookLTE2TX);
        } else if (Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel == 4) {
            m_pCodeBook = std::shared_ptr<CodeBook > (new CodeBookLTE4TX);
        }
     */

    m_pCodeBook = CodeBookFactory::Instance().GetCodeBook(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num);
    m_Priority = -1000;
    m_Capacity = -1000;
}

CompetorRankA::CompetorRankA(const vector<MSID>& _v):m_mMSID2Rank(Parameters::Instance().BASIC.DNumMSPerBTS),
                                                     m_vMSID2SINR(Parameters::Instance().BASIC.DNumMSPerBTS),
                                                     m_mMSID2SINR(Parameters::Instance().BASIC.DNumMSPerBTS),
                                                     m_dMSID2SINR(Parameters::Instance().BASIC.DNumMSPerBTS),
                                                     m_mMSID2MCS(Parameters::Instance().BASIC.DNumMSPerBTS),
                                                     m_iMSID2MCS(Parameters::Instance().BASIC.DNumMSPerBTS),
                                                     m_mSBID2Capacity(Parameters::Instance().BASIC.ISBNum),
                                                     m_mMSID2Capacity(Parameters::Instance().BASIC.DNumMSPerBTS) {
    /*
    
        if (Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel == 2) {
            m_pCodeBook = std::shared_ptr<CodeBook > (new CodeBookLTE2TX);
        } else if (Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel == 4) {
            m_pCodeBook = std::shared_ptr<CodeBook > (new CodeBookLTE4TX);
        }
     */

    m_pCodeBook = CodeBookFactory::Instance().GetCodeBook(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num);
    copy(_v.begin(), _v.end(), back_inserter(*this));
}

//@threads
void CompetorRankA::ComputeCompeteDataEigenBasedBF(
        const SBID& _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS) {

    // 计算预编码字
//    int mapsize = _mMSID2MSTxBufferBTS.size();
//    
//    cout<<size()<<endl;
    
    int iPairNum = static_cast<int> (size());
    LinkLevelInterface& lli = LinkLevelInterface::Instance_DL();
    int iTimeslot = Clock::Instance().GetTimeSlot();
    if (_sbid.ToInt() == 0 && Parameters::Instance().MIMO_CTRL.IApSounding == Parameters::IApSounding) {
        for (int i = 0; i < iPairNum; ++i) {
            MSID msid = (*this)[i];
            //                ms进行ApSounding
            BTS& bts = msid.GetMS().GetMainServBTS().GetBTS();
            msid.GetMS().BeginApSounding();
            bts.ReceiveApSoundingMessage();
        }
    }
    if (iPairNum == 1) {
        MSID msid = (*this)[0];
        MSTxBufferBTS& buffer = _mMSID2MSTxBufferBTS[msid.ToInt()];
        std::shared_ptr<CQIMessage> pCQIMes = buffer.GetCQIMessage();

//        itpp::imat mMCS = pCQIMes->GetSubBandMCS(_sbid);
//        itpp::imat mMCS = pCQIMes->GetWideBandMCS();
        itpp::imat mMCS;
        int iRank = m_mMSID2Rank[msid.ToInt()];
//        cout<<m_mMSID2Rank[msid]<<endl;
//        int iMCS = mMCS(0, 0);
//        double dSINR = lli.MCS2SINR(iMCS, Parameters::Instance().BASIC.ISBSize, GetTBSizeKbit(iMCS,iRank+1));
        double dSINR = DB2L(buffer.GetCQISINR(iRank));
//        double dSINR = lli.MCS2SINR_simplified(iMCS);
        //改完AMC之后这里用不到预编码矩阵了，所以等到确定最佳竞争体之后再计算预编码矩阵   只用于SU   fullbuffer的话跟原来保持一致
        if(Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FullBuffer){
            std::shared_ptr<CQIMessage> pCQIMes = buffer.GetCQIMessage();
            if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1) {
                int iPMI = pCQIMes->GetSubBandPMI(_sbid);
                m_CodeWord = m_pCodeBook->GetCodeWord(iPMI, iRank);
            } else {
                itpp::cmat mTemp;
                if (Parameters::Instance().MIMO_CTRL.IHorCovR == 1) {
                    mTemp = buffer.GetCovR(_sbid);
                } else {
                    mTemp = buffer.GetH(_sbid);
                }
                vec vS;
                cmat mU, mD;
                svd(mTemp, mU, vS, mD);
                cmat mV = mD.get_cols(0, iRank);
                m_CodeWord = mV;
            }
        }

        //assert((m_CodeWord.rows()) ==
        //(*this)[0].GetMS().GetMainServBTS().GetBTS().GetAntennaPointer()->GetTotalTXRU_Num());


        /*重计算开始*/
        imat mBFMCS = zeros_i(iRank + 1, 1);
        vector<double> tempSINR;
        int iBFMCS;
        if (Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1) {
            mBFMCS = mMCS;
        } else {
//            tempSINR = ReComputeVSINR(msid, m_CodeWord, m_CodeWord, _sbid, _mMSID2MSTxBufferBTS, dSINR);
            tempSINR = ReComputeVSINR_simplified(msid, _sbid, _mMSID2MSTxBufferBTS, dSINR);
//            iBFMCS = LinkLevelInterface::Instance_DL().SINR2MCS2(tempSINR,1);
            iBFMCS = LinkLevelInterface::Instance_DL().SINR2MCS_simplified(tempSINR);
        }
        
        // lc for dl
//        for(int i = 0; i < mBFMCS.rows();i++){
//            for(int j = 0; j < mBFMCS.cols(); j++){
//                if(mBFMCS(i,j) > 20){
//                    mBFMCS(i,j) = 20;
//                }
//            }
//        }
        //        cout<<"mMCS= "<<mMCS<<endl;
        //        cout<<"mBFMCS= "<<mBFMCS<<endl;
        /*重计算完毕*/
        //m_mMSID2MCS[msid] = mBFMCS;
        m_iMSID2MCS[msid.ToInt()] = iBFMCS;
        double dTBSKbit = GetTBSizeKbit(iBFMCS,iRank+1);
        //m_mMSID2SINR[msid] = lli.MCS2SINR(mBFMCS, Parameters::Instance().BASIC.ISBSize, dTBSKbit);
        m_vMSID2SINR[msid.ToInt()] = tempSINR;

        m_Capacity = dTBSKbit;
        m_mMSID2Capacity[msid.ToInt()] = dTBSKbit;

        double dPriority = 0.0;
        if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FullBuffer) {
            double dSuccessRxKbit = buffer.GetRateKbit();
            dPriority = pow(dTBSKbit, Parameters::Instance().SIM.DL.DProportionFairFactor) / dSuccessRxKbit;
            //double dTotalTxKbit = buffer.GetTotalTxKbit();
            //dPriority = pow(dTBSKbit, Parameters::Instance().SIM.DL.DProportionFairFactor) /dTotalTxKbit;
        } else if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP||Parameters::Instance().TRAFFIC.ITrafficModel ==Parameters::MixedTraffic) {
            double dMSID2BorntimeSec = _mMSID2MSTxBufferBTS[msid.ToInt()].GetFirstPacketBorntimeSlot();
            double dMSID2PackerSizeKbits = _mMSID2MSTxBufferBTS[msid.ToInt()].GetDLFirstAvailablePacket_RemainUntransmitted_SizeKbits();
            dPriority = (dTBSKbit * (iTimeslot - dMSID2BorntimeSec)) / dMSID2PackerSizeKbits;
            //下面注释的调度算法为会造成拖包，造成边缘比较低，但是多用户配对比较好
            //			 double dSuccessRxKbit = buffer.GetRateKbit();
            //			 dPriority = pow(dTBSKbit, Parameters::Instance().SIM.DL.DProportionFairFactor) / dSuccessRxKbit;
        } else if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_XR
                || Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_XRmulti) {

            double dSuccessRxKbit = buffer.GetRateKbit();
            double dBreakingPoint = 0.5 * Parameters::Instance().XR.iDelayBudget_slot;
            double dDelayWeight;
            double dMSID2Borntime_slot = _mMSID2MSTxBufferBTS[msid.ToInt()].GetFirstPacketBorntimeSlot();
            if ((iTimeslot - dMSID2Borntime_slot) <= dBreakingPoint) {
                dDelayWeight = (iTimeslot - dMSID2Borntime_slot) * (1.0 / dBreakingPoint);
            } else {
                dDelayWeight = 1;
            }
            dPriority = dDelayWeight * pow(dTBSKbit, Parameters::Instance().SIM.DL.DProportionFairFactor) / dSuccessRxKbit;

        }
        else {
            cout << "ComputeCompeteDataEigenBasedBF Wrong" << endl;
        }
        m_Priority = dPriority;
    } else if (iPairNum >= 2) {
        vector<itpp::cmat> vChannelMat;
        vector<itpp::cmat> vCodeWord;
        vector<int> vRank;
        vector<int> vRank_real;
        int iTotalRank = 0;
        if (Parameters::Instance().MIMO_CTRL.IHorCovR) {
            for (int i = 0; i < iPairNum; ++i) {
                MSID msid = (*this)[i];
                MSTxBufferBTS& buffer = _mMSID2MSTxBufferBTS[msid.ToInt()];
                itpp::cmat mTemp;
                if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1) {
                    mTemp = buffer.GetH(_sbid);
                } else {
                    mTemp = buffer.GetCovR(_sbid);
                }
                if (Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1) {
                    cmat final_matrix = ChangeMatFull2Half(mTemp);
                    vChannelMat.push_back(final_matrix);
                    vRank.push_back(0); //区分1Tx or 2Tx
                } else {
                    vChannelMat.push_back(mTemp);
                    vRank.push_back(m_mMSID2Rank[msid.ToInt()]);
                }
                vRank_real.push_back(m_mMSID2Rank[msid.ToInt()]);
                iTotalRank += (m_mMSID2Rank[msid.ToInt()] + 1);
            }
            /*
                        BDR_RankA(vChannelMat, vCodeWord, vRank);
             */

            //20180720
            bool _bIsCovR = false;
            MET_RankA(vChannelMat, vCodeWord, vRank, _bIsCovR);

        } else {
            for (int i = 0; i < iPairNum; ++i) {
                MSID msid = (*this)[i];
                MSTxBufferBTS& buffer = _mMSID2MSTxBufferBTS[msid.ToInt()];
                itpp::cmat mTemp = buffer.GetH(_sbid); //把mTemp做svd分解后取第三个，为64*64，然后只取其奇数行和奇数列，得到32*32矩阵，上面那一快还要加一句取第一列
                if (Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1) {
                    cmat final_matrix = ChangeMatFull2Half(mTemp);
                    vChannelMat.push_back(final_matrix);
                    vRank.push_back(0);
                } else {
                    vChannelMat.push_back(mTemp);
                    vRank.push_back(m_mMSID2Rank[msid.ToInt()]);
                }
                vRank_real.push_back(m_mMSID2Rank[msid.ToInt()]);
                iTotalRank += (m_mMSID2Rank[msid.ToInt()] + 1);

            }
            /*
                        BD_RankA(vChannelMat, vCodeWord, vRank);
             */

            //20180720
            bool _bIsCovR = false;
            MET_RankA(vChannelMat, vCodeWord, vRank, _bIsCovR);
        }
        itpp::cmat mCodeWord(vCodeWord[0].rows(), iTotalRank);
        int iTotalRankTemp = 0;
        if (Parameters::Instance().MIMO_CTRL.IEnable_Portbased_BF_CSIRS == 1) {
            for (int i = 0; i<static_cast<int> (vCodeWord.size()); ++i) {
                MSID msid = (*this)[i];
                MSTxBufferBTS& buffer = _mMSID2MSTxBufferBTS[msid.ToInt()];
                std::shared_ptr<CQIMessage> pCQIMes = buffer.GetCQIMessage();
                int iPMI = pCQIMes->GetSubBandPMI(_sbid);
                cmat vCode_Temp = ProductPrecodeMat(vCodeWord[i]);
                mCodeWord.set_cols(iTotalRankTemp, vCode_Temp * m_pCodeBook->GetCodeWord(iPMI, m_mMSID2Rank[msid.ToInt()])); //在这里改，vCodeWord[i]都是32*1矩阵
                iTotalRankTemp += (vRank_real[i] + 1);
            }
        } else {
            for (int i = 0; i<static_cast<int> (vCodeWord.size()); ++i) {
                mCodeWord.set_cols(iTotalRankTemp, vCodeWord[i]);
                iTotalRankTemp += (vRank[i] + 1);
            }
        }

//        assert(mCodeWord.rows() ==
//                (*this)[0].GetMS().GetMainServBTS().GetBTS().GetAntennaPointer()->GetTotalTXRU_Num());


        m_CodeWord = mCodeWord;
        double dTotalCapacityKbit = 0;
        double dTotalPriority = 0;
        iTotalRankTemp = 0;
        for (int i = 0; i < iPairNum; ++i) {
            MSID msid = (*this)[i];
            MSTxBufferBTS& buffer = _mMSID2MSTxBufferBTS[msid.ToInt()];
            std::shared_ptr<CQIMessage> pCQIMes = buffer.GetCQIMessage();
            itpp::imat mMCS = pCQIMes->GetSubBandMCS(_sbid);
            int iMCS = mMCS(0, 0);
            double dSINR = lli.MCS2SINR(iMCS, Parameters::Instance().BASIC.ISBSize, GetTBSizeKbit(iMCS, m_mMSID2Rank[msid.ToInt()]+1));
//            int iRank = m_mMSID2Rank[msid.ToInt()];   //MU部分先保持原来的样子
//            double dSINR = DB2L(buffer.GetCQISINR(iRank));
            /*重计算开始*/
            int iPrecodeStartIndex = iTotalRankTemp;
            iTotalRankTemp += (vRank_real[i] + 1);
            int iPrecodeEndIndex = iTotalRankTemp - 1;
            cmat mP = m_CodeWord.get_cols(iPrecodeStartIndex, iPrecodeEndIndex);
            vector<double>tempSINR;
            int iBFMCS;
            tempSINR = ReComputeVSINR(msid, m_CodeWord, mP, _sbid, _mMSID2MSTxBufferBTS, dSINR);
            iBFMCS = LinkLevelInterface::Instance_DL().SINR2MCS2(tempSINR,1);
//            iBFMCS = LinkLevelInterface::Instance_DL().SINR2MCS_simplified(tempSINR);  //MU部分先保持原来的样子
            /*重计算完毕*/
            //m_mMSID2MCS[msid] = mBFMCS;
            m_iMSID2MCS[msid.ToInt()] = iBFMCS;
            double dTBSKbit = GetTBSizeKbit(iBFMCS, m_mMSID2Rank[msid.ToInt()]+1);
            //m_mMSID2SINR[msid] = lli.MCS2SINR(mBFMCS, Parameters::Instance().BASIC.ISBSize, dTBSKbit);
            m_vMSID2SINR[msid.ToInt()] = tempSINR;
            //            double dTBSKbit = lli.MCS2TBSKBit(mBFMCS, Parameters::Instance().BASIC.ISBSize);
            if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FullBuffer) {
                double dSuccessRxKbit = buffer.GetRateKbit();
                dTotalPriority += pow(dTBSKbit, Parameters::Instance().SIM.DL.DProportionFairFactor) / dSuccessRxKbit;
            }
            else if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP||Parameters::Instance().TRAFFIC.ITrafficModel ==Parameters::MixedTraffic) {
                double dMSID2PackerSizeKbits = _mMSID2MSTxBufferBTS[msid.ToInt()].GetDLFirstAvailablePacket_RemainUntransmitted_SizeKbits();
                double dMSID2BorntimeSec = _mMSID2MSTxBufferBTS[msid.ToInt()].GetFirstPacketBorntimeSlot();
                dTotalPriority += (dTBSKbit * (iTimeslot - dMSID2BorntimeSec)) / dMSID2PackerSizeKbits;
                //下面注释的调度算法为会造成拖包，造成边缘比较低，但是多用户配对比较好
                //			      double dSuccessRxKbit = buffer.GetRateKbit();
                //			      dPriority = pow(dTBSKbit, Parameters::Instance().SIM.DL.DProportionFairFactor) / dSuccessRxKbit;
            } else if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_XR
                       || Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_XRmulti) {
                double dSuccessRxKbit = buffer.GetRateKbit();
                double dms2slot = Parameters::Instance().BASIC.DSlotDuration_ms * Parameters::Instance().XR.iDelayBudget_slot;
                double dDelayWeight;
                double dMSID2BorntimeSec = _mMSID2MSTxBufferBTS[msid.ToInt()].GetFirstPacketBorntimeSlot(); //danwei
                if ((iTimeslot - dMSID2BorntimeSec) <= dms2slot) {
                    dDelayWeight = (iTimeslot - dMSID2BorntimeSec) * (1.0 / dms2slot);
                } else {
                    dDelayWeight = 1;
                }
                dTotalPriority += dDelayWeight * pow(dTBSKbit, Parameters::Instance().SIM.DL.DProportionFairFactor) / dSuccessRxKbit;
            }
            else {
                cout << "ComputeCompeteDataEigenBasedBF Wrong" << endl;
            }
            dTotalCapacityKbit += dTBSKbit;
        }
        m_Capacity = dTotalCapacityKbit;
        m_Priority = dTotalPriority;

        // 20180621
//        assert((m_CodeWord.rows()) ==
//                (*this)[0].GetMS().GetMainServBTS().GetBTS().GetAntennaPointer()->GetTotalTXRU_Num());
    }
}
//@threads
int CompetorRankA::ReComputeSINR(
        const MSID& _msid, const cmat& _mPA,
        const cmat& _mP, const SBID& _sbid,
        std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS,
        double _dTxDSINR) {

    /*重计算开始*/
    vector<double> vSBSINR;
    MSTxBufferBTS& buffer = _mMSID2MSTxBufferBTS[_msid.ToInt()];
    LinkLevelInterface& lli = LinkLevelInterface::Instance_DL();

    vector<int> vSUPrecodeword_Pos = Detector_Common::GenSUPrecodeword_Pos(_mPA, _mP);

    //    Detector_Common::CalculateSINR(_mS, _mPA,
    //            _mH, vSUPrecodeword_Pos,
    //            _vSIS, _vSIH, _vSIP,
    //            _vWIS, _vWIL,
    //            _dSCNoisePowerMw, _IMMSEOption);

    for (RBID rbid = _sbid.GetFirstRBID(); rbid <= _sbid.GetLastRBID(); ++rbid) {
        itpp::cmat mH = buffer.GetH(rbid);
        cmat SH_SU = mH * mH.H();
        double Power = 0;
        int irows = mH.rows();
        for (int i = 0; i < irows; ++i) {
            Power += abs(SH_SU(i, i));
        }
        cmat mCRS = sqrt(Parameters::Instance().Macro.DL.DSCTxPowerMw / 1) * itpp::ones_c(1, 1);
        double S_SU = (real(mCRS.H() * mCRS) (0, 0)) * Power;
        double dCarrierNoiseMW = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
        /*小区间干扰*/
        double dI_SU = S_SU / _dTxDSINR - dCarrierNoiseMW;

        int iMSAntNum = mH.rows();
        int iRank = _mP.cols();
        int iRankA = _mPA.cols();
        if (iRank != (m_mMSID2Rank[_msid.ToInt()] + 1)) {
            cout << "CompetorRankA::ReComputeSINR Error!" << endl;
        }
        //找到本用户是多用户流中的哪几流
        vector<int> vRankID(iRank, -1);

        //20180522
        int row = vRankID.size();

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

            //    assert(vRankID[iRankIndex] != -1);
            if (vRankID[iRankIndex] == -1) {
                cout << "Error! Can't find the correct rank id!" << endl;
            }
            assert(vRankID[iRankIndex] != -1);
        }

        itpp::cmat mS = sqrt(Parameters::Instance().Macro.DL.DSCTxPowerMw / iRankA) * itpp::ones_c(iRankA, 1);
        itpp::cmat Rc(iMSAntNum, iMSAntNum);
        Rc.zeros();
        // 根据流间干扰累计干扰相关矩阵
        if (iRankA > iRank) {
            itpp::cmat t = itpp::eye_c(iRankA);
            for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
                t(vRankID[iRankIndex], vRankID[iRankIndex]) = 0;
            }
            itpp::cmat mP2 = _mPA * t;
            itpp::cmat mTH = mH * mP2;
            itpp::cmat mTS = t.T() * mS;
            ///这种计算方法只能适用于每个流之间等功率分配的情况
            Rc += (real((mTS.H() * mTS)(0, 0)) / (iRankA - iRank)) * mTH * mTH.H();
        }
        itpp::cmat SH = mH * _mP;
        //3.最不理想
        //        itpp::cmat mSSH = (real((mS.H() * mS) (0, 0)) / iRankA) * SH * SH.H();
        //        itpp::cmat mTemp = Rc+ (dI_SU/iMSAntNum)*itpp::eye_c(iMSAntNum)+mSSH;
        //        itpp:cmat G = SH.H()*itpp::inv(mTemp);
        itpp::cmat G = SH.H() * itpp::inv((real((mS.H() * mS) (0, 0)) / iRankA) * SH * SH.H() + Rc + (dI_SU / iMSAntNum) * itpp::eye_c(iMSAntNum));
        itpp::cmat W = G * mH * _mPA; // 计算均衡后的等效信道矩阵

        itpp::cmat mSTemp = mS;
        for (int iRankIndex = 1; iRankIndex < iRank; ++iRankIndex) {
            mSTemp = itpp::concat_horizontal(mSTemp, mS);
        }
        itpp::cmat symbol_de = itpp::elem_mult(W.T(), mSTemp); //计算均衡后的接收符号
        itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率
        //得到本用户的信号功率
        itpp::mat SS = zeros(iRank, 1);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            SS(iRankIndex, 0) = S(vRankID[iRankIndex], iRankIndex);
        }

        itpp::mat ISelfR = zeros(iRank, 1);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            ISelfR(iRankIndex, 0) = (itpp::ones(1, iRankA) * S.get_cols(iRankIndex, iRankIndex))(0, 0) - SS(iRankIndex, 0);
        }
        itpp::mat I_MU = ISelfR;
        //3.最不理想
        itpp::mat I_SU = (dI_SU / iMSAntNum) * real(diag(diag(G * G.H())));
        itpp::mat mNoise = dCarrierNoiseMW * real(diag(diag(G * G.H())));
        assert(I_SU.rows() == iRank);
        assert(I_SU.cols() == iRank);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            I_MU(iRankIndex, 0) += I_SU(iRankIndex, iRankIndex);
            I_MU(iRankIndex, 0) += mNoise(iRankIndex, iRankIndex);
        }
        itpp::mat mSINR = itpp::elem_div(SS, I_MU);
        int iSampleNum = Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace;
        for (int index = 0; index < iSampleNum; ++index) {
            double temp = 0.0;
            for (int iRankIndex = 0; iRankIndex < mSINR.rows(); ++iRankIndex) {
                temp+=mSINR(iRankIndex, 0);
            }
            temp/=mSINR.rows();
            //temp *= DB2L(_msid.GetMS().GetOllA(mSINR.rows()-1));
            
            vSBSINR.push_back(temp);
        }
    }
    /*重计算完毕*/
    int iBFMCS = lli.SINR2MCS2(vSBSINR,_mP.cols());
    return iBFMCS;

}
//@threads
vector<double> CompetorRankA::ReComputeVSINR(
        const MSID& _msid, const cmat& _mPA,
        const cmat& _mP, const SBID& _sbid,
        std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS,
        double _dTxDSINR) {

    /*重计算开始*/
    vector<double> vSBSINR;
    MSTxBufferBTS& buffer = _mMSID2MSTxBufferBTS[_msid.ToInt()];
    double ollaSINR = DB2L(buffer.GetOllaSINR());
    LinkLevelInterface& lli = LinkLevelInterface::Instance_DL();

    vector<int> vSUPrecodeword_Pos = Detector_Common::GenSUPrecodeword_Pos(_mPA, _mP);

    //    Detector_Common::CalculateSINR(_mS, _mPA,
    //            _mH, vSUPrecodeword_Pos,
    //            _vSIS, _vSIH, _vSIP,
    //            _vWIS, _vWIL,
    //            _dSCNoisePowerMw, _IMMSEOption);

    for (RBID rbid = _sbid.GetFirstRBID(); rbid <= _sbid.GetLastRBID(); ++rbid) {
        itpp::cmat mH = buffer.GetH(rbid);
        cmat SH_SU = mH * mH.H();
        double Power = 0;
        int irows = mH.rows();
        for (int i = 0; i < irows; ++i) {
            Power += abs(SH_SU(i, i));
        }
        cmat mCRS = sqrt(Parameters::Instance().Macro.DL.DSCTxPowerMw / 1) * itpp::ones_c(1, 1);
        double S_SU = (real(mCRS.H() * mCRS) (0, 0)) * Power/_mP.cols();
        double dCarrierNoiseMW = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
        /*小区间干扰*/
        double dI_SU = S_SU / _dTxDSINR - dCarrierNoiseMW;

        int iMSAntNum = mH.rows();
        int iRank = _mP.cols();
        int iRankA = _mPA.cols();
        if (iRank != (m_mMSID2Rank[_msid.ToInt()] + 1)) {
            cout << "CompetorRankA::ReComputeSINR Error!" << endl;
        }
        //找到本用户是多用户流中的哪几流
        vector<int> vRankID(iRank, -1);

        //20180522
        int row = vRankID.size();

        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            std::vector<int> vToBePastedRanks;
            for (int k1 = 0; k1 < iRankIndex; k1++) {
                vToBePastedRanks.push_back(k1);
            }

            for (int icol = 0; icol < iRankA; ++icol) {
                if (vToBePastedRanks.empty()) {
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

            //    assert(vRankID[iRankIndex] != -1);
            if (vRankID[iRankIndex] == -1) {
                cout << "Error! Can't find the correct rank id! ReComputeVSINR" << endl;
            }
            assert(vRankID[iRankIndex] != -1);
        }

        itpp::cmat mS = sqrt(Parameters::Instance().Macro.DL.DSCTxPowerMw / iRankA) * itpp::ones_c(iRankA, 1);
        itpp::cmat Rc(iMSAntNum, iMSAntNum);
        Rc.zeros();
        // 根据流间干扰累计干扰相关矩阵
        if (iRankA > iRank) {
            itpp::cmat t = itpp::eye_c(iRankA);
            for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
                t(vRankID[iRankIndex], vRankID[iRankIndex]) = 0;
            }
            itpp::cmat mP2 = _mPA * t;
            itpp::cmat mTH = mH * mP2;
            itpp::cmat mTS = t.T() * mS;
            ///这种计算方法只能适用于每个流之间等功率分配的情况
            Rc += (real((mTS.H() * mTS)(0, 0)) / (iRankA - iRank)) * mTH * mTH.H();
        }
        itpp::cmat SH = mH * _mP;
        //3.最不理想
        //        itpp::cmat mSSH = (real((mS.H() * mS) (0, 0)) / iRankA) * SH * SH.H();
        //        itpp::cmat mTemp = Rc+ (dI_SU/iMSAntNum)*itpp::eye_c(iMSAntNum)+mSSH;
        //        itpp:cmat G = SH.H()*itpp::inv(mTemp);
        itpp::cmat G = SH.H() * itpp::inv((real((mS.H() * mS) (0, 0)) / iRankA) * SH * SH.H() + Rc + (dI_SU / iMSAntNum) * itpp::eye_c(iMSAntNum));
        itpp::cmat W = G * mH * _mPA; // 计算均衡后的等效信道矩阵

        itpp::cmat mSTemp = mS;
        for (int iRankIndex = 1; iRankIndex < iRank; ++iRankIndex) {
            mSTemp = itpp::concat_horizontal(mSTemp, mS);
        }
        itpp::cmat symbol_de = itpp::elem_mult(W.T(), mSTemp); //计算均衡后的接收符号
        itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率
        //得到本用户的信号功率
        itpp::mat SS = zeros(iRank, 1);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            SS(iRankIndex, 0) = S(vRankID[iRankIndex], iRankIndex);
        }

        itpp::mat ISelfR = zeros(iRank, 1);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            ISelfR(iRankIndex, 0) = (itpp::ones(1, iRankA) * S.get_cols(iRankIndex, iRankIndex))(0, 0) - SS(iRankIndex, 0);
        }
        itpp::mat I_MU = ISelfR;
        //3.最不理想
        itpp::mat I_SU = (dI_SU / iMSAntNum) * real(diag(diag(G * G.H())));
        itpp::mat mNoise = dCarrierNoiseMW * real(diag(diag(G * G.H())));
        assert(I_SU.rows() == iRank);
        assert(I_SU.cols() == iRank);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            I_MU(iRankIndex, 0) += I_SU(iRankIndex, iRankIndex);
            I_MU(iRankIndex, 0) += mNoise(iRankIndex, iRankIndex);
        }
        itpp::mat mSINR = itpp::elem_div(SS, I_MU);
        int iSampleNum = Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace;
        for (int index = 0; index < iSampleNum; ++index) {
            double temp = 0.0;
            for (int iRankIndex = 0; iRankIndex < mSINR.rows(); ++iRankIndex) {
                temp+=mSINR(iRankIndex, 0);
            }
            temp/=mSINR.rows();
            temp *= DB2L(_msid.GetMS().GetOllA(mSINR.rows()-1));
//            temp *= ollaSINR;
            vSBSINR.push_back(temp);
        }
    }

    return vSBSINR;
}
vector<double> CompetorRankA::ReComputeVSINR_simplified(
        MSID _msid, SBID _sbid,
        std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS,
        double _dTxDSINR) {

    /*重计算开始*/
    int iSampleNum = Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace;
    int length = Parameters::Instance().BASIC.ISBSize * iSampleNum;

    int rank = m_mMSID2Rank[_msid.ToInt()];
    MSTxBufferBTS& buffer = _mMSID2MSTxBufferBTS[_msid.ToInt()];
    double msolla = DB2L(_msid.GetMS().GetOllA(rank));
    double ollaSINR = DB2L(buffer.GetOllaSINR());

    double temp = _dTxDSINR*ollaSINR*msolla;

    vector<double> vSBSINR(length, temp);
    return vSBSINR;
}

imat CompetorRankA::ReComputeIdealSINR(const MSID& _msid, const cmat& _mPA, const cmat& _mP, const SBID& _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS, double _dTxDSINR) {
    /*重计算开始*/
    cout << "ideal" << endl;
    vector<mat> vSBSINR;
    int iMSAntNum = Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel;
    MSTxBufferBTS& buffer = _mMSID2MSTxBufferBTS[_msid.ToInt()];
    LinkLevelInterface& lli = LinkLevelInterface::Instance_DL();
    for (RBID rbid = _sbid.GetFirstRBID(); rbid <= _sbid.GetLastRBID(); ++rbid) {
        itpp::cmat mH = buffer.GetH(rbid);
        cmat SH_SU = mH * mH.H();
        double Power = 0;
        int irows = mH.rows();
        for (int i = 0; i < irows; ++i) {
            Power += abs(SH_SU(i, i));
        }
        cmat mCRS = sqrt(Parameters::Instance().Macro.DL.DSCTxPowerMw / 1) * itpp::ones_c(1, 1);
        double S_SU = (real(mCRS.H() * mCRS) (0, 0)) * Power;
        double dCarrierNoiseMW = Parameters::Instance().MSS.FirstBand.DL.DSCNoisePowerMw;
        /*小区间干扰*/
        double dI_SU = S_SU / _dTxDSINR - dCarrierNoiseMW;
        int iRankA = _mPA.cols();
        int iRank = m_mMSID2Rank[_msid.ToInt()] + 1;
        if (iRank != _mP.cols()) {
            cout << "CompetorRankA::ReComputeSINR Error!" << endl;
        }
        assert(iRank == _mP.cols());
        itpp::cmat mS = sqrt(Parameters::Instance().Macro.DL.DSCTxPowerMw / iRankA) * itpp::ones_c(iRankA, 1);
        //找到本用户是多用户流中的哪几流
        vector<int> vRankID(iRank, -1);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            for (int icol = 0; icol < iRankA; ++icol) {
                if (iRankIndex == 1) {
                    if (icol == vRankID[0]) continue;
                }
                if (_mP.get_cols(iRankIndex, iRankIndex) == _mPA.get_cols(icol, icol)) {
                    vRankID[iRankIndex] = icol;
                    break;
                }
            }
            assert(vRankID[iRankIndex] != -1);
            if (vRankID[iRankIndex] == -1) {
                cerr << "Can't find the correct rank id!";
                exit(0);
            }
        }

        itpp::cmat Rc(iMSAntNum, iMSAntNum);
        Rc.zeros();
        // 根据流间干扰累计干扰相关矩阵
        if (iRankA > iRank) {
            itpp::cmat t = itpp::eye_c(iRankA);
            for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
                t(vRankID[iRankIndex], vRankID[iRankIndex]) = 0;
            }
            itpp::cmat mP2 = _mPA * t;
            itpp::cmat mTH = mH * mP2;
            itpp::cmat mTS = t.T() * mS;
            ///这种计算方法只能适用于每个流之间等功率分配的情况
            Rc += (real((mTS.H() * mTS)(0, 0)) / (iRankA - iRank)) * mTH * mTH.H();
        }
        itpp::cmat SH = mH * _mP;
        itpp::cmat G = SH.H();
        //3.最不理想
        itpp::cmat mCovR = (dI_SU / iMSAntNum) * itpp::eye_c(iMSAntNum);
        if (Clock::Instance().GetTimeSlot() > (Parameters::Instance().ERROR.ICQI_PERIOD + 1)) {
            mCovR = _msid.GetMS().GetCovR(rbid.GetFirstSCID());
        }
        G = SH.H() * itpp::inv((real((mS.H() * mS) (0, 0)) / iRankA) * SH * SH.H() + Rc + itpp::diag(itpp::diag(mCovR)));
        itpp::cmat W = G * mH * _mPA; // 计算均衡后的等效信道矩阵

        itpp::cmat mSTemp = mS;
        for (int iRankIndex = 1; iRankIndex < iRank; ++iRankIndex) {
            mSTemp = itpp::concat_horizontal(mSTemp, mS);
        }
        itpp::cmat symbol_de = itpp::elem_mult(W.T(), mSTemp); //计算均衡后的接收符号
        itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率
        //得到本用户的信号功率
        itpp::mat SS = zeros(iRank, 1);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            SS(iRankIndex, 0) = S(vRankID[iRankIndex], iRankIndex);
        }

        itpp::mat ISelfR = zeros(iRank, 1);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            ISelfR(iRankIndex, 0) = (itpp::ones(1, iRankA) * S.get_cols(iRankIndex, iRankIndex))(0, 0) - SS(iRankIndex, 0);
        }
        itpp::mat I_MU = ISelfR;
        //3.最不理想
        itpp::mat I_SU = (dI_SU / iMSAntNum) * real(diag(diag(G * G.H())));
        if (Clock::Instance().GetTimeSlot() > (Parameters::Instance().ERROR.ICQI_PERIOD + 1)) {
            I_SU = real(diag(diag(G * mCovR * G.H())));
        }
        itpp::mat mNoise = dCarrierNoiseMW * real(diag(diag(G * G.H())));
        assert(I_SU.rows() == iRank);
        assert(I_SU.cols() == iRank);
        for (int iRankIndex = 0; iRankIndex < iRank; ++iRankIndex) {
            I_MU(iRankIndex, 0) += I_SU(iRankIndex, iRankIndex);
            I_MU(iRankIndex, 0) += mNoise(iRankIndex, iRankIndex);
        }
        itpp::mat mSINR = itpp::elem_div(SS, I_MU);
        int iSampleNum = Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace;
        for (int index = 0; index < iSampleNum; ++index) {
//            for (int iRankIndex = 0; iRankIndex < mSINR.rows(); ++iRankIndex) {
//                mSINR(iRankIndex, 0) = mSINR(iRankIndex, 0) * DB2L(_msid.GetMS().GetOllA(mSINR.rows()-1, iRankIndex));
//            }
            vSBSINR.push_back(mSINR);
        }
    }
    /*重计算完毕*/
    imat mBFMCS = lli.SINR2MCS(vSBSINR);
    return mBFMCS;
}
//@threads
void CompetorRankA::ComputeCompeteData(
        int _iMIMOMode,
        const SBID& _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS,
        CodeBook* _pCodeBook) {

    switch (_iMIMOMode) {
        case Parameters::IMIMOMode_MUBFRankA:
            ComputeCompeteDataEigenBasedBF(_sbid, _mMSID2MSTxBufferBTS);
            break;
        default:
            assert(false);
            cout << "This function can only support multi-user beamforming case!" << endl;
            exit(0);
            break;
    }
}

void CompetorRankA::SetRank(const MSID& _msid, const int& _iRank) {
    m_mMSID2Rank[_msid.ToInt()] = _iRank;
}

int CompetorRankA::GetRank(const MSID& _msid) {
    return m_mMSID2Rank[_msid.ToInt()];
}

imat CompetorRankA::GetMCS(const MSID& _msid, const SBID& _sbid) {
    return m_mMSID2MCS[_msid.ToInt()];
}

double CompetorRankA::GetPerformanceMetric(bool _bCompareWithPriority) const {
    if (_bCompareWithPriority)
        return m_Priority;
    else
        return m_Capacity;
}

double CompetorRankA::GetRENum_SingleSB() {
    int iMUNumber = static_cast<int> (size());
    int iLayerNum = 0;
    for (int i = 0; i < iMUNumber; ++i) {
        MSID& msid = (*this)[i];
        iLayerNum += (m_mMSID2Rank[msid.ToInt()] + 1);
    }
    return LinkLevelInterface::Instance_DL().GetRENum(Parameters::Instance().BASIC.ISBSize, iLayerNum);
    //    //todo PDCCH
    //    int iSymbolNumPerSlot = 14;
    //    int iRENumPerPRB_DMRS = (iLayerNum + 1) / 2 * 4;
    //
    //    int iRENum_temp = Parameters::Instance().BASIC.IRBSize * iSymbolNumPerSlot - iRENumPerPRB_DMRS;
    //    int iRENum_SingleSB = min(iRENum_temp, 156) * Parameters::Instance().BASIC.ISBSize;
    //    return iRENum_SingleSB;
}

double CompetorRankA::GetTBSizeKbit(int _iMCS,int v) {
    return LinkLevelInterface::Instance_DL().MCS2TBSKBit2(_iMCS, (int)GetRENum_SingleSB(),v);
}

void CompetorRankA::CalcPreCodeWord(const SBID& _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS){
    MSID msid = (*this)[0];
    MSTxBufferBTS& buffer = _mMSID2MSTxBufferBTS[msid.ToInt()];
    std::shared_ptr<CQIMessage> pCQIMes = buffer.GetCQIMessage();

    int iRank = m_mMSID2Rank[msid.ToInt()];

    if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1) {
        int iPMI = pCQIMes->GetSubBandPMI(_sbid);

        m_CodeWord = m_pCodeBook->GetCodeWord(iPMI, iRank);
    } else {
        itpp::cmat mTemp;
        if (Parameters::Instance().MIMO_CTRL.IHorCovR == 1) {
            mTemp = buffer.GetCovR(_sbid);
        } else {
            mTemp = buffer.GetH(_sbid);
        }

        vec vS;
        cmat mU, mD;

        svd(mTemp, mU, vS, mD);
        cmat mV = mD.get_cols(0, iRank);
        m_CodeWord = mV;
    }
}
