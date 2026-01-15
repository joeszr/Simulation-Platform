///@file MRCTxer.cpp
///@brief  MS MRC发射机类函数实现
///@author wangsen

#include "../Utility/IDType.h"
#include "../Utility/RBID.h"
#include "../Utility/SBID.h"
#include "../Utility/SCID.h"
#include "../Parameters/Parameters.h"
#include "../BaseStation/BTSID.h"
#include "../MobileStation/MSID.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../ChannelModel/AntennaOrientGain.h"
#include "../ChannelModel/Rx.h"
#include "../ChannelModel/WrapAround.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/ACKNAKMessageUL.h"
#include "../BaseStation/BS.h"
#include "../BaseStation/BTS.h"
#include "../NetworkDrive/Clock.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../Statistician/Statistician.h"
#include "../MobileStation/MS.h"
#include "../MobileStation/SoundingMessage.h"
#include "MSTxer.h"
#include "MSMRCTxer.h"
#include<boost/thread.hpp>
boost::mutex MSMRCTxer_mutex;

MSMRCTxer::MSMRCTxer(BTSID _MainServBTS, MSID _msid, std::shared_ptr<HARQTxStateMS> _pHARQTxStateMS) : MSTxer(_MainServBTS, _msid, _pHARQTxStateMS) {
    m_dSCDataTxPowerMW = Parameters::Instance().MSS_UL.UL.DMaxTxPowerMw; //这里为什么没有除以载波数？zhengyi
    m_dSCSRSTxPowerMW = Parameters::Instance().MSS_UL.UL.DMaxTxPowerMw / (Parameters::Instance().BASIC.IRBNum * Parameters::Instance().BASIC.IRBSize / 2); //这里先暂时设个初值，以防出错
    m_iRBnumTx = 1; //初始化用户最大可以支持的RB个数
}

void MSMRCTxer::WorkSlot() {
    //检查scheduler队列，看pop出来的信息是否满足延时
    GetSchedulingMsg();

    bool HaveSchM = (m_pCurrentSchM != 0 && m_pCurrentSchM->GetSendNum() == 0);

    if(HaveSchM)
    {
        PowerControl();
        double TBSize = m_pCurrentSchM->GetTBSizeKbit();
        m_MSID.GetMS().m_clsMacMs.ReduceBuffer(TBSize);
    }
    m_MSID.GetMS().m_clsMacMs.WorkSlot(HaveSchM);
    sr.WorkSlot(HaveSchM);
}

void MSMRCTxer::GetSchedulingMsg() {
//    if (m_pHARQTxState_MS_UL->IsNeedReTx()) { //如果需要重传
//        m_pCurrentSchM = m_pHARQTxState_MS_UL->GetReTxMessage()->GetSchMessage();
//    } else {
//        m_pCurrentSchM = ReceiveSchedulingMessage();
//    }
    m_pCurrentSchM = ReceiveSchedulingMessage();
}

