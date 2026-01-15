///@file PathLossPico2UE_UrbanMicroNLOS.cpp
///@brief  PathLossPico2UE_UrbanMicroNLOS类定义
///@author wangfei

#include "P.h"
#include "./PathLossPico2UE_UrbanMicroNLOS.h"
using namespace cm;
///计算DB值
double PathLossPico2UE_UrbanMicroNLOS::Db( double _dDisM , double _dUEHeightM){
    assert ( cm::P::s().IChannelModel_for_Scenario ==  P::_5GCM_TWO_LAYER );
    if ( cm::P::s().IChannelModel_for_Scenario !=  P::_5GCM_TWO_LAYER ) {
        std::cout<<"PathLossPico2UE_UrbanMicroNLOS::Db出错！"<<std::endl;
    }

    const static double a = 22.7 + 26 * std::log10( P::s().FX.DRadioFrequencyMHz_Pico / 1e3 );
    return -1 * ( a + 36.7 * std::log10( _dDisM ) );
}