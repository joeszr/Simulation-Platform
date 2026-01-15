///@file PathLossUrbanMacroNLOS.cpp
///@brief  PathLossUrbanMacroNLOS类定义
///@author wangfei

#include "PathLossUrbanMacroNLOS.h"
#include "P.h"

using namespace cm;
// 20171204
///计算DB值
double PathLossUrbanMacroNLOS_ModeA::Db( double _dDisM_3D , double _dUEHeightM){
    assert( P::s().MacroTX.DAntennaHeightM == 25 );
    assert( _dUEHeightM >= 1.5 && _dUEHeightM <= 22.5 );    
    
    double d2DisM = sqrt(pow(_dDisM_3D, 2) - pow((P::s().MacroTX.DAntennaHeightM - _dUEHeightM), 2));

    assert( (d2DisM >= 10.0) && (d2DisM <= 5000));
        
    double dPL_LOS = m_PL_LOS.Db(_dDisM_3D, _dUEHeightM);
    
    //再计算NLOS时候的值
    const static double dAveBuildHeightM = 20;  // h
    const static double dStreetWidthM = 20;     // W
    
/*
    const static double NLOS_a 
        = 161.04 - 7.1 * std::log10( dStreetWidthM ) 
            + 7.5 * std::log10( dAveBuildHeightM ) 
            - ( 24.37 - 3.7 * pow( dAveBuildHeightM / P::s().MacroTX.DAntennaHeightM, 2.0 ) ) * std::log10( P::s().MacroTX.DAntennaHeightM ) 
            + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 ) 
            - 3.2 * pow( std::log10( 17.625), 2.0) + 4.97  
            - 0.6 * (_dUEHeightM - 1.5 ) ;
*/
    //20180517
    const static double NLOS_a 
        = 161.04 - 7.1 * std::log10( dStreetWidthM ) 
            + 7.5 * std::log10( dAveBuildHeightM ) 
            - ( 24.37 - 3.7 * pow( dAveBuildHeightM / P::s().MacroTX.DAntennaHeightM, 2.0 ) ) * std::log10( P::s().MacroTX.DAntennaHeightM ) 
            + 31.5 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 ) 
            - 3.2 * pow( std::log10( 17.625), 2.0) + 4.97  
            - 0.6 * (_dUEHeightM - 1.5 ) ;
    double dPL_NLOS_dot 
        =  -1 * ( NLOS_a 
            + ( 43.42 - 3.1 * std::log10( P::s().MacroTX.DAntennaHeightM ) ) 
                * ( std::log10( _dDisM_3D ) - 3.0 ) );

    double dPLb = std::min(dPL_LOS, dPL_NLOS_dot);    
    
    return dPLb;
}

/////计算DB值
//double PathLossUrbanMacroNLOS::Db( double _dDisM , double _dUEHeightM){
//    assert( P::s().MacroTX.DAntennaHeightM == 25 );
//    assert( P::s().RX.DAntennaHeightM == 1.5 );
//
//
//    const static double dEffectNBHeightM = P::s().MacroTX.DAntennaHeightM - 1;
//    const static double dEffectUEHeightM = P::s().RX.DAntennaHeightM - 1;
//    const static double dEffectBreakPointM = 4 * dEffectNBHeightM* dEffectUEHeightM* P::s().FX.DRadioFrequencyMHz_Macro * 1e6 / 3e8;
//    double d2DisM = sqrt(pow(_dDisM,2)-pow((P::s().MacroTX.DAntennaHeightM-_dUEHeightM),2));
//    double LOS_PL = 0;
//    double NLOS_PL = 0;
//    if( d2DisM <= dEffectBreakPointM ){
//        const static double LOS_a = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
//         LOS_PL = -1 * ( LOS_a + 22.0 * std::log10( _dDisM ) );
//    }else if( dEffectBreakPointM <= d2DisM && d2DisM <= 5000 ){
//        const static double LOS_b = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 ) - 9 * std::log10(pow(dEffectBreakPointM,2) + pow((dEffectNBHeightM - dEffectUEHeightM),2));//36.873
//        LOS_PL =  -1 * ( LOS_b + 40.0 * std::log10( _dDisM ) );
//    }
//
//    const static double dAveBuildHeightM = 20;
//    const static double dStreetWidthM = 20;
//    //const static double a = 161.04 - 7.1 * std::log10( dStreetWidthM ) + 7.5 * std::log10( dAveBuildHeightM ) - ( 24.37 - 3.7 * pow( dAveBuildHeightM / P::s().MacroTX.DAntennaHeightM, 2.0 ) ) * std::log10( P::s().MacroTX.DAntennaHeightM ) + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 ) - 3.2 * pow( std::log10( 11.75 * P::s().RX.DAntennaHeightM ), 2.0 ) + 4.97;
//    const static double NLOS_a = 161.04 - 7.1 * std::log10( dStreetWidthM ) + 7.5 * std::log10( dAveBuildHeightM ) - ( 24.37 - 3.7 * pow( dAveBuildHeightM / P::s().MacroTX.DAntennaHeightM, 2.0 ) ) * std::log10( P::s().MacroTX.DAntennaHeightM ) + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 ) - 3.2 * pow( std::log10( 17.625), 2.0)+ 4.97  - 0.6* (P::s().RX.DAntennaHeightM- 1.5 ) ;
//    NLOS_PL =  -1 * ( NLOS_a + ( 43.42 - 3.1 * std::log10( P::s().MacroTX.DAntennaHeightM ) ) * ( std::log10( _dDisM ) - 3 ) );
//    return std::min(LOS_PL, NLOS_PL);
//}