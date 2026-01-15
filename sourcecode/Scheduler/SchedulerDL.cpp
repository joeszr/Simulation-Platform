#include "../Utility/IDType.h"
#include "../MobileStation/MSID.h"
#include "../BaseStation/BTSID.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "SchedulerDL.h"

SchedulerDL::SchedulerDL(const BTSID& _MainServBTSID) {
    m_MainServBTSID = _MainServBTSID;
}

/*
SchedulerDL::SchedulerDL(ClusterID _ClusterID) {
    m_ClusterID = _ClusterID;
}
*/
//@threads
void Scheduling(BTSTxer* _pBTSTxer, vector<MSID>& _vActiveSet, HARQTxStateBTS& _HARQTxState, std::unordered_map<int, MSTxBufferBTS>& _mMSTxBuffer, SafeDeque<std::shared_ptr<SchedulingMessageDL> >& _qSchM){
    assert(false);
}

