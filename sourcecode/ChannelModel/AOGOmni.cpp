///@file AOGOmni.cpp
///@brief 返回全向天线增益的函数
///@author wangfei

#include "../Utility/Include.h"
#include "AOGOmni.h"
using namespace cm;
///Calculate the antenna orientation gain in dB
///@param _dAngleRad    antenna  angle in rad
///@param _dDownTiltRad    antenna downtilt angle in rad
///@return 0
///@see AntennaOrientGain ::Db()

double AOGOmni::Db(double _dAngleRad, double _dDownTiltRad) {
    ///this is omni antenna, the antenna gain is 0dB
    return 0;
}

///Calculate the antenna gain in linear value.
///@param _dAngleRad    antenna  angle in rad
///@param _dDownTiltRad    antenna downtilt angle
///@return the antenna gain in linear value
///@see AntennaOrientGain ::Linear()

double AOGOmni::Linear(double _dAngleRad, double _dDownTiltRad) {
    return DB2L(Db(_dAngleRad, _dDownTiltRad));
}
