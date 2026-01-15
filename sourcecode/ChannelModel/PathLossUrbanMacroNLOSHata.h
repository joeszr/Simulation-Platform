///@file PathLossUrbanMacroNLOSHata.h
///@brief  PathLossUrbanMacroNLOSHata类声明
///@author wangfei
#pragma once
#include "PathLoss.h"

namespace cm {
    /// @brief 城区宏蜂窝非直射径场景下用Hata模型计算路径损耗的类

    class PathLossUrbanMacroNLOSHata : public PathLoss {
    public:
        ///  @brief 计算本场景下的路径损耗
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossUrbanMacroNLOSHata() = default;
        /// @brief 析构函数
        ~PathLossUrbanMacroNLOSHata() override = default;
    };
}