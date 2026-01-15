///20171218
#include "PathLossRuralMacroNLOS.h"
#include "PathLossRuralMacroLOS.h"
#include "PathLossRuralMacroO2I.h"
using namespace cm;
///构造函数

PathLossRuralMacroO2I::PathLossRuralMacroO2I(double _dInDoorDisM, bool _bIsLOS) {
    m_dInDoorDisM = _dInDoorDisM;
    m_bIsLOS = _bIsLOS;
}
///计算DB值

double PathLossRuralMacroO2I::Db(double _dDisM_3D, double _dUEHeightM) {
//    double d2DisM = sqrt(pow(_dDisM_3D,2)-pow((P::s().MacroTX.DAntennaHeightM-_dUEHeightM),2));
//    double d3DisM_Ground = _dDisM_3D;//sqrt(pow(d2DisM,2)+pow(P::s().MacroTX.DAntennaHeightM-P::s().RX.DAntennaHeightM,2));
    double dPLb =  m_bIsLOS ? 
                    m_PLRMaLOS.Db(_dDisM_3D, _dUEHeightM) 
                    : m_PLRMaNLOS.Db(_dDisM_3D, _dUEHeightM);
    

    double dPLw = -10;
    double dPLin = -0.5 * m_dInDoorDisM;
    return dPLb + dPLw + dPLin;
}

void PathLossRuralMacroO2I::SetLOS(bool _bIsLOS) {
    m_bIsLOS = _bIsLOS;
}