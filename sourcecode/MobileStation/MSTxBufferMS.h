//
// Created by AAA on 2022/10/3.
//
#pragma once

#include "MSID.h"
#include "../TrafficModel/Packet.h"


class MSTxBufferMS {
public:
    /// 移动台的ID
    MSID m_MSID;

    deque<std::shared_ptr<Packet> > m_qPacketQueueUL;

public:
    void ReceivePacketUL(std::shared_ptr<Packet> _pPacket);
    void PopFinishedPacket();
    void PopTimeOutPacket(int _Time);
    //std::vector<pair<std::shared_ptr<Packet>,double> > ReducePacketBuf(double dLen);
    void RecordFinishedPacket();

    double GetULFirstAvailablePacket_RemainUntransmitted_SizeKbits();
    std::shared_ptr<Packet> GetULFirstAvailablePacket();

public:
    /// @brief 构造函数
    explicit MSTxBufferMS(MSID _msid = MSID(-1));
    /// @brief 析构函数
    ~MSTxBufferMS() = default;
};