void MSMRCTxer::PowerControl4SRS() {
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    BTS& ServBTS = m_MainServBTSID.GetBTS();
    MS& ms = m_MSID.GetMS();
    double u = Parameters::Instance().BASIC.DCarrierWidthHz/15000;
    double dRBSRSTxPowerMW = u * DB2L(Parameters::Instance().MSS_UL.UL.DP0Dbm4SRS) * DB2L(-1 * Parameters::Instance().MSS_UL.UL.DAlfa4SRS * lm.GetCouplingLossDB(ServBTS, ms));
    double dTotalSRSTxPowerMW = dRBSRSTxPowerMW * Parameters::Instance().BASIC.IRBNum / m_iSRSTxSpace;

    //    double dTotalSRSTxPowerMW = 400;

    dTotalSRSTxPowerMW = std::min(DB2L(Parameters::Instance().MSS_UL.UL.DMaxTxPowerDbm), dTotalSRSTxPowerMW);
    int iSRSSCNum = Parameters::Instance().BASIC.ISCNum / m_iSRSTxSpace;
    m_dSCSRSTxPowerMW = dTotalSRSTxPowerMW / iSRSSCNum;
}
void MSMRCTxer::PowerControl() {
    if (Clock::Instance().GetTimeSlot() % Parameters::Instance().MSS_UL.UL.ISRS_PERIOD == Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot - Parameters::Instance().SIM.FrameStructure4Sim.P1.IULSlot + 1) {
        PowerControl4SRS();
    }
    int iRBNum = m_pCurrentSchM->GetRBUsed().size();
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    BTS& ServBTS = m_MainServBTSID.GetBTS();
    MS& ms = m_MSID.GetMS();
    double u = Parameters::Instance().BASIC.DCarrierWidthHz/15000;
//    double dRBTxPowerMw = u * DB2L(Parameters::Instance().MSS_UL.UL.DP0Dbm) * DB2L((-1) * Parameters::Instance().MSS_UL.UL.DAlfa * lm.GetCouplingLossDB(ServBTS, ms));
    double dTxPowerMw = u * iRBNum * DB2L(Parameters::Instance().MSS_UL.UL.DP0Dbm) * DB2L((-1) * Parameters::Instance().MSS_UL.UL.DAlfa * lm.GetCouplingLossDB(ServBTS, ms));

    //20220828 cwq 闭环功控
    double f = 0;
    if(Parameters::Instance().MSS_UL.UL.CLPCofPUSCH)
    {
        f = Generate_f();
        dTxPowerMw *= DB2L(f);

        if(Parameters::Instance().MSS_UL.UL.ResetKey)
        {
            if(Clock::Instance().GetTimeSlot() % Parameters::Instance().MSS_UL.UL.ResetPeriod == 0){
                ResetTPC();
            }
        }
    }
    double PathLoss = lm.GetCouplingLossDB(ServBTS, ms);
    dTxPowerMw = max(dTxPowerMw, DB2L(Parameters::Instance().MSS_UL.UL.DMinTxPowerDbm));
    bool newResource = false;
//    phr.timerRun();
    if (m_pCurrentSchM != 0) {
        assert(m_MSID == m_pCurrentSchM->GetMSID());
        double dActualPowerTxMw = min(Parameters::Instance().MSS_UL.UL.DMaxTxPowerMw, dTxPowerMw);
        double EstimateTxPower_dBm = L2DB(dTxPowerMw);
        double MaxTxPower_dBm = Parameters::Instance().MSS_UL.UL.DMaxTxPowerDbm;

        if(m_pCurrentSchM->GetSendNum()<1){
            newResource = true;
        }
        phr.Workslot(MaxTxPower_dBm, EstimateTxPower_dBm, L2DB(dActualPowerTxMw), PathLoss, newResource);

        //zhengyi 用来记录用户可以支持的RB数目
        m_iRBnumTx = floor(Parameters::Instance().MSS_UL.UL.DMaxTxPowerMw / dActualPowerTxMw * iRBNum);
        //遗留问题，=4是不是有问题？
        if (m_iRBnumTx < 4) {
            m_iRBnumTx = 4;
        }
        LastTimeTxPowermW = dActualPowerTxMw;
        //        if (false) {
        //            dTotalPowerTxMw = max(DB2L(Parameters::Instance().MSS_UL.UL.DMinTxPowerDbm), dTotalPowerTxMw);
        //        }
        m_dSCDataTxPowerMW = dActualPowerTxMw / (iRBNum * Parameters::Instance().BASIC.IRBSize);

        if(Clock::Instance().GetTimeSlot() >= Parameters::Instance().BASIC.IWarmUpSlot)
        {
            Statistician::Instance().m_MSData_UL[m_MSID.ToInt()].m_dTxPowerMw += dActualPowerTxMw; //用户的发射功率
            Statistician::Instance().m_MSData_UL[m_MSID.ToInt()].m_dTxPowerMwIndex++;
        }
        int iTime = Clock::Instance().GetTimeSlot();
        {
            boost::mutex::scoped_lock lock(MSMRCTxer_mutex);
            //用来记录整个仿真中的用户发射功率，用于CDF
            Observer::SetIsEnable(true);
            //hyl 冗余
//            Observer::Print("watch_TxPowerMw") << dTotalPowerTxMw << endl;
            Observer::Print("TxPowerRecord")<<iTime
                                            <<setw(20)<<m_MainServBTSID
                                            <<setw(20)<<m_MSID
                                            <<setw(20)<<L2DB(dActualPowerTxMw)
                                            <<setw(20)<<f;
            Observer::Print("TxPowerRecord")<<endl;
        }
    }
}
//void MSMRCTxer::PowerControl() {
//    if (Clock::Instance().GetTimeSlot() % Parameters::Instance().MSS_UL.UL.ISRS_PERIOD == Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot - Parameters::Instance().SIM.FrameStructure4Sim.P1.IULSlot + 1) {
//        PowerControl4SRS();
//    }
//    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
//    BTS& ServBTS = m_MainServBTSID.GetBTS();
//    MS& ms = m_MSID.GetMS();
//    double u = Parameters::Instance().BASIC.DCarrierWidthHz/15000;
////    double dRBTxPowerMw = u * DB2L(Parameters::Instance().MSS_UL.UL.DP0Dbm) * DB2L((-1) * Parameters::Instance().MSS_UL.UL.DAlfa * lm.GetCouplingLossDB(ServBTS, ms));
//    double dRBTxPowerMw = u *  DB2L(Parameters::Instance().MSS_UL.UL.DP0Dbm) * DB2L((-1) * Parameters::Instance().MSS_UL.UL.DAlfa * lm.GetCouplingLossDB(ServBTS, ms));
//
//    //20220828 cwq 闭环功控
//    double f = 0;
//    if(Parameters::Instance().MSS_UL.UL.CLPCofPUSCH)
//    {
//        f = Generate_f();
//        dRBTxPowerMw *= DB2L(f);
//
//        if(Parameters::Instance().MSS_UL.UL.ResetKey)
//        {
//            if(Clock::Instance().GetTimeSlot() % Parameters::Instance().MSS_UL.UL.ResetPeriod == 0){
//                ResetTPC();
//            }
//        }
//    }
//    double EstimateTxPower_dBm = L2DB(dRBTxPowerMw);
//    double MaxTxPower_dBm;
//    double PathLoss = lm.GetCouplingLossDB(ServBTS, ms);
//
//    //    cout << dRBTxPowerMw<< endl;
//    dRBTxPowerMw = max(dRBTxPowerMw, DB2L(Parameters::Instance().MSS_UL.UL.DMinTxPowerDbm) / Parameters::Instance().BASIC.IRBNum);
//    //    cout << dRBTxPowerMw << endl;
//    //zhengyi 用来记录用户可以支持的RB数目
//    m_iRBnumTx = floor(Parameters::Instance().MSS_UL.UL.DMaxTxPowerMw / dRBTxPowerMw);
//    //遗留问题，=4是不是有问题？
////    if (m_iRBnumTx == 0) {
////        m_iRBnumTx = 1;
////    }
//    //180328 验证
//    if (m_iRBnumTx < 4) {
//        m_iRBnumTx = 4;
//    }
//    if(Clock::Instance().GetTimeSlot() == Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot - Parameters::Instance().SIM.FrameStructure4Sim.P1.IULSlot + 1){
//        ServBTS.UL.MSID2RBNumSupported[m_MSID.ToInt()] = m_iRBnumTx;
//    }
//    bool newResource = false;
////    phr.timerRun();
//    if (m_pCurrentSchM != 0) {
//        assert(m_MSID == m_pCurrentSchM->GetMSID());
//        vector<RBID> vRBUsed = m_pCurrentSchM->GetRBUsed();
//        int iRBNum = static_cast<int> (vRBUsed.size());
//        double dTotalPowerTxMw = dRBTxPowerMw * iRBNum;
//
//        MaxTxPower_dBm = L2DB(Parameters::Instance().MSS_UL.UL.DMaxTxPowerMw / iRBNum);
//
//        if(m_pCurrentSchM->GetSendNum()<1){
//            newResource = true;
//        }
//        phr.Workslot(MaxTxPower_dBm, EstimateTxPower_dBm, L2DB(dTotalPowerTxMw), PathLoss, newResource);
//
//        //        double dTotalPowerTxMw = 400;
//        dTotalPowerTxMw = std::min(DB2L(Parameters::Instance().MSS_UL.UL.DMaxTxPowerDbm), dTotalPowerTxMw);
//        LastTimeTxPowermW = dTotalPowerTxMw;
//        //        if (false) {
//        //            dTotalPowerTxMw = max(DB2L(Parameters::Instance().MSS_UL.UL.DMinTxPowerDbm), dTotalPowerTxMw);
//        //        }
//        m_dSCDataTxPowerMW = dTotalPowerTxMw / (iRBNum * Parameters::Instance().BASIC.IRBSize);
//
//        if(Clock::Instance().GetTimeSlot() >= Parameters::Instance().BASIC.IWarmUpSlot)
//        {
//            Statistician::Instance().m_MSData_UL[m_MSID.ToInt()].m_dTxPowerMw += dTotalPowerTxMw; //用户的发射功率
//            Statistician::Instance().m_MSData_UL[m_MSID.ToInt()].m_dTxPowerMwIndex++;
//        }
//        int iTime = Clock::Instance().GetTimeSlot();
//        {
//            boost::mutex::scoped_lock lock(MSMRCTxer_mutex);
//            //用来记录整个仿真中的用户发射功率，用于CDF
//            Observer::SetIsEnable(true);
//            //hyl 冗余
////            Observer::Print("watch_TxPowerMw") << dTotalPowerTxMw << endl;
//            Observer::Print("TxPowerRecord")<<iTime
//                                            <<setw(20)<<m_MainServBTSID
//                                            <<setw(20)<<m_MSID
//                                            <<setw(20)<<L2DB(dTotalPowerTxMw)
//                                            <<setw(20)<<f;
//            Observer::Print("TxPowerRecord")<<endl;
//        }
//    }
//}

