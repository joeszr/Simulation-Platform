///@file AOGSector.cpp
///@brief 返回120度扇区化天线增益的函数
///@author wangfei

#include "P.h"
#include "./functions.h"
#include "AOGSector_inH_Circle.h"
using namespace cm;

/// constructor of class AOGSector, sectorized antenna orientation gain
AOGSector_inH_Circle::AOGSector_inH_Circle(double _d3DBBeamWidthHDeg, double _d3DBBeamWidthVDeg, double _dHBackLossDB, double _dVBackLossDB, double _dABackLossDB, double _dDownTiltDeg) {
    m_d3DBBeamWidthHDeg = _d3DBBeamWidthHDeg;
    m_d3DBBeamWidthVDeg = _d3DBBeamWidthVDeg;
    m_dHBackLossDB = _dHBackLossDB;
    m_dVBackLossDB = _dVBackLossDB;
    m_dABackLossDB = _dABackLossDB;
    m_dDownTiltDeg = _dDownTiltDeg;
}

///Calculate the antenna orientation gain in dB
///@param _dAngleRad    antenna  angle in rad
///@param _dDownTiltRad    antenna downtilt angle in rad
///@return  sectorized antenna orientation gain in d
///@see AntennaOrientGain ::Db()
double AOGSector_inH_Circle::Db(double _dAngleRad, double _dDownTiltRad) {
    double result1 = 0;
    ///适用于CaseC
//    double result2 = std::max(-12 * pow(((DEG2RAD(m_dDownTiltDeg) - _dDownTiltRad) * 180 / M_PI) / m_d3DBBeamWidthVDeg, 2.0), m_dVBackLossDB);
    ///适用于CaseA&CaseB
    double dDownTiltRad = abs(ConvergeAngle(_dDownTiltRad));
    double result2 = std::max(-12 * pow((dDownTiltRad * 180 / M_PI - 90) / m_d3DBBeamWidthVDeg, 2.0), m_dVBackLossDB);
    double result3 = std::max(result1 + result2, m_dABackLossDB);
    return result3;
}

///Calculate the antenna gain in linear value.
///@param _dAngleRad    antenna  angle in rad
///@param _dDownTiltRad    antenna downtilt angle in rad
///@return  sectorized antenna orientation gain  in linear value
///@see AntennaOrientGain ::Linear()
double AOGSector_inH_Circle::Linear(double _dAngleRad, double _dDownTiltRad) {
    return DB2L(Db(_dAngleRad, _dDownTiltRad));
}
