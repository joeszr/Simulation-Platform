/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossIndoorHotspotLOS_Highfreq.h
///@brief  PathLossIndoorHotspotLOS_Highfreq类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei
#pragma once
#include "PathLoss.h"

namespace cm{
    ///PathLossIndoorHotspotLOS_Highfreq Class
    class PathLossIndoorHotspotLOS_ModeB final: public PathLoss{
    public:
        ///计算该场景下PathLoss的DB值
        double Db( double _dDisM , double _dUEHeightM) final;
    public:
        ///构造函数
        PathLossIndoorHotspotLOS_ModeB();
        ///析构函数
        ~PathLossIndoorHotspotLOS_ModeB() override = default;
    };
}
