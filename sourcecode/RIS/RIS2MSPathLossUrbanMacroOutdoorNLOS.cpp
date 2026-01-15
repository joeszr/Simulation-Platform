#include "RIS2MSPathLossUrbanMacroOutdoorNLOS.h"
#include "../ChannelModel/P.h"
#include "../Parameters/Parameters.h"

using namespace cm;
///构造函数
RIS2MSPathLossUrbanMacroOutdoorNLOS::RIS2MSPathLossUrbanMacroOutdoorNLOS( void ){
}
///析构函数
RIS2MSPathLossUrbanMacroOutdoorNLOS::~RIS2MSPathLossUrbanMacroOutdoorNLOS( void ){
}
///计算DB值
double RIS2MSPathLossUrbanMacroOutdoorNLOS::Db( double _dDisM_3D, double _dUEHeightM){
    //    assert( Parameters.Instance().Macro.LINK.DRadioFrequencyMHz == 2000 );
    //    //F=2GHz
    //    static double a = 145.4 + 37.5 * std::log10( _dDisM );
    //    return -1 * a;

    //    assert( P::s().MacroTX.DAntennaHeightM == 25 );
    //    assert( P::s().RX.DAntennaHeightM == 1.5 );

    assert(_dUEHeightM <= 22.5 && _dUEHeightM >= 1.5);

    double d2DisM = sqrt(pow(_dDisM_3D, 2) - pow((Parameters::Instance().RIS.DAntennaHeightM - _dUEHeightM), 2));

    assert( (d2DisM >= 10.0) && (d2DisM <= 5000));

    assert(Parameters::Instance().MIMO_CTRL.I2Dor3DMIMO == Parameters::IS3DCHANNEL);

    double dPL_LOS = m_PL_LOS.Db(_dDisM_3D, _dUEHeightM);

    //再计算NLOS时候的值
    double c  = 13.54 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
             - 0.6 * (_dUEHeightM - 1.5);
    double dPL_NLOS_dot = -1 * (c + 39.08 * std::log10(_dDisM_3D));

    double dPLb = std::min(dPL_LOS, dPL_NLOS_dot);

    return dPLb;

}