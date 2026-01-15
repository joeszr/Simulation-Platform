///@file DistributeMSAlgoHexagon.cpp
///@brief 六边形撒点函数
///@author dushaofeng

#include "../Utility/Include.h"
#include "../BaseStation/BTSID.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../DetectAlgorithm/Detector_DL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/BTS.h"
#include "../BaseStation/BS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../NetworkDrive/MSManager.h"
#include "../NetworkDrive/BSManager.h"
#include "DistributeMSAlgoHexagon.h"

///@brief 六边形撒点函数的实现
///
///在0号基站，0号BTS正对的小六边形范围内撒点，然后经过移位和旋转将这些点分布到其它的基站所对应的六边形中
///@param _msm
///@param _bsm
///@return  返回值为空

void DistributeMSAlgoHexagon::DistributeMS(MSManager& _msm) {
    ///为了适应BTS平均服务MS数是double类型的情况
    int iAllMS = _msm.CountMS();
    for (int ims = 0; ims < iAllMS; ims++) {
        DistributeMS(_msm.GetMS(ims));
    }
}
///单个MS撒点

void DistributeMSAlgoHexagon::DistributeMS(MS& _ms) {
    double x, y;
    double orient;
    int iBs = floor(random.xUniform_distributems(0, 1) * Parameters::Instance().BASIC.INumBSs);
    int iBts = floor(random.xUniform_distributems(0, 1) * Parameters::Instance().BASIC.IBTSPerBS);
    BTSID btsid = BTSID(iBs, iBts);
    BTS& bts = btsid.GetBTS();
    //
    double dx, dy;
    double rx = random.xUniform_distributems(-1, 0);
    double ry = random.xUniform_distributems(0, 1);
    double dRadus = Parameters::Instance().Macro.DCellRadiusM;
    double ytemp = dRadus * 0.5 * sqrt(3) * ry;
    double xtemp = dRadus * rx + ytemp / M_SQRT3;

//    double doffset1;
//    double doffset2;
    ///只有3扇区，为区分全向天线不同
    orient = floor(random.xUniform_distributems(0, 1) * 3) * 2 * M_PI / 3;
    dx = xtemp * cos(orient + M_PI / 6) - ytemp * sin(orient + M_PI / 6);
    dy = ytemp * cos(orient + M_PI / 6) + xtemp * sin(orient + M_PI / 6);
    ///
//    switch (iBts) {
//        case 0:
//            doffset1 = dRadus;
//            break;
//        case 1:
//            doffset1 = -1 * dRadus / 2;
//            doffset2 = dRadus * sqrt(3) / 2;
//            break;
//        case 2:
//            doffset1 = -1 * dRadus / 2;
//            doffset2 = -1 * dRadus * sqrt(3) / 2;
//            break;
//        default:
//            break;
//    }
    //x = bts.GetX() + dx + doffset1;
    //y = bts.GetY() + dy + doffset2;
    x = bts.GetX() + dx;
    y = bts.GetY() + dy;
    _ms.SetXY(x, y);
}

void DistributeMSAlgoHexagon::DistributeMS(MS& _ms, int _bsid, int _btsindex) {
    double x, y ;
    double orient;
    int iBs = _bsid;
    int iBts = _btsindex;
    BTSID btsid = BTSID(iBs, iBts);
    BTS& bts = btsid.GetBTS();
    //
    double dx, dy;
    double rx = random.xUniform_distributems(-1, 0);
    double ry = random.xUniform_distributems(0, 1);
    double dRadus = Parameters::Instance().Macro.DCellRadiusM;
    double ytemp = dRadus * 0.5 * sqrt(3) * ry;
    double xtemp = dRadus * rx + ytemp / M_SQRT3;
    //
    double doffset1 = 0;
    double doffset2 = 0;
    ///只有3扇区未区分全向天线
    orient = floor(random.xUniform_distributems(0, 1) * 3) * 2 * M_PI / 3;
    dx = xtemp * cos(orient) - ytemp * sin(orient);
    dy = ytemp * cos(orient) + xtemp * sin(orient);
    ///
    switch (iBts) {
        case 0:
            doffset1 = dRadus;
            break;
        case 1:
            doffset1 = -1 * dRadus / 2;
            doffset2 = dRadus * sqrt(3) / 2;
            break;
        case 2:
            doffset1 = -1 * dRadus / 2;
            doffset2 = -1 * dRadus * sqrt(3) / 2;
            break;
        default:
            break;
    }
    x = bts.GetX() + dx + doffset1;
    y = bts.GetY() + dy + doffset2;
    _ms.SetXY(x, y);
}