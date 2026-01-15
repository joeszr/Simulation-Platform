///@file  XRModel.h
///@brief XRModel类的声明
///@author DiXY

#pragma once
#include "TrafficModel.h"
#include "MSProperty_XR.h"
#include "../MobileStation/MSID.h"

class Packet;
class MSProperty_XR;

class XRModel final : public TrafficModel {
private:
    deque<std::shared_ptr<Packet> > m_qTotalPacketQueueDL;
public:
    std::unordered_map<int, MSProperty_XR> m_MSData;

    double dPeriod_ms;
    double dS; //13625;
    double dPacketSizeDefault_Kbit; //15000;
    double dPmax; //150000;
    double dPave; //104167 Bytes;
    double dPmin;
    void WorkSlot() override;
    void OutputTrafficInfo() override;
public:
    XRModel();
    ~XRModel() override = default;
};
