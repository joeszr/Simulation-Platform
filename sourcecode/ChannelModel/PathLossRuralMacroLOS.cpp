///@file PathLossRuralMacroLOS.cpp
///@brief  PathLossRuralMacroLOS类定义
///@author wangfei

#include "PathLossRuralMacroLOS.h"
#include "P.h"

using namespace cm;
///计算DB值
double PathLossRuralMacroLOS_ModeA::Db( double _dDisM_3D , double _dUEHeightM){
   // assert( P::s().MacroTX.DAntennaHeightM == 35 );
    assert( P::s().RX.DAntennaHeightM == 1.5 );


    const static double dBreakPointM = 
            2 * M_PI* P::s().MacroTX.DAntennaHeightM* _dUEHeightM
            * P::s().FX.DRadioFrequencyMHz_Macro * 1e6 / 3e8;
    const static double dAveBuildHeightM = 5;
    double d2DisM = 
            sqrt(pow(_dDisM_3D,2)
            -pow((P::s().MacroTX.DAntennaHeightM-_dUEHeightM),2));

    //const static double dStreetWidthM = 20;
    //assert( 10 <= _dDisM && _dDisM <= 10000 );

    double result;

    if( d2DisM >= 10 && d2DisM <= dBreakPointM ){
        result = 20 * std::log10(40 * M_PI * _dDisM_3D 
                    * P::s().FX.DRadioFrequencyMHz_Macro / 1e3 / 3 ) 
                + std::min( 0.03 * pow( dAveBuildHeightM, 1.72 ), 10.0 ) 
                    * std::log10( _dDisM_3D ) 
                - std::min( 0.044 * pow( dAveBuildHeightM, 1.72 ), 14.77 ) 
                + 0.002 * log10( dAveBuildHeightM ) * _dDisM_3D;
    }else if ( d2DisM >= dBreakPointM && d2DisM <= 21000 ){
        result = 20 * std::log10( 40 * M_PI * dBreakPointM 
                    * P::s().FX.DRadioFrequencyMHz_Macro / 1e3 / 3 ) 
                + std::min( 0.03 * pow( dAveBuildHeightM, 1.72 ), 10.0 ) 
                    * std::log10( dBreakPointM ) 
                - std::min( 0.044 * pow( dAveBuildHeightM, 1.72 ), 14.77 ) 
                + 0.002 * log10( dAveBuildHeightM ) * dBreakPointM 
                + 40 * std::log10( _dDisM_3D / dBreakPointM );
    }else {
        assert (false);
    }

    return -1 * result;
}