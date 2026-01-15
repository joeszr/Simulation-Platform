///@file PathLossUrbanMacroO2I.cpp
///@brief  PathLossUrbanMacroO2I类定义
///@author wangfei
#include "PathLossUrbanMacroNLOS.h"
#include "PathLossUrbanMacroLOS.h"
#include "PathLossUrbanMacroO2I.h"
#include "../Parameters/Parameters.h"
using namespace cm;
///构造函数

PathLossUrbanMacroO2I::PathLossUrbanMacroO2I(double _dInDoorDisM, bool _bIsLOS) {
    m_dInDoorDisM = _dInDoorDisM;
    m_bIsLOS = _bIsLOS;
}

///计算DB值

double PathLossUrbanMacroO2I::Db(double _dDisM, double _dUEHeightM) {
    double d3DisM_Ground = _dDisM;
    double dPLb =  m_bIsLOS ? m_PLUMaLOS.Db(d3DisM_Ground, _dUEHeightM) : m_PLUMaNLOS.Db(d3DisM_Ground, _dUEHeightM);

    double dPLw = -1* Parameters::Instance().Macro.DPenetrationLossdB;
    double dPLin = -0.5 * m_dInDoorDisM;
    return dPLb + dPLw + dPLin;
}

void PathLossUrbanMacroO2I::SetLOS(bool _bIsLOS) {
    m_bIsLOS = _bIsLOS;
}