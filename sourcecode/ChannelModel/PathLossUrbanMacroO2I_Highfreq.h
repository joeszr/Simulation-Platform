///@file PathLossUrbanMacroO2I_Highfreq.h
///@brief  PathLossUrbanMacroO2I类声明
///@author wangfei
#include "PathLoss.h"

namespace cm {
    class PathLossUrbanMacroLOS_Highfreq;
    class PathLossUrbanMacroNLOS_Highfreq;

    /// @brief 城区宏蜂窝室外到室内场景计算路径损耗的类

    class PathLossUrbanMacroO2I_Highfreq : public PathLoss {
    private:
        double m_dInDoorDisM;
        bool m_bIsLOS;
        PathLossUrbanMacroLOS_Highfreq m_PLUMaLOS;
        PathLossUrbanMacroNLOS_Highfreq m_PLUMaNLOS;
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
        PathLossUrbanMacroO2I_Highfreq(double _dInDoorDisM, bool _bIsLOS, int _iIslowloss, double _dPenetrationSFdb);
        /// @brief 析构函数
        ~PathLossUrbanMacroO2I_Highfreq() override =default;
    };
}
