/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossPico2UE_UrbanMicroLOS.h
///@brief  PathLossPico2UE_UrbanMicroLOS类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei

#pragma once
#include "PathLoss.h"

namespace cm {
    /// @brief 城区微蜂窝直射径场景下计算路径损耗的类

    class PathLossPico2UE_UrbanMicroLOS final : public PathLoss {
    public:
        /// @brief 计算本场景下的路径损耗，dB值
        double Db(double _dDisM, double _dUEHeightM) final;
    public:
        /// @brief 构造函数
        PathLossPico2UE_UrbanMicroLOS() = default;
        /// @brief 析构函数
        ~PathLossPico2UE_UrbanMicroLOS() override = default;
    };
}
