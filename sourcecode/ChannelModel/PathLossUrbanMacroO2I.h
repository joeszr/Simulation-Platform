///@file PathLossUrbanMacroO2I.h
///@brief  PathLossUrbanMacroO2I类声明
///@author wangfei
#include "PathLoss.h"

namespace cm {
    class PathLossUrbanMacroLOS_ModeA;
    class PathLossUrbanMacroNLOS_ModeA;

    /// @brief 城区宏蜂窝室外到室内场景计算路径损耗的类

    class PathLossUrbanMacroO2I : public PathLoss {
    private:
        double m_dInDoorDisM;
        bool m_bIsLOS;
        PathLossUrbanMacroLOS_ModeA m_PLUMaLOS;
        PathLossUrbanMacroNLOS_ModeA m_PLUMaNLOS;
    public:
        /// @brief 设置为LOS
        void SetLOS(bool _bIsLOS);
        /// @brief 返回路径损耗的值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossUrbanMacroO2I(double _dInDoorDisM, bool _bIsLOS);
        /// @brief 析构函数
        ~PathLossUrbanMacroO2I() override = default;
    };
}
