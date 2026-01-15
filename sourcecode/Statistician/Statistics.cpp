//
// Created by LAI on 2023/9/7.
//

#include "../Parameters/Parameters.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../BaseStation/BTSID.h"
#include "../MobileStation/MSID.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../Scheduler/SchedulerDL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/BTS.h"
#include "../BaseStation/BS.h"
#include "Directory.h"
#include "Statistician.h"
#include<numeric>
#include "Statistics.h"

/***** QPSKRate得取决于不同的Table，大话务平台实现了多种Table;需要重新设计。以及，MIMO部分暂时没有实现    *****/
Statistics::Statistics() {
    ///移动台移动次数
    iMoveTimes = -1;

//    boost::filesystem::path filename;
//    filename = Directory::Instance().GetPath()

}

void Statistics::Initialize() {
    //清除已有的数据
    Clear();
    Reset();
}

void Statistics::Reset(){

    fBTS.close();
//    fBTS.clear();

    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        int BTS2TxID = btsid.GetBTS().GetTxID();
//        cout << btsid.GetBTS().GetTxID() << endl;
        ResetPara(BTS2TxID, btsid);
    }
}

void Statistics::Clear() {

    ///小区RRC Connect态用户数
    m_BS_RRCConnect_MSNum.clear();
    ///%%小区初始新接入用户数
    m_BS_Initial_MSNum.clear();
    ///%%小区重选入用户数
    m_BS_CellReselection_MSNum.clear();
    ///%%小区切换入用户数
    m_BS_HandoffIn_MSNum.clear();
    ///%%小区切换出用户数
    m_BS_HandoffOut_MSNum.clear();

    ///小区上行激活用户数（统计周期内有调度的用户数累计值）
    m_BS_UL_Active_MSNum.clear();
    m_BS_UL_Active_vMSID.clear();
    ///小区下行激活用户数
    m_BS_DL_Active_MSNum.clear();
    m_BS_DL_Active_vMSID.clear();

    ///小区上行PRB利用率
    m_BS_UL_PRB_Rate.clear();
    m_BS_UL_PRB_Rate_Total.clear();
    m_BS_UL_PRB_Rate_Use.clear();
    ///小区下行PRB利用率
    m_BS_DL_PRB_Rate.clear();
    m_BS_DL_PRB_Rate_Total.clear();
    m_BS_DL_PRB_Rate_Use.clear();

    ///小区上行PDCCH CCE利用率（上行调度使用的CCE数/上行可用的CCE总数，当无其他限制条件时，上行可用CCE总数为所有可用CCE；当限制上行CCE占比时，则根据配置计算上行可用CCE总数）
    m_BS_UL_PDCCHCCE_Rate.clear();
    m_BS_UL_PDCCHCCE_Rate_Total.clear();
    m_BS_UL_PDCCHCCE_Rate_Use.clear();
    ///小区下行PDCCH CCE利用率（下行调度使用的CCE数/下行可用的CCE总数，当无其他限制条件时，下行可用CCE总数为所有可用CCE；当限制下行CCE占比时，则根据配置计算下行可用CCE总数）
    m_BS_DL_PDCCHCCE_Rate.clear();
    m_BS_DL_PDCCHCCE_Rate_Total.clear();
    m_BS_DL_PDCCHCCE_Rate_Use.clear();

    ///%%小区上行平均PDCCH CCE
    m_BS_UL_PDCCHCCE.clear();
    m_BS_UL_PDCCHCCE_Times.clear();
    ///%%小区下行平均PDCCH CCE
    m_BS_DL_PDCCHCCE.clear();
    m_BS_DL_PDCCHCCE_Times.clear();

    ///小区上行PDCCH CCE分配失败比例（PDCCH中用于上行调度的CCE分配失败总次数占比：分配失败次数为用户CCE侯选位置均被占用导致分不出CCE）
    m_BS_UL_PDCCHCCEAllocate_FailureRate.clear();
    m_BS_UL_PDCCHCCEAllocate_FailureRate_Total.clear();
    m_BS_UL_PDCCHCCEAllocate_FailureRate_Use.clear();
    ///小区下行PDCCH CCE分配失败比例（PDCCH中用于下行调度的CCE分配失败总次数占比：分配失败次数为用户CCE侯选位置均被占用导致分不出CCE）
    m_BS_DL_PDCCHCCEAllocate_FailureRate.clear();
    m_BS_DL_PDCCHCCEAllocate_FailureRate_Total.clear();
    m_BS_DL_PDCCHCCEAllocate_FailureRate_Use.clear();

    ///小区上行平均MCS
    m_BS_UL_MCS.clear();
    m_BS_UL_MCS_Times.clear();
    ///小区下行平均MCS
    m_BS_DL_MCS.clear();
    m_BS_DL_MCS_Times.clear();

    ///小区上行平均CQI
    m_BS_UL_CQI.clear();
    m_BS_UL_CQI_Times.clear();
    ///小区下行平均CQI
    m_BS_DL_CQI.clear();
    m_BS_DL_CQI_Times.clear();

    ///%%小区上行MIMO配对平均PRB数
    m_BS_UL_MIMO_PRBNum.clear();
    m_BS_UL_MIMO_PairTimes.clear();
    ///小区上行MIMO配对平均层数（配对用户数累计值/配对总次数）
    m_BS_UL_MIMO_Layer.clear();
    ///%%小区下行MIMO配对平均PRB数
    m_BS_DL_MIMO_PRBNum.clear();
    m_BS_DL_MIMO_PairTimes.clear();
    ///小区下行MIMO配对平均层数（配对用户数累计值/配对总次数）
    m_BS_DL_MIMO_Layer.clear();

//    ///小区上行业务量
//    m_BS_UL_Throughput.clear();
//    ///小区下行业务量
//    m_BS_DL_Throughput.clear();
    ///小区上行业务量
    m_BS_UL_Traffic.clear();
    ///小区下行业务量
    m_BS_DL_Traffic.clear();


    ///平台采用MCS Index Table 2 PDSCH; MCS小于等于4时采用QPSK调制
    ///大话务支持MCS Index Table 1 PDSCH;MCS小于等于9时采用QPSK调制   Parameters::Instance().DaHuaWu.bDl256QamSwitch == false
    ///小区上行QPSK编码比例（上行QPSK调制的初始TB数和统计周期内传输的总上行初始TB数之比表示）
    m_BS_UL_QPSK_Rate.clear();
    m_BS_UL_QPSK_Rate_Total.clear();
    m_BS_UL_QPSK_Rate_Used.clear();
    ///平台采用MCS Index Table 1 PUSCH; MCS小于等于9时采用QPSK调制
    ///小区下行QPSK编码比例（下行QPSK调制的初始TB数和统计周期内传输的总下行初始TB数之比表示）
    m_BS_DL_QPSK_Rate.clear();
    m_BS_DL_QPSK_Rate_Total.clear();
    m_BS_DL_QPSK_Rate_Used.clear();

    ///上行HARQ重传比例（UL-SCH上发生HARQ重传的TB数量与UL-SCH上传输的总的TB数量之比表示）
    m_BS_UL_HARQ_Rate.clear();
    m_BS_UL_HARQ_Total.clear();
    m_BS_UL_HARQ_Used.clear();
    ///下行HARQ重传比例（DL-SCH上发生HARQ重传的TB数量与DL-SCH上传输的总的TB数量之比表示）
    m_BS_DL_HARQ_Rate.clear();
    m_BS_DL_HARQ_Total.clear();
    m_BS_DL_HARQ_Used.clear();
}

