///@file BTSMRCRxer.h
///@brief  MRC接收机的实现
///@author zhengyi

#include "../Utility/IDType.h"
#include "../Utility/RBID.h"
#include "../Utility/SBID.h"
#include "../Utility/SCID.h"
#include "../Utility/functions.h"
#include "../BaseStation/BTSID.h"
#include "../NetworkDrive/Clock.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "ACKNAKMessageUL.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/BTS.h"
#include "../Statistician/Statistician.h"
#include "../DetectAlgorithm/Detector_UL.h"
#include "../MobileStation/MS.h"
#include "../BaseStation/BS.h"
#include "../NetworkDrive/MSManager.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../MobileStation/CQIMessage.h"
#include "../ChannelModel/LinkMatrix.h"
#include "BTSRxer.h"
#include "BTSVMIMORxer_trial_2.h"
#include "../LinklevelInterface/LinkLevelNR.h"
#include "CodeBookFactory.h"
#include "../Statistician/Statistics.h"
//chty 1110 b
#include <thread>
#include "../ChannelModel/LinkMatrix.h"
//chty 1110 e
extern int G_ICurDrop;
boost::mutex BTSVMIMORxer_trial_2_mutex;
//std::mutex CalcSINR_EBBCQI_AverageIlock;
//int BTSVMIMORxer_trial_2::threadnum = 10;//std::max(1.0,(double)cm::LinkMatrix::threadnum/(Parameters::Instance().BASIC.INumBSs*Parameters::Instance().BASIC.IBTSPerBS));

BTSVMIMORxer_trial_2::BTSVMIMORxer_trial_2(const BTSID& _BTSID)
: BTSRxer(_BTSID),
  m_mSCPMI(Parameters::Instance().BASIC.ISCNum),
  m_mSCPrecodeMat(Parameters::Instance().BASIC.ISCNum),
  m_mMSID2Rank(Parameters::Instance().BASIC.ITotalMSNum),
  MSID2HistoryTPC(Parameters::Instance().BASIC.DNumMSPerBTS)
{
    m_iMaxRank = min(Parameters::Instance().SIM_UL.UL.iMaxRBFindNum, Parameters::Instance().MSS.FirstBand.Total_TXRU_Num) - 1; //最大支持一流      2020.04修改单用户多流
    //m_iMaxRank = 1;
    m_iBestRank = m_iMaxRank;

}

//VMIMO combine the signal, precode and channel H
void BTSVMIMORxer_trial_2::CombineMultiMS(const MSID& _msid, const vector<MSID>& _vMSID, const SCID& _scid,
        cmat& _mS,
        cmat& _mH,
        cmat & _mP) {

    //    cmat mEffectiveP = _mP;
//    MS& ms = _msid.GetMS();
//    BTS& mainbts = ms.GetMainServBTS().GetBTS();
//    if (_vMSID.size() != 1) { //只要资源上的用户数不为1
//        //
//        //_mP.set_size(_vMSID.size() * P.MSS.UL.IAntennaNum, 1, true); //确定其他位置用0补 ？
//        _mEffectiveP.set_size(_vMSID.size(), _vMSID.size(), true); //check是否第一个数就是P
//
//        int icount = 0;
//        //这里合并新的功率，信道和加权值，同时还要知道那个是我们关心的值
//        for (int i = 0; i <static_cast<int> (_vMSID.size()); ++i) {
//            if (_msid == _vMSID[i])
//                continue;
//            MS& tmpMS = _vMSID[i].GetMS();
//            //组合信号功率
//            cmat mTmpS = tmpMS.UL.GetTxDataSymbol(_scid);
//            _mS = concat_vertical(_mS, mTmpS);
//            //组合信道矩阵
//            cmat mTmpH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(mainbts, tmpMS, _scid.ToInt()).H();
//            _mH = concat_horizontal(_mH, mTmpH);
//
//            //组合加权矩阵
//            cmat mTmpP = 1 / sqrt(Parameters::Instance().MSS.FirstBand.Total_TXRU_Num) * itpp::ones_c(Parameters::Instance().MSS.FirstBand.Total_TXRU_Num, 1);
//            ++icount;
//            _mEffectiveP.set(icount, icount, mTmpP(0, 0));
//        }
//    }
    MS& ms = _msid.GetMS();
    BTS& mainbts = ms.GetMainServBTS().GetBTS();

    vector<int> countS;
    vector<int> countH;
    countH.push_back(_mP.rows());
    countS.push_back(_mP.cols());


//    if(_vMSID.size() == 3 && static_cast<int>(_mP.rows()) == 2 && static_cast<int>(_mP.cols()) == 1 ){
//        cout<<"this maybe the wrong one!"<<endl;
//    }


    if (_vMSID.size() != 1) { //只要资源上的用户数不为1        
        // 这是P矩阵的行数
        int ITotalTXRUNum = Parameters::Instance().MSS.FirstBand.Total_TXRU_Num * _vMSID.size();
        int mPcols = 0;

        // 这是P矩阵的列数
        for(auto& msid :_vMSID){
            MS& tmpMS = msid.GetMS();
//            mPcols += tmpMS.ms_SCPrecodeMat[_scid.ToInt()].cols();
            mPcols += (m_mMSID2Rank[msid.ToInt()] + 1);
        }

        _mP.set_size(ITotalTXRUNum, mPcols, true);



        //这里合并新的功率，信道和加权值，同时还要知道那个是我们关心的值
        for (auto& msid : _vMSID) {
            if (_msid == msid) {
                continue;
            }

            MS& tmpMS = msid.GetMS();

            //组合信号功率
            //原先是cmat mTmpS = tmpMS.GetTxDataSymbol(_scid);
            cmat mTmpS = tmpMS.UL.GetTxDataSymbol(_scid, m_mMSID2Rank[msid.ToInt()]+1);

            countS.push_back(mTmpS.rows());

            _mS = concat_vertical(_mS, mTmpS);
            //组合信道矩阵
            cmat mTmpH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(mainbts, tmpMS, _scid.ToInt()).H();
            _mH = concat_horizontal(_mH, mTmpH);

            countH.push_back(mTmpH.cols());

            cmat mTmpP;
            if(Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1){
                mTmpP = tmpMS.ms_SCPrecodeMat[_scid.ToInt()];
            }
            else{
                cmat mU, mD;
                vec vS;
                svd(mTmpH, mU, vS, mD);
                mTmpP = mD.get_cols(0, m_mMSID2Rank[msid.ToInt()]);
            }

//            cout<<"this is "<< i <<" mTmpP!"<<endl;
//            for(int i = 0 ; i < mTmpP.rows(); i++){
//                for(int j = 0 ; j < mTmpP.cols(); j++){                 
//                    cout<<mTmpP(i,j)<<"     ";
//                }
//                cout<<endl;      
//            }

            int rows = 0;
            int cols = 0;
            for(int j = 0 ; j < static_cast<int>(countS.size() - 1); j++){
                rows += countH[j];
                cols += countS[j];
            }

            _mP.set_submatrix(rows, cols, mTmpP);

        }

    }

}

void vEVMbackoff(vector<double>& _vsinr) {
    assert(!_vsinr.empty());
    for (auto& dsinr : _vsinr) {
        dsinr = (dsinr * DB2L(Parameters::Instance().SIM_UL.UL.DEVMdb)) / (dsinr + DB2L(Parameters::Instance().SIM_UL.UL.DEVMdb));
    }
}
vector< mat > vEVMbackoffmat(vector<mat> _vsinr){
    assert(!_vsinr.empty());

    for (auto& dsinr : _vsinr) {
        for(int j = 0; j <dsinr.rows(); j++){
            double dsinrOld = dsinr(j,0);
           // cout<<"第 i= "<<i<<"第 j= "<<j<<"个 dsinrOld= "<<dsinrOld<<endl;
            double dsinrNew = (dsinr(j,0) * cm::DB2L(Parameters::Instance().SIM_UL.UL.DEVMdb)) / (dsinr(j,0) + cm::DB2L(Parameters::Instance().SIM_UL.UL.DEVMdb));
           // cout<<"第 i= "<<i<<"第 j= "<<j<<"个 dsinrNew= "<<dsinrNew<<endl;
            //dsinr(j,0) = (dsinr(j,0) * cm::DB2L(Parameters::Instance().SIM_UL.UL.DEVMdb)) / (dsinr(j,0) + cm::DB2L(Parameters::Instance().SIM_UL.UL.DEVMdb));
            dsinr(j,0) = dsinrNew;
        }
    }
    return _vsinr;
}

void BTSVMIMORxer_trial_2::ComputeEstimateSINR(const MSID& _MSID) {
    //计算SRS的SINR
    BTS& MainServBTS = m_BTSID.GetBTS();
    MS& ms = _MSID.GetMS();
    int iSpace = 4;
    iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);
//    vector<double> vSRSIoT(Parameters::Instance().SIM_UL.UL.ISCNum, 0); //用于SRS IOT的平均

    double dTimeSec = Clock::Instance().GetTimeSec();
//    cm::LinkMatrix::Instance().WorkSlot(ms, dTimeSec);

    //chty 1110 b
   // testlock.lock();
    auto& vmSINR4BestRank = m_MSID2mSINR4BestRank[_MSID.ToInt()];
    auto& RxBufferBTS = (*m_pRxBufferBTS)[_MSID.ToInt()];
    //testlock.unlock();
    //chty 1110 e

    for (SCID scid = SCID(ms.UL.GetSRSTxCyclicSCID()); scid <= SCID::End(); scid += iSpace) {
        //int iRank = ms.ms_SCPrecodeMat[scid.ToInt()].cols();
        int iRank = m_mMSID2Rank[_MSID.ToInt()];
        //chty  1110 b
        /*
//        cmat mS = ms.UL.GetTxSRS(scid,iRank+1); //srs
//        //std::cout<<m_BTSID.ToInt()<<' '<<MSID.ToInt()<<endl;//tsm 12-24
//        cmat mH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(m_BTSID.GetBTS(), ms, scid.ToInt()).H();
//        cmat mP;
//        if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1)
//        {
//            mP = ms.ms_SCPrecodeMat[scid.ToInt()];
//        //cout<<mP<<endl;
//        //  cout<<ms.GetID().ToInt()<<"  "<<scid<<"  :  "<<mP<<endl;
//
//        }else{
//            cmat mU, mD;
//            vec vS;
//            svd(mH, mU, vS, mD);
//            mP = mD.get_cols(0,iRank);
//        }
//        ///计算干扰
//        vector<cmat> vSIS, vSIH, vSIP, vWIS;
//        vector<double> vWIL;
//        for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
//            MS& msI = msid.GetMS();
//            if (find(m_pActiveSet->begin(), m_pActiveSet->end(), msI.GetID()) != m_pActiveSet->end()) {
//                continue; //排除本小区的用户
//            }
//            //critical
//            bool flag = cm::LinkMatrix::Instance().IsStrong(MainServBTS, msI);
////            bool flag = false;
//            if (flag) {
//                // 获取强干扰信号
//                cmat mSIS = msI.UL.GetTxSRS(scid);
//                vSIS.push_back(mSIS);
//                // 获取强干扰信道
//                cmat mSIH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(MainServBTS, msI, scid.ToInt()).H();
//                vSIH.push_back(mSIH);
//                // 获取强干扰信号的预编码
//                cmat mSIP = 1 / sqrt(Parameters::Instance().MSS.FirstBand.Total_TXRU_Num) * itpp::ones_c(Parameters::Instance().MSS.FirstBand.Total_TXRU_Num, 1);
//                vSIP.push_back(mSIP);
//            } else {
//                // 获取弱干扰的信号
//                cmat mWIS = msI.UL.GetTxSRS(scid);
//                vWIS.push_back(mWIS);
//                // 获取弱干扰的信道
//                double dLinkLoss = DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(MainServBTS, msI));
//                vWIL.push_back(dLinkLoss);
//            }
//        }
//        mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);
        //zhengyi,补充计算考虑解调非理想性的干扰
        //Channel Estimation Tag
         */
        //chty 1110 b
        //由于上方修改，这里暂时无法使用；考虑到平台本身一直也不开启BIsDMRSErrorOn，且CalculateSINR_DmrsError目前实现较简单，可能还需要修改，暂时直接注释掉
/*        if (Parameters::Instance().SIM_UL.UL.ERROR.BIsDMRSErrorOn) {
//            mat HerrorScale_test;
//            cmat Herror_test;
//            //            GetH4SRSError(mS, mH, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL, HerrorScale_test, Herror_test);
//            //            mat mSINR_another = m_pDetector->CalculateSINR_DmrsError(mS, mH, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL
//            //                    , HerrorScale_test, Herror_test);
//            //替换mEffectiveP，为mP
//            GetH4SRSError(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL, HerrorScale_test, Herror_test);
//            mat mSINR_another = m_pDetector->CalculateSINR_DmrsError(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL
//                    , HerrorScale_test, Herror_test);
//            //            cout << "sinr,sinr_e = " << mSINR(0, 0) << "," << mSINR_another(0, 0) << endl;
//            mSINR = mSINR_another;
//        }
 */
        mat mSINR= vmSINR4BestRank[scid.ToInt()/iSpace];
//chty 1110 e

        int differentMehtod = 1;
        switch (differentMehtod) {
            case 1:
                //方法1：老版本,仅仅利用srs的sinr估计SINR然后再用OLLA去调整
                // ((*m_pRxBufferBTS)[_MSID].m_vSRSSINR)[scid.ToInt()] = mSINR(0, 0);
               // testlock.lock();
                (RxBufferBTS.m_vSRSSINR)[scid.ToInt()/iSpace] = mSINR;
              //  testlock.unlock();
                break;
            //chty 1110 b
                //由于上方修改，这里暂时也无法使用暂时直接注释掉即使开启，和BIsDMRSErrorOn一样，也有大量重复计算
//            case 3:
//                mat mInf = CalculateInf(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);
//                //方法2： 将干扰替换为业务信道的平均干扰直接采用干扰的线性值
//                if (((*m_pRxBufferBTS)[_MSID.ToInt()].m_vInfEst)[scid.ToInt()] == -1) {
//                    ((*m_pRxBufferBTS)[_MSID.ToInt()].m_vSRSSINR)[scid.ToInt()] = mSINR(0, 0);
//                } else {
//                    ((*m_pRxBufferBTS)[_MSID.ToInt()].m_vSRSSINR)[scid.ToInt()] = mSINR(0, 0)
//                            * mInf(0, 0) / ((*m_pRxBufferBTS)[_MSID.ToInt()].m_vInfEst)[scid.ToInt()];
//                }
//                break;
        //chty 1110 e
            default:
                assert(false);
                break;
        }
    }


//    auto begin = RxBufferBTS.m_vSRSSINR.begin();
//    auto end = RxBufferBTS.m_vSRSSINR.end();
//    InterpolateLog(begin, end, iSpace, ms.UL.GetSRSTxCyclicSCID()); //根据用户SRS占用的SC进行偏移

    //对SRS 上的IoT插值，并进行长时平均,单独计算只与干扰估计（SINR估计）有关
