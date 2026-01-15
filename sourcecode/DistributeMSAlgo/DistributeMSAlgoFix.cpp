///@file DistributeMSAlgoFix.cpp
///@brief 按照定点的方式撒点的函数
///@author dushaofeng

#include "../Parameters/Parameters.h"
#include "../BaseStation/BTSID.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../Utility/RBID.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../DetectAlgorithm/Detector_DL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/BTS.h"
#include "../BaseStation/BS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../NetworkDrive/MSManager.h"
#include "DistributeMSAlgoFix.h"

///@brief 按照定点的方式撒点的算法
///
///@param _msm
///@param _bsm
///@return  返回值为空

void DistributeMSAlgoFix::DistributeMS(MSManager &_msm) {
    for (int i = 0; i < _msm.CountMS(); ++i) {
        int j = i % Parameters::Instance().BASIC.INumBSs;
        BSID bsid = BSID(j);
        BS& bs = bsid.GetBS();
        double x = bs.GetX() + 30;
        double y = bs.GetY();
        _msm.Instance().GetMS(i).SetXY(x, y);
    }
}

///单个MS撒点
void DistributeMSAlgoFix::DistributeMS(MS& _ms){

    double x = 0 ;
    double y = 0 ;
    _ms.SetXY(x,y);
}
