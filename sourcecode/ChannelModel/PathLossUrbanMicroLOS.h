/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossUrbanMicroLOS.h
///@brief  PathLossUrbanMicroLOS类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei

#pragma once
#include "PathLoss.h"

namespace cm {
    /// @brief 城区微蜂窝直射径场景下计算路径损耗的类

    class PathLossUrbanMicroLOS_ModeA : public PathLoss {
    public:
        /// @brief 计算本场景下的路径损耗，dB值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossUrbanMicroLOS_ModeA() = default;

        /// @brief 析构函数
        ~PathLossUrbanMicroLOS_ModeA() override = default;
    };
}
