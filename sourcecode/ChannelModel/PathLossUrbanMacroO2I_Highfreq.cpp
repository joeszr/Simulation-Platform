///@file PathLossUrbanMacroO2I_Highfreq.cpp
///@brief  PathLossUrbanMacroO2I_Highfreq类定义
///@author wangfei
#include "P.h"
#include "PathLossUrbanMacroNLOS_Highfreq.h"
#include "PathLossUrbanMacroLOS_Highfreq.h"
#include "PathLossUrbanMacroO2I_Highfreq.h"
#include "../Parameters/Parameters.h"
using namespace cm;
///构造函数

PathLossUrbanMacroO2I_Highfreq::PathLossUrbanMacroO2I_Highfreq(double _dInDoorDisM, bool _bIsLOS,int _iIslowloss, double _dPenetrationSFdb) {
    m_dInDoorDisM = _dInDoorDisM;
    m_bIsLOS = _bIsLOS;
    m_iIsLowloss = _iIslowloss;
    m_dPenetrationSFdb = _dPenetrationSFdb;
}
///计算DB值

double PathLossUrbanMacroO2I_Highfreq::Db(double _dDisM_3D, double _dUEHeightM) {
    assert(_dUEHeightM <= 22.5 && _dUEHeightM >= 1.5);

//    double _dDisM_2D  = sqrt(pow(_dDisM_3D, 2) - pow((P::s().MacroTX.DAntennaHeightM - _dUEHeightM), 2));
//    double _dDisM_3D = sqrt(pow(d2DisM, 2) + pow(P::s().MacroTX.DAntennaHeightM - P::s().RX.DAntennaHeightM, 2));
    double dPLb = m_bIsLOS ? m_PLUMaLOS.Db(_dDisM_3D, _dUEHeightM) : m_PLUMaNLOS.Db(_dDisM_3D, _dUEHeightM);

    //考虑penetration loss
    double dCarrierFrequencyGHz = Parameters::Instance().Macro.LINK.DRadioFrequencyMHz * 1e-3;
    double d_glass_loss = 2 + 0.2 * dCarrierFrequencyGHz;
    double d_IRRglass_loss = 23 + 0.3 * dCarrierFrequencyGHz;
    double d_Concrete_loss = 5 + 4 * dCarrierFrequencyGHz;

//    double dPenetrationLossdB = 20;
    double dPenetrationLossdB;

    if (m_iIsLowloss) {
        dPenetrationLossdB 
            = 5 - 10 * log10(0.3 * pow(10, -1 * d_glass_loss / 10)
                + 0.7 * pow(10, -1 * d_Concrete_loss / 10));

    } else {
        dPenetrationLossdB 
            = 5 - 10 * log10(0.7 * pow(10, -1 * d_IRRglass_loss / 10)
                + 0.3 * pow(10, -1 * d_Concrete_loss / 10));
    }
    //
    //    double dPLw = -20;
    double dPLw = -1 * (dPenetrationLossdB + m_dPenetrationSFdb);
    double dPLin = -0.5 * m_dInDoorDisM;
    return dPLb + dPLw + dPLin;
}

void PathLossUrbanMacroO2I_Highfreq::SetLOS(bool _bIsLOS) {
    m_bIsLOS = _bIsLOS;
}