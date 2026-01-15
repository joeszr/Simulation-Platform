/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossPico2UE_UrbanMicroNLOS.h
///@brief  PathLossPico2UE_UrbanMicroNLOS类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei

#pragma once
#include "PathLoss.h"

namespace cm {
    /// @brief 计算城区微蜂窝非直射径场景下的路径损耗

    class PathLossPico2UE_UrbanMicroNLOS final : public PathLoss {
    public:
        /// @brief 返回本场景下的路径损耗，dB值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossPico2UE_UrbanMicroNLOS() = default;
        /// @brief 析构函数
        ~PathLossPico2UE_UrbanMicroNLOS() override = default;
    };
}
