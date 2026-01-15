/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossIndoorHotspotNLOS.h
///@brief  PathLossIndoorHotspotNLOS类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei
#pragma once
#include "PathLoss.h"

namespace cm{
    ///PathLossIndoorHotspotNLOS Class
    class PathLossIndoorHotspotNLOS_ModeA final : public PathLoss{
    public:
        ///计算该场景下PathLoss的DB值
        double Db( double _dDisM , double _dUEHeightM) final;
    public:
        ///构造函数
        PathLossIndoorHotspotNLOS_ModeA() = default;
        ///析构函数
        ~PathLossIndoorHotspotNLOS_ModeA() override = default;
    };
}
