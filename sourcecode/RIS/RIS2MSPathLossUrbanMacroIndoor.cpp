#include "RIS2MSPathLossUrbanMacroIndoor.h"
#include "../ChannelModel/P.h"
#include "../Parameters/Parameters.h"

using namespace cm;
///构造函数
RIS2MSPathLossUrbanMacroIndoor::RIS2MSPathLossUrbanMacroIndoor(double _dInDoorDisM, bool _bIsLOS, bool _iIslowloss, double _dPenetrationSFdb){
    m_dInDoorDisM = _dInDoorDisM;
    m_bIsLOS = _bIsLOS;
    m_iIsLowloss = _iIslowloss;
    m_dPenetrationSFdb = _dPenetrationSFdb;
}
///析构函数
RIS2MSPathLossUrbanMacroIndoor::~RIS2MSPathLossUrbanMacroIndoor( void ){
}

///计算DB值
double RIS2MSPathLossUrbanMacroIndoor::Db(double _dDisM_3D,double _dUEHeightM) {
    assert(_dUEHeightM <= 22.5 && _dUEHeightM >= 1.5);
    

    double dPLb = m_bIsLOS ? m_PL_LOS.Db(_dDisM_3D, _dUEHeightM) : m_PL_NLOS.Db(_dDisM_3D, _dUEHeightM);

    //考虑penetration loss
    double dCarrierFrequencyGHz = Parameters::Instance().Macro.LINK.DRadioFrequencyMHz * 1e-3;
    double d_glass_loss = 2 + 0.2 * dCarrierFrequencyGHz;
    double d_IRRglass_loss = 23 + 0.3 * dCarrierFrequencyGHz;
    double d_Concrete_loss = 5 + 4 * dCarrierFrequencyGHz;

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
    double dPLw = -1 * (dPenetrationLossdB + m_dPenetrationSFdb);
    double dPLin = -0.5 * m_dInDoorDisM;
    return dPLb + dPLw + dPLin;
}