//    auto beginIoT = vSRSIoT.begin();
//    auto endIoT = vSRSIoT.end();
//    InterpolateLog(beginIoT, endIoT, iSpace, ms.UL.GetSRSTxCyclicSCID());
    //iot 长时平均
//    for (SCID scid = SCID::Begin(); scid <= SCID::End(); ++scid) {
//        if (RxBufferBTS.m_vSRSIoT[scid.ToInt()] < 0) {
//            RxBufferBTS.m_vSRSIoT[scid.ToInt()] = vSRSIoT[scid.ToInt()];
//        }
//        RxBufferBTS.m_vSRSIoT[scid.ToInt()] *= 0.8;
//        RxBufferBTS.m_vSRSIoT[scid.ToInt()] += 0.2 * vSRSIoT[scid.ToInt()];
//    }
}
std::vector<mat> BTSVMIMORxer_trial_2::ComputeEstimateSINR(const MSID& _MSID, const RBID& FirstRBID, const RBID& LastRBID) {
    //计算SRS的SINR
    vector<mat> vSINR((LastRBID.ToInt() - FirstRBID.ToInt() + 1) * Parameters::Instance().BASIC.IRBSize);
    BTS& MainServBTS = m_BTSID.GetBTS();
    MS& ms = _MSID.GetMS();
    int iSpace = 4;
    iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);
//    if(Parameters::Instance().SIM_UL.UL.IMSnumOfMUMIMO>1){
//        iSpace = 1;
//    }
//    vector<double> vSRSIoT(Parameters::Instance().SIM_UL.UL.ISCNum, 0); //用于SRS IOT的平均

    double dTimeSec = Clock::Instance().GetTimeSec();
//    cm::LinkMatrix::Instance().WorkSlot(ms, dTimeSec);

    SCID firstSCID = FirstRBID.GetFirstSCID();
    SCID lastSCID = LastRBID.GetLastSCID();

    for (SCID scid = firstSCID; scid <= lastSCID; scid += iSpace) {
//        int iRank = ms.ms_SCPrecodeMat[scid.ToInt()].cols();
        int iRank = m_mMSID2Rank[_MSID.ToInt()];
        cmat mS = ms.UL.GetTxSRS(SCID(ms.UL.GetSRSTxCyclicSCID()),iRank+1); //srs
        //std::cout<<m_BTSID.ToInt()<<' '<<MSID.ToInt()<<endl;//tsm 12-24
        cmat mH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(m_BTSID.GetBTS(), ms, scid.ToInt()).H();
        cmat mP;
        if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1)
        {
        mP = ms.ms_SCPrecodeMat[scid.ToInt()];
        //cout<<mP<<endl;
        //  cout<<ms.GetID().ToInt()<<"  "<<scid<<"  :  "<<mP<<endl;

        }else{
        cmat mU, mD;
        vec vS;
        svd(mH, mU, vS, mD);
        mP = mD.get_cols(0, iRank);
        }
        ///计算干扰
        vector<cmat> vSIS, vSIH, vSIP, vWIS;
        vector<double> vWIL;
        for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
            MS& msI = msid.GetMS();
            if (find(m_pActiveSet->begin(), m_pActiveSet->end(), msI.GetID()) != m_pActiveSet->end()) {
                continue; //排除本小区的用户
            }

            if (cm::LinkMatrix::Instance().IsStrong(MainServBTS, msI)) {
                // 获取强干扰信号
                cmat mSIS = msI.UL.GetTxSRS(scid);
                vSIS.push_back(mSIS);
                // 获取强干扰信道
                cmat mSIH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(MainServBTS, msI, scid.ToInt()).H();
                vSIH.push_back(mSIH);
                // 获取强干扰信号的预编码
                cmat mSIP = 1 / sqrt(Parameters::Instance().MSS.FirstBand.Total_TXRU_Num) * itpp::ones_c(Parameters::Instance().MSS.FirstBand.Total_TXRU_Num, 1);
                vSIP.push_back(mSIP);
            } else {
                // 获取弱干扰的信号
                cmat mWIS = msI.UL.GetTxSRS(scid);
                vWIS.push_back(mWIS);
                // 获取弱干扰的信道
                double dLinkLoss = DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(MainServBTS, msI));
                vWIL.push_back(dLinkLoss);
            }
        }
        mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);
        vSINR[scid.ToInt() - firstSCID.ToInt()] = mSINR;
        //zhengyi,补充计算考虑解调非理想性的干扰

    }
    auto begin_1 = vSINR.begin();
    auto end_1 = vSINR.end();
    InterpolateLog(begin_1, end_1, iSpace, 0);

    return vSINR;
}

void BTSVMIMORxer_trial_2::ComputeReceiveSINR(const std::shared_ptr<SchedulingMessageUL>& _pSchM) {
    if (!_pSchM)
        return;

    BTS& MainServBTS = m_BTSID.GetBTS();
    MSID msid = _pSchM->GetMSID();
    std::unordered_map<int, vector<MSID> >& mRB2vMSID = _pSchM->GetRB2vMSID();
    MS& ms = msid.GetMS();
    int iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);


    double dTimeSec = Clock::Instance().GetTimeSec();


    vector<RBID> vRBUsed = _pSchM->GetRBUsed();
    sort(vRBUsed.begin(), vRBUsed.end());
    vector<SCID> SCIDUsed;
    for (auto& rbid : vRBUsed) {
        for (SCID scid = rbid.GetFirstSCID(); scid <= rbid.GetLastSCID(); ++scid) {
            SCIDUsed.push_back(scid);
        }
    }

    vector<mat> vSINR(vRBUsed.size() * Parameters::Instance().BASIC.IRBSize / iSpace);
    vector<double> vIoT(vRBUsed.size() * Parameters::Instance().BASIC.IRBSize / iSpace, 0);
    vector<double> vInfAfterDetec(vRBUsed.size() * Parameters::Instance().BASIC.IRBSize / iSpace, 0);
    vector<double> vIOT_forAverage(vRBUsed.size() * Parameters::Instance().BASIC.IRBSize / iSpace, 0);
    vector<cmat> vRe(vRBUsed.size() * Parameters::Instance().BASIC.IRBSize / iSpace, itpp::zeros_c(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num, Parameters::Instance().MIMO_CTRL.Total_TXRU_Num));

    //for (SCID scid = scidBegin; scid <= scidEnd; scid += iSpace) { //这里应该是利用调度信息
    for (int SCIDIndex = 0; SCIDIndex < SCIDUsed.size(); SCIDIndex += iSpace) {
        SCID scid = SCIDUsed[SCIDIndex];
        RBID rbid = scid.GetRBID();
        vector<MSID > vMSID = mRB2vMSID[rbid.ToInt()];
        if (find(vMSID.begin(), vMSID.end(), msid) == vMSID.end())
            continue;
        int iChannelSize = static_cast<int> (vMSID.size());

        //这里将主用户放在第一个，这样检查P值时，也会优先检测到主用户，待check
        cmat mS, mH, mP, mEffectiveP;

        //        mS.set_size(vMSID.size(),P.MSS.UL.IAntennaNum);
        //        mH.set_size(P.MSS.UL.IAntennaNum,vMSID.size());
        //
        //        mS.set_rows(0,ms.GetTxDataSymbol(scid));
        //        mH.set_cols(0,cm::LinkMatrix::Instance().GetFadingMat(m_BTSID.GetBTS(), ms, scid.ToInt()).H());

        int iRank = m_mMSID2Rank[msid.ToInt()];
        mS = ms.UL.GetTxDataSymbol(scid,iRank+1);

        mH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(m_BTSID.GetBTS(), ms, scid.ToInt()).H();

        if(Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1){
            mP = ms.ms_SCPrecodeMat[scid.ToInt()];
        }
        else{
            //cwq dahuawu
            cmat mU, mD;
            vec vS;
            svd(mH, mU, vS, mD);
            mP = mD.get_cols(0,iRank);
        }

        mEffectiveP = mP;

        //test
        cmat test_mS(mS);
        cmat test_mH(mH);
        cmat test_mP(mP);
        //        CombineMultiMS(msid, vMSID, scid, test_mS, test_mH, test_mP);
        CombineMultiMS(msid, vMSID, scid, mS, mH, mP);

        ///计算干扰
        vector<cmat> vSIS, vSIH, vSIP, vWIS;
        vector<double> vWIL;
        for (int i = 0; i < MSManager::Instance().CountMS(); ++i) {
            MS& msI = MSManager::Instance().GetMS(i);

                if (find(vMSID.begin(), vMSID.end(), msI.GetID()) != vMSID.end()) {
                    continue;
                }

            int iRankI = msI.iBestRank_UL;
            if (cm::LinkMatrix::Instance().IsStrong(MainServBTS, msI)) {
                // 获取强干扰信号
                cmat mSIS = msI.UL.GetTxDataSymbol(scid,iRankI+1);

                //different load
                double drnd = random.xUniform_DiffLoad(0, 1);
                if (drnd >= Parameters::Instance().SIM_UL.UL.dLoad) {
                    mSIS = itpp::zeros_c(mSIS.rows(), mSIS.cols());
                }
                // different load end

                //assert(mSIS.rows() == 1);
                vSIS.push_back(mSIS);
                // 获取强干扰信道
                //                cmat mSIH;
                //                mSIH.set_size(mH.rows(),mH.cols());
                //                mSIH.set_cols(0,cm::LinkMatrix::Instance().GetFadingMat(MainServBTS, msI, scid.ToInt()).H());
                //
                cmat mSIH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(MainServBTS, msI, scid.ToInt()).H();
                //改变维数是为了配合MUSINR的校验，对于实际结果应该是没有影响的
                //mSIH.set_size(mSIH.rows(), iChannelSize * Parameters::Instance().MSS.FirstBand.Total_TXRU_Num, true);
                //                if(mSIH.rows()!=1)
                //                {
                //                    cmat mSIHtmp = cm::LinkMatrix::Instance().GetFadingMat(MainServBTS, msI, scid.ToInt()).H();
                //                    mSIHtmp.set_size(mSIHtmp.rows(), iChannelSize * P.MSS.UL.IAntennaNum, true);
                //                    assert(mSIH.rows()==1);
                //                }

                vSIH.push_back(mSIH);
                // 获取强干扰信号的预编码
                cmat mSIP;
                if(Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1){
                    mSIP = msI.ms_SCPrecodeMat[scid.ToInt()];
                }
                else{
                    //cwq dahuawu
                    cmat mUI, mDI;
                    vec vSI;
                    svd(mSIH, mUI, vSI, mDI);
                    mSIP = mDI.get_cols(0,iRankI);
                }
                vSIP.push_back(mSIP);
            } else {
                // 获取弱干扰的信号
                cmat mWIS = msI.UL.GetTxDataSymbol(scid,iRankI+1);

                //zhengyi different load
                double drnd = random.xUniform_DiffLoad(0, 1);
                if (drnd >= Parameters::Instance().SIM_UL.UL.dLoad) {
                    mWIS = itpp::zeros_c(mWIS.rows(), mWIS.cols());
                }
                //zhengyi load end

                vWIS.push_back(mWIS);
                // 获取弱干扰的信道
                double dLinkLoss = DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(MainServBTS, msI));
                vWIL.push_back(dLinkLoss);
            }
        }

        //zhengyi 20150124-1206
        //单独计算DRMS的SINR，本小区复用的的用户都不算作干扰



        mat mSINR_another;
        //add dmrs ERROR
        if (Parameters::Instance().SIM_UL.UL.ERROR.BIsDMRSErrorOn) {
            if (Parameters::Instance().SIM_UL.UL.ERROR.IDmrsErrorOptions == 0) {
                //origanl DMRS error calculation
                mat HerrorStd;
                cmat Herror;
                assert(false);
                GetH4DMRSError(msid, mS, mH, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL, HerrorStd, Herror);
                mSINR_another = m_pDetector->CalculateSINR_UL(mS, mH, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL, HerrorStd, Herror);
            } else if (Parameters::Instance().SIM_UL.UL.ERROR.IDmrsErrorOptions == 1) {
                //new dmrs error
                //apply the SRS error model to this DMRS error
                mat HerrorScale_test;
                cmat Herror_test;
                //                assert(false);
                GetH4SRSError(mS, mH, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL, HerrorScale_test, Herror_test);
                mSINR_another = m_pDetector->CalculateSINR_DmrsError(mS, mH, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL
                        , HerrorScale_test, Herror_test);

            } else {
                assert(false);
            }

        } else {
            mSINR_another = m_pDetector->CalculateSINR(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);
            //Observer::Print("receive") << "SCID " << scid.ToInt() << "out" << endl;

        }
        vSINR[SCIDIndex/iSpace] = mSINR_another;


        //另一种计算IOT的方式
        itpp::mat mIoT = CalculateInf(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);

        double dIoT_forAverage = CalculateIoTave(mS, mH, mP, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL);

        //暂时先用全部的干扰
        //in use
        //chty 1110 b
        //itpp::mat mInfAfterDetec = CalculateInf(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);
        //chty 1110 e
        //用于长时平均干扰相关阵

        vIoT[SCIDIndex / iSpace] = mIoT(0, 0);
        //chty 1110 b
        vInfAfterDetec[SCIDIndex / iSpace] = mIoT(0, 0);
        //chty 1110 e
        vIOT_forAverage[SCIDIndex / iSpace] = dIoT_forAverage;
    }

    SINRInterpolate(msid, vSINR, iSpace, SCIDUsed); //这些还是有问题的

    //对IoT插值，并进行长时平均
    vIoTEstimateInterpolate(msid, vIoT, iSpace, SCIDUsed);

    //计算小区间干扰的长时平均
    vInfAfterDetecInterpolate(msid, vInfAfterDetec, iSpace, SCIDUsed);

    //临时保存每个载波上的IOT，待全部保存完毕后，平均然后打印
    vIOT_forAverageInterpolate(msid, vIOT_forAverage, iSpace, SCIDUsed);

    //计算业务信道的Re
    vReEstimateInterpolate(msid, vRe, iSpace, SCIDUsed);
}

void BTSVMIMORxer_trial_2::WorkSlot() {
    //chty 1110 b
    int iTime = Clock::Instance().GetTimeSlot();
    int ULFrameSlot = Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot -
                      Parameters::Instance().SIM.FrameStructure4Sim.P1.IULSlot + 1;
    int index = mod(ULFrameSlot, Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot);
    if (iTime % Parameters::Instance().MSS_UL.UL.ISRS_PERIOD == index) {
        for (const auto &msid: (*m_pActiveSet)) {
            ComputeRIandPMI(msid);
            ComputeEstimateSINR(msid);
        }
    }
//    // 每个上行时隙检测数据包
    ReceiveProcess();
    //chty 1110 e
}

