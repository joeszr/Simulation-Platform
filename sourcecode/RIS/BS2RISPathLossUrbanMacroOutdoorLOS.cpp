#include "BS2RISPathLossUrbanMacroOutdoorLOS.h"
#include "../ChannelModel/P.h"
#include "../ChannelModel/functions.h"
#include "../Utility/functions.h"

using namespace cm;
///构造函数
BS2RISPathLossUrbanMacroOutdoorLOS::BS2RISPathLossUrbanMacroOutdoorLOS( void ){
}
///析构函数
BS2RISPathLossUrbanMacroOutdoorLOS::~BS2RISPathLossUrbanMacroOutdoorLOS( void ){
}
///计算DB值
double BS2RISPathLossUrbanMacroOutdoorLOS::Db( double _dDisM_3D, double _dRISHeightM){
//    assert( Parameters.Instance().Macro.LINK.DRadioFrequencyMHz == 2000 );
//    //F=2GHz
//    static double a = 100.7 + 23.5 * std::log10( _dDisM );
//    return -1 * a;
    
//    assert(P::s().MacroTX.DAntennaHeightM == 25);
//    assert(P::s().RX.DAntennaHeightM == 1.5);

    double d2DisM = sqrt(pow(_dDisM_3D, 2) - pow((Parameters::Instance().Macro.DAntennaHeightM - _dRISHeightM), 2));
    double G, C;
    if (d2DisM > 18) {
        G = 1.25 * pow(10, -6) * pow(d2DisM, 3) * std::exp(-1 * d2DisM / 150);
    } else {
        G = 0;
    }
    if (_dRISHeightM >= 13 && _dRISHeightM <= 23) {
        C = pow((_dRISHeightM - 13) / 10.0, 1.5) * G;
    } else if (_dRISHeightM < 13) {
        C = 0;
    }

    double hE;
    if (xUniform_channel() <= 1.0 / (1.0 + C)) {
        hE = 1.0;
    } else {

        int MaxBuildingFloorNum = (_dRISHeightM - 1.5) / 3.0;

        int MinBuildingFloorNum = 12.0 / 3.0;

        assert(_dRISHeightM >= 12);

        if (MaxBuildingFloorNum >= MinBuildingFloorNum) {

            int iBuildingFloorNum = xUniformInt(MinBuildingFloorNum, MaxBuildingFloorNum);

            hE = 3.0 * iBuildingFloorNum;
        }
    }

    double dEffectNBHeightM = Parameters::Instance().RIS.DAntennaHeightM - hE;
    double dEffectUEHeightM = _dRISHeightM - hE;

    double dEffectBreakPointM
            = 4.0 * dEffectNBHeightM * dEffectUEHeightM
            * P::s().FX.DRadioFrequencyMHz_Macro * 1e6 / 3e8;

    double tolerance = 0.1;
    if (d2DisM >= 10.0 - tolerance && d2DisM <= dEffectBreakPointM) {
        //low frequency
        //        const static double a = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
        //        return -1 * ( a + 22.0 * std::log10( _dDisM ) );

        //high frequency
        double a = 28.0 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
        return -1 * (a + 22.0 * std::log10(_dDisM_3D));
    } else if (dEffectBreakPointM <= d2DisM && d2DisM <= 5000) {
        //low frequency
        //        const static double b = 7.8 - 18 * std::log10( dEffectNBHeightM ) - 18 * std::log10( dEffectUEHeightM ) + 2.0 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 );
        //        const static double b = 28.0 + 20 * std::log10( P::s().FX.DRadioFrequencyMHz_Macro / 1e3 ) 
        //        - 9 * std::log10(pow(dEffectBreakPointM,2) + pow((dEffectNBHeightM - dEffectUEHeightM),2));//36.873
        //        return -1 * ( b + 40.0 * std::log10( _dDisM ) );

        //high freq
        double b = 28.0 + 20 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3)
                - 9 * std::log10(
                pow(dEffectBreakPointM, 2)
                + pow((Parameters::Instance().RIS.DAntennaHeightM - _dRISHeightM), 2)); //36.900
        return -1 * (b + 40.0 * std::log10(_dDisM_3D));
    } else {
        assert(false);
        return 1.0;
    }

}