void Statistics::ResetPara(int iTxID, BTSID& _btsid){

    ///小区RRC Connect态用户数
    m_BS_RRCConnect_MSNum[iTxID] = 0;
    ///%%小区初始新接入用户数
//    m_BS_Initial_MSNum[iTxID] = 0;
    ///%%小区重选入用户数
    m_BS_CellReselection_MSNum[iTxID] = 0;
    ///%%小区切换入用户数
    m_BS_HandoffIn_MSNum[iTxID] = 0;
    ///%%小区切换出用户数
    m_BS_HandoffOut_MSNum[iTxID] = 0;

    ///小区上行激活用户数（统计周期内有调度的用户数累计值）
    m_BS_UL_Active_MSNum[iTxID] = 0;
    m_BS_UL_Active_vMSID.clear();
    ///小区下行激活用户数
    m_BS_DL_Active_MSNum[iTxID] = 0;
    m_BS_DL_Active_vMSID.clear();

    ///小区上行PRB利用率
    m_BS_UL_PRB_Rate[iTxID] = 0;
    m_BS_UL_PRB_Rate_Total[iTxID] = 0;
    m_BS_UL_PRB_Rate_Use[iTxID] = 0;
    ///小区下行PRB利用率
    m_BS_DL_PRB_Rate[iTxID] = 0;
    m_BS_DL_PRB_Rate_Total[iTxID] = 0;
    m_BS_DL_PRB_Rate_Use[iTxID] = 0;

    ///小区上行PDCCH CCE利用率（上行调度使用的CCE数/上行可用的CCE总数，当无其他限制条件时，上行可用CCE总数为所有可用CCE；当限制上行CCE占比时，则根据配置计算上行可用CCE总数）
    m_BS_UL_PDCCHCCE_Rate[iTxID] = 0;
    m_BS_UL_PDCCHCCE_Rate_Total[iTxID] = 0;
    m_BS_UL_PDCCHCCE_Rate_Use[iTxID] = 0;
    ///小区下行PDCCH CCE利用率（下行调度使用的CCE数/下行可用的CCE总数，当无其他限制条件时，下行可用CCE总数为所有可用CCE；当限制下行CCE占比时，则根据配置计算下行可用CCE总数）
    m_BS_DL_PDCCHCCE_Rate[iTxID] = 0;
    m_BS_DL_PDCCHCCE_Rate_Total[iTxID] = 0;
    m_BS_DL_PDCCHCCE_Rate_Use[iTxID] = 0;

    ///%%小区上行平均PDCCH CCE
    m_BS_UL_PDCCHCCE[iTxID] = 0;
    m_BS_UL_PDCCHCCE_Times[iTxID] = 0;
    ///%%小区下行平均PDCCH CCE
    m_BS_DL_PDCCHCCE[iTxID] = 0;
    m_BS_DL_PDCCHCCE_Times[iTxID] = 0;

    ///小区上行PDCCH CCE分配失败比例（PDCCH中用于上行调度的CCE分配失败总次数占比：分配失败次数为用户CCE侯选位置均被占用导致分不出CCE）
    m_BS_UL_PDCCHCCEAllocate_FailureRate[iTxID] = 0;
    m_BS_UL_PDCCHCCEAllocate_FailureRate_Total[iTxID] = 0;
    m_BS_UL_PDCCHCCEAllocate_FailureRate_Use[iTxID] = 0;
    ///小区下行PDCCH CCE分配失败比例（PDCCH中用于下行调度的CCE分配失败总次数占比：分配失败次数为用户CCE侯选位置均被占用导致分不出CCE）
    m_BS_DL_PDCCHCCEAllocate_FailureRate[iTxID] = 0;
    m_BS_DL_PDCCHCCEAllocate_FailureRate_Total[iTxID] = 0;
    m_BS_DL_PDCCHCCEAllocate_FailureRate_Use[iTxID] = 0;

    ///小区上行平均MCS
    m_BS_UL_MCS[iTxID] = 0;
    m_BS_UL_MCS_Times[iTxID] = 0;
    ///小区下行平均MCS
    m_BS_DL_MCS[iTxID] = 0;
    m_BS_DL_MCS_Times[iTxID] = 0;

    ///小区上行平均CQI
    m_BS_UL_CQI[iTxID] = 0;
    m_BS_UL_CQI_Times[iTxID] = 0;
    ///小区下行平均CQI
    m_BS_DL_CQI[iTxID] = 0;
    m_BS_DL_CQI_Times[iTxID] = 0;

    ///%%小区上行MIMO配对平均PRB数
    m_BS_UL_MIMO_PRBNum[iTxID] = 0;
    m_BS_UL_MIMO_PairTimes[iTxID] = 0;
    ///小区上行MIMO配对平均层数（配对用户数累计值/配对总次数）
    m_BS_UL_MIMO_Layer[iTxID] = 0;
    ///%%小区下行MIMO配对平均PRB数
    m_BS_DL_MIMO_PRBNum[iTxID] = 0;
    m_BS_DL_MIMO_PairTimes[iTxID] = 0;
    ///小区下行MIMO配对平均层数（配对用户数累计值/配对总次数）
    m_BS_DL_MIMO_Layer[iTxID] = 0;

//    ///小区上行业务量
//    m_BS_UL_Throughput[iTxID] = 0;
//    ///小区下行业务量
//    m_BS_DL_Throughput[iTxID] = 0;
    ///小区上行业务量
    m_BS_UL_Traffic[iTxID] = 0;
    ///小区下行业务量
    m_BS_DL_Traffic[iTxID] = 0;

    ///平台采用MCS Index Table 2 PDSCH; MCS小于等于4时采用QPSK调制
    ///大话务支持MCS Index Table 1 PDSCH;MCS小于等于9时采用QPSK调制   Parameters::Instance().DaHuaWu.bDl256QamSwitch == false
    ///小区上行QPSK编码比例（上行QPSK调制的初始TB数和统计周期内传输的总上行初始TB数之比表示）
    m_BS_UL_QPSK_Rate[iTxID] = 0;
    m_BS_UL_QPSK_Rate_Total[iTxID] = 0;
    m_BS_UL_QPSK_Rate_Used[iTxID] = 0;
    ///平台采用MCS Index Table 1 PUSCH; MCS小于等于9时采用QPSK调制
    ///小区下行QPSK编码比例（下行QPSK调制的初始TB数和统计周期内传输的总下行初始TB数之比表示）
    m_BS_DL_QPSK_Rate[iTxID] = 0;
    m_BS_DL_QPSK_Rate_Total[iTxID] = 0;
    m_BS_DL_QPSK_Rate_Used[iTxID] = 0;

    ///上行HARQ重传比例（UL-SCH上发生HARQ重传的TB数量与UL-SCH上传输的总的TB数量之比表示）
    m_BS_UL_HARQ_Rate[iTxID] = 0;
    m_BS_UL_HARQ_Total[iTxID] = 0;
    m_BS_UL_HARQ_Used[iTxID] = 0;
    ///下行HARQ重传比例（DL-SCH上发生HARQ重传的TB数量与DL-SCH上传输的总的TB数量之比表示）
    m_BS_DL_HARQ_Rate[iTxID] = 0;
    m_BS_DL_HARQ_Total[iTxID] = 0;
    m_BS_DL_HARQ_Used[iTxID] = 0;
}

