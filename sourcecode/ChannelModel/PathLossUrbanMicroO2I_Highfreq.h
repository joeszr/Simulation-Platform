///@file PathLossUrbanMicroO2I_Highfreq.h
///@brief  PathLossUrbanMicroO2I_Highfreq类声明
///@author wangfei
#include "PathLoss.h"

namespace cm {
    class PathLossUrbanMicroLOS_ModeB;
    class PathLossUrbanMicroNLOS_ModeB;

    /// @brief 城区微蜂窝室外到室内场景计算路径损耗的类

    class PathLossUrbanMicroO2I_Highfreq : public PathLoss {
    private:
        double m_dInDoorDisM;
        bool m_bIsLOS;
        PathLossUrbanMicroLOS_ModeB m_PLUMiLOS;
        PathLossUrbanMicroNLOS_ModeB m_PLUMiNLOS;
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
        //        PathLossUrbanMicroO2I(double _dInDoorDisM, bool _bIsLOS);
        PathLossUrbanMicroO2I_Highfreq(double _dInDoorDisM, bool _bIsLOS, int _iIslowloss, double _dPenetrationSFdb);
        /// @brief 析构函数
        ~PathLossUrbanMicroO2I_Highfreq() override = default;
    };
}
