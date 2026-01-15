#include "../Utility/Include.h"
#include "../BaseStation/BTSID.h"
#include "../BaseStation/MSRxBufferBTS.h"
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
#include "../NetworkDrive/BSManager.h"
#include "DistributeMSAlgoDiamond.h"

void DistributeMSAlgoDiamond::DistributeMS(MSManager& _msm) {
    int iAllMsNum = Parameters::Instance().BASIC.ITotalMSNum;
    double dDis = Parameters::Instance().Macro.DSiteDistanceM;
    double dRadus = 0;
    double dorient = 0;
    double dx, dy;
    double dOffsetx = 0;
    double dOffsety = 0;
    switch(Parameters::Instance().BASIC.INumBSs){
        case 7:
            dRadus = sqrt(7) * dDis;
            dorient =  atan(M_SQRT3 / 5) - (2.0 / 3) * pi;
            dOffsetx = 0 ;
            dOffsety = 0;
            break;
        case 19:
            dRadus = sqrt(19) * dDis;
            dorient =  atan(3 * M_SQRT3 / 7) - (2.0 / 3) * pi;
            dOffsetx = 0;
            dOffsety = 0;
            break;
        default:
            break;
    }
    
    //2208sincos
    double temp_cos = cos(dorient);
    double temp_sin = sin(dorient);  
    
    for (int iMsNum = 0; iMsNum < iAllMsNum; ++iMsNum) {
        double rx = random.xUniform_distributems(-1, 0);
        double ry = random.xUniform_distributems(0, 1);
        double ytemp = dRadus * 0.5 * M_SQRT3 * ry;
        double xtemp = dRadus * rx + ytemp / M_SQRT3;
        dx = xtemp * temp_cos - ytemp * temp_sin + dOffsetx;
        dy = ytemp * temp_cos + xtemp * temp_sin + dOffsety;
        _msm.GetMS(iMsNum).SetXY(dx, dy);
    }
}
///单个MS撒点
void DistributeMSAlgoDiamond::DistributeMS(MS& _ms) {
    double dDis = Parameters::Instance().Macro.DSiteDistanceM;
    double dRadus = 0;
    double dorient = 0;
    double dx, dy;
    double dOffsetx = 0;
    double dOffsety = 0;
    switch(Parameters::Instance().BASIC.INumBSs){
        case 7:
            dRadus = sqrt(7) * dDis;
            dorient =  atan(M_SQRT3 / 5) - (2.0 / 3) * pi;
            dOffsetx = 0 ;
            dOffsety = 0;
            break;
        case 19:
            dRadus = sqrt(19) * dDis;
             dorient =  atan(3 * M_SQRT3 / 7) - (2.0 / 3) * pi;
            dOffsetx = 0 ;
            dOffsety = 0;
            break;
        default:
            break;
    }
    double rx = random.xUniform_distributems(-1, 0);
    double ry = random.xUniform_distributems(0, 1);
    double ytemp = dRadus * 0.5 * M_SQRT3 * ry;
    double xtemp = dRadus * rx + ytemp / M_SQRT3;
    dx = xtemp * cos(dorient) - ytemp * sin(dorient) + dOffsetx;
    dy = ytemp * cos(dorient) + xtemp * sin(dorient) + dOffsety;
    _ms.SetXY(dx, dy);
}
