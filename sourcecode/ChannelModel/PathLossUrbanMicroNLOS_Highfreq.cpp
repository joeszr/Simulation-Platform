///@file PathLossUrbanMicroNLOS_Highfreq.cpp
///@brief  PathLossUrbanMicroNLOS_Highfreq类定义
///@author wangfei

#include "P.h"
#include "PathLossUrbanMicroLOS_Highfreq.h"
#include "./PathLossUrbanMicroNLOS_Highfreq.h"
#include "../Parameters/Parameters.h"

using namespace cm;

///计算DB值
double PathLossUrbanMicroNLOS_ModeB::Db( double _dDisM_3D , double _dUEHeightM){
    if ( cm::P::s().IChannelModel_for_Scenario ==  P::_5GCM_TWO_LAYER ) {
        assert( Parameters::Instance().SmallCell.LINK.DPicoAntennaHeightM == 10);
    } else {
        assert(P::s().MacroTX.DAntennaHeightM == 10);
        assert(P::s().RX.DAntennaHeightM == 1.5);
    }
//    assert( P::s().TX.DAntennaHeightM == 10 );
    assert( 1 <= P::s().RX.DAntennaHeightM && P::s().RX.DAntennaHeightM <= 2.5 );
    //20171219
    double d2DisM 
        = sqrt(pow(_dDisM_3D, 2) 
            - pow((P::s().MacroTX.DAntennaHeightM - _dUEHeightM), 2));
    assert(d2DisM >= 10 && d2DisM <= 5000);
    
    double dPL_UMi_LOS = dPLUMI_LOS.Db(_dDisM_3D,_dUEHeightM);
    
 
    double dPL_UMI_dot_NLOS = -1.0 * (
            35.3 * std::log10(_dDisM_3D)
             +22.4 + 21.3 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
             - 0.3 * (_dUEHeightM - 1.5) );
    
    double dPL_UMI_NLOS = std::min(dPL_UMi_LOS, dPL_UMI_dot_NLOS);
    
    return dPL_UMI_NLOS;
}
    //    double d3DisM_Ground = sqrt(pow(d2DisM, 2) + pow(P::s().TX.DAntennaHeightM - P::s().RX.DAntennaHeightM, 2));

    //    double dPLb = m_bIsLOS ? m_PLUMiLOS.Db(d3DisM_Ground) : m_PLUMiNLOS.Db(d3DisM_Ground); // ?
//
//    double dPLb = -10000;
//    if (Parameters::Instance().LINK_CTRL.I2DOr3DChannel == Parameters::IS3DCHANNEL) {
//        double dPL_LOS = 0;
//        double dEffectNBHeightM = P::s().MacroTX.DAntennaHeightM - 1;
//        double dEffectUEHeightM = _dUEHeightM - 1;
//        double dEffectBreakPointM = 4 * dEffectNBHeightM * dEffectUEHeightM * P::s().FX.DRadioFrequencyMHz_Macro * 1e6 / 3e8;
//        double dEffectGroundUEHeightM = P::s().RX.DAntennaHeightM - 1;
//
//        //计算 los的pl
//        if (d2DisM <= dEffectBreakPointM) {
//            //zhengyi 
//            //high freq
//            double a = 32.4 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
//            dPL_LOS = -1 * (a + 21.0 * std::log10(_dDisM_3D));
//
//        } else if (dEffectBreakPointM <= d2DisM && d2DisM <= 5000) {
//            //zhengyi
//
//            //high freq
//            double b = 32.4 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
//                    - 9.5 * std::log10(pow(dEffectBreakPointM, 2) + pow((dEffectNBHeightM - dEffectUEHeightM), 2)); //36.873
//            dPL_LOS = -1 * (b + 40.0 * std::log10(_dDisM_3D));
//
//
//        } else {
//            assert(false);
//        }
//
//
//        //再计算NLOS时候的值
//        double dPL_NLOS = 0;
//        //zhengyi
//        double a = 22.4 + 21.3 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
//                - 0.3 * (P::s().RX.DAntennaHeightM - 1.5);
//        dPL_NLOS = -1 * (a + 35.3 * std::log10(_dDisM_3D));
//
//        dPLb = std::min(dPL_LOS, dPL_NLOS);
//    }
//    if (test != dPLb) {
//        assert(false);
//    }
//    return dPLb;


