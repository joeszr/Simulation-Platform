///@file PathLossUrbanMacroNLOS_Highfreq.h
///@brief  PathLossUrbanMacroNLOS_Highfreq类声明
///@author wangfei

#pragma once
#include "PathLoss.h"
#include "PathLossUrbanMacroLOS_Highfreq.h"

namespace cm {
    /// @brief 城区宏蜂窝非直射径场景下计算路径损耗的类

    class PathLossUrbanMacroNLOS_Highfreq : public PathLoss {
    public:
        /// @brief 计算路径损耗
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossUrbanMacroNLOS_Highfreq();
        /// @brief 析构函数
        ~PathLossUrbanMacroNLOS_Highfreq() override = default;
                
    protected:
        PathLossUrbanMacroLOS_Highfreq m_PL_LOS;
    };
}
