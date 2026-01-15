/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossUrbanMicroNLOS_Highfreq.h
///@brief  PathLossUrbanMicroNLOS_Highfreq类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei

#pragma once
#include "../ChannelModel/PathLoss.h"
#include "BS2RISPathLossUrbanMicroLOS_Highfreq.h"

namespace cm {
    /// @brief 计算城区微蜂窝非直射径场景下的路径损耗

    class BS2RISPathLossUrbanMicroNLOS_ModeB : public PathLoss {

    protected:
        BS2RISPathLossUrbanMicroLOS_ModeB dPLUMI_LOS;
    public:
        /// @brief 返回本场景下的路径损耗，dB值
        virtual double Db(double _dDisM, double _dUEHeightM = 1.5 );
    public:
        /// @brief 构造函数
        BS2RISPathLossUrbanMicroNLOS_ModeB(void);
        /// @brief 析构函数
        virtual ~BS2RISPathLossUrbanMicroNLOS_ModeB(void);
    };
}
