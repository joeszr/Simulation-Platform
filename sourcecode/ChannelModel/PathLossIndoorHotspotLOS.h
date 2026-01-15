/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossIndoorHotspotLOS.h
///@brief  PathLossIndoorHotspotLOS类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei
#pragma once
#include "PathLoss.h"

namespace cm{
    ///PathLossIndoorHotspotLOS Class
    class PathLossIndoorHotspotLOS_ModeA final: public PathLoss{
    public:
        ///计算该场景下PathLoss的DB值
        double Db( double _dDisM , double _dUEHeightM) override;
    public:
        ///构造函数
        PathLossIndoorHotspotLOS_ModeA() = default;
        ///析构函数
        ~PathLossIndoorHotspotLOS_ModeA() override = default;
    };
}
