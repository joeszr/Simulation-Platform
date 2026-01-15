/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossUrbanMicroNLOS.h
///@brief  PathLossUrbanMicroNLOS类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei

#pragma once
#include "PathLoss.h"
#include "PathLossUrbanMicroLOS.h"

namespace cm {
    /// @brief 计算城区微蜂窝非直射径场景下的路径损耗

    class PathLossUrbanMicroNLOS_ModeA : public PathLoss {
    protected:
        PathLossUrbanMicroLOS_ModeA PL_UMi_LOS;
    public:
        /// @brief 返回本场景下的路径损耗，dB值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossUrbanMicroNLOS_ModeA() = default;
        /// @brief 析构函数
        ~PathLossUrbanMicroNLOS_ModeA() override = default;
    };
}