void BTSVMIMORxer_trial_2::ReceiveProcess() {
    //可以建立一个队列，如果用户检测过了，就不需要进行检测

    //buffer reTx ms Scheduling msg for MU-mimo
//    BufferReTxACKmsg();
    //chty 1110 b
//    vector<std::thread> group;
//    for (const auto &msid: (*m_pActiveSet)) {
//        group.emplace_back(std::thread(&BTSVMIMORxer_trial_2::MSReceiveThread, this, msid.ToInt()));
//    }
//    for (auto &thread: group) {
//        thread.join();
//    }
    int iTime = Clock::Instance().GetTimeSlot();
    int iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);
    for (auto& msid : (*m_pActiveSet)) {
        MS& ms = msid.GetMS();

        //优先判断用户是否有重传，如果没有，则从调度消息队列中提取信息
        std::shared_ptr<SchedulingMessageUL> pSchM = GetSchedulingMsg(msid);

        if (!pSchM && Clock::Instance().GetTimeSlot() > 6) { //在时隙超过一个值之后，如果还有空消息，则应该报错
            if (Parameters::Instance().SIM_UL.UL.IMIMOMode != Parameters::IMIMOMode_VirtualMIMO)
                assert(false);
        }
        if (!pSchM)
            continue;
        assert(msid == pSchM->GetMSID());

        //////////////////////////////////for phr
        ReceivePHR(pSchM);
        //////////////////////////////////

        // 计算数据上的SINR
        ComputeReceiveSINR(pSchM);

        //zhengyi 计算上行geoemtry
        ComputeULGeometry(pSchM);

        vector<RBID> vRBUsed = pSchM->GetRBUsed();
        vector<MSID> vVmimoGroup = pSchM->GetVmimoGroup();

        imat iMCS = pSchM->GetMCSmat();
        int iSendNum = pSchM->GetSendNum();
        int iUsedRBNum = static_cast<int> (vRBUsed.size());
        int iHARQID = pSchM->GetHARQID();
        double iTBSize = pSchM->GetTBSizeKbit();
        std::vector<mat> vSINR;
        std::vector<double> vULGeometry;

        for (auto& rbid : vRBUsed) {
            for (SCID scid = rbid.GetFirstSCID(); scid <= rbid.GetLastSCID(); scid += iSpace)
            {
                assert((*m_pRxBufferBTS)[msid.ToInt()].m_vSINR[scid.ToInt() / iSpace].rows() > 0);
                vSINR.push_back((*m_pRxBufferBTS)[msid.ToInt()].m_vSINR[scid.ToInt() / iSpace]);
                //zhengyi 20170217
                vULGeometry.push_back((*m_pRxBufferBTS)[msid.ToInt()].m_vULGeometry[scid.ToInt() / iSpace]); //这里记录了每个子载波的SINR
            }
        }


        //EVM
        vSINR = vEVMbackoffmat(vSINR);

        //计算UL Per UE Average SINR
        double dSINR = 0;
        LinkLevelInterface &lli = LinkLevelInterface::Instance_UL();


        double dEstimateSINR = pSchM->EstimateSINR;
        dSINR = lli.SINRCombineUL2(vSINR);

        int TPC=-1;
        //20220828 cwq
        if(Parameters::Instance().MSS_UL.UL.CLPCofPUSCH){
            double pathloss = -(cm::LinkMatrix::Instance().GetCouplingLossDB(m_BTSID.GetBTS(), ms));
            if(Parameters::Instance().MSS_UL.UL.UserStrategy){
                TPC = GenerateTPCCommand_ByUser(L2DB(dSINR), pathloss);
            }
            else{
                TPC = GenerateTPCCommand(msid,L2DB(dSINR),pathloss);
            }
            TPCMessage TPCMes = {msid, Clock::Instance().GetTimeSlot(), TPC, Parameters::Instance().MSS_UL.UL.TPC_Accumulation};
//            pair<MSID,pair<int,int> > TPCMes = make_pair(msid,make_pair(Clock::Instance().GetTimeSlot(),TPC) );
            ms.GetMainServBTS().GetBTS().GetDci()->CollectTPC(TPCMes);
//            ms.GetMainServBTS().GetBTS().GetDci()->SendTPCMes();
        }

        //zhengyi 20170217
        double dUL_geometry = 0;
        for (int j = 0; j < static_cast<int> (vULGeometry.size()); ++j) {
            dUL_geometry += vULGeometry[j] / vULGeometry.size();
        }
        if(iTime >= Parameters::Instance().BASIC.IWarmUpSlot)
        {
            Statistician::Instance().m_MSData_UL[msid.ToInt()].m_dAVE_SINR += dSINR; //统计平均SINR
            Statistician::Instance().m_MSData_UL[msid.ToInt()].m_dAVE_SINR_Index += 1;
            Statistician::Instance().m_MSData_UL[msid.ToInt()].m_dul_geoemtry += dUL_geometry; //统计平均SINR//这个统计有什么意义？ 载波平均后，时隙平均
        }


        //HARQ合并SINR
        m_pHARQRxState_BTS_UL->CombineSINR(msid, iHARQID, vSINR);
        vSINR = m_pHARQRxState_BTS_UL->GetCombinedSINR(msid, iHARQID);
        int iRank = iMCS.rows();
        double dBLER = 0.0;
        for(int j = 0; j < iRank; j++){
            dBLER += LinkLevelInterface::Instance_UL().BLERmat(vSINR,iMCS(j,0),iTBSize);
        }
        dBLER /= iRank;
//        cout<<endl;
//        cout<<"MS="<<msid.ToInt()<<"  MCS="<<iMCS(0,0)<<"  TBSize="<<iTBSize<<"  dSINR="<<dSINR<<"  dBLER="<<dBLER<<endl;
        //bool bACKNAK = (xUniform() >= dBLER);
        //xUniform_Detection
        bool bACKNAK = (random.xUniform_Detection() >= dBLER);
        if (bACKNAK)
        {
            double dCorrectKbit = pSchM->GetTBSizeKbit();
            (*m_pRxBufferBTS)[msid.ToInt()].AccumulateSuccessRxKbit(dCorrectKbit);

            if(iTime >= Parameters::Instance().BASIC.IWarmUpSlot)
            if(iTime >= Parameters::Instance().BASIC.IWarmUpSlot)
            {
                Statistician::Instance().m_BTSData[m_BTSID.GetBTS().GetTxID()].UL.m_dThroughputKbps += dCorrectKbit;

                Statistician::Instance().m_MSData_UL[msid.ToInt()].m_iReTransTimePerMS[0] += 1;
                Statistician::Instance().m_MSData_UL[msid.ToInt()].m_iReTransTimePerMS[iSendNum] += 1;
                Statistician::Instance().m_MSData_UL[msid.ToInt()].UL.m_iCorrectBlockHit[iSendNum + 1] += 1;
                Statistician::Instance().m_MSData_UL[msid.ToInt()].UL.m_iCorrectBlockHit[0] += 1;
            }
        }
        else
        {
            if(iTime >= Parameters::Instance().BASIC.IWarmUpSlot)
            {
                Statistician::Instance().m_MSData_UL[msid.ToInt()].m_iReTransTimePerMS[0] += 1;
                Statistician::Instance().m_MSData_UL[msid.ToInt()].m_iReTransTimePerMS[iSendNum] += 1;
                Statistician::Instance().m_MSData_UL[msid.ToInt()].UL.m_iCorruptBlockHit[iSendNum + 1] += 1;
                Statistician::Instance().m_MSData_UL[msid.ToInt()].UL.m_iCorruptBlockHit[0] += 1;
            }
        }

        if(iSendNum == 0)
        {
            Statistician::Instance().m_BTSData[m_BTSID.GetBTS().GetTxID()].UL.m_iNewTxNum += 1;
            Statistician::Instance().m_BTSData[m_BTSID.GetBTS().GetTxID()].UL.m_iNewTxErrorNum += bACKNAK ? 0:1;
        }
        ///小区级指标新增
        int BTS2TxID = this->m_BTSID.GetBTS().GetTxID();
        for(int j = 0; j < iRank; j++){
            int i_Statistics_MCS = iMCS(j,0);
            ///小区上行平均MCS
            Statistics::Instance().m_BS_UL_MCS[BTS2TxID] += i_Statistics_MCS;
            Statistics::Instance().m_BS_UL_MCS_Times[BTS2TxID] += 1;
            ///平台采用MCS Index Table 1 PUSCH; MCS小于等于9时采用QPSK调制
            ///小区下行QPSK编码比例（下行QPSK调制的初始TB数和统计周期内传输的总下行初始TB数之比表示）
            Statistics::Instance().m_BS_UL_QPSK_Rate_Total[BTS2TxID] += 1;
            if(i_Statistics_MCS <= 9){
                Statistics::Instance().m_BS_UL_QPSK_Rate_Used[BTS2TxID] += 1;
            }
        }
        if (bACKNAK) {
            double dCorrectKbit = pSchM->GetTBSizeKbit();
            ///小区上行业务量
            Statistics::Instance().m_BS_UL_Traffic[BTS2TxID] += dCorrectKbit;
        } else {
            ///上行HARQ重传比例（UL-SCH上发生HARQ重传的TB数量与UL-SCH上传输的总的TB数量之比表示）
            Statistics::Instance().m_BS_UL_HARQ_Used[BTS2TxID] += 1;
        }
        Statistics::Instance().m_BS_UL_HARQ_Total[BTS2TxID] += 1;

        //chty 1031 end
        //        int iUsedRENum = LinkLevelInterface::Instance_UL().GetRENum(iUsedRBNum);
        //        dScheduleRate += LinkLevelInterface::Instance_UL().MCS2TBSKBit(iMCS, iUsedRENum);
        //        if (bACKNAK) {
        //            int iUsedRENum = LinkLevelInterface::Instance_UL().GetRENum(iUsedRBNum);
        //            dRxRate += LinkLevelInterface::Instance_UL().MCS2TBSKBit(iMCS, iUsedRENum);
        //        }
        //        if (bACKNAK || (iSendNum >= Parameters::Instance().SIM_UL.UL.IHARQMaxTransNum))
        //            m_pHARQRxState_BTS_UL->Refresh(msid, iHARQID);

        //OLLA
        if (Parameters::Instance().SIM_UL.UL.BisMUollaOn) {
            if (vVmimoGroup.size() == 1) {
                (*m_pRxBufferBTS)[msid.ToInt()].OllA(bACKNAK, iSendNum); //OLLA里设定的第一次传输是0
            } else if (vVmimoGroup.size() > 1) {
                (*m_pRxBufferBTS)[msid.ToInt()].OllA4MU(bACKNAK, iSendNum);
            } else {
                assert(false);
            }
        } else {
            (*m_pRxBufferBTS)[msid.ToInt()].OllA(bACKNAK, iSendNum); //OLLA里设定的第一次传输是0
        }

        std::shared_ptr<ACKNAKMessageUL> pACKM = std::make_shared<ACKNAKMessageUL > (pSchM);
        pACKM->SetACKNAK(bACKNAK);

        if (pSchM->ReSchMessageAdjust(bACKNAK)) {
            if (pACKM->GetSchMessage()->GetSendNum() < Parameters::Instance().SIM_UL.UL.IHARQMaxTransNum) {
				pACKM->GetSchMessage()->SetBornTime(Clock::Instance().GetTimeSlot());
                m_pHARQRxState_BTS_UL->RegistReTxMessage(pACKM); //在BTS端保存待重传用户的信息
            }
        }
        if (bACKNAK || (pACKM->GetSchMessage()->GetSendNum() >= Parameters::Instance().SIM_UL.UL.IHARQMaxTransNum)) {
            m_pHARQRxState_BTS_UL->Refresh(msid, iHARQID);
        }
        {
            //boost::mutex::scoped_lock lock(BTSVMIMORxer_trial_2_mutex);
            BTSVMIMORxer_trial_2_mutex.lock();
            int iTime = Clock::Instance().GetTimeSlot();
            Observer::SetIsEnable(true);
            int MCS1 = -1;
            if(iMCS.rows()>1){
                MCS1 = iMCS(1,0);
            }
            //hyl 冗余输出
//            Observer::Print("MSDecode_UL") << iTime
//                    << setw(20) << m_BTSID
//                    << setw(20) << msid
//                    << setw(20) << iHARQID
//                    << setw(20) << iTBSize
//                    << setw(20) << iSendNum
//                    << setw(20) << iMCS(0,0)
//                    << setw(20) << MCS1
//                    << setw(20) << L2DB(dSINR)
//                    << setw(20) << dBLER
//                    << setw(20) << bACKNAK;
//            Observer::Print("MSDecode_UL") << endl;

            //一个无线帧中有多少个时隙 10ms * slotperms
            //int iFrameNum = 10 / Parameters::Instance().BASIC.DSlotDuration_ms;
            int iFrameID = (iTime - 1) / Parameters::Instance().BASIC.iFrameNum + 1;
            Observer::Print("SchMes") << m_BTSID << setw(20) << G_ICurDrop << setw(20) << iFrameID << setw(20) << iTime << setw(20) << DownOrUpLink(iTime) << setw(20) << msid << setw(20)
                                      << iHARQID << setw(20) << iRank << setw(20) << iSendNum+1 << setw(20) << L2DB(dSINR) << setw(20) << L2DB(dEstimateSINR) << setw(20) << L2DB(dEstimateSINR) << setw(20) << iMCS(0, 0) << setw(20) << LinkLevelNR::ModulationOrder(iMCS(0, 0), Parameters::Instance().BASIC.IDLORUL) << setw(20)
                                      << dBLER << setw(20) << bACKNAK << setw(20) << iTBSize << setw(20) << iUsedRBNum << setw(20);

            vector<RBID> vRBUsed_temp = pSchM->GetRBUsed();
            sort(vRBUsed_temp.begin(), vRBUsed_temp.end());
            //vRBUsed_temp.sort();
            for (const auto& rbid : vRBUsed_temp) {
                Observer::Print("SchMes") << rbid << "\\";
            }
            Observer::Print("SchMes") << setw(20) << TPC << endl;

            //刨除热启动时隙的调度信息
            int iTime_WarmUpslot = iTime - Parameters::Instance().BASIC.IWarmUpSlot;
            if(iTime_WarmUpslot > 0){
                int iFrameID_WarmUpslot = iTime_WarmUpslot / Parameters::Instance().BASIC.iFrameNum + 1;
                //Hyl 冗余
//                Observer::Print("SchMes_WarmUpslot") << m_BTSID << setw(20) << iFrameID_WarmUpslot << setw(20) << iTime_WarmUpslot << setw(20) << DownOrUpLink(iTime) << setw(20) << msid << setw(20)
//                                                     << iHARQID << setw(20) << iRank << setw(20) << L2DB(dSINR) << setw(20) << iMCS(0, 0) << setw(20) << LinkLevelNR::ModulationOrder(iMCS(0, 0), Parameters::Instance().BASIC.IDLORUL) << setw(20)
//                                                     << dBLER << setw(20) << bACKNAK << setw(20) << iTBSize << setw(20) << iUsedRBNum << setw(20);

                vector<RBID> vRBUsed_temp = pSchM->GetRBUsed();
                sort(vRBUsed_temp.begin(), vRBUsed_temp.end());
                //vRBUsed_temp.sort();

                //Hyl 冗余
//                for (auto& rbid :vRBUsed_temp) {
//                    Observer::Print("SchMes_WarmUpslot") << rbid << "\\";
//                }
//                Observer::Print("SchMes_WarmUpslot") << setw(20) << TPC << endl;

            }
            BTSVMIMORxer_trial_2_mutex.unlock();
        }
//        ms.UL.ReceiveACK(pACKM);
        ms.GetMainServBTS().GetBTS().GetDci()->CollectACKUL(pACKM);
    }
    //chty 1110 e
    //clear buffer of retx
    ResetACKMsgBuffer();

    //计算IOT before detection
    ComputeIoTPreDetection();
}

