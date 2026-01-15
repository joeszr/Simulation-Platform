///@file PathLossIndoorHotspotLOS_Highfreq.cpp
///@brief  PathLossIndoorHotspotLOS_Highfreq类定义
///@author wangfei
#include "PathLossIndoorHotspotLOS_Highfreq.h"
#include "P.h"

using namespace cm;

///计算DB值
double PathLossIndoorHotspotLOS_ModeB::Db( double _dDisM , double _dUEHeightM){
    //assert( _dDisM <= 100 );
    assert( 3 <= P::s().MacroTX.DAntennaHeightM && P::s().MacroTX.DAntennaHeightM <= 6 );
    assert( 1 <= P::s().RX.DAntennaHeightM && P::s().RX.DAntennaHeightM <= 2.5 );

    //low freqeuncy
    //    static double a = 32.8 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
    //    return -1 * (a + 16.9 * std::log10(_dDisM));

    //high frequency
    static double a = 32.4 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
    return -1 * (a + 17.3 * std::log10(_dDisM));

}