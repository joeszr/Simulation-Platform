///@file PathLossUrbanMacroNLOSHata.cpp
///@brief  PathLossUrbanMacroNLOSHata类定义
///@author wangfei
#include "PathLossUrbanMacroNLOSHata.h"
#include "P.h"
using namespace cm;

///计算DB值
double PathLossUrbanMacroNLOSHata::Db( double _dDisM , double _dUEHeightM){
    static const double dAveBuildHeightM = 15;
    static const double dDeltaM = P::s().MacroTX.DAntennaHeightM - dAveBuildHeightM;
    static const double b = 40 * ( 1 - 4 * dDeltaM / 1000 );
    static const double a = -18 * std::log10( dDeltaM ) + 21 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro ) + 80.0;
    double result = a + b* std::log10( _dDisM / 1000 );
    return -1 * result;
}