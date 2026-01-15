///@file  FullBufferModel.h
///@brief FullBufferModel类的声明
///@author wangsen

#pragma once
#include "TrafficModel.h"
#include "../MobileStation/MSID.h"
class Packet;
class MSID;
///FullBuffer业务类型类
class FTPModel2 : public TrafficModel {
private:
    double m_dMacroLamda;
    double m_dPacketSize;
    deque<std::shared_ptr<Packet> > m_qTotalPacketQueueDL;
    std::unordered_map<int, int> mMSID2ReadTimeMsec;
    std::unordered_map<int, deque<std::shared_ptr<Packet> > > mMSID2PacketQueueDL;
public:
    void WorkSlot() override;
    void OutputTrafficInfo() override;
public:
    FTPModel2(double _dPacketSize, double _dMacroLamda);
    ~FTPModel2() override =default;
};
