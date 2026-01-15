///@file BSData.cpp
///@brief 用来记录BTS统计数据的函数
///@author dushaofeng

#include "BSData.h"

BSData::BSData() {
    m_iNumServMS = 0;
    DL.m_dThroughputKbps = 0;
    DL.m_dAveSE_20MHz_withoutOverhead_bps_Per_Hz = 0;
    DL.m_dThroughputKbps_Converted = 0;
    DL.m_dAveSE_StatBandWidth_withOverhead_bps_Per_Hz = 0;

    DL.m_iNewTxNum = 0;
    DL.m_iNewTxErrorNum = 0;
    DL.m_iScheMSNum = 0;
    DL.m_iScheTime = 0;

    UL.m_dThroughputKbps = 0;
    
    //上行添加_begin
    UL.m_dIoT = 0;
    //m_dIoT_load
    UL.m_dIoT_load = 0;
    UL.m_Iiot_count = 0;
    UL.m_Iiot_count_load = 0;

    UL.m_iNewTxNum = 0;
    UL.m_iNewTxErrorNum = 0;
    UL.m_iScheMSNum = 0;
    UL.m_iScheTime = 0;
    //上行添加_end
}