itpp::mat BTSVMIMORxer_trial_2::CalculateIoTafterDet(std::shared_ptr<Detector_UL>& _pDetector,
        const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {

    int iSizeI = static_cast<int> (_vSIS.size()); //强干扰的个数
    int iSizeW = static_cast<int> (_vWIS.size()); //弱干扰的个数
    int iBSAntNum = _mH.cols(); //BS天线数
    int iMSAntNum = _mH.rows(); //MS天线数
    int iRank = _mP.cols(); //流数

    //    输入参数校验
    assert(iRank == 1);
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

    cmat mDH = _pDetector->CalculateDetecMatrix(_mS, _mH, _mP, _vSIS, _vSIH, _vSIP, _vWIS, _vWIL);
    itpp::mat I = itpp::zeros(1, 1); //干扰
    for (int i = 0; i < iSizeI; ++i) { //处理强干扰
        itpp::cmat mIDH = _vSIH[i] * _vSIP[i]; //干扰加权后的等效信道
        itpp::cmat mIRS = mIDH * _vSIS[i]; //干扰的接收符号
        itpp::cmat mIDS = mDH * mIRS; //干扰的检测符号
        I += real(itpp::elem_mult(conj(mIDS), mIDS)); //累加干扰
    }

    double dNoisePowerMw = Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw; //处理热噪声
    for (int i = 0; i < iSizeW; ++i) { //处理弱干扰
        double dWeakI = real((_vWIS[i].H() * _vWIS[i])(0, 0)) * _vWIL[i];
        dNoisePowerMw += dWeakI;
    }
    itpp::mat mIoT = itpp::elem_div(dNoisePowerMw * real(mDH * mDH.H()) + I, Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw * real(mDH * mDH.H()));
    return mIoT;
}


//计算IOT

double BTSVMIMORxer_trial_2::CalculateIoTave(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const cmat& _mPA,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) { //修改输入参数

    int iNs = static_cast<int> (_mS.size());
    int iNr = _mH.rows();
    ///计算干扰相关阵
    double dNoisePSDMw = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);

    //IOT只考虑小区间干扰
    ///计算小区间干扰分量
    cmat mInterI(iNr, iNr); //Ns ? 还是Nr？
    mInterI.zeros();
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        mInterI += (_vSIH[i] * _vSIP[i] * _vSIS[i])*(_vSIH[i] * _vSIP[i] * _vSIS[i]).H();
    }

    //这里是不是应该把两个分开？因为上下两维 为两个跟天线的
    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        cmat mTemp = _vWIL[i] * _vWIS[i].H() * _vWIS[i];
        mInterI += abs(mTemp(0, 0)) * eye_c(iNr);
    }
    vector<double> vInterI;
    for (int i = 0; i < iNr; ++i) {
        double dInterI = abs(mInterI(i, i));
        vInterI.push_back(dInterI);
    }
    ///计算噪声分量
    double dNoiseMw = dNoisePSDMw;
    double dSumIOT = 0;
    for (int i = 0; i < iNr; ++i) { //iNs 是不是应该等于 1 阿？
        double dIOT = (vInterI[i] + dNoiseMw) / dNoiseMw; //实际这两个噪声应该是一样的
        dSumIOT += dIOT;
    }
    double dAveIOT = dSumIOT / iNr;
    return dAveIOT;
}

itpp::mat BTSVMIMORxer_trial_2::CalculateIoTafterDet(const cmat& _mS,
        const cmat& _mH, const cmat& _mP,
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
    //assert(iRank == 1);
    assert(_mS.rows() == iRankA);
    assert(_mP.rows() == iBSAntNum);
    assert(_mPA.rows() == iBSAntNum);
    assert(_vSIS.size() == _vSIH.size() && _vSIH.size() == _vSIP.size());
    assert(_vWIS.size() == _vWIL.size());
    for (int i = 0; i < iSizeI; ++i) {
        assert(_vSIS[i].rows() == _vSIP[i].cols());
        assert(_vSIP[i].rows() == iBSAntNum); //?? 这两个校验使得干扰加权的维数 需要改变
        assert(_vSIP[i].rows() == _vSIH[i].cols());
        if (_vSIH[i].rows() != iMSAntNum) {
            cout << _vSIH[i].rows() << endl;
            cout << iMSAntNum << endl;
            assert(_vSIH[i].rows() == iMSAntNum);
        }

        assert(_vSIH[i].cols() == iBSAntNum); //??这两个校验使得干扰加权的维数 需要改变
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

    //    cmat mDH = _pDetector->CalculateDetecMatrix(_mS, _mH, _mP, _vSIS, _vSIH, _vSIP, _vWIS, _vWIL);

    itpp::cmat symbol_de = itpp::elem_mult(W.T(), _mS); //计算均衡后的接收符号
    itpp::mat S = real(itpp::elem_mult(conj(symbol_de), symbol_de)); //计算信号的功率
    itpp::mat SS = S.get_rows(iRankID, iRankID); //得到本用户的信号功率

    // step3: 计算干扰功率
    itpp::mat I;
    I.zeros();
    double dCarrierNoiseMW = DSCNoisePowerMw;

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

    itpp::mat mIoT = itpp::elem_div(mNoise + I, DSCNoisePowerMw * real(diag(diag(G * G.H()))));

    return mIoT;
}

//该函数是用于计算双流预测的SINR

vector<vector<mat > > BTSVMIMORxer_trial_2::ComputeEstimateSINR(const MSID& _msid_1,
        const MSID& _msid_2,
        const RBID& _rbid_1,
        const RBID& _rbid_2) {
    //保存配对的用户，方便后面循环
    vector<MSID> vVmimoGroup;
    vVmimoGroup.push_back(_msid_1);
    vVmimoGroup.push_back(_msid_2);

    //计算SRS的SINR
    BTS& MainServBTS = m_BTSID.GetBTS();
    MS& ms_1 = _msid_1.GetMS();
    MS& ms_2 = _msid_2.GetMS();
    assert(ms_1.GetMainServBTS() == m_BTSID);
    assert(ms_2.GetMainServBTS() == m_BTSID);

    int iSpace = 4;
    //tsm-12-01
    iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);
    vector<mat> vSINR_1((_rbid_2.ToInt() - _rbid_1.ToInt() + 1) * Parameters::Instance().BASIC.IRBSize);
    //    vector<vector<double> > vSINR2stream(P.SIM.UL.IMSnumOfMUMIMO, vSINR_1);
    //    vector<vector<double> > vSINR2stream_2(P.SIM.UL.IMSnumOfMUMIMO, vSINR_1);

//    vector<vector<mat> > vSINR2stream(vVmimoGroup.size(), vSINR_1);
//    vector<vector<mat> > vSINR2stream_2(vVmimoGroup.size(), vSINR_1);

    vector<vector<mat> > vSINR2stream(1, vSINR_1);
    vector<vector<mat> > vSINR2stream_2(1, vSINR_1);


    double dTimeSec = Clock::Instance().GetTimeSec();
   // cm::LinkMatrix::Instance().WorkSlot(ms_1, dTimeSec); //这个函数是干什么的？更新阴影衰落？
    //cm::LinkMatrix::Instance().WorkSlot(ms_2, dTimeSec);

    SCID firstSCID = _rbid_1.GetFirstSCID();
    SCID lastSCID = _rbid_2.GetLastSCID();


    for (SCID scid = firstSCID; scid <= lastSCID; scid += iSpace) {
        int iRank = m_mMSID2Rank[_msid_1.ToInt()];
//        int iRankcols = ms_1.ms_SCPrecodeMat[scid].cols();

        cmat mS = ms_1.UL.GetTxSRS(SCID(ms_1.UL.GetSRSTxCyclicSCID()), iRank + 1); //SCID(ms.UL.GetSRSTxCyclicSCID())


        cmat mH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(m_BTSID.GetBTS(), ms_1, scid.ToInt()).H();


        //        int iSize = P.SIM.UL.IMSnumOfMUMIMO * P.MSS.UL.IAntennaNum;
        int IAntennnaNum = Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.Polarize_Num;
        int iSize = vVmimoGroup.size() * IAntennnaNum;
        cmat mEffectiveP;
        mEffectiveP.set_size(iSize, iSize, false); //最后获得联合加权系数
        mEffectiveP.zeros(); //想要获得全零的矩阵


        //cmat mP = 1 / sqrt(IAntennnaNum) * itpp::ones_c(IAntennnaNum, 1); //这么写并不符合多天先的情况
        cmat mP;
        if(Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1){
            mP = ms_1.ms_SCPrecodeMat[scid.ToInt()];
        }
        else{
            cmat mU, mD;
            vec vS;
            svd(mH, mU, vS, mD);
            mP = mD.get_cols(0, iRank);
        }
        for (int ii = 0; ii < iSize; ++ii) {
            mEffectiveP.set(ii, ii, mP(0, 0));
        }

        ///计算干扰
        vector<cmat> vSIS, vSIH, vSIP, vWIS;
        vector<double> vWIL;
        cmat mSIS;
        cmat mSIH;
        for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
            MS& msI = msid.GetMS();
            if (find(m_pActiveSet->begin(), m_pActiveSet->end(), msI.GetID()) != m_pActiveSet->end()) {
                continue; //排除本小区的用户
            }
            if (cm::LinkMatrix::Instance().IsStrong(MainServBTS, msI)) {
                // 获取强干扰信号
                 mSIS = msI.UL.GetTxSRS(scid);
                //mSIS += ms_2.UL.GetTxSRS(scid);
                vSIS.push_back(mSIS);
                // 获取强干扰信道
                mSIH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(MainServBTS, msI, scid.ToInt()).H();

                //mSIH += cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(MainServBTS, ms_2, scid.ToInt()).H();
                vSIH.push_back(mSIH);
                // 获取强干扰信号的预编码
                int msAntennaNum= Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.Polarize_Num;
                cmat mSIP = 1 / sqrt(msAntennaNum) * itpp::ones_c(msAntennaNum, 1);
//                cmat mSIP = msI.ms_SCPrecodeMat[scid];
                vSIP.push_back(mSIP);
            } else {
                // 获取弱干扰的信号
                cmat mWIS = msI.UL.GetTxSRS(scid);
                vWIS.push_back(mWIS);
                // 获取弱干扰的信道
                double dLinkLoss = cm::DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(MainServBTS, msI));
                vWIL.push_back(dLinkLoss);
            }
        }
        // 配对用户的有用信号、强干扰、弱干扰计算
//        int iRankcols2= ms_2.ms_SCPrecodeMat[scid.ToInt()].cols();
        int iRank2 = m_mMSID2Rank[_msid_2.ToInt()];
        cmat mSIS_mu = ms_2.UL.GetTxSRS(scid,iRank2+1);
        vSIS.push_back(mSIS_mu);
        cmat mSIH_mu = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(MainServBTS, ms_2, scid.ToInt()).H();
        vSIH.push_back(mSIH_mu);
        cmat mSIP_mu;
        if(Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1){
            mSIP_mu = ms_2.ms_SCPrecodeMat[scid.ToInt()];
        }
        else{
            cmat mU, mD;
            vec vS;
            svd(mSIH_mu, mU, vS, mD);
            mSIP_mu = mD.get_cols(0, iRank2);
        }
        vSIP.push_back(mSIP_mu);

        //mat mSINR = m_pDetector->CalculateSINR(mS, mH, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL); //这个SINR
        mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);

       // cout<<"mSINR rows is: "<<mSINR.rows()<<" ,mSINR cols is: "<<mSINR.cols()<<endl;
        //暂时使用算法中的IOT计算公式
        mat mIoT = m_pDetector->CalculateIoT(mS, mH, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL);

        mat mInf = CalculateInf(mS, mH, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL);

        mat mSINR_tmp;


        //利用RE的长时平均来预测干扰
        if (m_vReEstimate[scid.ToInt()] == itpp::zeros_c(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num, Parameters::Instance().MIMO_CTRL.Total_TXRU_Num)) {
            mSINR_tmp = mSINR;
        } else {
            mSINR_tmp = CalculateSINR(mS, mH, mEffectiveP, vSIS, vSIH, vSIP, vWIS, vWIL, m_vReEstimate[scid.ToInt()]);
        }
//        for (int index = 0; index <static_cast<int> (vVmimoGroup.size()); ++index) {
////            vSINR2stream_2[index][scid.ToInt() - firstSCID.ToInt()] = mSINR_tmp(index, 0);
//            vSINR2stream_2[index][scid.ToInt() - firstSCID.ToInt()] = mSINR(index, 0);
//        }

        int differentMehtod = 1;
        switch (differentMehtod) {
            case 1:
                //方法1：老版本,仅仅利用srs的sinr估计SINR然后再用OLLA去调整
               // for (int index = 0; index <static_cast<int> (vVmimoGroup.size()); ++index) {
                    vSINR2stream[0][scid.ToInt() - firstSCID.ToInt()] = mSINR;
              //  }
                break;
            case 2:
                //方法2： 将干扰替换为业务信道的平均干扰
                for (int index = 0; index <static_cast<int> (vVmimoGroup.size()); ++index) {
                    if (((*m_pRxBufferBTS)[vVmimoGroup[index].ToInt()].m_vIoTEstimate)[scid.ToInt()] == -1) {
                        vSINR2stream[index][scid.ToInt() - firstSCID.ToInt()] = mSINR(index, 0);
                    } else {
                        vSINR2stream[index][scid.ToInt() - firstSCID.ToInt()] = mSINR(index, 0)
                                * mIoT(index, 0) / ((*m_pRxBufferBTS)[vVmimoGroup[index].ToInt()].m_vIoTEstimate)[scid.ToInt()];
                    }
                }
                break;
            case 3:
                //方法2： 将干扰替换为业务信道的瞬时干扰
                for (int index = 0; index <static_cast<int> (vVmimoGroup.size()); ++index) {
                    if (((*m_pRxBufferBTS)[vVmimoGroup[index].ToInt()].m_vInfEst)[scid.ToInt()] == -1) {
                        vSINR2stream[index][scid.ToInt() - firstSCID.ToInt()] = mSINR(index, 0);
                    } else {
                        vSINR2stream[index][scid.ToInt() - firstSCID.ToInt()] = mSINR(index, 0)
                                * mInf(index, 0) / ((*m_pRxBufferBTS)[vVmimoGroup[index].ToInt()].m_vInfEst)[scid.ToInt()];
                    }
                }
                break;
            case 4:
                //方法3： 用两个长时平均的比值(SRS的IOT,和业务信道的IOT)作为参考
                for (int index = 0; index <static_cast<int> (vVmimoGroup.size()); ++index) {
                    if (((*m_pRxBufferBTS)[vVmimoGroup[index].ToInt()].m_vIoTEstimate)[scid.ToInt()] == -1) {
                        vSINR2stream[index][scid.ToInt() - firstSCID.ToInt()] = mSINR(index, 0);
                    } else {
                        vSINR2stream[index][scid.ToInt() - firstSCID.ToInt()] = mSINR(index, 0)
                                * (*m_pRxBufferBTS)[vVmimoGroup[index].ToInt()].m_vSRSIoT[scid.ToInt()] / ((*m_pRxBufferBTS)[vVmimoGroup[index].ToInt()].m_vIoTEstimate)[scid.ToInt()];
                    }
                }
                break;
            default:
                assert(false);
                break;
        }
    }

