///@file MSRxBufferBTS.h
///@brief  MSRxBufferBTS类声明
///@author wangsen

#pragma once
#include "../Utility/Include.h"
#include "../MobileStation/MSID.h"
#include "../TrafficModel/Packet.h"

class MSID;
class SCID;
class SchedulingMessageUL;

/// @brief 位于BTS上的移动台接收缓存

class MSRxBufferBTS {
public:
    /// 移动台的ID
    MSID m_MSID;
    /// 移动台成功接收的总比特数
    double m_dSuccessRxKbit;
    /// SRS的SINR
    vector<mat> m_vSRSSINR;
    /// 数据信道的SINR
    vector<mat> m_vSINR;
    //    vector<double> m_vSINR_test;
    vector<double> m_vULGeometry;
    ///每个载波的IoT,线性值,用于预测SINR时需要的干扰
    vector<double> m_vIoTEstimate;
    ///预测的干扰值
    vector<double> m_vInfEst;
    ///SRS上的干扰值（预测？）
    vector<double> m_vSRSIoT;
    /// OLLA调整偏置（dB)
    double m_dOLLAOffsetDB;

    //    double m_dOLLAoffset2streamDB;

    //ZHENGYI
    double m_dOLLAOffset4MUDB;
    /// 保存上行调度信息
    deque<std::shared_ptr<SchedulingMessageUL> > m_qScheduleMessageQueue;
    vector<pair<std::shared_ptr<Packet>,double> > ReducePacketBuf(double dLen);
    deque<std::shared_ptr<Packet> > m_qPacketQueueUL;

public:
    /// @brief 累计成功传输的总比特数
    void AccumulateSuccessRxKbit(double _dKbit);
    /// @brief 获得满足时延要求的调度消息
    std::shared_ptr<SchedulingMessageUL> GetSchedulingMessage();
    ///olla
    void OllA(bool _bACK, int _iSendNum);

    //    void OllA(bool _bACK, int _iSendNum, bool _bACKanother);

    //ZHENGYI     OllA4MU
    void OllA4MU(bool _bACK, int _iSendNum);
    void PopFinishedPacket();
    void PopTimeOutPacket(int);
public:
    /// @brief 构造函数
    explicit MSRxBufferBTS(const MSID& _msid = MSID(-1));
    /// @brief 析构函数
    ~MSRxBufferBTS() = default;
};
