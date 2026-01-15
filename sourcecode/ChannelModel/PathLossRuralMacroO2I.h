///20171218
#include "PathLoss.h"
#include "PathLossRuralMacroLOS.h"
#include "PathLossRuralMacroNLOS.h"
namespace cm {

    /// @brief 城区宏蜂窝室外到室内场景计算路径损耗的类

    class PathLossRuralMacroO2I : public PathLoss {
    private:
        double m_dInDoorDisM;
        bool m_bIsLOS;
        PathLossRuralMacroLOS_ModeA m_PLRMaLOS;
        PathLossRuralMacroNLOS_ModeA m_PLRMaNLOS;
    public:
        /// @brief 设置为LOS
        void SetLOS(bool _bIsLOS);
        /// @brief 返回路径损耗的值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossRuralMacroO2I(double _dInDoorDisM, bool _bIsLOS);
        /// @brief 析构函数
        ~PathLossRuralMacroO2I() override = default;
    };
}
