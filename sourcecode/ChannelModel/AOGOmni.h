///@file  AOGOmni.h
///@brief 返回全向天线增益的函数
///@author wangfei

#pragma once
#include "AntennaOrientGain.h"

namespace cm {

    /// @brief 全向天线类
    class AOGOmni final: public AntennaOrientGain {
    public:
        ///return the antenna orientation gain in dB
        double Db(double _dAngleRad, double _dDownTiltRad) final;
        ///return the antenna gain in linear value.
        double Linear(double _dAngleRad, double _dDownTiltRad) final;
    public:
        ///constructor of class AOGOmni
        AOGOmni() = default;
        ///destructor of class AOGOmni
        ~AOGOmni() override = default;
    } ;
}