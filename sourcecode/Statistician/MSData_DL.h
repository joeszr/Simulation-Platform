///@file MSData.h
///@brief 用来记录MS统计数据的函数
///@author dushaofeng

#pragma once
#include "../Utility/Include.h"
#include "../BaseStation/BTSID.h"
class BTSID;

///用来记录移动台统计数据的类

class MSData_DL {
public:
    ///移动台的横座标
    double m_x;
    ///移动台的纵座标
    double m_y;
    ///移动台的海拔高度
    double m_z;
    ///移动台的主服务基站
    BTSID m_ActiveBTS;
    ///移动台到激活基站的路径损耗
    double m_dLinkLossDB;
    ///移动台到激活基站的天线PatternGain
    double m_dAveAntGainDB;
    ///移动台ESA
    double m_dESA;
    ///移动台ESD
    double m_dESD;

    struct {
        ///移动台的Geometry
        double m_dGeometryDB;
        ///移动台的平均速率(下行)
        double m_dAveRateKbps;
        ///移动台的平均频谱效率(下行)(折算前)
        double m_dAveSE_20MHz_withoutOverhead_bps_Per_Hz;
        ///移动台的平均速率(下行)(折算后)
        double m_dAveRateKbps_Converted;
        ///移动台的平均频谱效率(下行)(折算后)
        double m_dAveSE_StatBandWidth_withOverhead_bps_Per_Hz;
        ///移动台的帧错误率(下行)
        double m_dBLERTx[5];
        ///移动台的每种传输格式所占的比例(下行)
        int m_iMCSHit[29];
        ///移动台每个drop中的平均SINR
        double m_dAveSINR;
        ///平均SINR的记录次数
        int m_iSINRHit;
        ///移动台每个drop中的平均Post SINR
        double m_dAvePostSINR;
        //移动台每个drop中的平均Post SINR对数直接平均
        double m_dLogAvePostSINR;
        ///平均SINR的记录次数
        int m_iPostSINRHit;
        ///数据包正确传输次数的点击数，下标0表示正确传输的总包数，下标1为1次传对，2为2次传对，依次类推
        int m_iCorrectBlockHit[5];
        ///数据包错误传输次数的点击数，下标0表示错误传输的总包数，下标1为1次传错，2为2次传错，依次类推
        int m_iCorruptBlockHit[5];
        ///每个移动台每种rank的使用次数,第一个记录Rank1的使用次数，第二个记录Rank2的使用次数
        int m_iRankHitNum[4];
        int m_iTM3HitNum;
        ///CRS的两个Port平均SINR
        double m_dCRSSINR;
        int m_iCRSSINRHit;
        ///每个移动台被调度的次数
        int m_iSchedulingNum;

    } DL;
    int Packetscounts;
    int SuccessPackets;
    int LostPackets;
    //    struct {
    //        ///移动台的平均速率(上行)
    ////        double m_dAveRateKbps;
    //        ///移动台的帧错误率(上行)
    ////        double m_dBLER;
    //        ///移动台的每种传输格式所占的比例（上行）
    ////        int m_iMCSHit[29];
    //    } UL;

public:
    ///MSData类的构造函数
    MSData_DL();
    ///MSData类的析构函数
    ~MSData_DL() = default;
};
