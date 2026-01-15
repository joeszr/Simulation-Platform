///@file PathLossUrbanMacroO2I_Highfreq.h
///@brief  PathLossUrbanMacroO2I类声明
///@author wangfei
#include "PathLoss.h"
#include "PathLossRuralMacroLOS_Highfreq.h"
#include "PathLossRuralMacroNLOS_Highfreq.h"
namespace cm {
    /// @brief 城区宏蜂窝室外到室内场景计算路径损耗的类

    class PathLossRuralMacroO2I_ModeB final : public PathLoss {
    private:
        double m_dInDoorDisM;
        bool m_bIsLOS;
        PathLossRuralMacroLOS_ModeB m_PLRMaLOS;
        PathLossRuralMacroNLOS_ModeB m_PLRMaNLOS;
        //zhengyi
        int m_iIsLowloss;
        double m_dPenetrationSFdb;
    public:
        /// @brief 设置为LOS
        void SetLOS(bool _bIsLOS);
        /// @brief 返回路径损耗的值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        //        PathLossUrbanMacroO2I(double _dInDoorDisM, bool _bIsLOS);
        PathLossRuralMacroO2I_ModeB(double _dInDoorDisM, bool _bIsLOS, int _iIslowloss, double _dPenetrationSFdb);
        /// @brief 析构函数
        ~PathLossRuralMacroO2I_ModeB() override = default;
    };
}
