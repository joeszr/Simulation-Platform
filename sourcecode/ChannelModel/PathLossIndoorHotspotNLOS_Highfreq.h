/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossIndoorHotspotNLOS_Highfreq.h
///@brief  PathLossIndoorHotspotNLOS_Highfreq类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei
#pragma once
#include "PathLoss.h"

namespace cm{
    ///PathLossIndoorHotspotNLOS_Highfreq Class
    class PathLossIndoorHotspotNLOS_ModeB final : public PathLoss{
    public:
        ///计算该场景下PathLoss的DB值
        double Db( double _dDisM , double _dUEHeightM) final;
    public:
        ///构造函数
        PathLossIndoorHotspotNLOS_ModeB() = default;
        ///析构函数
        ~PathLossIndoorHotspotNLOS_ModeB() override = default;
    };
}
