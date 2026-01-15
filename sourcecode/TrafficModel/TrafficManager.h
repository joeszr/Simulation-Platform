///@file  TrafficManager.h
///@brief TrafficManager类的声明
///@author wangfei
#pragma once
#include "TrafficModel.h"

///

class TrafficManager {
private:
    std::shared_ptr<TrafficModel> m_pTrafficModel;

public:
    void WorkSlot();
    void OutputTrafficInfo();
    
public:
    ///Singleton设计模式

    static TrafficManager& Instance() {
        static TrafficManager& m_Trafficmanager = *(new TrafficManager);
        return m_Trafficmanager;
    };
private:
    TrafficManager();
    TrafficManager(const TrafficManager&);
    TrafficManager & operator =(const TrafficManager&);
    virtual ~TrafficManager() = default;
};
