///@file PathLossUrbanMicroNLOS.cpp
///@brief  PathLossUrbanMicroNLOS类定义
///@author wangfei

#include "P.h"
#include "./PathLossUrbanMicroNLOS.h"
#include "../Parameters/Parameters.h"
using namespace cm;
///计算DB值
double PathLossUrbanMicroNLOS_ModeA::Db( double _dDisM_3D , double _dUEHeightM){
    if ( cm::P::s().IChannelModel_for_Scenario ==  P::_5GCM_TWO_LAYER ) {
        assert( Parameters::Instance().SmallCell.LINK.DPicoAntennaHeightM == 10);
    } else {
        assert( P::s().MacroTX.DAntennaHeightM == 10 );
    }
//    assert( P::s().TX.DAntennaHeightM == 10 );
    assert( 1 <= P::s().RX.DAntennaHeightM && P::s().RX.DAntennaHeightM <= 2.5 );
    double d2DisM 
            = sqrt(pow(_dDisM_3D,2)
                -pow((P::s().MacroTX.DAntennaHeightM-_dUEHeightM),2));
    assert(d2DisM>10 && d2DisM < 2000);
    //20171221 此处有疑问
    const static double a 
        = 22.7 + 26 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
    double PL_dot_NLOS = 
        -1 * ( a + 36.7 * std::log10( _dDisM_3D ) -0.3*(_dUEHeightM - 1.5));
    
    double PL_LOS = PL_UMi_LOS.Db(_dDisM_3D,_dUEHeightM);
    
    double PL_NLOS = std::min(PL_LOS,PL_dot_NLOS);
    
    return PL_NLOS;
}