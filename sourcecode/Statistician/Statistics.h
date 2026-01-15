//
// Created by LAI on 2023/9/7.
//

#ifndef BTS_UL_CPP_STATISTICS_H
#define BTS_UL_CPP_STATISTICS_H

#pragma once
#include "../Utility/Include.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "MSData_DL.h"
#include "MSData_UL.h"
#include "BSData.h"

/// @brief 用来统计数据的类
/// 目前只有BTS类的数据统计

class Statistics {
public:
    ///移动台移动次数
    int iMoveTimes;
    /// 用来记录基站统计数据的文件
    boost::filesystem::ofstream fBTS;

public:
    /***** 新增小区级指标：按照配置的drop内用户移动时间间隔为粒度（可按照该粒度生成多个文件） *****/

    ///小区RRC Connect态用户数
    std::unordered_map<int, int> m_BS_RRCConnect_MSNum;
    ///%%小区初始新接入用户数
    std::unordered_map<int, int> m_BS_Initial_MSNum;
    ///%%小区重选入用户数
    std::unordered_map<int, int> m_BS_CellReselection_MSNum;
    ///%%小区切换入用户数
    std::unordered_map<int, int> m_BS_HandoffIn_MSNum;
    ///%%小区切换出用户数
    std::unordered_map<int, int> m_BS_HandoffOut_MSNum;

    ///小区上行激活用户数（统计周期内有调度的用户数累计值）
    std::unordered_map<int, int> m_BS_UL_Active_MSNum;
    std::unordered_map<int, vector<MSID>> m_BS_UL_Active_vMSID;
    ///小区下行激活用户数
    std::unordered_map<int, int> m_BS_DL_Active_MSNum;
    std::unordered_map<int, vector<MSID>> m_BS_DL_Active_vMSID;

    ///小区上行PRB利用率
    std::unordered_map<int, double> m_BS_UL_PRB_Rate;
    std::unordered_map<int, double> m_BS_UL_PRB_Rate_Total;
    std::unordered_map<int, double> m_BS_UL_PRB_Rate_Use;
    ///小区下行PRB利用率
    std::unordered_map<int, double> m_BS_DL_PRB_Rate;
    std::unordered_map<int, double> m_BS_DL_PRB_Rate_Total;
    std::unordered_map<int, double> m_BS_DL_PRB_Rate_Use;

    ///小区上行PDCCH CCE利用率（上行调度使用的CCE数/上行可用的CCE总数，当无其他限制条件时，上行可用CCE总数为所有可用CCE；当限制上行CCE占比时，则根据配置计算上行可用CCE总数）
    std::unordered_map<int, double> m_BS_UL_PDCCHCCE_Rate;
    std::unordered_map<int, double> m_BS_UL_PDCCHCCE_Rate_Total;
    std::unordered_map<int, double> m_BS_UL_PDCCHCCE_Rate_Use;
    ///小区下行PDCCH CCE利用率（下行调度使用的CCE数/下行可用的CCE总数，当无其他限制条件时，下行可用CCE总数为所有可用CCE；当限制下行CCE占比时，则根据配置计算下行可用CCE总数）
    std::unordered_map<int, double> m_BS_DL_PDCCHCCE_Rate;
    std::unordered_map<int, double> m_BS_DL_PDCCHCCE_Rate_Total;
    std::unordered_map<int, double> m_BS_DL_PDCCHCCE_Rate_Use;

    ///%%小区上行平均PDCCH CCE
    std::unordered_map<int, double> m_BS_UL_PDCCHCCE;
    std::unordered_map<int, int> m_BS_UL_PDCCHCCE_Times;
    ///%%小区下行平均PDCCH CCE
    std::unordered_map<int, double> m_BS_DL_PDCCHCCE;
    std::unordered_map<int, int> m_BS_DL_PDCCHCCE_Times;

    ///小区上行PDCCH CCE分配失败比例（PDCCH中用于上行调度的CCE分配失败总次数占比：分配失败次数为用户CCE侯选位置均被占用导致分不出CCE）
    std::unordered_map<int, double> m_BS_UL_PDCCHCCEAllocate_FailureRate;
    std::unordered_map<int, double> m_BS_UL_PDCCHCCEAllocate_FailureRate_Total;
    std::unordered_map<int, double> m_BS_UL_PDCCHCCEAllocate_FailureRate_Use;
    ///小区下行PDCCH CCE分配失败比例（PDCCH中用于下行调度的CCE分配失败总次数占比：分配失败次数为用户CCE侯选位置均被占用导致分不出CCE）
    std::unordered_map<int, double> m_BS_DL_PDCCHCCEAllocate_FailureRate;
    std::unordered_map<int, double> m_BS_DL_PDCCHCCEAllocate_FailureRate_Total;
    std::unordered_map<int, double> m_BS_DL_PDCCHCCEAllocate_FailureRate_Use;

