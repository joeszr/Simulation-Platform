///@file  FullBufferModel.h
///@brief FullBufferModel类的声明
///@author wangsen

#pragma once
#include "TrafficModel.h"
#include "../MobileStation/MSID.h"
class Packet;
///FullBuffer业务类型类
class FTPModel3 final: public TrafficModel {
private:
    double m_dMacroLamda;
    double m_dPacketSize;
    deque<std::shared_ptr<Packet> > m_qTotalPacketQueueDL;
    std::unordered_map<int, int> mMSID3ReadTimeMsec;
    std::unordered_map<int, deque<std::shared_ptr<Packet> > > mMSID3PacketQueueDL;
public:
    void WorkSlot() final;
    void OutputTrafficInfo() final;
public:
    FTPModel3(double _dPacketSize, double _dMacroLamda);
    ~FTPModel3() override = default;
};
