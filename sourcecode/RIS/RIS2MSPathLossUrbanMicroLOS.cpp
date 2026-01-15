///@file PathLossUrbanMicroLOS.cpp
///@brief  PathLossUrbanMicroLOS类定义
///@author wangfei

//#include "RIS2MSPathLossUrbanMacroOutdoorLOS.h"
#include "../ChannelModel/P.h"
//#include "../ChannelModel/functions.h"
//#include "../Utility/functions.h"
//
//#include "P.h"
#include "RIS2MSPathLossUrbanMicroLOS.h"
#include "../Parameters/Parameters.h"
using namespace cm;
///构造函数
RIS2MSPathLossUrbanMicroLOS_ModeA::RIS2MSPathLossUrbanMicroLOS_ModeA( void ){
}
///析构函数
RIS2MSPathLossUrbanMicroLOS_ModeA::~RIS2MSPathLossUrbanMicroLOS_ModeA( void ){
}
///计算DB值
double RIS2MSPathLossUrbanMicroLOS_ModeA::Db( double _dDisM_3D , double _dUEHeightM){
//    if ( cm::P::s().IChannelModel_for_Scenario ==  P::_5GCM_TWO_LAYER ) {
//        assert( Parameters::Instance().SmallCell.LINK.DPicoAntennaHeightM == 10);
//    } else {
//        assert( P::s().MacroTX.DAntennaHeightM == 10 );
//    }
    assert( 1 <= Parameters::Instance().MSS.DAntennaHeightM && Parameters::Instance().MSS.DAntennaHeightM <= 2.5 );

//    double dEffectNBHeightM = P::s().MacroTX.DAntennaHeightM - 1;
    double dEffectNBHeightM = Parameters::Instance().RIS.DAntennaHeightM - 1;
    if ( cm::P::s().IChannelModel_for_Scenario ==  P::_5GCM_TWO_LAYER ) {
        dEffectNBHeightM = Parameters::Instance().SmallCell.LINK.DPicoAntennaHeightM - 1;
    }
    const static double dEffectUEHeightM = _dUEHeightM - 1;
    const static double dEffectBreakPointM
            = 4 * dEffectNBHeightM* dEffectUEHeightM
                * P::s().FX.DRadioFrequencyMHz_Macro * 1e6 / 3e8;
    double d2DisM
            = sqrt(pow(_dDisM_3D,2)
                -pow((Parameters::Instance().RIS.DAntennaHeightM-_dUEHeightM),2));
    if( d2DisM > 10 && d2DisM <= dEffectBreakPointM ){
        const static double a
            = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
        return -1 * ( a + 22.0 * std::log10( _dDisM_3D ) );
    }else if( dEffectBreakPointM <= d2DisM && d2DisM <= 5000 ){
//        const static double b = 7.8 - 18.0 * std::log10( dEffectNBHeightM ) - 18.0 * std::log10( dEffectUEHeightM ) + 2.0 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );//36.814
          const static double b
            = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 )
                - 9 * std::log10(pow(dEffectBreakPointM,2)
                    + pow((dEffectNBHeightM - dEffectUEHeightM),2));//36.873
          return -1 * ( b + 40.0 * std::log10( _dDisM_3D ) );
    }else{
        assert( false );
        return 0;
    }


}