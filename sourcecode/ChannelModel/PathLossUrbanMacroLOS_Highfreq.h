///@file PathLossUrbanMacroLOS_Highfreq.h
///@brief  PathLossUrbanMacroLOS_Highfreq类声明
///@author wangfei

#pragma once
#include "PathLoss.h"
#include "../Utility/Random.h"
namespace cm {
    /// @brief 计算城区宏蜂窝场景下直射径路径损耗的类

    class PathLossUrbanMacroLOS_Highfreq : public PathLoss {
    public:
        Random random;
        /// @brief 计算本场景下的路径损耗，dB值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossUrbanMacroLOS_Highfreq() = default;
        /// @brief 析构函数
        ~PathLossUrbanMacroLOS_Highfreq() override = default;
    };
}