/// 实现在输出数据前的数据预处理工作的函数
void Statistics::PreProcess() {
    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        int BTS2TxID = btsid.GetBTS().GetTxID();
        {
            PreProcessPara(BTS2TxID, btsid);
        }
    }
}

/*****  预处理统计信息:其中无法取值的值全部设为 0   *****/
void Statistics::PreProcessPara(int iTxID, BTSID& _btsid) {

    ///小区RRC Connect态用户数
    m_BS_RRCConnect_MSNum[iTxID] = _btsid.GetBTS().GetActiveSet().size();

    ///小区上行激活用户数（统计周期内有调度的用户数累计值）
    m_BS_UL_Active_MSNum[iTxID] = m_BS_UL_Active_vMSID[iTxID].size();
    ///小区下行激活用户数
    m_BS_DL_Active_MSNum[iTxID] = m_BS_DL_Active_vMSID[iTxID].size();

    ///小区上行PRB利用率
    if(m_BS_UL_PRB_Rate_Total[iTxID] != 0){
        m_BS_UL_PRB_Rate[iTxID] = (m_BS_UL_PRB_Rate_Use[iTxID] / m_BS_UL_PRB_Rate_Total[iTxID]);
    }else{
        m_BS_UL_PRB_Rate[iTxID] = 0;
    }
    ///小区下行PRB利用率
    if(m_BS_DL_PRB_Rate_Total[iTxID] != 0){
        m_BS_DL_PRB_Rate[iTxID] = m_BS_DL_PRB_Rate_Use[iTxID] / m_BS_DL_PRB_Rate_Total[iTxID];
    }else{
        m_BS_DL_PRB_Rate[iTxID] = 0;
    }

    ///小区上行PDCCH CCE利用率（上行调度使用的CCE数/上行可用的CCE总数，当无其他限制条件时，上行可用CCE总数为所有可用CCE；当限制上行CCE占比时，则根据配置计算上行可用CCE总数）
    if(m_BS_UL_PDCCHCCE_Rate_Total[iTxID] != 0){
        m_BS_UL_PDCCHCCE_Rate[iTxID] = m_BS_UL_PDCCHCCE_Rate_Use[iTxID] / m_BS_UL_PDCCHCCE_Rate_Total[iTxID];
    }else{
        m_BS_UL_PDCCHCCE_Rate[iTxID] = 0;
    }
    ///小区下行PDCCH CCE利用率（下行调度使用的CCE数/下行可用的CCE总数，当无其他限制条件时，下行可用CCE总数为所有可用CCE；当限制下行CCE占比时，则根据配置计算下行可用CCE总数）
    if(m_BS_DL_PDCCHCCE_Rate_Total[iTxID] != 0){
        m_BS_DL_PDCCHCCE_Rate[iTxID] = m_BS_DL_PDCCHCCE_Rate_Use[iTxID] / m_BS_DL_PDCCHCCE_Rate_Total[iTxID];
    }else{
        m_BS_DL_PDCCHCCE_Rate[iTxID] = 0;
    }

//    cout << "m_BS_UL_PDCCHCCE_Rate_Use[ " << iTxID << " ]= " <<  m_BS_UL_PDCCHCCE_Rate_Use[iTxID] << endl;
//    cout << "m_BS_UL_PDCCHCCE_Rate_Total[ " << iTxID << " ]= " <<  m_BS_UL_PDCCHCCE_Rate_Total[iTxID] << endl;
//    cout << "m_BS_UL_PDCCHCCE_Rate[ " << iTxID << " ]= " <<  m_BS_UL_PDCCHCCE_Rate[iTxID] << endl;
//
//    cout << "m_BS_DL_PDCCHCCE_Rate_Use[ " << iTxID << " ]= " <<  m_BS_DL_PDCCHCCE_Rate_Use[iTxID] << endl;
//    cout << "m_BS_DL_PDCCHCCE_Rate_Total[ " << iTxID << " ]= " <<  m_BS_DL_PDCCHCCE_Rate_Total[iTxID] << endl;
//    cout << "m_BS_DL_PDCCHCCE_Rate[ " << iTxID << " ]= " <<  m_BS_DL_PDCCHCCE_Rate[iTxID] << endl;

    ///%%小区上行平均PDCCH CCE
    if(m_BS_UL_PDCCHCCE_Times[iTxID] != 0){
        m_BS_UL_PDCCHCCE[iTxID] /= m_BS_UL_PDCCHCCE_Times[iTxID];
    }else{
        m_BS_UL_PDCCHCCE[iTxID] = 0;
    }
    ///%%小区下行平均PDCCH CCE
    if(m_BS_DL_PDCCHCCE_Times[iTxID] != 0){
        m_BS_DL_PDCCHCCE[iTxID] /= m_BS_DL_PDCCHCCE_Times[iTxID];
    }else{
        m_BS_DL_PDCCHCCE[iTxID] = 0;
    }

//    cout << "m_BS_UL_PDCCHCCE_Times[ " << iTxID << " ]= " <<  m_BS_UL_PDCCHCCE_Times[iTxID] << endl;
//    cout << "m_BS_UL_PDCCHCCE[ " << iTxID << " ]= " <<  m_BS_UL_PDCCHCCE[iTxID] << endl;
//
//    cout << "m_BS_DL_PDCCHCCE_Times[ " << iTxID << " ]= " <<  m_BS_DL_PDCCHCCE_Times[iTxID] << endl;
//    cout << "m_BS_DL_PDCCHCCE[ " << iTxID << " ]= " <<  m_BS_DL_PDCCHCCE[iTxID] << endl;

    ///小区上行PDCCH CCE分配失败比例（PDCCH中用于上行调度的CCE分配失败总次数占比：分配失败次数为用户CCE侯选位置均被占用导致分不出CCE）
    if(m_BS_UL_PDCCHCCEAllocate_FailureRate_Total[iTxID] != 0){
        m_BS_UL_PDCCHCCEAllocate_FailureRate[iTxID] = m_BS_UL_PDCCHCCEAllocate_FailureRate_Use[iTxID] / m_BS_UL_PDCCHCCEAllocate_FailureRate_Total[iTxID];
    }else{
        m_BS_UL_PDCCHCCEAllocate_FailureRate[iTxID] = 0;
    }
    ///小区下行PDCCH CCE分配失败比例（PDCCH中用于下行调度的CCE分配失败总次数占比：分配失败次数为用户CCE侯选位置均被占用导致分不出CCE）
    if(m_BS_DL_PDCCHCCEAllocate_FailureRate_Total[iTxID] != 0){
        m_BS_DL_PDCCHCCEAllocate_FailureRate[iTxID] = m_BS_DL_PDCCHCCEAllocate_FailureRate_Use[iTxID] / m_BS_DL_PDCCHCCEAllocate_FailureRate_Total[iTxID];
    }else{
        m_BS_DL_PDCCHCCEAllocate_FailureRate[iTxID] = 0;
    }

//    cout << "m_BS_UL_PDCCHCCEAllocate_FailureRate_Use[ " << iTxID << " ]= " <<  m_BS_UL_PDCCHCCEAllocate_FailureRate_Use[iTxID] << endl;
//    cout << "m_BS_UL_PDCCHCCEAllocate_FailureRate_Total[ " << iTxID << " ]= " <<  m_BS_UL_PDCCHCCEAllocate_FailureRate_Total[iTxID] << endl;
//    cout << "m_BS_UL_PDCCHCCEAllocate_FailureRate[ " << iTxID << " ]= " <<  m_BS_UL_PDCCHCCEAllocate_FailureRate[iTxID] << endl;
//
//    cout << "m_BS_DL_PDCCHCCEAllocate_FailureRate_Use[ " << iTxID << " ]= " <<  m_BS_DL_PDCCHCCEAllocate_FailureRate_Use[iTxID] << endl;
//    cout << "m_BS_DL_PDCCHCCEAllocate_FailureRate_Total[ " << iTxID << " ]= " <<  m_BS_DL_PDCCHCCEAllocate_FailureRate_Total[iTxID] << endl;
//    cout << "m_BS_DL_PDCCHCCEAllocate_FailureRate[ " << iTxID << " ]= " <<  m_BS_DL_PDCCHCCEAllocate_FailureRate[iTxID] << endl;

    ///小区上行平均MCS
    if(m_BS_UL_MCS_Times[iTxID] != 0){
        m_BS_UL_MCS[iTxID] /= m_BS_UL_MCS_Times[iTxID];
    }else{
        m_BS_UL_MCS[iTxID] = 0;
    }
    ///小区下行平均MCS
    if(m_BS_DL_MCS_Times[iTxID] != 0){
        m_BS_DL_MCS[iTxID] /= m_BS_DL_MCS_Times[iTxID];
    }else{
        m_BS_DL_MCS[iTxID] = 0;
    }

    ///小区上行平均CQI
    if(m_BS_UL_CQI_Times[iTxID] != 0){
        m_BS_UL_CQI[iTxID] /= m_BS_UL_CQI_Times[iTxID];
    }else{
        m_BS_UL_CQI[iTxID] = 0;
    }
    ///小区下行平均CQI
    if(m_BS_DL_CQI_Times[iTxID] != 0){
        m_BS_DL_CQI[iTxID] /= m_BS_DL_CQI_Times[iTxID];
    }else{
        m_BS_DL_CQI[iTxID] = 0;
    }

//    cout << "m_BS_DL_MIMO_PairTimes[ " << iTxID << " ]= " <<  m_BS_DL_MIMO_PairTimes[iTxID] << endl;
//    cout << "m_BS_DL_MIMO_PRBNum[ " << iTxID << " ]= " <<  m_BS_DL_MIMO_PRBNum[iTxID] << endl;
//    cout << "m_BS_DL_MIMO_Layer[ " << iTxID << " ]= " <<  m_BS_DL_MIMO_Layer[iTxID] << endl;
    ///%%小区上行MIMO配对平均PRB数
    if(m_BS_UL_MIMO_PairTimes[iTxID] != 0){
        m_BS_UL_MIMO_PRBNum[iTxID] /= m_BS_UL_MIMO_PairTimes[iTxID];
    }else{
        m_BS_UL_MIMO_PRBNum[iTxID] = 0;
    }
    ///小区上行MIMO配对平均层数（配对用户数累计值/配对总次数）
    if(m_BS_UL_MIMO_PairTimes[iTxID] != 0){
        m_BS_UL_MIMO_Layer[iTxID] /= m_BS_UL_MIMO_PairTimes[iTxID];
    }else{
        m_BS_UL_MIMO_Layer[iTxID] = 0;
    }
    ///%%小区下行MIMO配对平均PRB数
    if(m_BS_DL_MIMO_PairTimes[iTxID] != 0){
        m_BS_DL_MIMO_PRBNum[iTxID] /= m_BS_DL_MIMO_PairTimes[iTxID];
    }else{
        m_BS_DL_MIMO_PRBNum[iTxID] = 0;
    }
    ///小区下行MIMO配对平均层数（配对用户数累计值/配对总次数）
    if(m_BS_DL_MIMO_PairTimes[iTxID] != 0){
        m_BS_DL_MIMO_Layer[iTxID] /= m_BS_DL_MIMO_PairTimes[iTxID];
    }else{
        m_BS_DL_MIMO_Layer[iTxID] = 0;
    }

    ///平台采用MCS Index Table 2 PDSCH; MCS小于等于4时采用QPSK调制
    ///大话务支持MCS Index Table 1 PDSCH;MCS小于等于9时采用QPSK调制   Parameters::Instance().DaHuaWu.bDl256QamSwitch == false
    ///小区上行QPSK编码比例（上行QPSK调制的初始TB数和统计周期内传输的总上行初始TB数之比表示）
    if(m_BS_UL_QPSK_Rate_Total[iTxID] != 0){
        m_BS_UL_QPSK_Rate[iTxID] = m_BS_UL_QPSK_Rate_Used[iTxID] / m_BS_UL_QPSK_Rate_Total[iTxID];
    }else{
        m_BS_UL_QPSK_Rate[iTxID] = 0;
    }
    ///平台采用MCS Index Table 1 PUSCH; MCS小于等于9时采用QPSK调制
    ///小区下行QPSK编码比例（下行QPSK调制的初始TB数和统计周期内传输的总下行初始TB数之比表示）
    if(m_BS_DL_QPSK_Rate_Total[iTxID] != 0){
        m_BS_DL_QPSK_Rate[iTxID] = m_BS_DL_QPSK_Rate_Used[iTxID] / m_BS_DL_QPSK_Rate_Total[iTxID];
    }else{
        m_BS_DL_QPSK_Rate[iTxID] = 0;
    }

    ///上行HARQ重传比例（UL-SCH上发生HARQ重传的TB数量与UL-SCH上传输的总的TB数量之比表示）
    if(m_BS_UL_HARQ_Total[iTxID] != 0){
        m_BS_UL_HARQ_Rate[iTxID] = m_BS_UL_HARQ_Used[iTxID] / m_BS_UL_HARQ_Total[iTxID];
    }else{
        m_BS_UL_HARQ_Rate[iTxID] = 0;
    }
    ///下行HARQ重传比例（DL-SCH上发生HARQ重传的TB数量与DL-SCH上传输的总的TB数量之比表示）
    if(m_BS_DL_QPSK_Rate_Total[iTxID] != 0){
        m_BS_DL_HARQ_Rate[iTxID] = m_BS_DL_HARQ_Used[iTxID] / m_BS_DL_HARQ_Total[iTxID];
    }else{
        m_BS_DL_HARQ_Rate[iTxID] = 0;
    }

}

