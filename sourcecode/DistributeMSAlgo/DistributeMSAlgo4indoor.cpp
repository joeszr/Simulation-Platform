/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file DistributeMSAlgo4indoor.cpp
///@brief 室内移动台撒点函数
///
///@author dushaofeng
#include "../Utility/Include.h"
#include "../BaseStation/BTSID.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../DetectAlgorithm/Detector_DL.h"
#include "../BaseStation/BTS.h"
#include "../BaseStation/BS.h"
#include "../NetworkDrive/BSManager.h"
#include "../NetworkDrive/MSManager.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "DistributeMSAlgo4indoor.h"

///@brief 室内移动台撒点算法的成员函数
///
///@param _msm
///@param _bsm
///@return  返回值为空
void DistributeMSAlgo4indoor::DistributeMS(MSManager& _msm) {

    ///为了适应BTS平均服务MS数是double类型的情况
    int iAllMS = _msm.CountMS();
    for(int ims = 0;ims < iAllMS;ims++){
        DistributeMS(_msm.GetMS(ims));
    }
}

///单个MS撒点
void DistributeMSAlgo4indoor::DistributeMS(MS& _ms){


    if (12 == Parameters::Instance().BASIC.INumBSs) {
//        assert(3 == Parameters::Instance().BASIC.IBTSPerBS);
    } else {
        assert(2 == Parameters::Instance().BASIC.INumBSs|| 4 == Parameters::Instance().BASIC.INumBSs|| 8 == Parameters::Instance().BASIC.INumBSs);
        assert(1 == Parameters::Instance().BASIC.IBTSPerBS);
    }
    double x , y;
    bool flag = true;
    do {
        flag = true;
        double rx = random.xUniform_distributems(-60, 60);
        double ry = random.xUniform_distributems(-25, 25);
        x = rx;
        y = ry;
        _ms.SetXY(x,y);
        for(BTSID btsid = BTSID::Begin(); btsid<=BTSID::End();++btsid){
            if(cm::Distance(btsid.GetBTS(),_ms)<Parameters::Instance().Macro.LINK.DMinDistanceM){
                flag = false;
                break;
            }
        }
    }while(!flag);
    if(8 == Parameters::Instance().BASIC.INumBSs){
        int iFloorNum = random.xUniform_distributems(-1, 1)<0?0:1;
        double dHeight = iFloorNum * Parameters::Instance().Macro.DAntennaHeightM+Parameters::Instance().MSS.DAntennaHeightM;
        _ms.SetRxHeightM(dHeight);
    }
    Observer::Print("IndoorUEPos")<<x<<setw(20)<<y<<endl;
}