/*
    //这是根据一般方法进行估计的SINR
    std::vector<double>::iterator begin_1 = vSINR_1.begin();
    std::vector<double>::iterator end_1 = vSINR_1.end();
    InterpolateLog(begin_1, end_1, iSpace, 0);
    //    assert(false); //如果被采用，这些值需要被保存到(*m_pRxBufferBTS)[_MSID] 中
    for (int index = 0; index <static_cast<int> (vVmimoGroup.size()); ++index) {
        std::vector<double>::iterator begin = vSINR2stream[index].begin();
        std::vector<double>::iterator end = vSINR2stream[index].end();
        InterpolateLog(begin, end, iSpace, 0);
    }

    //这是利用RE平均进行干扰估计的SINR
    for (int index = 0; index <static_cast<int> (vVmimoGroup.size()); ++index) {
        std::vector<double>::iterator begin = vSINR2stream_2[index].begin();
        std::vector<double>::iterator end = vSINR2stream_2[index].end();
        InterpolateLog(begin, end, iSpace, 0);
    }
    return vSINR2stream;*/

    //这是根据一般方法进行估计的SINR
    auto begin_1 = vSINR_1.begin();
    auto end_1 = vSINR_1.end();
    InterpolateLog(begin_1, end_1, iSpace, 0);
    //    assert(false); //如果被采用，这些值需要被保存到(*m_pRxBufferBTS)[_MSID] 中
//    for (int index = 0; index <static_cast<int> (vVmimoGroup.size()); ++index) {
        auto begin = vSINR2stream[0].begin();
        auto end = vSINR2stream[0].end();
        InterpolateLog(begin, end, iSpace, 0);
//    }

    //这是利用RE平均进行干扰估计的SINR
//    for (int index = 0; index <static_cast<int> (vVmimoGroup.size()); ++index) {
        begin = vSINR2stream_2[0].begin();
        end = vSINR2stream_2[0].end();
        InterpolateLog(begin, end, iSpace, 0);
//    }
    return vSINR2stream;
}

//itpp::mat BTSVMIMORxer_trial_2::CalculateIntraInf(const cmat& _mS, const cmat& _mH, const cmat& _mP,
//        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
//        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
//        const vector<double>& _vWIL) {
//    ///确定信号的流数和接收天线数
//    int iNs = static_cast<int> (_mS.size());
//    int iNr = _mH.rows();
//    ///计算干扰相关阵
//    cmat mRe(iNr, iNr);
//    mRe.zeros();
//    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
//        cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
//        mCovSIS = diag(diag(mCovSIS)); //取对角线
//        mRe += (_vSIH[i] * _vSIP[i]) * mCovSIS * (_vSIH[i] * _vSIP[i]).H();
//    }
//    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
//        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
//    }
//    double dNoisePSD = Parameters::Instance().BASIC_UL.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
//    mRe += dNoisePSD * eye_c(iNr);
//    ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
//    cmat mCovS = _mS * _mS.H();
//    mCovS = diag(diag(mCovS)); //取对角线
//    ///计算均衡矩阵
//    cmat mG = (inv((_mH * _mP) * mCovS * (_mH * _mP).H() + mRe)*(_mH * _mP)).H();
//    ///定义两个中间变量mD,mIself
//    cmat mD = diag(diag(mG * _mH * _mP));
//    cmat mIself = mG * _mH * _mP - mD;
//    ///计算有用信号分量
//    cmat mSignal = mD*_mS;
//    vector<double> vSignal;
//    for (int i = 0; i < iNs; ++i) {
//        double dSignal = pow(abs(mSignal(i, 0)), 2);
//        vSignal.push_back(dSignal);
//    }
//    ///计算流间干扰分量
//    cmat mIntraI = mIself*_mS;
//    mat mInf(iNs, 1);
//    vector<double> vIntraI;
//    for (int i = 0; i < iNs; ++i) {
//        double dIntraI = pow(abs(mIntraI(i, 0)), 2);
//        vIntraI.push_back(dIntraI);
//        mInf(i, 0) = dIntraI;
//    }
//    return mInf;
//}

//itpp::mat BTSVMIMORxer_trial_2::CalculateInterInf(const cmat& _mS, const cmat& _mH, const cmat& _mP,
//        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
//        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
//        const vector<double>& _vWIL) {
//    ///确定信号的流数和接收天线数
//    int iNs = static_cast<int> (_mS.size());
//    int iNr = _mH.rows();
//    ///计算干扰相关阵
//    cmat mRe(iNr, iNr);
//    mRe.zeros();
//    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
//        cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
//        mCovSIS = diag(diag(mCovSIS)); //取对角线
//        mRe += (_vSIH[i] * _vSIP[i]) * mCovSIS * (_vSIH[i] * _vSIP[i]).H();
//    }
//    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
//        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
//    }
//    double dNoisePSD = Parameters::Instance().BASIC_UL.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
//    mRe += dNoisePSD * eye_c(iNr);
//    ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
//    cmat mCovS = _mS * _mS.H();
//    mCovS = diag(diag(mCovS)); //取对角线
//    ///计算均衡矩阵
//    cmat mG = (inv((_mH * _mP) * mCovS * (_mH * _mP).H() + mRe)*(_mH * _mP)).H();
//    ///定义两个中间变量mD,mIself
//    cmat mD = diag(diag(mG * _mH * _mP));
//    cmat mIself = mG * _mH * _mP - mD;
//    ///计算有用信号分量
//    cmat mSignal = mD*_mS;
//    vector<double> vSignal;
//    for (int i = 0; i < iNs; ++i) {
//        double dSignal = pow(abs(mSignal(i, 0)), 2);
//        vSignal.push_back(dSignal);
//    }
//    ///计算流间干扰分量
//    //    cmat mIntraI = mIself*_mS;
//    //    vector<double> vIntraI;
//    //    for (int i = 0; i < iNs; ++i) {
//    //        double dIntraI = pow(abs(mIntraI(i, 0)), 2);
//    //        vIntraI.push_back(dIntraI);
//    //    }
//    ///计算小区间干扰分量
//    cmat mInterI(iNs, iNs);
//    mInterI.zeros();
//    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
//        mInterI += (mG * _vSIH[i] * _vSIP[i] * _vSIS[i])*(mG * _vSIH[i] * _vSIP[i] * _vSIS[i]).H();
//    }
//    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
//        cmat mTemp = _vWIL[i] * _vWIS[i].H() * _vWIS[i];
//        mInterI += abs(mTemp(0, 0)) * mG * mG.H();
//    }
//    vector<double> vInterI;
//    for (int i = 0; i < iNs; ++i) {
//        double dInterI = abs(mInterI(i, i));
//        vInterI.push_back(dInterI);
//    }
//    ///计算噪声分量
//    cmat mNoise(iNs, iNs);
//    mNoise = dNoisePSD * mG * mG.H();
//    vector<double> vNoise;
//    for (int i = 0; i < iNs; ++i) {
//        double dNoise = abs(mNoise(i, i));
//        vNoise.push_back(dNoise);
//    }
//    mat mInf(iNs, 1);
//    for (int i = 0; i < iNs; ++i) {
//        double dInf = vInterI[i] + vNoise[i];
//        mInf(i, 0) = dInf;
//    }
//    return mInf;
//}

itpp::mat BTSVMIMORxer_trial_2::CalculateInf(const cmat& _mS, const cmat& _mH, const cmat& _mP,
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
        mRe += w(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
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
    mat mInf(iNs, 1);
    for (int i = 0; i < iNs; ++i) {
        double dInf = vIntraI[i] + vInterI[i] + vNoise[i];
        mInf(i, 0) = dInf;
    }
    return mInf;
}

//cmat BTSVMIMORxer_trial_2::CalculateRe(const cmat& _mS, const cmat& _mH, const cmat& _mP,
//        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
//        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
//        const vector<double>& _vWIL) {
//    ///确定信号的流数和接收天线数
//    int iNs = static_cast<int> (_mS.size());
//    int iNr = _mH.rows();
//    ///计算干扰相关阵
//    cmat mRe(iNr, iNr);
//    mRe.zeros();
//    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
//        cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
//        mCovSIS = diag(diag(mCovSIS)); //取对角线
//        mRe += (_vSIH[i] * _vSIP[i]) * mCovSIS * (_vSIH[i] * _vSIP[i]).H();
//    }
//    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
//        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
//    }
//    double dNoisePSD = Parameters::Instance().BASIC_UL.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
//    mRe += dNoisePSD * eye_c(iNr);
//
//    return mRe;
//}

itpp::mat BTSVMIMORxer_trial_2::CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL,
        const cmat& _mRe) {

    ///确定信号的流数和接收天线数
    int iNs = static_cast<int> (_mS.size());
    int iNr = _mH.rows();
    ///计算干扰相关阵
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);

    ///构造MMSE的检测矩阵，G是NrXNs的复矩阵
    cmat mCovS = _mS * _mS.H();
    mCovS = diag(diag(mCovS)); //取对角线
    ///计算均衡矩阵
    cmat mG = (inv((_mH * _mP) * mCovS * (_mH * _mP).H() + _mRe)*(_mH * _mP)).H();
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
    for (int i = 0; i < iNs; ++i) {
        double dSINR = vSignal[i] / (vIntraI[i] + vInterI[i] + vNoise[i]);
        mSINR(i, 0) = dSINR;
    }
    return mSINR;
}

std::shared_ptr<SchedulingMessageUL> BTSVMIMORxer_trial_2::GetSchedulingMsg(const MSID& _msid) {
    std::shared_ptr<SchedulingMessageUL> pSchM;
//    if (m_pHARQRxState_BTS_UL->IsReTxCurrentSF(_msid)) {
//        pSchM = m_pHARQRxState_BTS_UL->GetReTxMessage(_msid)->GetSchMessage();
//        assert(pSchM);
//        //遍历占用的RB，根据RB上找到另外那个用户，然后看用户是否重传，再然后据此更改该消息中的RB到MS的映射表
//        std::unordered_map<int, vector<MSID> > mRB2vMSID = pSchM->GetRB2vMSID();
//        vector<RBID> vRBID = pSchM->GetRBUsed();
//        for (auto& rbid : vRBID) { //遍历所有用户占用的资源
//            //判断另外的用户是否重传
//            vector<MSID> vMSID = mRB2vMSID[rbid.ToInt()];
//            if (vMSID.size() == 1) {
//                assert(vMSID[0] == _msid);
//                continue;
//            }
//            assert(static_cast<int> (vMSID.size()) <= Parameters::Instance().SIM_UL.UL.IMSnumOfMUMIMO);
//
//            vector<MSID> tmp;
//            for (auto& other : vMSID) {
//                //                cout << "msid mu = " << other.ToInt() << endl;
//                if (IsReTx(other)) {
//                    tmp.push_back(other);
//                }
//            }
//            mRB2vMSID[rbid.ToInt()] = tmp;
//        }
//        //修改了映射表，当前的重传消息中就将正确传输的用户排除掉了
//        pSchM->SetRB2vMSID(mRB2vMSID); //将上面修改的调度消息重新放回那个位置
//    } else {
//        assert((*m_pRxBufferBTS).count(_msid.ToInt()) != 0);
//        pSchM = (*m_pRxBufferBTS)[_msid.ToInt()].GetSchedulingMessage(); //这个是新调度的？
//        //        assert(pSchM != 0);
//    }
    assert((*m_pRxBufferBTS).count(_msid.ToInt()) != 0);
    pSchM = (*m_pRxBufferBTS)[_msid.ToInt()].GetSchedulingMessage(); //这个是新调度的？
    return pSchM;
}

void BTSVMIMORxer_trial_2::SINRInterpolate(const MSID& _msid,
        vector<mat> _vSINR,
        int _iSpace,
        vector<SCID> _SCIDUsed) {

//    auto begin = _vSINR.begin();
//    auto end = _vSINR.end();

//    InterpolateLog(begin, end, _iSpace, 0);
    for (int SCIDIndex = 0; SCIDIndex < static_cast<int>(_SCIDUsed.size()); SCIDIndex += _iSpace) {
        (*m_pRxBufferBTS)[_msid.ToInt()].m_vSINR[_SCIDUsed[SCIDIndex].ToInt()/_iSpace] = _vSINR[SCIDIndex/_iSpace];
    }
}

void BTSVMIMORxer_trial_2::vIoTEstimateInterpolate(const MSID& _msid,
        vector<double>& _vIoT,
        int _iSpace,
        const vector<SCID>& _SCidUsed) {

//        if(_SCidUsed.size() > 12){
//            auto begin = _vIoT.begin();
//            auto end = _vIoT.end();
//            InterpolateLog(begin, end, _iSpace, 0);
//        } else if(_SCidUsed.size() == 12){
//            for (int Index = 0; Index < static_cast<int>(_vIoT.size()); Index++) {
//                _vIoT[Index] = _vIoT[Index / _iSpace * _iSpace];
//            }
//        } else{
//            assert(false);
//        }
    for (int SCidIndex = 0; SCidIndex < _SCidUsed.size(); SCidIndex += _iSpace) {
        SCID scid = _SCidUsed[SCidIndex];
        double& frombuffer = (*m_pRxBufferBTS)[_msid.ToInt()].m_vIoTEstimate[scid.ToInt() / _iSpace];
        double& _iot=_vIoT[SCidIndex / _iSpace];
        if (frombuffer < 0) {
            frombuffer = _iot;
            continue;
        }
        frombuffer *= 0.8;
        frombuffer += 0.2 * _iot;
    }
}

