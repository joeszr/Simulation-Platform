
#include "../Utility/IDType.h"
#include "../MobileStation/MSID.h"
#include "../BaseStation/BTSID.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/BTSRxer.h"
#include "SchedulerUL.h"

SchedulerUL::SchedulerUL(const BTSID& mainServBTSID) {
    m_MainServBTSID = mainServBTSID;
}

void SchedulerUL::SetRxer(const std::shared_ptr<BTSRxer>& pRxer) {
    m_pRxer = pRxer;
}