/// 打印输出文件的表头
void Statistics::PrintHead() {

//    ///每次移动后重新打印表头
//    while(iMoveTimes != 0){
//        iMoveTimes += 1;
//    }
    iMoveTimes += 1;
    cout << "iMoveTimes = " << iMoveTimes << endl;

    boost::filesystem::path filename;
//    boost::filesystem::ofstream fBTS;

    string string1 = "BSResults_Move_";
    string1 += to_string(iMoveTimes);
    string1 += ".txt";
    cout << "Directory is : " << string1 << endl;
    filename = Directory::Instance().GetPath(string1);
    fBTS.open(filename);

    int width = 30;
    fBTS << "BTS" << setw(width);

    fBTS << "RRC_ConnectMSNum" << setw(width)
         //         << "RRC_InitialMSNum" << setw(width)
         //         << "CellReSelectMSNum" << setw(width)
         //         << "CellHandoffInMSNum" << setw(width)
         //         << "CellHandoffOutMSNum" << setw(width)
         << "ActiveMSNumUL" << setw(width)
         << "ActiveMSNumDL" << setw(width)
         << "PRB_RateUL" << setw(width)
         << "PRB_RateDL" << setw(width)
         << "PDCCHCCERateUL" << setw(width)
         << "PDCCHCCERateDL" << setw(width)
         //         << "PDCCHCCEAveUL" << setw(width)
         //         << "PDCCHCCEAveDL" << setw(width)
         << "CCEFailRateUL" << setw(width)
         << "CCEFailRateDL" << setw(width)
         << "MCSUL" << setw(width)
         << "MCSDL" << setw(width)
         //         << "CQIUL" << setw(width)
          << "CQI" << setw(width)
         //         << "MIMO_PRBUL" << setw(width)
         << "MIMO_LayerUL" << setw(width)
         //         << "MIMO_PRBDL" << setw(width)
         << "MIMO_LayerDL" << setw(width)
         << "TrafficUL" << setw(width)
         << "TrafficDL" << setw(width)
         << "QPSKRateUL" << setw(width)
         << "QPSKRateDL" << setw(width)
         << "HARQRateUL" << setw(width)
         << "HARQRateDL" << endl;
}

