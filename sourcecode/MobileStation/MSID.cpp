
#include "../Utility/IDType.h"
#include "../BaseStation/BTSID.h"
#include "./HARQRxStateMS.h"
#include "HARQTxStateMS.h"
#include "../DetectAlgorithm/Detector_DL.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../NetworkDrive/MSManager.h"
#include "MSID.h"

MS& MSID::GetMS() const{
    MS& ms = MSManager::Instance().GetMS(m_iID);
    return ms;
}

MSID::MSID(int _id) : IDType(_id) {
}

///

MSID MSID::Begin() {
    MSID result = MSID(0);
    return result;
}
///

MSID MSID::End() {
    MSID result = MSID(Parameters::Instance().BASIC.ITotalMSNum-1);
    return result;
}
