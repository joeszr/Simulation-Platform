/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossUrbanMacroLOS_Highfreq.cpp
///@brief  PathLossUrbanMacroLOS_Highfreq类定义
///
///包括构造函数，析构函数，计算该场景下DB值的函数定义
///
///@author wangfei

#include "PathLossUrbanMacroLOS_Highfreq.h"
#include "P.h"
#include "../Utility/functions.h"


using namespace cm;

///计算DB值
//

// 20171204
double PathLossUrbanMacroLOS_Highfreq::Db( double _dDisM_3D , double _dUEHeightM){
    assert( P::s().MacroTX.DAntennaHeightM == 25 );
    assert( P::s().RX.DAntennaHeightM == 1.5 );

    double d2DisM = sqrt(pow(_dDisM_3D,2)-pow((P::s().MacroTX.DAntennaHeightM-_dUEHeightM),2));
    double G, C;
    if (d2DisM > 18) {
        G = 1.25 * pow(10, -6) * pow(d2DisM, 3) * std::exp(-1 * d2DisM / 150);
    } else {
        G = 0;
    }
    if (_dUEHeightM >= 13 && _dUEHeightM <= 23) {
        C = pow((_dUEHeightM - 13) / 10.0, 1.5) * G;
    } else if (_dUEHeightM < 13) {
        C = 0;
    }
    
    double hE;
    if(random.xUniform_channel()<=1.0 / (1.0 + C)){
        hE = 1.0;
    }else{
       
        int MaxBuildingFloorNum =(int) ((_dUEHeightM - 1.5) / 3.0);
        
        int MinBuildingFloorNum = 12.0 / 3.0;
        
        assert(_dUEHeightM >= 12);
        
        if(MaxBuildingFloorNum >= MinBuildingFloorNum) {
        
            int iBuildingFloorNum = random.xUniformInt(MinBuildingFloorNum, MaxBuildingFloorNum);
       
            hE = 3.0 * iBuildingFloorNum;
        }
    }
    
    double dEffectNBHeightM = P::s().MacroTX.DAntennaHeightM - hE;
    double dEffectUEHeightM = _dUEHeightM - hE;
    
    double dEffectBreakPointM 
        = 4.0 * dEffectNBHeightM * dEffectUEHeightM
            * P::s().FX.DRadioFrequencyMHz_Macro * 1e6 / 3e8;
        
    double tolerance = 0.1;
    if( d2DisM >= 10.0 - tolerance && d2DisM <= dEffectBreakPointM ){
        //low frequency
        //        const static double a = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
        //        return -1 * ( a + 22.0 * std::log10( _dDisM ) );

        //high frequency
        double a = 28.0 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
        return -1 * (a + 22.0 * std::log10(_dDisM_3D));
    }else if( dEffectBreakPointM <= d2DisM && d2DisM <= 5000 ){
        //low frequency
        //        const static double b = 7.8 - 18 * std::log10( dEffectNBHeightM ) - 18 * std::log10( dEffectUEHeightM ) + 2.0 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
        //        const static double b = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 ) 
        //        - 9 * std::log10(pow(dEffectBreakPointM,2) + pow((dEffectNBHeightM - dEffectUEHeightM),2));//36.873
        //        return -1 * ( b + 40.0 * std::log10( _dDisM ) );

        //high freq
        double b = 28.0 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
            - 9 * std::log10(
                pow(dEffectBreakPointM, 2) 
                + pow((P::s().MacroTX.DAntennaHeightM - _dUEHeightM), 2)); //36.900
        return -1 * (b + 40.0 * std::log10(_dDisM_3D));
    } else {
        assert(false);
    }
}