///打印输出数据的函数
void Statistics::PrintTable() {
    ///使用overwrite读取的小区数BASIC.ITotalMacroNum，使用用户轨迹可能会出错
//    for (int i = 0; i < Parameters::Instance().BASIC.ITotalMacroNum; i++){
//    }

    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        int BTS2TxID = btsid.GetBTS().GetTxID();
        {
            PrintTablePara(BTS2TxID, btsid);
        }
    }
    Reset();
}

void Statistics::PrintTablePara(int iTxID, BTSID& _btsid){

    int width = 30;
    fBTS << "(" << _btsid.GetBSID().ToInt() << "," << _btsid.GetIndex() << ")" << setw(width);

    fBTS << m_BS_RRCConnect_MSNum[iTxID] << setw(width)
         //         << m_BS_Initial_MSNum[iTxID] << setw(width)
         //         << m_BS_CellReselection_MSNum[iTxID] << setw(width)
         //         << m_BS_HandoffIn_MSNum[iTxID] << setw(width)
         //         << m_BS_HandoffOut_MSNum[iTxID] << setw(width)
         << m_BS_UL_Active_MSNum[iTxID] << setw(width)
         << m_BS_DL_Active_MSNum[iTxID] << setw(width)
         << m_BS_UL_PRB_Rate[iTxID] << setw(width)
         << m_BS_DL_PRB_Rate[iTxID] << setw(width)
         << m_BS_UL_PDCCHCCE_Rate[iTxID] << setw(width)
         << m_BS_DL_PDCCHCCE_Rate[iTxID] << setw(width)
         //         << m_BS_UL_PDCCHCCE[iTxID] << setw(width)
         //         << m_BS_DL_PDCCHCCE[iTxID] << setw(width)
         << m_BS_UL_PDCCHCCEAllocate_FailureRate[iTxID] << setw(width)
         << m_BS_DL_PDCCHCCEAllocate_FailureRate[iTxID] << setw(width)
         << m_BS_UL_MCS[iTxID] << setw(width)
         << m_BS_DL_MCS[iTxID] << setw(width)
         //         << m_BS_UL_CQI[iTxID] << setw(width)
         << m_BS_DL_CQI[iTxID] << setw(width)
         //         << m_BS_UL_MIMO_PRBNum[iTxID] << setw(width)
         << m_BS_UL_MIMO_Layer[iTxID] << setw(width)
         //         << m_BS_DL_MIMO_PRBNum[iTxID] << setw(width)
         << m_BS_DL_MIMO_Layer[iTxID] << setw(width)
         << m_BS_UL_Traffic[iTxID] << setw(width)
         << m_BS_DL_Traffic[iTxID] << setw(width)
         << m_BS_UL_QPSK_Rate[iTxID] << setw(width)
         << m_BS_DL_QPSK_Rate[iTxID] << setw(width)
         << m_BS_UL_HARQ_Rate[iTxID] << setw(width)
         << m_BS_DL_HARQ_Rate[iTxID] << endl;

}

Statistics& Statistics::Instance() {
    static Statistics& m_S = *(new Statistics);
    return m_S;
}