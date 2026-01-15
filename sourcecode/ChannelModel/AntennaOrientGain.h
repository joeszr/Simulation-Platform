///@file AntennaOrientGain.h
///@brief 返回天线方向性增益的函数
///@author wangfei


#pragma once

namespace cm {

    /// @brief 天线方向性增益的接口类
    class AntennaOrientGain {
    public:
        /// @brief 计算天线的方向性增益
        /// @param _dAngleRad 发送者与接收者连线与发送者天线朝向的夹角，单位弧度
        /// @param _dDownTiltRad 发送天线的下倾角
        /// @return 天线的增益值，单位DB
        virtual double Db(double _dAngleRad, double _dDownTiltRad) = 0;
        /// @brief 计算天线的方向性增益
        /// @param _dAngleRad 发送者与接收者连线与发送者天线朝向的夹角，单位弧度
        /// @param _dDownTiltRad 发送天线的下倾角
        /// @return 天线的增益值，线性值
        virtual double Linear(double _dAngleRad, double _dDownTiltRad) = 0;
    public:
        /// @brief 构造函数
        AntennaOrientGain() = default;
        /// @brief 析构函数
        virtual ~AntennaOrientGain() = default;
    };
}