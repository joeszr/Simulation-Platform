///@file DistributeMSAlgoFile.cpp
///@brief 按照输入文件规定的点值撒点函数
///@author dushaofeng
#include "../Utility/Include.h"
#include "../Utility/IDType.h"
#include "../BaseStation/BTSID.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../Utility/RBID.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../BaseStation/BTSRxer.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/BTS.h"
#include "../BaseStation/BS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../NetworkDrive/MSManager.h"
#include "../NetworkDrive/BSManager.h"
#include "DistributeMSAlgoFile.h"
///@brief 按照输入文件规定的点值撒点函数
///
///@param _msm
///@param _bsm
///@return  返回值为空
void DistributeMSAlgoFile::DistributeMS(MSManager& _msm) {
    ///读取输入文件规定坐标的标点值
    ifstream fposx("./inputfiles/msposx.txt");
    ifstream fposy("./inputfiles/msposy.txt");
    assert(fposx);
    assert(fposy);
    vector<double> vx, vy;
    copy(istream_iterator<double>(fposx), istream_iterator<double>(), back_inserter(vx));
    copy(istream_iterator<double>(fposy), istream_iterator<double>(), back_inserter(vy));

    for (int i = 0; i < _msm.CountMS(); ++i) {
    ///设置移动台的点值
        _msm.GetMS(i).SetXY(vx[i], vy[i]);
    }
}

///单个MS撒点
void DistributeMSAlgoFile::DistributeMS(MS& _ms){
    cout<<"this is DistributeMSAlgoFile!"<<endl;
    assert(false);
}
