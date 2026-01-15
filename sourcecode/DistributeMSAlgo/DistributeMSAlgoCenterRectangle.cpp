///@file DistributeMSAlgoCenterRectangle.h
///@brief 中心小区矩形撒点函数实现
///@author wangxiaozhou
#include "../Utility/Include.h"
#include "../BaseStation/BTSID.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../Utility/RBID.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/BTS.h"
#include "../BaseStation/BS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../NetworkDrive/MSManager.h"
#include "../NetworkDrive/BSManager.h"
#include "DistributeMSAlgoCenterRectangle.h"
///中心小区矩形撒点，是移动台撒点类的继承类
////方法一
//void DistributeMSAlgoCenterRectangle::DistributeMS(MS& _ms) {
//    double x = 0, y = 0;
//    bool bOutOfRange = false;
//    double dRadus = Parameters::Instance().Macro.DCellRadiusM;
//    do {
//        x = xUniform_distributems(-1, 1)* dRadus ;
//        y = xUniform_distributems(-1, 1) * dRadus;
//        bOutOfRange = pow(x, 2) + pow(y, 2) < pow(Parameters::Instance().Macro.LINK.DMinDistanceM, 2);
//    } while (bOutOfRange);
//    _ms.SetXY(x, y);
//}

////方法二
void DistributeMSAlgoCenterRectangle::DistributeMS(MS& _ms) {
    double x, y;
    bool bOutOfRange = false;
    double dRadus = Parameters::Instance().Macro.DSiteDistanceM / 2;
    do {
        x = random.xUniform_distributems(-1, 1)* dRadus ;
        y = random.xUniform_distributems(-1, 1) * dRadus;
        bOutOfRange =((pow(x, 2) + pow(y, 2) < pow(Parameters::Instance().Macro.LINK.DMinDistanceM, 2))
                                 ||(pow(x, 2) + pow(y, 2) > pow(dRadus, 2)));
    } while (bOutOfRange);
    _ms.SetXY(x, y);
}

void DistributeMSAlgoCenterRectangle::DistributeMS(MSManager& _msm) {
    ///为了适应BTS平均服务MS数是double类型的情况
    int iAllMS = _msm.CountMS();
    for (int ims = 0; ims < iAllMS; ims++) {
        DistributeMS(_msm.GetMS(ims));
    }
}