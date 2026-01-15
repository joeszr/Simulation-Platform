//20171219

#pragma once
#include "PathLoss.h"
#include "PathLossRuralMacroLOS.h"

namespace cm {
    /// @brief 计算城区宏蜂窝场景下直射径路径损耗的类

    class PathLossRuralMacroLOS_ModeB final: public PathLoss {
    protected:
        PathLossRuralMacroLOS_ModeA PL_RMa_LOS;
    public:
        /// @brief 计算本场景下的路径损耗，dB值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossRuralMacroLOS_ModeB() = default;
        /// @brief 析构函数
        ~PathLossRuralMacroLOS_ModeB() override = default;
    };
}