///@file  TrafficModel.h
///@brief TrafficModel类的声明
///@author wangsen
#pragma once
#include "../Parameters/Parameters.h"
#include "../NetworkDrive/Clock.h"
#include "../Utility/functions.h"
#include "../Utility/Random.h"
///业务类型基类

class TrafficModel {
public:
    Random random;
    virtual void WorkSlot() = 0;
    virtual void OutputTrafficInfo() = 0;
public:
    TrafficModel() = default;
    virtual ~TrafficModel() = default;
};
