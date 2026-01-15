/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossUrbanMicroLOS_Highfreq.h
///@brief  PathLossUrbanMicroLOS_Highfreq类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei

#pragma once
#include "../ChannelModel/PathLoss.h"

namespace cm {
    /// @brief 城区微蜂窝直射径场景下计算路径损耗的类

    class BS2RISPathLossUrbanMicroLOS_ModeB : public PathLoss {
    public:
        /// @brief 计算本场景下的路径损耗，dB值
        double Db(double _dDisM, double _dUEHeightM = 1.5 );
    public:
        /// @brief 构造函数
        BS2RISPathLossUrbanMicroLOS_ModeB(void);
        /// @brief 析构函数
        ~BS2RISPathLossUrbanMicroLOS_ModeB(void);
    };
}
