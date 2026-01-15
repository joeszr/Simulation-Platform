///@file Statistician.h
///@brief 用来统计数据的函数
///@author dushaofeng

#pragma once
#include "../Utility/Include.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "MSData_DL.h"
#include "MSData_UL.h"
#include "BSData.h"

/// @brief 用来统计数据的类

class Statistician {
public:
    /// 数组的下标对应移动台的ID，其中MSData是用来记录统计数据的结构体
    std::unordered_map<int, MSData_DL> m_MSData_DL;
    std::unordered_map<int, MSData_UL> m_MSData_UL;
    /// 从基站的ID到基站统计数据的映射表，其中BSData是用来记录基站数据的结构体
    std::unordered_map<int, BSData> m_BTSData;//上下行公用

    vector<double> m_vdOH_DMRS_Rate;
    vector<double> m_vdOH_DMRS_Rate_Old;
    double m_dOH_DMRS_Rate; 
    double m_dOH_DMRS_Rate_Old;
    vector<double> RSRP;
    vector<int> msbts;

public:
    /// @brief 在输出数据之前，对统计数据作一些处理
    void PreProcess();
    /// @brief 在输出文件的第一行打印输出数据的表头
    void PrintHead();
    /// @brief 把统计结果输出到文件
    void PrintTable();
    /// @brief 重置所有的记录
    void Reset();
    /// @brief 清除所有的数据记录
    void Clear();
    /// @brief 初始化过程
    void Initialize();

private:

    struct {
        /// 用来记录移动台统计数据的文件(下行)
        boost::filesystem::ofstream fMS;
        /// RedCap :用来记录移动台统计数据的文件(下行)
        boost::filesystem::ofstream fMSeMBB;
        /// RedCap :用来记录移动台统计数据的文件(下行)
        boost::filesystem::ofstream fMSRedCap;
        /// 用来记录基站统计数据的文件(下行)
        boost::filesystem::ofstream fBS;
        /// 用来统计MCS等级的文件
        boost::filesystem::ofstream fMCS;
        /// RedCap: 用来统计MCS等级的文件
        boost::filesystem::ofstream fMCS_eMBB;
        /// RedCap: 用来统计MCS等级的文件
        boost::filesystem::ofstream fMCS_RedCap;
        /// 用来记录Macro下的移动台统计数据的文件(下行)
        boost::filesystem::ofstream fMacroMS;
        /// 用来记录Pico下的移动台统计数据的文件(下行)
        boost::filesystem::ofstream fPicoMS;
        /// 用来统计Macro下的MCS等级的文件
        boost::filesystem::ofstream fMacroMCS;
        /// 用来统计Pico下的MCS等级的文件
        boost::filesystem::ofstream fPicoMCS;
        
        boost::filesystem::ofstream fOH_DMRS;
    } DL;

    struct {
        /// 用来记录移动台统计数据的文件(上行)
        boost::filesystem::ofstream fMS;
        /// 用来记录基站统计数据的文件(上行)
        boost::filesystem::ofstream fBS;
        /// 用来记录基站统计IoT的文件(上行)
        boost::filesystem::ofstream fIoT;
        /// RedCap：用来记录类型一移动台统计数据的文件(上行)
        boost::filesystem::ofstream fMSeMBB;
        /// RedCap：用来记录类型二移动台统计数据的文件(上行)
        boost::filesystem::ofstream fMSRedCap;
    } UL;

public:
    ///@brief 实现返回唯一的统计员对象的实例
    ///@return 统计员对象的引用
    static Statistician& Instance();
private:
    /// @brief 构造函数
    Statistician();//2020416

    /// @brief 拷贝构造函数
    Statistician(const Statistician&);
    /// @brief 赋值构造函数
    Statistician & operator =(const Statistician&);
    /// @brief 析构函数
    ~Statistician() = default;
};
