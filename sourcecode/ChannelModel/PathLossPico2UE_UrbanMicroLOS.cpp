///@file PathLossPico2UE_UrbanMicroLOS.cpp
///@brief  PathLossPico2UE_UrbanMicroLOS类定义
///@author wangfei

#include "P.h"
#include "PathLossPico2UE_UrbanMicroLOS.h"
#include "../Parameters/Parameters.h"
using namespace cm;

///计算DB值
double PathLossPico2UE_UrbanMicroLOS::Db(double _dDisM, double _dUEHeightM) {
    if (cm::P::s().IChannelModel_for_Scenario != P::_5GCM_TWO_LAYER) {
        std::cout << "PathLossPico2UE_UrbanMicroLOS::Db出错！" << std::endl;
    }
    double dEffectNBHeightM = Parameters::Instance().SmallCell.LINK.DPicoAntennaHeightM - 1;
    const static double dEffectUEHeightM = P::s().RX.DAntennaHeightM - 1;
    const static double dEffectBreakPointM = 4 * dEffectNBHeightM * dEffectUEHeightM * P::s().FX.DRadioFrequencyMHz_Pico * 1e6 / 3e8;
    if (_dDisM <= dEffectBreakPointM) {
        const static double a = 28.0 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Pico / 1e3);
        return -1 * (a + 22.0 * std::log10(_dDisM));
    } else if (dEffectBreakPointM <= _dDisM && _dDisM <= 5000) {
        const static double b = 7.8 - 18.0 * std::log10(dEffectNBHeightM) - 18.0 * std::log10(dEffectUEHeightM) + 2.0 * std::log10(P::s().FX.DRadioFrequencyMHz_Pico / 1e3);
        return -1 * (b + 40.0 * std::log10(_dDisM));
    } else {
        assert(false);
    }
}