void BTSVMIMORxer_trial_2::vInfAfterDetecInterpolate(const MSID& _msid, vector<double>& _vInfAfterDetec, int _iSpace, vector<SCID> _SCidUsed) {
//    auto beginInter = _vInfAfterDetec.begin();
//    auto endInter = _vInfAfterDetec.end();
//    InterpolateLog(beginInter, endInter, _iSpace, 0);

//    if(_SCidUsed.size() > 12){
//        auto beginInter = _vInfAfterDetec.begin();
//        auto endInter = _vInfAfterDetec.end();
//        InterpolateLog(beginInter, endInter, _iSpace, 0);
//    } else if(_SCidUsed.size() == 12){
//        for (int Index = 0; Index < static_cast<int>(_vInfAfterDetec.size()); Index++) {
//            _vInfAfterDetec[Index] = _vInfAfterDetec[Index / _iSpace * _iSpace];
//        }
//    } else{
//        assert(false);
//    }
    for (int SCidIndex = 0; SCidIndex < _SCidUsed.size(); SCidIndex += _iSpace) {
        SCID scid = _SCidUsed[SCidIndex];
        double& frombuffer=(*m_pRxBufferBTS)[_msid.ToInt()].m_vInfEst[_SCidUsed[SCidIndex].ToInt() / _iSpace];
        double& fromv=_vInfAfterDetec[SCidIndex / _iSpace];
        if (frombuffer < 0) {
            frombuffer = fromv;
            continue;
        }
        frombuffer *= 0.8;
        frombuffer += 0.2 * fromv;
    }
}

void BTSVMIMORxer_trial_2::vIOT_forAverageInterpolate(const MSID& _msid, vector<double>& _vIOT_forAverage,
        int _iSpace,
        vector<SCID> _SCidUsed) {
//    auto begin = _vIOT_forAverage.begin(); //这个插值行吗？
//    auto end = _vIOT_forAverage.end();
//    Interpolate(begin, end, _iSpace, 0); //这个是不是有问题？

//    if(_SCidUsed.size() > 12){
//        auto begin = _vIOT_forAverage.begin(); //这个插值行吗？
//        auto end = _vIOT_forAverage.end();
//        Interpolate(begin, end, _iSpace, 0); //这个是不是有问题？
//    } else if(_SCidUsed.size() == 12){
//        for (int Index = 0; Index < static_cast<int>(_vIOT_forAverage.size()); Index++) {
//            _vIOT_forAverage[Index] = _vIOT_forAverage[Index / _iSpace * _iSpace];
//        }
//    } else{
//        assert(false);
//    }
    for (int SCidIndex = 0; SCidIndex < _SCidUsed.size(); SCidIndex += _iSpace) {
        SCID scid = _SCidUsed[SCidIndex];
        if (m_vAveIoT[scid.ToInt() / _iSpace] == -1)
            m_vAveIoT[scid.ToInt() / _iSpace] = _vIOT_forAverage[SCidIndex / _iSpace];
    }
}

void BTSVMIMORxer_trial_2::vReEstimateInterpolate(const MSID& _msid, vector<cmat> _vRe,
        int _iSpace,
        vector<SCID> _SCidUsed) {
//    if(_SCidUsed.size() > 12){
//        auto begin = _vRe.begin(); //这个插值行吗？
//        auto end = _vRe.end();
//        Interpolate(begin, end, _iSpace, 0); //这个是不是有问题？
//    } else if(_SCidUsed.size() == 12){
//        for (int Index = 0; Index < static_cast<int>( _vRe.size()); Index++) {
//            _vRe[Index] =  _vRe[Index / _iSpace * _iSpace];
//        }
//    } else{
//        assert(false);
//    }

    for (int SCidIndex = 0; SCidIndex < _SCidUsed.size(); SCidIndex += _iSpace) {
        itpp:cmat& c= m_vReEstimate[_SCidUsed[SCidIndex].ToInt() / _iSpace];
        SCID scid = _SCidUsed[SCidIndex];
        if (c == itpp::zeros_c(Parameters::Instance().MIMO_CTRL.Total_TXRU_Num, Parameters::Instance().MIMO_CTRL.Total_TXRU_Num)) {
            c = _vRe[SCidIndex / _iSpace];
            continue;
        }
        c = 0.8*c+0.2*_vRe[SCidIndex / _iSpace];
    }
}

void BTSVMIMORxer_trial_2::ComputeIoTPreDetection() {
    //计算SRS的SINR
    BTS& MainServBTS = m_BTSID.GetBTS();
    int iSpace = 4;
    iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);
    vector<double > IOTpreDetection(Parameters::Instance().BASIC.ISCNum / iSpace, 0); //保存每个SC上的IOT

    for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
        double dNoisePowerMw = Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw; //处理热噪声

        for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
            MS& msI = msid.GetMS();
            if (find(m_pActiveSet->begin(), m_pActiveSet->end(), msI.GetID()) != m_pActiveSet->end()) {
                continue; //排除本小区的用户
            }
            // 获取弱干扰的信号
            cmat mWIS = msI.UL.GetTxDataSymbol(scid);
            if (real((mWIS.H() * mWIS)(0, 0)) > 0) {
                int debug = 0;
            }
            // 获取弱干扰的信道
            double dLinkLoss = DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(MainServBTS, msI));
            double dWeakIMW = real((mWIS.H() * mWIS)(0, 0)) * dLinkLoss;
            dNoisePowerMw += dWeakIMW;
        }

        dNoisePowerMw = dNoisePowerMw / Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw;
        IOTpreDetection[scid.ToInt() / iSpace] = dNoisePowerMw;
    }

//    auto begin = IOTpreDetection.begin();
//    auto end = IOTpreDetection.end();
//    InterpolateLog(begin, end, iSpace, 0); //根据用户SRS占用的SC进行偏移

    double dmeanIOToverWideBand = 0;
    for (const auto& it : IOTpreDetection) {
        dmeanIOToverWideBand += it;
    }
    dmeanIOToverWideBand /= IOTpreDetection.size();
    double dmeanIOToverWideBandDB = L2DB(dmeanIOToverWideBand);

    int islot = Clock::Instance().GetTimeSlot();
    if(islot >= Parameters::Instance().BASIC.IWarmUpSlot)
    {
        Statistician::Instance().m_BTSData[m_BTSID.GetBTS().GetTxID()].UL.m_dIoT += dmeanIOToverWideBand; //统计平均SINR
        Statistician::Instance().m_BTSData[m_BTSID.GetBTS().GetTxID()].UL.m_Iiot_count += 1;

    }
}

vector<vector<mat > > BTSVMIMORxer_trial_2::ComputeEstSINR4MultiUE(vector<MSID> _vmsid,
        RBID _rbid_1,
        RBID _rbid_2) {
    //保存配对的用户，方便后面循环


    //计算SRS的SINR
    BTS& MainServBTS = m_BTSID.GetBTS();
    for (int i = 0; i < _vmsid.size(); ++i) {
        MS& ms_temp = _vmsid[i].GetMS();
        assert(ms_temp.GetMainServBTS() == m_BTSID);
    }

    int iSpace = 4;
    iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);
    vector<mat> vSINR_1((_rbid_2.ToInt() - _rbid_1.ToInt() + 1) * Parameters::Instance().BASIC.IRBSize);
    vector<vector<mat> > vSINR2stream(_vmsid.size(), vSINR_1);


    double dTimeSec = Clock::Instance().GetTimeSec();
//    for (int i = 0; i < static_cast<int> (_vmsid.size()); ++i) {
//        MS& ms_temp = _vmsid[i].GetMS();
////        cm::LinkMatrix::Instance().WorkSlot(ms_temp, dTimeSec);
//    }

    SCID firstSCID = _rbid_1.GetFirstSCID();
    SCID lastSCID = _rbid_2.GetLastSCID();

    MSID mainMSID = _vmsid[0];
    MS& mainMS = _vmsid[0].GetMS();


    for (SCID scid = firstSCID; scid <= lastSCID; scid += iSpace) {
        //        cmat mS_test = ms_1.UL.GetTxSRS(SCID(ms_1.UL.GetSRSTxCyclicSCID())); //SCID(ms.UL.GetSRSTxCyclicSCID())
        //        mS_test = concat_vertical(mS_test, ms_2.UL.GetTxSRS(SCID(ms_2.UL.GetSRSTxCyclicSCID())));
        //gengxin

        cmat mS;
        int iRank = m_mMSID2Rank[mainMSID.ToInt()];
//        int iRank = mainMS.ms_SCPrecodeMat[scid.ToInt()].cols();
        mS = mainMS.UL.GetTxSRS( SCID(mainMS.UL.GetSRSTxCyclicSCID()), iRank+1);

//        for (int i = 0; i < static_cast<int> (_vmsid.size()); ++i) {
//            MS& ms_temp = _vmsid[i].GetMS();
//            mS = concat_vertical(mS, ms_temp.UL.GetTxSRS(SCID(ms_temp.UL.GetSRSTxCyclicSCID())));
//        }

        //gengxin
        cmat mH;
        mH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(m_BTSID.GetBTS(), mainMS, scid.ToInt()).H();

//        for (int i = 0; i < _vmsid.size(); ++i) {
//            MS& ms_temp = _vmsid[i].GetMS();
//            cmat mTmpH_test = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(m_BTSID.GetBTS(), ms_temp, scid.ToInt()).H();
//            mH = concat_horizontal(mH, mTmpH_test);
//        }

//        int IAntennnaNum=Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.Polarize_Num;
//        int iSize = _vmsid.size() * IAntennnaNum;
//        //_vmsid
//        cmat mEffectiveP;
//        mEffectiveP.set_size(iSize, iSize, false); //最后获得联合加权系数
//        mEffectiveP.zeros(); //想要获得全零的矩阵
//
//
//        cmat mP = 1 / sqrt(IAntennnaNum) * itpp::ones_c(IAntennnaNum, 1); //这么写并不符合多天先的情况
//        for (int ii = 0; ii < iSize; ++ii) {
//            mEffectiveP.set(ii, ii, mP(0, 0));
//        }
        cmat mP;
        if(Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1){
            mP = mainMS.ms_SCPrecodeMat[scid.ToInt()];
        }
        else{
            cmat mU, mD;
            vec vS;
            svd(mH, mU, vS, mD);
            mP = mD.get_cols(0, iRank);
        }

//        cout<<"this is ComputeEstSINR4MultiUE!"<<endl;
        //CombineMultiMS(mainMSID,_vmsid, scid, mS, mH, mP);

        ///计算干扰
        vector<cmat> vSIS, vSIH, vSIP, vWIS;
        vector<double> vWIL;
        for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
            MS& msI = msid.GetMS();
            if (find(m_pActiveSet->begin(), m_pActiveSet->end(), msI.GetID()) != m_pActiveSet->end()) {
                continue; //排除本小区的用户
            }
            if (cm::LinkMatrix::Instance().IsStrong(MainServBTS, msI)) {
                // 获取强干扰信号
                cmat mSIS = msI.UL.GetTxSRS(scid);
                vSIS.push_back(mSIS);
                // 获取强干扰信道
                cmat mSIH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(MainServBTS, msI, scid.ToInt()).H();
                //                assert(false); //检查信道的维数，与后面的检测算法是否匹配
                vSIH.push_back(mSIH);
                // 获取强干扰信号的预编码
                int IAntennnaNum = Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.Polarize_Num;
                cmat mSIP = 1 / sqrt(IAntennnaNum) * itpp::ones_c(IAntennnaNum, 1);
                vSIP.push_back(mSIP);
            } else {
                // 获取弱干扰的信号
                cmat mWIS = msI.UL.GetTxSRS(scid);
                vWIS.push_back(mWIS);
                // 获取弱干扰的信道
                double dLinkLoss = cm::DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(MainServBTS, msI));
                vWIL.push_back(dLinkLoss);
            }
        }
        //配对用户干扰 20220101 chuwq
        for(int i=1;i<_vmsid.size();i++){
            MS& msI = _vmsid[i].GetMS();
            int msI_rank = m_mMSID2Rank[_vmsid[i].ToInt()];
//            int msI_rank = msI.ms_SCPrecodeMat[scid.ToInt()].cols();
            // 获取强干扰信号
            cmat mSIS = msI.UL.GetTxSRS(SCID(msI.UL.GetSRSTxCyclicSCID()), msI_rank+1);
            vSIS.push_back(mSIS);
            // 获取强干扰信道
            cmat mSIH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(MainServBTS, msI, scid.ToInt()).H();
            //            assert(false); //检查信道的维数，与后面的检测算法是否匹配
            vSIH.push_back(mSIH);
            // 获取强干扰信号的预编码
            cmat mSIP;
            if(Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1){
                mSIP = msI.ms_SCPrecodeMat[scid.ToInt()];
            }
            else{
                cmat mU, mD;
                vec vS;
                svd(mSIH, mU, vS, mD);
                mSIP = mD.get_cols(0, msI_rank);
            }
            vSIP.push_back(mSIP);
        }

        mat mSINR = m_pDetector->CalculateSINR(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL); //这个SINR应该是两维的
        //暂时使用算法中的IOT计算公式

        mat mInf = CalculateInf(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);

        int differentMehtod = 1;
        switch (differentMehtod) {
             case 1:
                //方法1：老版本,仅仅利用srs的sinr估计SINR然后再用OLLA去调整
                for (int index = 0; index <static_cast<int> (_vmsid.size()); ++index) {
//                    vSINR2stream[index][scid.ToInt() - firstSCID.ToInt()] = mSINR(index, 0);
                    vSINR2stream[index][scid.ToInt() - firstSCID.ToInt()] = mSINR;
                }
                break;

            case 3:
                //方法2： 将干扰替换为业务信道的瞬时干扰
                //_vmsid
                for (int index = 0; index <static_cast<int> (_vmsid.size()); ++index) {
                    if (((*m_pRxBufferBTS)[_vmsid[index].ToInt()].m_vInfEst)[scid.ToInt()] == -1) {
                        vSINR2stream[index][scid.ToInt() - firstSCID.ToInt()] = mSINR(index, 0);
                    } else {
                        vSINR2stream[index][scid.ToInt() - firstSCID.ToInt()] = mSINR(index, 0)
                                * mInf(index, 0) / ((*m_pRxBufferBTS)[_vmsid[index].ToInt()].m_vInfEst)[scid.ToInt()];
                    }
                }
                break;
            default:
                assert(false);
        }
    }

    //    assert(false); //如果被采用，这些值需要被保存到(*m_pRxBufferBTS)[_MSID] 中
//    for (int index = 0; index <static_cast<int> (_vmsid.size()); ++index) {
//        std::vector<double>::iterator begin = vSINR2stream[index].begin();
//        std::vector<double>::iterator end = vSINR2stream[index].end();
//        InterpolateLog(begin, end, iSpace, 0);
//    }

     for (int index = 0; index <static_cast<int> (_vmsid.size()); ++index) {
        auto begin = vSINR2stream[index].begin();
        auto end = vSINR2stream[index].end();
        InterpolateLog(begin, end, iSpace, 0);
    }

    //这是利用RE平均进行干扰估计的SINR

    return vSINR2stream;
}

