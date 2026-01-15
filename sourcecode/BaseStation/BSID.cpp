///@file BSID.cpp
///@brief  基类BSID的函数实现
///@author wangxiaozhou

#include "../Utility/IDType.h"
#include "../MobileStation/MSID.h"
#include "../BaseStation/BTSID.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../Utility/RBID.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "HARQRxStateBTS.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "HARQTxStateBTS.h"
#include "BTSRxer.h"
#include "BTS.h"
#include "BS.h"
#include "../NetworkDrive/BSManager.h"
#include "BSID.h"
bool operator==(const BSID& bsid1,const BSID& bsid2){
    return bsid1.ToInt()==bsid2.ToInt();
}

BSID::BSID(int _ib) : IDType(_ib) {
}

BS& BSID::GetBS() const{
    BS& bs = BSManager::Instance().GetBS(m_iID);
    return bs;
}

BSID BSID::Begin() {
    BSID result = BSID(0);
    return result;
}

BSID BSID::End() {
    BSID result = BSID(Parameters::Instance().BASIC.INumBSs-1);
    return result;
}
