///@file PathLossUrbanMicroO2I.h
///@brief  PathLossUrbanMicroO2I类声明
///@author wangfei
#include "PathLoss.h"

namespace cm {
    class PathLossUrbanMicroLOS_ModeA;
    class PathLossUrbanMicroNLOS_ModeA;

    /// @brief 城区微蜂窝室外到室内场景计算路径损耗的类

    class PathLossUrbanMicroO2I_ModeA : public PathLoss {
    private:
        double m_dInDoorDisM;
        bool m_bIsLOS;
        PathLossUrbanMicroLOS_ModeA m_PLUMiLOS;
        PathLossUrbanMicroNLOS_ModeA m_PLUMiNLOS;
    public:
        /// @brief 设置为LOS
        void SetLOS(bool _bIsLOS);
        /// @brief 返回路径损耗的值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossUrbanMicroO2I_ModeA(double _dInDoorDisM, bool _bIsLOS);
        /// @brief 析构函数
        ~PathLossUrbanMicroO2I_ModeA() override = default;
    };
}
