///@file PathLossUrbanMicroO2I.cpp
///@brief  PathLossUrbanMicroO2I类定义
///@author wangfei
#include "PathLossUrbanMicroNLOS.h"
#include "PathLossUrbanMicroLOS.h"
#include "PathLossUrbanMicroO2I.h"
using namespace cm;
///构造函数

PathLossUrbanMicroO2I_ModeA::PathLossUrbanMicroO2I_ModeA(double _dInDoorDisM, bool _bIsLOS) {
    m_dInDoorDisM = _dInDoorDisM;
    m_bIsLOS = _bIsLOS;
}

///计算DB值
double PathLossUrbanMicroO2I_ModeA::Db(double _dDisM_3D, double _dUEHeightM) {
//    double d2DisM 
//        = sqrt(pow(_dDisM_3D,2)
//            -pow((P::s().MacroTX.DAntennaHeightM-_dUEHeightM),2));
//    double d3DisM_Ground = sqrt(pow(d2DisM,2)+pow(P::s().MacroTX.DAntennaHeightM-P::s().RX.DAntennaHeightM,2));
    double dPLb = m_bIsLOS ? 
                    m_PLUMiLOS.Db(_dDisM_3D,_dUEHeightM) 
                    : m_PLUMiNLOS.Db(_dDisM_3D,_dUEHeightM);
//    if(Parameters::Instance().LINK_CTRL.I2DOr3DChannel == Parameters::IS3DCHANNEL) {
//        double dPL_LOS = 0;
//        double dEffectNBHeightM = P::s().MacroTX.DAntennaHeightM - 1;
//        double dEffectUEHeightM = _dUEHeightM - 1;
//        double dEffectBreakPointM = 4 * dEffectNBHeightM* dEffectUEHeightM* P::s().FX.DRadioFrequencyMHz_Macro * 1e6 / 3e8;
//        double dEffectGroundUEHeightM = P::s().RX.DAntennaHeightM - 1;
//        if( d2DisM <= dEffectBreakPointM ){
//            double a = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
//            dPL_LOS = -1 * ( a + 22.0 * std::log10( d3DisM_Ground ) );
//        }else if( dEffectBreakPointM <= d2DisM && d2DisM <= 5000 ){
////            double b = 7.8 - 18.0 * std::log10( dEffectNBHeightM ) - 18.0 * std::log10( dEffectGroundUEHeightM ) + 2.0 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );//36.814
//            double b = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 ) - 9 * std::log10(pow(dEffectBreakPointM,2) + pow((P::s().MacroTX.DAntennaHeightM - _dUEHeightM),2));//36.873
//            dPL_LOS = -1 * ( b + 40.0 * std::log10( d3DisM_Ground ) );
//        }else{
//            assert(false);
//        }
//
//        if(m_bIsLOS){
//            dPLb = dPL_LOS;
//        }else{
//            //再计算NLOS时候的值
//            double dPL_NLOS = 0;
//            double a = 22.7 + 26 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
//            double alfa = 0.3;
//            dPL_NLOS = -1 * ( a + 36.7 * std::log10( d3DisM_Ground ) - alfa *(_dUEHeightM-1.5));
//            dPLb = std::min(dPL_LOS,dPL_NLOS);
//        }
//    }
    double dPLw = -20;
    double dPLin = -0.5 * m_dInDoorDisM;
    return dPLb + dPLw + dPLin;
}

void PathLossUrbanMicroO2I_ModeA::SetLOS(bool _bIsLOS) {
    m_bIsLOS = _bIsLOS;
}