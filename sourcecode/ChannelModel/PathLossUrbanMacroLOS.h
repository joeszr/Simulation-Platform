///@file PathLossUrbanMacroLOS.h
///@brief  PathLossUrbanMacroLOS类声明
///@author wangfei

#pragma once
#include "PathLoss.h"
#include "PathLossUrbanMacroLOS_Highfreq.h"

namespace cm {
    /// @brief 计算城区宏蜂窝场景下直射径路径损耗的类

    class PathLossUrbanMacroLOS_ModeA : public PathLoss {
    public:
        /// @brief 计算本场景下的路径损耗，dB值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossUrbanMacroLOS_ModeA()= default;
        /// @brief 析构函数
        ~PathLossUrbanMacroLOS_ModeA() override = default;
        
    protected:
       PathLossUrbanMacroLOS_Highfreq m_PL_LOS_5GUMA;  

    };
}