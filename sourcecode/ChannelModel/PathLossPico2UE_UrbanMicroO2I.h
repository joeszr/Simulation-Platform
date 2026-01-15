///@file PathLossPico2UE_UrbanMicroO2I.h
///@brief  PathLossPico2UE_UrbanMicroO2I类声明
///@author wangfei
#include "PathLoss.h"

namespace cm {
    class PathLossPico2UE_UrbanMicroLOS;
    class PathLossPico2UE_UrbanMicroNLOS;

    /// @brief 城区微蜂窝室外到室内场景计算路径损耗的类

    class PathLossPico2UE_UrbanMicroO2I : public PathLoss {
    private:
        double m_dInDoorDisM;
        bool m_bIsLOS;
        PathLossPico2UE_UrbanMicroLOS m_PLUMiLOS;
        PathLossPico2UE_UrbanMicroNLOS m_PLUMiNLOS;
    public:
        /// @brief 设置为LOS
        void SetLOS(bool _bIsLOS);
        /// @brief 返回路径损耗的值
        double Db(double _dDisM, double _dUEHeightM) override;
    public:
        /// @brief 构造函数
        PathLossPico2UE_UrbanMicroO2I(double _dInDoorDisM, bool _bIsLOS);
        /// @brief 析构函数
        ~PathLossPico2UE_UrbanMicroO2I() override = default;
    };
}
