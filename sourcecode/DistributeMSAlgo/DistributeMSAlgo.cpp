//// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file DistributeMSAlgo.cpp
///@brief  实现移动台撒点的函数
///
///@author dushaofeng

#include "DistributeMSAlgo.h"


#include "../Utility/Include.h"
#include "../BaseStation/BTSID.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/BTS.h"
#include "../BaseStation/BS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../NetworkDrive/MSManager.h"
#include "../NetworkDrive/BSManager.h"

void DistributeMSAlgo::DistributeMS(MS& _ms, int _bsid, int _btsindex){
    
}

// 20171204
void DistributeMSAlgo::DistributeMS_with_distance_check(MS& _ms) {
    
//    m_pDMSA->DistributeMS(_ms);
    
    // 20171204
    bool bDropValid = false;
    while (!bDropValid) {
        this->DistributeMS(_ms);
        for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
            BTS& curBTS = btsid.GetBTS();
            
            cm::Point tx_wrap;
            
//            cm::Point ms_wrap;
            
            if (BSManager::IsMacro(btsid)) {
                tx_wrap = cm::WrapAround::Instance().WrapTx(_ms, curBTS);
//                ms_wrap = cm::WrapAround::Instance().WrapRx(_ms, curBTS);
            } else {
                tx_wrap = cm::WrapAround::Instance().WrapTx(_ms, BSManager::GetMacroIDofPico(btsid).GetBTS());
//                ms_wrap = cm::WrapAround::Instance().WrapRx(
//                    _ms, BSManager::GetMacroIDofPico(btsid).GetBTS());
            }
            
            double dDis_2D = cm::Distance(tx_wrap, _ms);
//            double dDis_2D = cm::Distance(curBTS, ms_wrap);
            
            assert(Parameters::Instance().BASIC.IMsDistributeMode 
                    != Parameters::DistributeMS_CenterRetangle);
            
            ///xlong:根据基站类型选择最小限制距离
            double dMinDist = (BSManager::IsMacro(btsid)) ? 
                Parameters::Instance().Macro.LINK.DMinDistanceM 
                : Parameters::Instance().SmallCell.LINK.DMinDistPico2UEM;
            
            if (dDis_2D < dMinDist) {
                bDropValid = false;
                break;
            } else {
                bDropValid = true;
            }
        }
    }
}

