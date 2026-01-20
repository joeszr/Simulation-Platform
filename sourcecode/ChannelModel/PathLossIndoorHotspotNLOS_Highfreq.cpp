///@file PathLossIndoorHotspotNLOS_Highfreq.cpp
///@brief  PathLossIndoorHotspotNLOS_Highfreq类定义
///@author wangfei
#include "PathLossIndoorHotspotNLOS_Highfreq.h"
#include "P.h"

using namespace cm;
///构造函数
PathLossIndoorHotspotNLOS_ModeB::PathLossIndoorHotspotNLOS_ModeB( void ){
}
///计算DB值 发射节点和接收端节点之间的大尺度衰落
double PathLossIndoorHotspotNLOS_ModeB::Db(double _dDisM, double _dUEHeightM) {
    assert(3 <= P::s().MacroTX.DAntennaHeightM && P::s().MacroTX.DAntennaHeightM <= 6);
    assert(1 <= P::s().RX.DAntennaHeightM && P::s().RX.DAntennaHeightM <= 2.5);

    //low frequency
    //    static double a = 11.5 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
    //    return -1 * ( a + 43.3 * std::log10( _dDisM ) );

    //high frequency
    double LOS;
    double NLOS;
    static double a = 32.4 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
    LOS = -1 * (a + 17.3 * std::log10(_dDisM));
    static double b = 17.3 + 24.9 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
    NLOS = -1 * (b + 38.3 * std::log10(_dDisM));
    return std::min(LOS, NLOS);
    //static double a = 17.3 + 24.9 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
    //return -1 * (a + 38.3 * std::log10(_dDisM));

}