    ///小区上行平均MCS
    std::unordered_map<int, double> m_BS_UL_MCS;
    std::unordered_map<int, int> m_BS_UL_MCS_Times;
    ///小区下行平均MCS
    std::unordered_map<int, double> m_BS_DL_MCS;
    std::unordered_map<int, int> m_BS_DL_MCS_Times;

    ///小区上行平均CQI
    std::unordered_map<int, double> m_BS_UL_CQI;
    std::unordered_map<int, int> m_BS_UL_CQI_Times;
    ///小区下行平均CQI
    std::unordered_map<int, double> m_BS_DL_CQI;
    std::unordered_map<int, int> m_BS_DL_CQI_Times;

    ///%%小区上行MIMO配对平均PRB数
    std::unordered_map<int, double> m_BS_UL_MIMO_PRBNum;
    std::unordered_map<int, double> m_BS_UL_MIMO_PairTimes;
    ///小区上行MIMO配对平均层数（配对用户数累计值/配对总次数）
    std::unordered_map<int, double> m_BS_UL_MIMO_Layer;
    ///%%小区下行MIMO配对平均PRB数
    std::unordered_map<int, double> m_BS_DL_MIMO_PRBNum;
    std::unordered_map<int, double> m_BS_DL_MIMO_PairTimes;
    ///小区下行MIMO配对平均层数（配对用户数累计值/配对总次数）
    std::unordered_map<int, double> m_BS_DL_MIMO_Layer;

//    ///小区上行业务量
//    std::unordered_map<int, int> m_BS_UL_Throughput;
//    ///小区下行业务量
//    std::unordered_map<int, int> m_BS_DL_Throughput;
    ///小区上行业务量
    std::unordered_map<int, double> m_BS_UL_Traffic;
    ///小区下行业务量
    std::unordered_map<int, double> m_BS_DL_Traffic;

    ///平台采用MCS Index Table 2 PDSCH; MCS小于等于4时采用QPSK调制
    ///大话务支持MCS Index Table 1 PDSCH;MCS小于等于9时采用QPSK调制   Parameters::Instance().DaHuaWu.bDl256QamSwitch == false
    ///小区上行QPSK编码比例（上行QPSK调制的初始TB数和统计周期内传输的总上行初始TB数之比表示）
    std::unordered_map<int, double> m_BS_UL_QPSK_Rate;
    std::unordered_map<int, double> m_BS_UL_QPSK_Rate_Total;
    std::unordered_map<int, double> m_BS_UL_QPSK_Rate_Used;
    ///平台采用MCS Index Table 1 PUSCH; MCS小于等于9时采用QPSK调制
    ///小区下行QPSK编码比例（下行QPSK调制的初始TB数和统计周期内传输的总下行初始TB数之比表示）
    std::unordered_map<int, double> m_BS_DL_QPSK_Rate;
    std::unordered_map<int, double> m_BS_DL_QPSK_Rate_Total;
    std::unordered_map<int, double> m_BS_DL_QPSK_Rate_Used;

    ///上行HARQ重传比例（UL-SCH上发生HARQ重传的TB数量与UL-SCH上传输的总的TB数量之比表示）
    std::unordered_map<int, double> m_BS_UL_HARQ_Rate;
    std::unordered_map<int, double> m_BS_UL_HARQ_Total;
    std::unordered_map<int, double> m_BS_UL_HARQ_Used;
    ///下行HARQ重传比例（DL-SCH上发生HARQ重传的TB数量与DL-SCH上传输的总的TB数量之比表示）
    std::unordered_map<int, double> m_BS_DL_HARQ_Rate;
    std::unordered_map<int, double> m_BS_DL_HARQ_Total;
    std::unordered_map<int, double> m_BS_DL_HARQ_Used;

public:
    /// @brief 在输出数据之前，对统计数据作一些处理
    void PreProcess();
    void PreProcessPara(int index, BTSID& _btsid);
    /// @brief 在输出文件的第一行打印输出数据的表头
    void PrintHead();
    /// @brief 把统计结果输出到文件
    void PrintTable();
    void PrintTablePara(int index, BTSID& _btsid);
    /// @brief 重置所有的记录
    void Reset();
    void ResetPara(int index, BTSID& _btsid);
    /// @brief 清除所有的数据记录
    void Clear();
    /// @brief 初始化过程
    void Initialize();

public:
    ///@brief 实现返回唯一的统计对象的实例
    ///@return 统计对象的引用
    static Statistics& Instance();
private:
    /// @brief 构造函数
    Statistics();
    /// @brief 拷贝构造函数
    Statistics(const Statistics&);
    /// @brief 赋值构造函数
    Statistics & operator =(const Statistics&);
    /// @brief 析构函数
    ~Statistics() = default;
};

#endif //BTS_UL_CPP_STATISTICS_H
