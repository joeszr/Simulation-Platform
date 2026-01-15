/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossUrbanMacroLOS.cpp
///@brief  PathLossUrbanMacroLOS类定义
///
///包括构造函数，析构函数，计算该场景下DB值的函数定义
///
///@author wangfei

#include "PathLossUrbanMacroLOS.h"
#include "P.h"

using namespace cm;

///计算DB值
double PathLossUrbanMacroLOS_ModeA::Db( double _dDisM_3D , double _dUEHeightM){
    assert( P::s().MacroTX.DAntennaHeightM == 25 );
    assert( P::s().RX.DAntennaHeightM == 1.5 );
    return m_PL_LOS_5GUMA.Db(_dDisM_3D, _dUEHeightM);
}

/////计算DB值
//double PathLossUrbanMacroLOS::Db( double _dDisM , double _dUEHeightM){
//    assert( P::s().MacroTX.DAntennaHeightM == 25 );
//    assert( P::s().RX.DAntennaHeightM == 1.5 );
//
//    const static double dEffectNBHeightM = P::s().MacroTX.DAntennaHeightM - 1;
//    const static double dEffectUEHeightM = P::s().RX.DAntennaHeightM - 1;
//    const static double dEffectBreakPointM = 4 * dEffectNBHeightM* dEffectUEHeightM* P::s().FX.DRadioFrequencyMHz_Macro * 1e6 / 3e8;
//    double d2DisM = sqrt(pow(_dDisM,2)-pow((P::s().MacroTX.DAntennaHeightM-_dUEHeightM),2));
//    if( d2DisM <= dEffectBreakPointM ){
//        const static double a = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
//        return -1 * ( a + 22.0 * std::log10( _dDisM ) );
//    }else if( dEffectBreakPointM <= d2DisM && d2DisM <= 5000 ){
////        const static double b = 7.8 - 18 * std::log10( dEffectNBHeightM ) - 18 * std::log10( dEffectUEHeightM ) + 2.0 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
//        const static double b = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 ) - 9 * std::log10(pow(dEffectBreakPointM,2) + pow((dEffectNBHeightM - dEffectUEHeightM),2));//36.873
//        return -1 * ( b + 40.0 * std::log10( _dDisM ) );
//    }
//    return 0;
//}