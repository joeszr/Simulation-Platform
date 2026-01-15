///@file BSData.h
///@brief 用来记录BTS统计数据的函数
///@author dushaofeng

#pragma once
#include "../Utility/Include.h"

///用来记录BTS统计数据的类

class BSData {
public:
    ///服务移动台的数目
    int m_iNumServMS;
    
    struct {
        ///基站的下行平均吞吐量
        double m_dThroughputKbps;
        ///基站的下行平均频谱效率
        double m_dAveSE_20MHz_withoutOverhead_bps_Per_Hz;
        ///基站的下行平均吞吐量(折算后)
        double m_dThroughputKbps_Converted;
        ///基站的下行平均吞吐量(折算后)
        double m_dAveSE_StatBandWidth_withOverhead_bps_Per_Hz;

        ///基站的下行初传次数
        int m_iNewTxNum;
        ///基站下行初传错误次数
        int m_iNewTxErrorNum;

        //下行调度用户数（新传）
        int m_iScheMSNum;
        //基站下行调度次数
        int m_iScheTime;

        
    } DL;

    struct {
        ///基站的上行平均吞吐量
        double m_dThroughputKbps;//没有使用
        
        //上行添加_begin
        double m_dIoT;
        double m_dIoT_load;
        int m_Iiot_count;
        int m_Iiot_count_load;
        //下行添加_end

        ///基站的上行初传次数
        int m_iNewTxNum;
        ///基站上行初传错误次数
        int m_iNewTxErrorNum;

        //上行调度用户数（新传）
        int m_iScheMSNum;
        //基站上行调度次数
        int m_iScheTime;
    } UL;
public:
    ///BSData类的构造函数
    BSData();
    ///BSData类的析构函数
    ~BSData()=default;
};
