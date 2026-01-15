///@file PathLossUrbanMacroNLOS.h
///@brief  PathLossUrbanMacroNLOS类声明
///@author wangfei

#pragma once
#include "PathLoss.h"
#include "PathLossUrbanMacroLOS.h"

namespace cm {
    /// @brief 城区宏蜂窝非直射径场景下计算路径损耗的类

    class PathLossUrbanMacroNLOS_ModeA : public PathLoss {
    public:
        /// @brief 计算路径损耗
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossUrbanMacroNLOS_ModeA() = default;
        /// @brief 析构函数
        ~PathLossUrbanMacroNLOS_ModeA() override= default;
        
    protected:
       PathLossUrbanMacroLOS_ModeA m_PL_LOS;  
    };
}
