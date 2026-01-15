///@file AOGSector.h
///@brief 返回120度扇区化天线增益的函数
///@author wangfei

#pragma once
#include "AntennaOrientGain.h"

namespace cm {
    
    /// @brief 120度扇区化天线
    class AOGSector final: public AntennaOrientGain {
    private:
        /// 天线的水平向3dB带宽
        double m_d3DBBeamWidthHDeg; 
        /// 天线的垂直向3dB带宽
        double m_d3DBBeamWidthVDeg; 
        /// 水平方向的背向损耗
        double m_dHBackLossDB; 
        /// 垂直方向的背向损耗
        double m_dVBackLossDB; 
        /// 总体背向损耗
        double m_dABackLossDB; 
        /// 天线的下倾角
        double m_dDownTiltDeg; 
    public:
        ///return the antenna orientation gain in dB
        double Db(double _dAngleRad, double _dDownTiltRad) final;
        ///return the antenna gain in linear value.
        double Linear(double _dAngleRad, double _dDownTiltRad) final;
    public:
        /// constructor of class AOGSector
        AOGSector(double _d3DBBeamWidthHDeg, double _d3DBBeamWidthVDeg, double _dHBackLossDB, double _dVBackLossDB, double _dABackLossDB, double _dDownTiltDeg);
        /// destructor of class AOGSector
        ~AOGSector() override = default;
    };
}