///@file PathLossUrbanMacroNLOS_Highfreq.cpp
///@brief  PathLossUrbanMacroNLOS_Highfreq类定义
///@author wangfei

#include "PathLossUrbanMacroNLOS_Highfreq.h"
#include "P.h"
#include "../Parameters/Parameters.h"

using namespace cm;
///构造函数
PathLossUrbanMacroNLOS_Highfreq::PathLossUrbanMacroNLOS_Highfreq( void ){
}
double PathLossUrbanMacroNLOS_Highfreq::Db( double _dDisM_3D , double _dUEHeightM){
    assert( P::s().MacroTX.DAntennaHeightM == 25 );
//    assert( P::s().RX.DAntennaHeightM == 1.5 );

    assert(_dUEHeightM <= 22.5 && _dUEHeightM >= 1.5);
    
    double d2DisM = sqrt(pow(_dDisM_3D, 2) - pow((P::s().MacroTX.DAntennaHeightM - _dUEHeightM), 2));

    assert( (d2DisM >= 10.0) && (d2DisM <= 5000));
    
    assert(Parameters::Instance().LINK_CTRL.I2DOr3DChannel == Parameters::IS3DCHANNEL);
    
    double dPL_LOS = m_PL_LOS.Db(_dDisM_3D, _dUEHeightM);
    
    //再计算NLOS时候的值
    double c = 13.54 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
            - 0.6 * (_dUEHeightM - 1.5);
    double dPL_NLOS_dot = -1 * (c + 39.08 * std::log10(_dDisM_3D));

    double dPLb = std::min(dPL_LOS, dPL_NLOS_dot);    
    
    return dPLb;
}

///计算DB值
//double PathLossUrbanMacroNLOS_Highfreq::Db( double _dDisM_3D , double _dUEHeightM){
//    assert( P::s().MacroTX.DAntennaHeightM == 25 );
//    assert( P::s().RX.DAntennaHeightM == 1.5 );
//
//    //    const static double dAveBuildHeightM = 20;
//    //    const static double dStreetWidthM = 20;
//
//    //low
//    //    const static double a = 161.04 - 7.1 * std::log10( dStreetWidthM ) + 7.5 * std::log10( dAveBuildHeightM ) - ( 24.37 - 3.7 * pow( dAveBuildHeightM / P::s().TX.DAntennaHeightM, 2.0 ) ) * std::log10( P::s().TX.DAntennaHeightM ) + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 ) - 3.2 * pow( std::log10( 11.75 * P::s().RX.DAntennaHeightM ), 2.0 ) + 4.97;
//    //    return -1 * ( a + ( 43.42 - 3.1 * std::log10( P::s().TX.DAntennaHeightM ) ) * ( std::log10( _dDisM ) - 3 ) );
//
//    //high frequency
//    //    const static double a = 13.54 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
//    //            - 0.6 * (P::s().RX.DAntennaHeightM - 1.5);
//    //    return -1 * (a + 39.08 * std::log10(_dDisM));
//
//    assert(_dUEHeightM <= 22.5 && _dUEHeightM >= 1.5);
//    double d2DisM = sqrt(pow(_dDisM_3D, 2) - pow((P::s().MacroTX.DAntennaHeightM - _dUEHeightM), 2));
//    //    double d3DisM_Ground = sqrt(pow(d2DisM, 2) + pow(P::s().TX.DAntennaHeightM - P::s().RX.DAntennaHeightM, 2));
//    //    double dPLb = m_bIsLOS ? m_PLUMaLOS.Db(d3DisM_Ground) : m_PLUMaNLOS.Db(d3DisM_Ground);
//    double dPLb = -10000;
//
//    if (Parameters::Instance().LINK_CTRL.I2DOr3DChannel == Parameters::IS3DCHANNEL) {
//        //UMA nlos 只对应 h_environment = 1;
//
//        double dPL_LOS = 0;
//        //环境高度和原来不一样
//        double dEffectNBHeightM = P::s().MacroTX.DAntennaHeightM - 1;
//        double dEffectUEHeightM = _dUEHeightM - 1;
//
//        double dEffectBreakPointM = 4 * dEffectNBHeightM * dEffectUEHeightM * P::s().FX.DRadioFrequencyMHz_Macro * 1e6 / 3e8;
//        
//        if (d2DisM <= dEffectBreakPointM) {
//            //zhengyi high
//            double a = 32.4 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
//            //            dPL_LOS = -1 * (a + 20.0 * std::log10(d3DisM_Ground));
//            //采用3D的距离
//            dPL_LOS = -1 * (a + 20.0 * std::log10(_dDisM_3D));
//        } else if (dEffectBreakPointM <= d2DisM && d2DisM <= 5000) {
//            //zhengyi high
//            double b = 32.4 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
//                    - 10 * std::log10(pow(dEffectBreakPointM, 2) + pow((dEffectNBHeightM - dEffectUEHeightM), 2)); //36.900
//            //            dPL_LOS = -1 * (b + 40.0 * std::log10(d3DisM_Ground));
//            //采用3D的距离
//            dPL_LOS = -1 * (b + 40.0 * std::log10(_dDisM_3D));
//
//        } else {
//            assert(false);
//        }
//
//
//        //再计算NLOS时候的值
//        double dPL_NLOS = 0;
//        double c = 13.54 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
//                - 0.6 * (P::s().RX.DAntennaHeightM - 1.5);
//        dPL_NLOS = -1 * (c + 39.08 * std::log10(_dDisM_3D));
//
//        dPLb = std::min(dPL_LOS, dPL_NLOS);
//
//        assert(dPLb != -10000);
//        return dPLb;
//
//    } else {
//        assert(false);
//    }
//    return 0;
//}

// 20171204
