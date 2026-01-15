///@file  SchedulerUL.h
///@brief 调度器基类的声明
///@author zhengyi

#pragma once
#include "../Utility/Include.h"
class MSID;
class BTSID;
class MSTxBufferBTS;
class MSRxBufferBTS;
class HARQTxStateBTS;
class HARQRxStateBTS;
class MSRxBufferBTS;
class MSTxBufferBTS;
class SchedulingMessageUL;
class BTSRxer;

///调度器基类

class SchedulerUL {
protected:
    BTSID m_MainServBTSID;
    //算法参数
    std::shared_ptr<BTSRxer> m_pRxer;
    vector<vector<bool>> m_vRBSetUsedFlag;
public:
    virtual void Scheduling(vector<MSID>& _vActiveSet,  ///Uplink
            HARQRxStateBTS& _HARQRxState,
            std::unordered_map<int, MSRxBufferBTS>& _mMSRxBuffer,   ///这个也要改
            deque<std::shared_ptr<SchedulingMessageUL> >& _qSchM, vector<vector<vector<bool>>>& _vRBUsedFlagUL, vector<vector<bool>>& _vRBUsedFlagDL)=0; // = 0;
    void SetRxer(const std::shared_ptr<BTSRxer>&);

public:
    explicit SchedulerUL(const BTSID& _MainServBTSID);
    ~SchedulerUL() = default;
};




