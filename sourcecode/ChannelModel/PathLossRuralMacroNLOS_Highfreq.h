///20171219

#pragma once
#include "PathLoss.h"
#include "PathLossRuralMacroNLOS_Highfreq.h"
#include "PathLossRuralMacroLOS_Highfreq.h"
#include "PathLossRuralMacroNLOS.h"

namespace cm {
    /// @brief 城区宏蜂窝非直射径场景下计算路径损耗的类

    class PathLossRuralMacroNLOS_ModeB : public PathLoss {
    protected:
        PathLossRuralMacroLOS_ModeB PL_RMa_LOS_ModelB;
//        PathLossRuralMacroNLOS_ModeA PL_RMa_NLOS_ModelA;
    public:
        /// @brief 计算路径损耗
        double Db(double _dDisM, double _dUEHeightM);
    public:
        /// @brief 构造函数
        PathLossRuralMacroNLOS_ModeB() = default;
        /// @brief 析构函数
        ~PathLossRuralMacroNLOS_ModeB() override = default;
    };
}
