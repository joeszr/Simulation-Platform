///@file PathLossUrbanMicroO2I_Highfreq.cpp
///@brief  PathLossUrbanMicroO2I_Highfreq类定义
///@author wangfei
#include "P.h"
#include "PathLossUrbanMicroNLOS_Highfreq.h"
#include "PathLossUrbanMicroLOS_Highfreq.h"
#include "PathLossUrbanMicroO2I_Highfreq.h"
#include "../Parameters/Parameters.h"
using namespace cm;
///构造函数

PathLossUrbanMicroO2I_Highfreq::PathLossUrbanMicroO2I_Highfreq(double _dInDoorDisM, bool _bIsLOS,int _iIslowloss,double _dPenetrationSFdb) {
    m_dInDoorDisM = _dInDoorDisM;
    m_bIsLOS = _bIsLOS;
    //zhengyi
    m_iIsLowloss = _iIslowloss;
    m_dPenetrationSFdb = _dPenetrationSFdb;
}
///计算DB值

double PathLossUrbanMicroO2I_Highfreq::Db(double _dDisM_3D, double _dUEHeightM) {
//    double d2DisM = sqrt(pow(_dDisM_3D,2)-pow((P::s().MacroTX.DAntennaHeightM-_dUEHeightM),2));
//    double d3DisM_Ground = sqrt(pow(d2DisM,2)+pow(P::s().MacroTX.DAntennaHeightM-P::s().RX.DAntennaHeightM,2));
    double dPLb = m_bIsLOS 
                ? m_PLUMiLOS.Db(_dDisM_3D,_dUEHeightM) 
                : m_PLUMiNLOS.Db(_dDisM_3D,_dUEHeightM);

    //
    //考虑penetration loss
    double dCarrierFrequencyGHz 
        = Parameters::Instance().Macro.LINK.DRadioFrequencyMHz * 1e-3;
    double d_glass_loss = 2 + 0.2 * dCarrierFrequencyGHz;
    double d_IRRglass_loss = 23 + 0.3 * dCarrierFrequencyGHz;
    double d_Concrete_loss = 5 + 4 * dCarrierFrequencyGHz;

    int iIsLowloss = m_iIsLowloss;
    double dPenetrationLossdB ;

    //zhengyi 这里注意high loss和low loss是 ue specific的
    if (iIsLowloss == 1) {//low loss
        //        dPenetrationLossdB = 5
        //                - 10 * log10(0.3 * pow(10, -1 * d_glass_loss / 10) + 0.7 * pow(10, -1 * d_Concrete_loss / 10))
        //                + xNormal_channel(0, 7);
        dPenetrationLossdB = 5
                - 10 * log10(0.3 * pow(10, -1 * d_glass_loss / 10) 
                            + 0.7 * pow(10, -1 * d_Concrete_loss / 10));

    } else if (iIsLowloss == 0) {//high loss
        //        dPenetrationLossdB = 5
        //                - 10 * log10(0.7 * pow(10, -1 * d_IRRglass_loss / 10) + 0.3 * pow(10, -1 * d_Concrete_loss / 10))
        //                + xNormal_channel(0, 7);
        dPenetrationLossdB = 5
                - 10 * log10(0.7 * pow(10, -1 * d_IRRglass_loss / 10) 
                            + 0.3 * pow(10, -1 * d_Concrete_loss / 10));

    } else {
        assert(false);
    }

    //    double dPLw = -20;
    double dPLw = -1 * (dPenetrationLossdB + m_dPenetrationSFdb);
    double dPLin = -0.5 * m_dInDoorDisM;
    return dPLb + dPLw + dPLin;
}

void PathLossUrbanMicroO2I_Highfreq::SetLOS(bool _bIsLOS) {
    m_bIsLOS = _bIsLOS;
}