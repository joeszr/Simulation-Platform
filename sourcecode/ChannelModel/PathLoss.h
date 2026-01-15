/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLoss.h
///@brief  PathLoss类声明
///
///包括构造函数，析构函数，计算DB值和线性值的函数声明，其中计算DB的函数声明为纯虚函数
///
///@author wangfei
#pragma once

namespace cm{
    /// 计算路径损耗的接口类
    class PathLoss{
    public:
        /// @brief 路径损耗的接口函数
        /// @param _dDisM 发送者与接收者之间的距离，单位米
        /// @return 以DB值表示的路径损耗
        virtual double Db( double _dDisM, double _dUEHeightM) = 0;
        /// @brief 以线性值计算路径损耗
        /// @param _dDisM 发送者与接收者之间的距离，单位米
        /// @return 以线性值表示的路径损耗
        double Linear( double _dDisM, double _dUEHeightM=1.5);
    public:
        /// @brief 构造函数
        PathLoss( void ) ;
        /// @breif 析构函数
        virtual ~PathLoss() =0;
    };
}
