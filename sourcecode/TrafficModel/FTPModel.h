///@file  FullBufferModel.h
///@brief FullBufferModel类的声明
///@author wangsen

#pragma once
#include "TrafficModel.h"

class Packet;
///FullBuffer业务类型类
class FTPModel : public TrafficModel {
private:
    double m_dPacketSize1;
    double m_dPacketSize2;
    double lamda1;
    double lamda2;
    deque<std::shared_ptr<Packet> > m_qTotalPacketQueueDL;
public:
    void WorkSlot() override;
    void OutputTrafficInfo() override;
public:
    FTPModel();
    ~FTPModel() override = default;
};
