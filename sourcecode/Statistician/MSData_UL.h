///@file MSData.h
///@brief 用来记录MS统计数据的函数
///@author dushaofeng
#pragma once
#include "../Utility/Include.h"


class BTSID;

///用来记录移动台统计数据的类

class MSData_UL {
public:
    ///移动台的横座标
    double m_x;
    ///移动台的纵座标
    double m_y;
    ///移动台的海拔高度
    double m_z;
    ///移动台的主服务基站
    BTSID m_ActiveBTS;
    ///移动台的Geometry
    double m_dGeometryDB;
    /// szx get AODDGRE
    double m_AODLOSDEG;
    ///szx 
    double m_dUE2BSTiltDEG;
    ///szx
    double m_dPlusOffsetDEG;
    ///szx
    double m_dAveTxAntennaPatternDB;
    //szx 2D距离
    double m_d2D_Distance;
    //SZX
    double m_dRxHeight;
    ///移动台的各个传输格式和对错的击中次数(下行)
    //    double m_iFormat2TrueFalseFrameHitDL[2][29];
    ///移动台的各个传输格式和对错的击中次数(上行)
    //    double m_iFormat2TrueFalseFrameHitUL[2][29];
    ///移动台每个RB被使用的次数
    //    int RBnBeenUsed[50];
    ///移动台的各个包的结果统计 1时延小于50ms。2时延大于50ms。3重传失败（下行）
    //    int m_iPacketResultDL[3];
    ///移动台的各个包的结果统计 1时延小于50ms。2时延大于50ms。3重传失败（上行）
    //    int m_iPacketResultUL[3];
    ///某移动台成功传对的包的总时延
    //    double m_SumDelayOfSucceedPacket;
    ///某移动台成功传对的包的数目
    //    int m_SucceedPacketNumber;
    ///移动台的平均速率(下行)
    //    double m_dAveRateKbpsDL;
    ///移动台的平均速率(上行)
    //    double m_dAveRateKbpsUL;
    ///移动台的帧错误率(下行)
    //    double m_dPERDL;
    ///移动台的帧错误率(上行)
    //    double m_dPERUL;
    ///移动台到激活基站的路径损耗
    double m_dLinkLossDB;
    ///移动台的每种传输格式所占的比例(下行)
    //    double m_dFormatFractionDL[29];
    ///移动台的每种传输格式所占的比例（上行）
    double m_dFormatFractionUL[29];
    ///延时小于50ms的包比例（下行）
    //    double m_dPDelayBelow50msDL;
    ///延时小于50ms的包比例（上行）
    //    double m_dPDelayBelow50msUL;
    ///延时大于50ms的包比例（下行）
    //    double m_dPDelayUp50msDL;
    ///延时大于50ms的包比例（上行）
    //    double m_dPDelayUp50msUL;
    ///丢包率（下行）
    //    double m_dPDiscardDL;
    ///丢包率（上行）
    //    double m_dPDiscardUL;
    ///移动台收到的主服务基站的信噪比
    //    double m_dMainSNRDB;
    ///移动台每个drop中的平均SINR
    long double m_dAVE_SINR;
    //    long double m_dAVE_SINR_test;
    long double m_dul_geoemtry;
    ///计算SINR均值的标记符,记录每个移动台所使用的RB中计算SINR的次数
    double m_dAVE_SINR_Index;
    ///统计用户平均预测SINR
    long double m_dAVE_preSINR;
    ///统计预测SINR指示符，记录每个移动台所使用的RB中计算SINR的次数
    double m_dAVE_preSINR_Index;
    ///每个移动台的经过不同重传次数统计，分为直接传对，重传1、2、3次以及最后始终没传对的次数
    int m_iReTransTimePerMS[5];

    int phr_trigger[3];
    ///MS使用双流的概率
    //    int m_iTwoStreamHitTimes;
    ///MS使用单流的概率
    //    int m_iOneStreamHitTimes;
    ///每个移动台每种rank的使用次数,第一个记录Rank1的使用次数，第二个记录Rank2的使用次数
    int m_iRank[2];
    ///移动台发射功率
    double m_dTxPowerMw;
    ///移动台发射功率记录，计数，用于计算平均值
    double m_dTxPowerMwIndex;
    ///记录用户调度的RB的总数
    double m_dRBCount;
    double m_dRBCount2;
    ///记录用户被调度的次数
    int m_iSchCount;
    int m_iSchCount2;
    ///当前MS支持的RB数
    int m_iRBSupported;
    ///当前MS占用的RBSet数
    int m_iOccupiedRBS;
    int m_iOccupiedRBS2;
    ///the num of bts IN comp
    //    int m_iCompSize;
    /// tne num of MU count
    int m_iMUcount;
    int m_iSUcount;
    int m_iIsO2I;
    int m_iIsLOS; // 1-LOS 0-NLOS nml
    double m_dShadowfading;

    ///test
    long double m_dAVE_preCQISINR;
    double m_dAVE_preCQISINR_Index;

    //zhengyi 3D MIMO
    double m_dPathlossDB;
    double m_dAvePathlossDB;

    //    struct {
    //        ///移动台的Geometry
    //        //        double m_dGeometryDB;//没有使用
    //        ///移动台的平均速率(下行)
    //        //        double m_dAveRateKbps;
    //        ///移动台的帧错误率(下行)
    //        //        double m_dBLERTx[5];
    //        ///移动台的每种传输格式所占的比例(下行)
    //        //        int m_iMCSHit[29];
    //        ///移动台每个drop中的平均SINR
    //        //        double m_dAveSINR;
    //        ///平均SINR的记录次数
    //        //        int m_iSINRHit;
    //        ///移动台每个drop中的平均Post SINR
    //        //        double m_dAvePostSINR;
    //        ///平均SINR的记录次数
    //        //        int m_iPostSINRHit;
    //        ///数据包正确传输次数的点击数，下标0表示正确传输的总包数，下标1为1次传对，2为2次传对，依次类推
    //        //        int m_iCorrectBlockHit[5];
    //        ///数据包错误传输次数的点击数，下标0表示错误传输的总包数，下标1为1次传错，2为2次传错，依次类推
    //        //        int m_iCorruptBlockHit[5];
    //        ///每个移动台每种rank的使用次数,第一个记录Rank1的使用次数，第二个记录Rank2的使用次数
    //        //        int m_iRankHitNum[2];
    //        ///移动台的各个传输格式和对错的击中次数(下行)
    ////        double m_iFormat2TrueFalseFrameHitDL[2][29];
    //        ///移动台的帧错误率(下行)
    //        //        double m_dPERDL;
    //    } DL;

    struct {
        //需要查看哪些变量是UL的，添加在下面！

        ///移动台的平均速率(上行)
        //        double m_dAveRateKbps;
        ///移动台的平均速率(上行)
        double m_dAveRateKbpsUL;
        ///移动台的帧错误率(上行)
        //        double m_dBLER;
        ///移动台的每种传输格式所占的比例（上行）
        //        int m_iMCSHit[29];
        //上行重传统计
        double m_iCorrectBlockHit[5];
        double m_iCorruptBlockHit[5];
        ///移动台的各个传输格式和对错的击中次数(上行)
        double m_iFormat2TrueFalseFrameHitUL[2][29];
        ///移动台的帧错误率(上行)
        double m_dBLERTx[5];
    } UL;

public:
    ///MSData类的构造函数
    MSData_UL();
    ///MSData类的析构函数
    ~MSData_UL() = default;

};
