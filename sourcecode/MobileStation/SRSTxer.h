///@file SRSTxer.h
///@brief  SRS发射机的声明
///@author wangfei

#pragma once
#include "../Utility/Include.h"
using namespace std;
using namespace itpp;

class BTSID;
class MSID;
class RBID;

class SRSTxer {
protected:
    int m_iCombNum; // LTE: 2; NR: 4
    int m_iRefSRSPeried_slot; // LTE: 20ms (20slot); NR: 20ms (40slot)

    // LTE: 2 OS per 5ms (5 slot); NR: 4 OS per 5ms (10 slot)
    double m_dRefOSNum_per_slot; // LTE: 2 / 5; NR: 4 / 10

    double DSRS_SystemBW_MHz; // LTE: 20MHz ; NR: 100MHz 
    double DSRS_SRS_UnitBW_MHz; // LTE: 5MHz ; NR: 20MHz 

    int IRBNUM_in_SystemBW; // LTE: 100RB ; NR: 250RB

    // m_iRB_per_SRS = IRBNUM_in_SystemBW / (DSRS_SystemBW_MHz / DSRS_SRS_UnitBW_MHz)
    int m_iRB_per_SRS; // LTE: 25RB ; NR: 50RB      

    int m_iRxAntNum; // LTE: 2; NR: 4

protected:
    // m_iCrashNum = m_iCombNum * (m_iRefSRSPeried_slot * m_iRefOSNum_per_slot) / m_iRxAntNums
    //  int m_iCrashNum;                // 

    // m_iMultiplexID = 0 ... m_iCrashNum - 1
    int m_iMultiplexID;

public:
    void UpdateMultiplexID();

    void SetMultiplexID(int iMultiplexID) {
        m_iMultiplexID = iMultiplexID;
    }

    int GetMultiplexID() {
        return m_iMultiplexID;
    }


    //protected:
    //    // m_iUnitSRS_MultiplexNum_per_RefPeriod = 
    //    //      m_iCombNum * m_iRefSRSPeried_slot * m_iRefOSNum_per_slot 
    //    //      * (m_dSystemBW_MHz / m_dSRS_UnitBW_MHz)
    //    int m_iUnitSRS_MultiplexNum_per_RefPeriod;  // consdier inter-gNB interference
    //    
    //protected:
    //    int m_iMaxAllowed_UnitSRSBW_Multiper;   // 2
    //    int m_iUsed_UnitSRSBW_Num;    // m_iUsed_UnitSRSBW_Num <= m_iMaxAllowed_UnitSRSBW_Multiper
    //    
    //    int m_iUsedSRSPeried_slot;    // m_iUsedSRSPeried_slot = m_iRefSRSPeried_slot / m_iUsed_UnitSRSBW_Num  
    //    
    //protected:
    //    // m_iUsedMultiplexNum_per_RefPeriod = (m_dSystemBW_MHz / m_dSRS_UnitBW_MHz) 
    //    //      * m_iRxAntNum * m_iUsed_UnitSRSBW_Num
    //    int m_iUsedMultiplexNum_per_RefPeriod; 
    //    
    //    // size(m_vMultiplexResourceIndexs_per_RefPeriod) = m_iUsedMultiplexNum_per_RefPeriod
    //    // element = 0 ... m_iUnitSRS_MultiplexNum_per_RefPeriod - 1
    //    vector<int> m_vMultiplexResourceIndexs_per_RefPeriod; // updated per RefPeriod
    //    



private:
    BTSID m_MainServBTSID;
    MSID m_MSID;
    int m_iGroupID;
    RBID m_FirstRB;
    RBID m_LastRB;
    int m_iBandIndex;
    double m_dSRSTxPowerMW_per_SC;
public:
    void Initialize();
    static int m_iCrashNum;
    static int GetCrashNum();
    //    int GetGroupID();GetMultiplexID
    //    RBID GetFirstRB();
    //    RBID GetLastRB();
    //    int GetBandIndex();
    //    void SRSScheduling();
    void SRSPowerControl();
    double GetSRSTxPowerMW_per_SC();
public:
    SRSTxer(const BTSID& _MainServBTSID, const MSID& _msid);
    ~SRSTxer() = default;

};