void BTSVMIMORxer_trial_2::GetH4SRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL, mat& _HerrorScale, cmat & _Herror
        ) {
    //need to consider the multi-dimension

    //计算折算的SINR
    mat mSINR = GetSinr4SRSError(_mS, _mH, _mP, _vSIS, _vSIH, _vSIP, _vWIS, _vWIL);
    //    cout << mSINR << endl;
    //    for (int i = 0; i < mSINR.rows(); ++i) {
    //        for (int j = 0; j < mSINR.cols(); ++j) {
    //            Observer::SetIsEnable(true);
    //            Observer::Print("presinr") << mSINR(i, j) << endl;
    //        }
    //    }
    //如果要支持多个端口的话，这个SINR应该是CMAT类型的
    cmat cmOutputError;
    mat mAlpha;
    //这个输出是直接加载等效信道上的
    cmOutputError.set_size(_mH.rows(), _mH.cols(), false);
    mAlpha.set_size(_mH.rows(), _mH.cols(), false);

    //what if in vmimo case?
    for (int i = 0; i < _mH.rows(); ++i) {
        for (int j = 0; j < _mH.cols(); ++j) {
            //DDeltaMSEDb
            double tmp = mSINR(i, j) * DB2L(Parameters::Instance().SIM_UL.UL.ERROR.DDeltaMSEDb);
            double dstd = 1 / tmp;
            double dalpha = tmp / (1 + tmp);
            mAlpha.set(i, j, dalpha);

            complex<double> cRND;
            cRND.real(random.xNormal_error(0, dstd));
            cRND.imag(random.xNormal_error(0, dstd));
            cmOutputError.set(i, j, cRND); //当前假设只有一根发射天线
            //函数还需要返回STD
        }
    }
    _Herror = cmOutputError;
    _HerrorScale = mAlpha;
}

mat BTSVMIMORxer_trial_2::GetSinr4SRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL) {


    ///确定信号的流数和接收天线数
    int iNs = static_cast<int> (_mS.size());
    int iNr = _mH.rows();

    mat mResult(iNr, iNs);
    mResult.zeros();
    ///计算干扰相关阵
    cmat mRe(iNr, iNr);
    mRe.zeros();

    cmat mTest(iNr, iNr);
    mTest.zeros();
    //如果干扰是两流的，则在SIS中能体现出来
    //SRS 计算干扰，这里应该根据流数分开
    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        cmat mCovSIS = _vSIS[i] * _vSIS[i].H();
        mCovSIS = diag(diag(mCovSIS)); //取对角线
        mRe += (_vSIH[i] * _vSIP[i]) * mCovSIS * (_vSIH[i] * _vSIP[i]).H();
    }
    //    cout << mRe << endl;

    for (int i = 0; i<static_cast<int> (_vSIS.size()); ++i) {
        cmat tempS = _vSIH[i] * _vSIP[i] * _vSIS[i];
        mTest += tempS * tempS.H();
    }
    //    cout << mTest << endl;

    for (int i = 0; i<static_cast<int> (_vWIS.size()); ++i) {
        //        mRe += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr); ///这儿维数一定要准确，编译器不会报错，但是计算结果是错的！
        mTest += _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr);
        //        cout << _vWIL[i]*(_vWIS[i].H() * _vWIS[i])(0, 0) * eye_c(iNr) << endl;
    }
    double dNoisePSD = Parameters::Instance().BASIC.IDLORUL == 1 ? (Parameters::Instance().MSS_UL.DL.DSCNoisePowerMw) : (Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw);
    mRe += dNoisePSD * eye_c(iNr);
    mTest += dNoisePSD * eye_c(iNr);
    //如果采用2天线发送，则累计的干扰是两根发射天线的
    //if using 2 tx antenna, how to combine the power. e.g. matrix mulitplication
    //for now supposing only one signal(one layer) send from one UE
    //以上可以作为计算SINR的分母
    vector<double> vInf;

    //inter-cell interference and noise
    for (int i = 0; i < iNr; ++i) {
        //检查输出是不是想要的
        complex<double> cInf = mTest._elem(i, i);
        double dInf = cInf.real();
        vInf.push_back(dInf);
    }
    cmat signalmat(iNs, iNs);
    signalmat.zeros();

    for (int i = 0; i < iNs; ++i) {
        signalmat(i, i) = _mS(i, 0);
    }
    signalmat = _mH * _mP * signalmat;

    for (int i = 0; i < iNs; ++i) {
        for (int j = 0; j < iNr; ++j) {
            //column index and row index has some problem;
            complex<double > sig = signalmat(j, i);
            double signal = pow(abs(sig), 2);

            double inf = vInf[j];
            //intra inf
            //            for (int index = 0; index < iNs; ++index) {
            //                if (index == i) continue;
            //                inf += pow(abs(signalmat(index, j)), 2);
            //            }

            //column index and row index has some problem;
            mResult(j, i) = signal / inf;
        }
    }

    return mResult;
}

void BTSVMIMORxer_trial_2::GetH4DMRSError(const MSID& _msid,
        const cmat& _mS, const cmat& _mH, const cmat& _mP,
        const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
        const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
        const vector<double>& _vWIL, const mat& _HerrorStd, const cmat& _Herror
        ) {
    assert(false);
    //    MS& ms = _msid.GetMS();
    //    bool bIsLOS = cm::LinkMatrix::Instance().IsLOS(m_BTSID.GetBTS(), ms);
    //    double dVarA, dVarB;
    //    if (bIsLOS) {
    //        if (P.MSS.DVelocityMPS == 0.833) {//3公里，LOS
    //            dVarA = 0.2711;
    //            dVarB = 0.0933;
    //        } else if (P.MSS.DVelocityMPS == 8.33) {//30公里，LOS
    //            dVarA = 0.2711;
    //            dVarB = 0.0898;
    //        } else {
    //            assert(false);
    //        }
    //    } else {//NLOS
    //        if (P.MSS.DVelocityMPS == 0.833) {//3公里，NLOS
    //            dVarA = 0.2663;
    //            dVarB = 0.09;
    //        } else if (P.MSS.DVelocityMPS == 8.33) {//30公里，NLOS
    //            dVarA = 0.2711;
    //            dVarB = 0.0903;
    //        } else {
    //            assert(false);
    //        }
    //    }
    //    vector<double> vSINR = GetSinr4DMRSError(_mS, _mH, _mP,
    //            _vSIS, _vSIH, _vSIP, _vWIS, _vWIL);
    //    //如果要支持多个端口的华，这个SINR应该是CMAT类型的
    //
    //    cmat cmOutputH;
    //    mat mStd;
    //    //这个输出是直接加载等效信道上的
    //    //下面的这个写法还是有点问题的
    //    //        cmOutputH.set_size(_mH.rows(), _mS.rows(), false);
    //    cmOutputH.set_size(_mH.rows(), _mH.cols(), false);
    //    mStd.set_size(_mH.rows(), _mH.cols(), false);
    //    int iHrowNum = _mH.rows();
    //    assert(_mH.cols() == 1);
    //    for (int i = 0; i < iHrowNum; ++i) {
    //        double dSigma = dVarA * exp(-1 * dVarB * vSINR[i]);
    //        dSigma = pow(dSigma, 2);
    //        double dstd = min(0.49, dSigma / (1 - 2 * dSigma));
    //        mStd.set(i, 0, dstd);
    //        //        assert(false);
    //        //需要跟别人比对这个值一般在多少
    //        complex<double> cRND;
    //        //        cRND.real(xNormal(0, dstd));
    //        //        cRND.imag(xNormal(0, dstd));
    //        //xNormal_error
    //        cRND.real(xNormal_error(0, dstd));
    //        cRND.imag(xNormal_error(0, dstd));
    //        //        assert(false);
    //        //这里需要check？
    //        cmOutputH.set(i, 0, cRND); //当前假设只有一根发射天线
    //        //函数还需要返回STD
    //    }
    //    _Herror = cmOutputH;
    //    _HerrorStd = mStd;
}


void BTSVMIMORxer_trial_2::BufferReTxACKmsg() {
    for (const auto& msid : (*m_pActiveSet)) {
        if (m_pHARQRxState_BTS_UL->IsReTxCurrentSF(msid)) {
            //ZHENGYI 20120703
            std::shared_ptr<ACKNAKMessageUL> Keep4ReTxInMU = m_pHARQRxState_BTS_UL->GetReTxMsgwithoutDel(msid);
            BufferACKmsg4retxinMU(Keep4ReTxInMU);
        }
    }
}

void BTSVMIMORxer_trial_2::BufferACKmsg4retxinMU(const std::shared_ptr<ACKNAKMessageUL>& _temp) {
    m_qRetxACKMsgQue_CurrentSlot.push_back(_temp);
}

void BTSVMIMORxer_trial_2::ResetACKMsgBuffer() {
    m_qRetxACKMsgQue_CurrentSlot.clear();
}

bool BTSVMIMORxer_trial_2::IsReTx(const MSID& _msid) {
    //m_qRetxACKmsgQue_currentSlot
    bool result = false;
    for (const auto& temp : m_qRetxACKMsgQue_CurrentSlot) {
        if (temp->GetSchMessage()->GetMSID() == _msid) {
            result = true;
        }
    }
    return result;

}
// 找出吞吐量最大的流和最好的码字
void BTSVMIMORxer_trial_2::ComputeRIandPMI(const MSID& _msid){
    int iTime = Clock::Instance().GetTimeSlot();
    vector<double> vRank2Capacity(m_iMaxRank + 1, 0.0);
    MS& ms = _msid.GetMS();
    m_iBestRank = -1;
    double dC = -1;
    //const static int iSpace = Parameters::Instance().BASIC_UL.IFrequencySpace; //子载波插值
    int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace;
    //chty 1110 b
    vector<mat>& vmSINR4BestRank=m_MSID2mSINR4BestRank[_msid.ToInt()];
    //chty 1110 e
    for (int rank = 0; rank <= m_iMaxRank; ++rank) {
        std::unordered_map<int, int> vSC2BestPMI(Parameters::Instance().BASIC.ISCNum / iSpace);
        //map<SBID, int> m_mSubBandPMI;
        //vSB2BestPMI.resize(Parameters::Instance().BASIC.ISBNum);
        if(Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1){
            for (SCID scid = SCID(ms.UL.GetSRSTxCyclicSCID()); scid <= scid.End(); scid += iSpace) {
                int iBestPMI = CalcBestPMI(_msid,rank, scid);
                vSC2BestPMI[scid.ToInt()] = iBestPMI;
            }
        }
        //ComputeEstimateSINR_EBBCQI_AverageI(rank);
        vector<mat> _mSINR4SelectedRank;
        _mSINR4SelectedRank.resize(Parameters::Instance().BASIC.ISCNum /iSpace, zeros(rank+1, 1));
        ComputeEstimateSINR_ForSelectedRank_EBBCQI_AverageI(
                _msid,rank, _mSINR4SelectedRank, vSC2BestPMI);
//        auto begin = _mSINR4SelectedRank.begin();
//        auto end = _mSINR4SelectedRank.end();
//
//        InterpolateLog(begin, end, iSpace, ms.UL.GetSRSTxCyclicSCID()); //根据用户SRS占用的SC进行偏移

        // lc for cross-slot interference
        double dCapacity = EstimateWholeBandCapacity_For_mSINR(_mSINR4SelectedRank , _msid);
        //cout<<dCapacity<<"   "<<rank<<" "<<endl;
        vRank2Capacity[rank] = dCapacity;

        if (vRank2Capacity[rank] > dC) {
            dC = vRank2Capacity[rank];
            m_iBestRank = rank;
            m_mSCPMI = vSC2BestPMI;
            //chty 1110 b
            static std::mutex swaplock;
            swaplock.lock();
            swap(vmSINR4BestRank,_mSINR4SelectedRank);
            swaplock.unlock();
            //chty 1110 e
           /* for (SCID scid = SCID::Begin(); scid <= SCID::End(); scid += iSpace) {
                ((*m_pRxBufferBTS)[_msid].m_vSRSSINR)[scid.ToInt() ] = _mSINR4SelectedRank[scid.ToInt()];
            }*/
        }

    }
    //            m_iBestRank = 0; //单流不注释、否则注释该行
//    m_iBestRank = std::min(m_iBestRank,
//            Parameters::Instance().MIMO_CTRL.IMaxRankNum-1 );
//    if(iTime >= Parameters::Instance().BASIC.IWarmUpSlot)
//    {//大话务只在drop开始时计算一次，如果加了if就一直是0了
    Statistician::Instance().m_MSData_UL[_msid.ToInt()].m_iRank[m_iBestRank]++;
//    }
    m_mMSID2Rank[_msid.ToInt()]=m_iBestRank;
    ms.iBestRank_UL = m_iBestRank;
    if(Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1){
        for (SCID scid = scid.Begin(); scid <= scid.End(); ++scid) {
            ms.ms_SCPrecodeMat[scid.ToInt()] =
                    CodeBookFactory::Instance().GetCodeBook(ms.GetAntennaPointer()->GetActiveTXRU_Num())
                            ->GetCodeWord(m_mSCPMI[scid.ToInt()], m_iBestRank);
//        m_mSCPrecodeMat[scid] = CodeBookLTE2TX::Instance().GetCodeWord(m_mSCPMI[scid], m_iBestRank);
        }
    }
//        ms.ms_SCPrecodeMat =m_mSCPrecodeMat;
}

