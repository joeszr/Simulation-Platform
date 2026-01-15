///@file  SchedulerDL.h
///@brief 调度器基类的声明
///@author wangfei

#pragma once
#include "../Utility/Include.h"
//@threads
#include "../SafeDeque.h"
#include "../SafeUnordered_map.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "SchedulingMessageDL.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../CalcRENumperRB.h"

class BTSTxer;


///调度器基类

class SchedulerDL {
protected:
    BTSID m_MainServBTSID;
public:
//    CalcRENum _mCalcRENum;
    //@threads
    virtual void Scheduling(BTSTxer* _pBTSTxer, vector<MSID>& _vActiveSet, HARQTxStateBTS& _HARQTxState, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer, SafeDeque<std::shared_ptr<SchedulingMessageDL> >& _qSchM, vector<vector<bool>>& _vSBUsedFlag) = 0;
    //virtual void Scheduling(Cluster& _pCluster, vector<MSID>& _vClusterActiveSet, map<BTSID, HARQTxStateBTS>& _mClusterHARQTxState, map<MSID,MSTxBufferBTS>& _mClusterMSTxBuffer, map<BTSID, deque<std::shared_ptr<SchedulingMessageDL> > >& _mClusterScheduleMesQueue);
public:
    explicit SchedulerDL(const BTSID& _MainServBTSID);
    //explicit SchedulerDL(ClusterID _ClusterID);
    virtual ~SchedulerDL() = default;
};