cmat MSMRCTxer::GetTxSRS(SCID _scid) {
    if (_scid.ToInt() % m_iSRSTxSpace == m_iSRSTxCyclicSCID)
        return sqrt(m_dSCSRSTxPowerMW) * itpp::ones_c(1, 1);
    else
        return itpp::zeros_c(1, 1);
}
cmat MSMRCTxer::GetTxSRS(SCID _scid,int _iPortNum) {
    if (_scid.ToInt() % m_iSRSTxSpace == m_iSRSTxCyclicSCID)
        return sqrt(m_dSCSRSTxPowerMW/_iPortNum) * itpp::ones_c(_iPortNum, 1);
    else
        return itpp::zeros_c(_iPortNum, 1);
}

cmat MSMRCTxer::GetTxSRS() {
    assert(false);
    return itpp::zeros_c(1, 1);
}

cmat MSMRCTxer::GetLastTxP() {
    return sqrt(m_dSCSRSTxPowerMW) * itpp::ones_c(1, 1);
}

cmat MSMRCTxer::GetTxDataSymbol(SCID _scid) {
    //    if (m_pCurrentSchM == 0 && Clock::Instance().GetTimeSlot() > 6) {
    //        if (Parameters::Instance().SIM_UL.UL.IMIMOMode != Parameters::IMIMOMode_VirtualMIMO) //vMIMO的情况下是会有用户不被调度的
    //        {
    //            //            assert(false);
    //        }
    //    }
    //    if (Parameters::Instance().SIM_UL.UL.IMIMOMode != Parameters::IMIMOMode_VirtualMIMO && Clock::Instance().GetTimeSlot() > 6) {
    //        //        assert(m_pCurrentSchM != 0);
    //    }
    if (m_pCurrentSchM == 0) {
        return itpp::zeros_c(1, 1);
    } else {
        vector<RBID>& vRBUsed = m_pCurrentSchM->GetRBUsed();
        if (find(vRBUsed.begin(), vRBUsed.end(), _scid.GetRBID()) != vRBUsed.end()) {
            return sqrt(m_dSCDataTxPowerMW) * itpp::ones_c(1, 1);
        } else {
            return itpp::zeros_c(1, 1);
        }
    }



    //    vector<RBID> vRBUsed;
    //    if (m_pCurrentSchM != 0) {
    //        vRBUsed = m_pCurrentSchM->GetRBUsed();
    //    } else {
    //        
    //    }
    //    if (find(vRBUsed.begin(), vRBUsed.end(), _scid.GetRBID()) != vRBUsed.end()) {
    //        return sqrt(m_dSCDataTxPowerMW) * itpp::ones_c(1, 1);
    //    } else {
    //        return itpp::zeros_c(1, 1);
    //    }
}
cmat MSMRCTxer::GetTxDataSymbol(SCID _scid,int iPortNum) {
    if (m_pCurrentSchM == 0) {
        //cout<<"schedule msg is null!"<<endl;
        return itpp::zeros_c(iPortNum, 1);
    } else {
//        if(Clock::Instance().GetTimeSlot()==40 &&this->m_MSID.ToInt()==15){
//            cout<<m_pCurrentSchM->GetBornTime()<<endl;
//        }
        vector<RBID>& vRBUsed = m_pCurrentSchM->GetRBUsed();
//        cout<<"_scid.GetRBID()= "<<_scid.GetRBID()<<endl;
        if (find(vRBUsed.begin(), vRBUsed.end(), _scid.GetRBID()) != vRBUsed.end()) {
            //cout<<"ms is not null!"<<endl;
            return sqrt(m_dSCDataTxPowerMW) * itpp::ones_c(iPortNum, 1);
        } else {
            //cout<<"ms is null!"<<endl;
            return itpp::zeros_c(iPortNum, 1);
        }
    }

}

//获得用户功率当前可支持调度的RB个数

int MSMRCTxer::GetRBNumSupport() {
    return m_iRBnumTx;
}