int BTSVMIMORxer_trial_2::CalcBestPMI(const MSID& _MSID, const int& _rank, const SCID& _scid) {

    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    MS& ms = _MSID.GetMS();
    BTS& MainServBTS = m_BTSID.GetBTS();
    double dMaxres = 0.0;
    int iBestPMI = 0;
    int totalCodewordNum = CodeBookFactory::Instance().GetCodeBook(ms.GetAntennaPointer()->GetActiveTXRU_Num())->GetCodeNum(_rank);
    for (int j = 0; j < totalCodewordNum; j++) {
        double dresult = 0.0;
        cmat mH = lm.GetFadingMat_wABF_for_all_active_TXRU_Pairs(
                m_BTSID.GetBTS(), ms, _scid.ToInt()).H();
        cmat mHe = mH * CodeBookFactory::Instance().GetCodeBook(ms.GetAntennaPointer()->GetActiveTXRU_Num())->GetCodeWord(j,_rank);
        for (int k = 0; k < mHe.rows(); k++) {
            for (int l = 0; l < mHe.cols(); l++) {
                dresult = dresult + norm(mHe(k, l));
            }
        }
        if (dresult > dMaxres) {
            dMaxres = dresult;
            iBestPMI = j;
        }
    }
    return iBestPMI;
}
void BTSVMIMORxer_trial_2::ComputeEstimateSINR_ForSelectedRank_EBBCQI_AverageI(
        const MSID& _msid,const int& _iRank, vector<mat>& _vmSINR4SelectedRank, std::unordered_map<int, int>& _vBestPMI) {
   // cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    //const static int iSpace = Parameters::Instance().BASIC_UL.IFrequencySpace; //子
    int iSpace = 4;
    iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);


    MS& ms = _msid.GetMS();

    //子载波Loop

    for (SCID scid = SCID(ms.UL.GetSRSTxCyclicSCID()); scid <= SCID::End(); scid += iSpace  ) {

        //_vmSINR4SelectedRank[scid.ToInt()].set_size(_iRank + 1, 1);
        //_vmSINR4SelectedRank[scid.ToInt()] = itpp::zeros(_iRank + 1, 1);


        // 每一流都算一次 每个地方存储的是对应子载波的SINR
        _vmSINR4SelectedRank[scid.ToInt()/iSpace] =  CalcSINR_EBBCQI_AverageI(_msid,_iRank, scid, _vBestPMI[scid.ToInt()]);

//        for(int i=1;i<iSpace;i++){
//            if((scid.ToInt()+i)<=SCID::End().ToInt()){
//            _vmSINR4SelectedRank[scid.ToInt()+i] =_vmSINR4SelectedRank[scid.ToInt()] ;
//            }
//        }
    }
}
mat BTSVMIMORxer_trial_2::CalcSINR_EBBCQI_AverageI(const MSID& _msid,const int& _iRank, const SCID& _scid, const int& _iBestPMI) {
//    std::lock_guard<std::mutex>l(CalcSINR_EBBCQI_AverageIlock);
    BTS& MainServBTS = m_BTSID.GetBTS();
    MS& ms = _msid.GetMS();
    int iSpace = 4;
    iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);
    //获取有用信号
    cmat mS = ms.UL.GetTxSRS(_scid,_iRank + 1);

    cmat mH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(m_BTSID.GetBTS(), ms, _scid.ToInt()).H();

    cmat mP;
    // todo: if PMI_based_Precoding == true
    if (Parameters::Instance().MIMO_CTRL.PMI_based_Precoding == 1) {
//        mP = CodeBookLTE2TX::Instance().GetCodeWord(_iBestPMI, _iRank);

        mP = CodeBookFactory::Instance().GetCodeBook(ms.GetAntennaPointer()->GetActiveTXRU_Num())->GetCodeWord(_iBestPMI, _iRank);
    } else {
        //cwq dahuawu
        cmat mU, mD;
        vec vS;
        svd(mH, mU, vS, mD);
        mP = mD.get_cols(0, _iRank);
    }
    vector<cmat> vSIS, vSIH, vSIP, vWIS;
    vector<double> vWIL;
    for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
        MS& msI = msid.GetMS();
        if (find(m_pActiveSet->begin(), m_pActiveSet->end(), msI.GetID()) != m_pActiveSet->end()) {
            continue; //排除本小区的用户
        }
        //int IiRank = msI.ms_SCPrecodeMat[_scid].cols();
        bool flag = cm::LinkMatrix::Instance().IsStrong(MainServBTS, msI);
        if (flag) {
            //
            //int IiRank = m_mMSID2Rank[msid];
            cmat mSIS = msI.UL.GetTxSRS(_scid);
            vSIS.push_back(mSIS);
            // 获取强干扰信道
            cmat mSIH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(MainServBTS, msI, _scid.ToInt()).H();
            vSIH.push_back(mSIH);
            // 获取强干扰信号的预编码
            /*cmat mIU, mID,mSIP;
            vec vIS;
            svd(mSIH, mIU, vIS, mID);
            mSIP = mID.get_cols(0, IiRank);*/
            int IAntennnaNum=Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel * Parameters::Instance().MSS.FirstBand.Polarize_Num;
            cmat mSIP = 1 / sqrt(IAntennnaNum) * itpp::ones_c(IAntennnaNum, 1);
            vSIP.push_back(mSIP);
        } else {
            // 获取弱干扰的信号
            cmat mWIS = msI.UL.GetTxSRS(_scid);
            vWIS.push_back(mWIS);
            // 获取弱干扰的信道
            double dLinkLoss = cm::DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(MainServBTS, msI));
            vWIL.push_back(dLinkLoss);
        }
    }
    //@brief 未重计算时采用以下，要考虑欧拉
    //cout<<"Strong: "<<vSIH.size()<<" Weak: "<<vWIS.size()<<endl;
    mat mSINR = m_pDetector->CalculateSINR_UL2(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);

//    mSINR = m_pDetector->CalculateSINR_UL2Re(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL ,mSINR);

//    int iTime = Clock::Instance().GetTimeSlot();
//    if (iTime >=200 && iTime <=220)
//    {
//        mat mSINRe = m_pDetector->CalculateSINR_UL2(mS, mH, mP, vSIS, vSIH, vSIP, vWIS, vWIL);
//    }

    /*for (int i = 0; i < _iRank; i++) {
        //double dOLLAOffsetDB = m_dOLLAOffsetDB(_iRank, i);
        mSINR(i, 0) = mSINR(i, 0) * (*m_pRxBufferBTS)[_msid].m_dOLLAOffsetDB;
    }*/

    return mSINR;
}
double BTSVMIMORxer_trial_2::EstimateWholeBandCapacity_For_mSINR(vector<mat>& _vmSINR4SelectedRank,const MSID& msid) {
    LinkLevelInterface& lli = LinkLevelInterface::Instance_UL();
    itpp::imat mMCS = lli.SINR2MCS(_vmSINR4SelectedRank);
    double dCapacityKbit = lli.MCS2TBSKBit(mMCS, Parameters::Instance().BASIC.IRBNum);
    return dCapacityKbit;
}

void BTSVMIMORxer_trial_2::ComputeULGeometry(const std::shared_ptr<SchedulingMessageUL>& _pSchM) {

    if (!_pSchM)
        return;

    BTS& MainServBTS = m_BTSID.GetBTS();
    MSID msid = _pSchM->GetMSID();
    std::unordered_map<int, vector<MSID> > mRB2MS = _pSchM->GetRB2vMSID();

    MS& ms = msid.GetMS();
    int iSpace = 4;
    iSpace = max(4, Parameters::Instance().LINK_CTRL.IFrequencySpace);

    //校验调度信息的内容tsm11-23
    //assert(_pSchM->GetHARQID() == m_pHARQRxStateBTS->GetHARQID());

    double dTimeSec = Clock::Instance().GetTimeSec();
//    cm::LinkMatrix::Instance().WorkSlot(ms, dTimeSec);

    vector<RBID> vRBUsed = _pSchM->GetRBUsed();
    sort(vRBUsed.begin(), vRBUsed.end());
    vector<SCID> SCidUsed;
    for (const auto& rbid : vRBUsed) {
        for (SCID scid = rbid.GetFirstSCID(); scid <= rbid.GetLastSCID(); ++scid) {
            SCidUsed.push_back(scid);
        }
    }

    vector<double> vULGeometry(vRBUsed.size() * Parameters::Instance().BASIC.IRBSize / iSpace, 0);

    for(int SCidIndex = 0; SCidIndex < SCidUsed.size(); SCidIndex += iSpace){
        SCID scid = SCidUsed[SCidIndex];
        double dNoisePowerMw = Parameters::Instance().BTS_UL.UL.DSCNoisePowerMw; //处理热噪声

        cmat mS = ms.UL.GetTxDataSymbol(scid);
        cmat mH = cm::LinkMatrix::Instance().GetFadingMat_wABF_for_all_active_TXRU_Pairs(m_BTSID.GetBTS(), ms, scid.ToInt()).H();
        //服务小区的linkloss
        double dServer_LinkLoss = cm::DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(MainServBTS, ms));
        double dDesire_signal = real((mS.H() * mS)(0, 0)) * dServer_LinkLoss;
        //        cmat mP = 1 / sqrt(P.MSS.UL.IAntennaNum) * itpp::ones_c(P.MSS.UL.IAntennaNum, 1);
        ///计算干扰
        for (int i = 0; i < MSManager::Instance().CountMS(); ++i) {
            MS& msI = MSManager::Instance().GetMS(i);
            if (msI.GetID() == msid)
                continue;

            // 获取弱干扰的信号
            cmat mIS = msI.UL.GetTxDataSymbol(scid);

            //zhengyi different load
            //different load
            double drnd = random.xUniform_DiffLoad(0, 1);
            if (drnd >= Parameters::Instance().SIM_UL.UL.dLoad) {
                mIS = itpp::zeros_c(mIS.rows(), mIS.cols());
            }
            // different load end


            // 获取弱干扰的信道
            double dLinkLoss = cm::DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(MainServBTS, msI));
            double dIterferenceMW = real((mIS.H() * mIS)(0, 0)) * dLinkLoss;
            dNoisePowerMw += dIterferenceMW;

        }
        double dSINR_perSC = dDesire_signal / dNoisePowerMw;

        vULGeometry[SCidIndex / iSpace] = dSINR_perSC; //保持相对位置不变

    }

//    auto begin = vULGeometry.begin();
//    auto end = vULGeometry.end();
    Observer::SetIsEnable(true);
//    InterpolateLog(begin, end, iSpace, 0);

//    for (int Index = 0; Index < static_cast<int>( vULGeometry.size()); Index++) {
//        vULGeometry[Index] =  vULGeometry[Index / iSpace * iSpace];
//    }


    for(int SCidIndex = 0; SCidIndex < SCidUsed.size(); SCidIndex += iSpace){
        SCID scid = SCidUsed[SCidIndex];
        (*m_pRxBufferBTS)[msid.ToInt()].m_vULGeometry[scid.ToInt() / iSpace] = vULGeometry[SCidIndex / iSpace];
    }

}

//20220828 cwq
int BTSVMIMORxer_trial_2::GenerateTPCCommand_ByUser(const double& SINR, const double& PathLoss){
    return -1;
}
int BTSVMIMORxer_trial_2::GenerateTPCCommand(const MSID& msid, const double& SINR, const double& PathLoss){
    double targetsinr;
    string point;
    //close point
    if(PathLoss<=Parameters::Instance().MSS_UL.UL.LowTargetPL){
        targetsinr = Parameters::Instance().MSS_UL.UL.HighTargetSINR;
        point = "close";
    }
        //far point
    else if(PathLoss>=Parameters::Instance().MSS_UL.UL.HighTargetPL){
        targetsinr = Parameters::Instance().MSS_UL.UL.LowTargetSINR;
        point = "far";
    }
        //midddle point
    else{
        targetsinr = Parameters::Instance().MSS_UL.UL.MidTargetSINR;
        point = "mid";
    }

    int TPC;
    if(Parameters::Instance().MSS_UL.UL.TPC_Accumulation){
        TPC = Generate_AccumulationTCP(msid, SINR, targetsinr);
    }
    else{
        TPC =  Generate_absTPC(msid, SINR, targetsinr);
    }
    //hyl 冗余输出
//    {
//        BTSVMIMORxer_trial_2_mutex.lock();
//        Observer::Print("TPCRecord")<<Clock::Instance().GetTimeSlot()
//                                    <<setw(20)<<msid
//                                    <<setw(20)<<PathLoss
//                                    <<setw(20)<<SINR
//                                    <<setw(20)<<point
//                                    <<setw(20)<<TPC<<endl;
//        BTSVMIMORxer_trial_2_mutex.unlock();
//    }
    return TPC;
}
int BTSVMIMORxer_trial_2::Generate_AccumulationTCP(const MSID& msid, const double& SINR, const double& TargetSINR){
    int TPCCommand;
    //increase power
    if(SINR < TargetSINR - Parameters::Instance().MSS_UL.UL.TargetSINRMargin){
        if(MSID2HistoryTPC[msid.ToInt()].size() == 3){
            TPCCommand = 3;
            for(int i=0;i<MSID2HistoryTPC[msid.ToInt()].size();i++){
                if(MSID2HistoryTPC[msid.ToInt()][i]<=1){
                    TPCCommand = 2;
                    break;
                }
            }
            MSID2HistoryTPC[msid.ToInt()].pop_front();
        }
        else{
            TPCCommand = 2;
        }
        MSID2HistoryTPC[msid.ToInt()].push_back(TPCCommand);
    }
        //decrease power
    else if(SINR > TargetSINR + Parameters::Instance().MSS_UL.UL.TargetSINRMargin){
        TPCCommand = 0;
    }
        //keep power
    else{
        TPCCommand = 1;
    }
    return TPCCommand;
}
int BTSVMIMORxer_trial_2::Generate_absTPC(const MSID& msid, const double& SINR, const double& TargetSINR){
    int TPCCommand;
    //increase power
    if(SINR <= TargetSINR){
        if(MSID2HistoryTPC[msid.ToInt()].size() == 3){
            TPCCommand = 3;
            for(int i=0;i<MSID2HistoryTPC[msid.ToInt()].size();i++){
                if(MSID2HistoryTPC[msid.ToInt()][i]<=1){
                    TPCCommand = 2;
                    break;
                }
            }
            MSID2HistoryTPC[msid.ToInt()].pop_front();
        }
        else{
            TPCCommand = 2;
        }
        MSID2HistoryTPC[msid.ToInt()].push_back(TPCCommand);
    }
        //decrease power
    else{
        if(MSID2HistoryTPC[msid.ToInt()].size() == 3){
            TPCCommand = 0;
            for(int i=0;i<MSID2HistoryTPC[msid.ToInt()].size();i++){
                if(MSID2HistoryTPC[msid.ToInt()][i]>=2){
                    TPCCommand = 1;
                    break;
                }
            }
            MSID2HistoryTPC[msid.ToInt()].pop_front();
        }
        else{
            TPCCommand = 1;
        }
        MSID2HistoryTPC[msid.ToInt()].push_back(TPCCommand);
    }
    return TPCCommand;
}
void BTSVMIMORxer_trial_2::ReceivePHR(const std::shared_ptr<SchedulingMessageUL>& pSchM){
        BTS& bts = m_BTSID.GetBTS();
        MSID msid = pSchM->GetMSID();
        int iRBNum = pSchM->GetRBUsed().size();
        if(bts.UL.PhrUpdateFlag[msid.ToInt()]){
            int phr_index = bts.UL.MSID2PHR[msid.ToInt()];
            double phr = phr_index+0.5;
            double MaxTxPowerdBm = Parameters::Instance().MSS_UL.UL.DMaxTxPowerDbm;
            double EstimateTxPowerdBm = MaxTxPowerdBm - phr;
            double ActualTxPowerdBm = min(MaxTxPowerdBm, EstimateTxPowerdBm);
            bts.UL.MSID2RBNumSupported[msid.ToInt()] = floor(DB2L(MaxTxPowerdBm)/DB2L(ActualTxPowerdBm)*iRBNum);
            if(bts.UL.MSID2RBNumSupported[msid.ToInt()] == 0){
                bts.UL.MSID2RBNumSupported[msid.ToInt()] = 1;
            }
            bts.UL.PhrUpdateFlag[msid.ToInt()]=false;
        }
}

void BTSVMIMORxer_trial_2::RefreshHARQSINR(MSID msid, int harqID){
    m_pHARQRxState_BTS_UL->Refresh(msid, harqID);
}
