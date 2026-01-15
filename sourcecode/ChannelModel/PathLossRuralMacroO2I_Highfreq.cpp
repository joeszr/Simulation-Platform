//20171219
#include "P.h"
#include "PathLossRuralMacroLOS_Highfreq.h"
#include "PathLossRuralMacroNLOS_Highfreq.h"
#include "PathLossRuralMacroO2I_Highfreq.h"
#include "../Parameters/Parameters.h"
using namespace cm;
///构造函数

PathLossRuralMacroO2I_ModeB::PathLossRuralMacroO2I_ModeB(double _dInDoorDisM, bool _bIsLOS,int _iIslowloss, double _dPenetrationSFdb) {
    m_dInDoorDisM = _dInDoorDisM;
    m_bIsLOS = _bIsLOS;
    m_iIsLowloss = _iIslowloss;
    m_dPenetrationSFdb = _dPenetrationSFdb;
}

///计算DB值

double PathLossRuralMacroO2I_ModeB::Db(double _dDisM_3D, double _dUEHeightM) {
    assert(_dUEHeightM <= 22.5 && _dUEHeightM >= 1.5);

    double dPLb = m_bIsLOS 
                ? m_PLRMaLOS.Db(_dDisM_3D, _dUEHeightM) 
                : m_PLRMaNLOS.Db(_dDisM_3D, _dUEHeightM);

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

void PathLossRuralMacroO2I_ModeB::SetLOS(bool _bIsLOS) {
    m_bIsLOS = _bIsLOS;
}