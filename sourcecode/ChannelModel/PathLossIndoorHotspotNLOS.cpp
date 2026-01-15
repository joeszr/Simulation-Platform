///@file PathLossIndoorHotspotNLOS.cpp
///@brief  PathLossIndoorHotspotNLOS类定义
///@author wangfei
#include "PathLossIndoorHotspotNLOS.h"
#include "P.h"

using namespace cm;

///计算DB值
double PathLossIndoorHotspotNLOS_ModeA::Db( double _dDisM , double _dUEHeightM){
    assert( 3 <= P::s().MacroTX.DAntennaHeightM && P::s().MacroTX.DAntennaHeightM <= 6 );
    assert( 1 <= P::s().RX.DAntennaHeightM && P::s().RX.DAntennaHeightM <= 2.5 );

    static double a = 11.5 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
    return -1 * ( a + 43.3 * std::log10( _dDisM ) );
}