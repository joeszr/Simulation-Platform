///@file PathLossPico2UE_UrbanMicroO2I.cpp
///@brief  PathLossPico2UE_UrbanMicroO2I类定义
///@author wangfei
#include "PathLossPico2UE_UrbanMicroNLOS.h"
#include "PathLossPico2UE_UrbanMicroLOS.h"
#include "PathLossPico2UE_UrbanMicroO2I.h"

using namespace cm;
///构造函数

PathLossPico2UE_UrbanMicroO2I::PathLossPico2UE_UrbanMicroO2I(double _dInDoorDisM, bool _bIsLOS) {
    m_dInDoorDisM = _dInDoorDisM;
    m_bIsLOS = _bIsLOS;
}

///计算DB值
double PathLossPico2UE_UrbanMicroO2I::Db(double _dDisM, double _dUEHeightM) {
    double dPLb = m_bIsLOS ? m_PLUMiLOS.Db(_dDisM,_dUEHeightM) : m_PLUMiNLOS.Db(_dDisM,_dUEHeightM);
    double dPLw = -20;
    double dPLin = -0.5 * m_dInDoorDisM;
    return dPLb + dPLw + dPLin;
}

void PathLossPico2UE_UrbanMicroO2I::SetLOS(bool _bIsLOS) {
    m_bIsLOS = _bIsLOS;
}