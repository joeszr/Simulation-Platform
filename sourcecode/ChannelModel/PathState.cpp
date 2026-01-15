/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathState.cpp
///@brief  PathState类定义
///
///包括构造函数，析构函数，SCM和ITU模型下各条径的状态初始化函数，WorkSlot函数以及信道矩阵的初始化和更新函数的定义
#include "P.h"
#include "SubpathState.h"
#include "BasicChannelState.h"
#include "AntennaOrientGain.h"
#include "Tx.h"
#include "Rx.h"
#include "./functions.h"
#include "PathState.h"

#include "CTXRU.h"

using namespace cm;

///构造函数
PathState::PathState() {
//    m_ChannelMat = itpp::zeros_c(P::s().RX.ITotalAntNum, P::s().MacroTX.ITotalAntNum);
    m_vSubpath.resize(P::s().NumOfRayPerCluster, SubpathState());
    for (unsigned i = 0; i < m_vSubpath.size(); ++i) {
        m_vSubpath[i].m_pPathState = this;
        m_vSubpath[i].m_Subpath_Index = i;
    }
	
    m_bIsFirstPath = false;
}

PathState::PathState(const PathState& _PS) {
    *this = _PS;
    for (auto& subpath : m_vSubpath) {
        subpath.m_pPathState = this;
    }
    
    if(m_bIsFirstPath && _PS.m_pLOS_Subpath) {
        m_pLOS_Subpath = std::make_shared<SubpathState>();
        
        m_pLOS_Subpath->m_pPathState = this;
            
        m_pLOS_Subpath->Set_LOS_Subpath();
        m_pLOS_Subpath->Initialize_step11B();
    }
}

void PathState::Initialize() {
    for (auto& subpath : m_vSubpath) {
        subpath.Initialize();
    }  
}


void PathState::Init_Step8() {
    std::vector<double> vAlfaAOAM, vAlfaAODM, vAlfaEOAM, vAlfaEODM;
    vAlfaAOAM.resize(P::s().NumOfRayPerCluster, 0.0);
    vAlfaAOAM[0] = 0.0447;
    vAlfaAOAM[1] = -0.0447;
    vAlfaAOAM[2] = 0.1413;
    vAlfaAOAM[3] = -0.1413;
    vAlfaAOAM[4] = 0.2492;
    vAlfaAOAM[5] = -0.2492;
    vAlfaAOAM[6] = 0.3715;
    vAlfaAOAM[7] = -0.3715;
    vAlfaAOAM[8] = 0.5129;
    vAlfaAOAM[9] = -0.5129;

    vAlfaAOAM[10] = 0.6797;
    vAlfaAOAM[11] = -0.6797;
    vAlfaAOAM[12] = 0.8844;
    vAlfaAOAM[13] = -0.8844;
    vAlfaAOAM[14] = 1.1481;
    vAlfaAOAM[15] = -1.1481;
    vAlfaAOAM[16] = 1.5195;
    vAlfaAOAM[17] = -1.5195;
    vAlfaAOAM[18] = 2.1551;
    vAlfaAOAM[19] = -2.1551;

    vAlfaAODM = vAlfaAOAM;
    vAlfaEOAM = vAlfaAOAM;
    vAlfaEODM = vAlfaAOAM;
//    std::random_shuffle(vAlfaAOAM.begin(), vAlfaAOAM.end());
//    std::random_shuffle(vAlfaEOAM.begin(), vAlfaEOAM.end());
//    std::random_shuffle(vAlfaEODM.begin(), vAlfaEODM.end());
    for (int j = 0; j < P::s().NumOfRayPerCluster; ++j) {           
        m_vSubpath[j].m_AODOffsetDeg = vAlfaAODM[j];
        m_vSubpath[j].m_AOAOffsetDeg = vAlfaAOAM[j];
        
        m_vSubpath[j].m_AODDeg = 
            m_pBCS->m_dClusterASD * m_vSubpath[j].m_AODOffsetDeg + m_AODDeg_Path;
        m_vSubpath[j].m_AODDeg = RAD2DEG(ConvergeAngle(DEG2RAD(
            m_vSubpath[j].m_AODDeg)));
        
        m_vSubpath[j].m_AOADeg = 
            m_pBCS->m_dClusterASA * m_vSubpath[j].m_AOAOffsetDeg + m_AOADeg_Path;
        m_vSubpath[j].m_AOADeg = RAD2DEG(ConvergeAngle(DEG2RAD(
            m_vSubpath[j].m_AOADeg)));

        m_vSubpath[j].m_EODOffsetDeg = vAlfaEODM[j];
        m_vSubpath[j].m_EOAOffsetDeg = vAlfaEOAM[j];
        
        double uzsd = m_pBCS->m_EODSpreadAVE_LogDeg;
        
        m_vSubpath[j].m_EODDeg = (3.0 / 8.0 * pow(10, uzsd)) 
            * m_vSubpath[j].m_EODOffsetDeg + m_EODDeg_Path;
        m_vSubpath[j].m_EODDeg = abs(RAD2DEG(ConvergeAngle_to_0_PI(DEG2RAD(
            m_vSubpath[j].m_EODDeg))));
        
        m_vSubpath[j].m_EOADeg = m_pBCS->m_dClusterESA 
            * m_vSubpath[j].m_EOAOffsetDeg + m_EOADeg_Path;
        m_vSubpath[j].m_EOADeg = abs(RAD2DEG(ConvergeAngle_to_0_PI(DEG2RAD(
            m_vSubpath[j].m_EOADeg))));
    }   
}

void PathState::Init_Step9() {
    m_PowerPerRay = sqrt(m_PowerNlos / P::s().NumOfRayPerCluster);
    if (m_pBCS->IsMacroToUE()) {
        if (m_pBCS->m_bIsLOS) {
            m_dXPD1 = DB2L(random.xNormal_channel(P::s().Macro2UE_LOS.XPR_u, P::s().Macro2UE_LOS.XPR_sigma));
        } else {
            m_dXPD1 = DB2L(random.xNormal_channel(P::s().Macro2UE_NLOS.XPR_u, P::s().Macro2UE_NLOS.XPR_sigma));
        }
        if (m_pBCS->m_pRx->GetSpecial() == 1 
                && cm::P::s().IChannelModel_for_Scenario != cm::P::InH) {
            m_dXPD1 = DB2L(random.xNormal_channel(P::s().Macro2UE_O2I.XPR_u, P::s().Macro2UE_O2I.XPR_sigma));
        }
    } else {
        if (m_pBCS->m_bIsLOS) {
            m_dXPD1 = DB2L(P::s().Pico2UE_LOS.XPR);
        } else {
            m_dXPD1 = DB2L(P::s().Pico2UE_NLOS.XPR);
        }
        if (m_pBCS->m_pRx->GetSpecial() == 1 
                && cm::P::s().IChannelModel_for_Scenario != cm::P::InH) {
            m_dXPD1 = DB2L(P::s().Pico2UE_O2I.XPR);
        }
    }
    m_dXPD2 = m_dXPD1;
}

void PathState::Build_LOS_Subpath() {
    if (m_pBCS->m_bIsLOS) {
        if (m_bIsFirstPath) {
            m_pLOS_Subpath = std::make_shared<SubpathState>();
            
            m_pLOS_Subpath->m_pPathState = this;
            
            assert(m_pLOS_Subpath->m_pPathState != nullptr);
            m_pLOS_Subpath->Initialize();
            
            m_pLOS_Subpath->Set_LOS_Subpath();
            m_pLOS_Subpath->Initialize_step11B();
            
            
        }
    }
}

std::complex<double> PathState::CalcPath_TimeH_for_TXRUPair(
            int BS_BeamIndex, int UE_BeamIndex,
            std::shared_ptr<CTXRU>& _pBS_TXRU,
            std::shared_ptr<CTXRU>& _pUE_TXRU,
            int _AntIndex_in_BS_TXRU, 
            int _AntIndex_in_UE_TXRU,
            double _time_s) {
    
    std::complex<double> Path_TimeH (0.0, 0.0);
    //chty 1111 b
    if(_time_s<0.000001){
        for (int i = 0; i < P::s().NumOfRayPerCluster; ++i) {
            Path_TimeH += m_vSubpath[i].InitialCalcSubpath_TimeH_for_TXRUPair(
                    BS_BeamIndex, UE_BeamIndex,
                    _pBS_TXRU, _pUE_TXRU,
                    _AntIndex_in_BS_TXRU, _AntIndex_in_UE_TXRU, _time_s);
        }
    }else{
        for (int i = 0; i < P::s().NumOfRayPerCluster; ++i) {
            Path_TimeH += m_vSubpath[i].CalcSubpath_TimeH_for_TXRUPair(
                    BS_BeamIndex, UE_BeamIndex,
                    _pBS_TXRU, _pUE_TXRU,
                    _AntIndex_in_BS_TXRU, _AntIndex_in_UE_TXRU, _time_s);
        }
    }
    //chty 1111 e

    Path_TimeH *= m_PowerPerRay;

    if (m_pBCS->m_bIsLOS) {
        double K = DB2L(m_pBCS->m_KFactorDB);
        Path_TimeH *= sqrt(1.0 / (1.0 + K));

        if (m_bIsFirstPath) {
            //chty 1111 b
            if(_time_s<0.000001){
                Path_TimeH += sqrt(K / (1 + K)) *
                              m_pLOS_Subpath->InitialCalcSubpath_TimeH_for_TXRUPair(
                                      BS_BeamIndex, UE_BeamIndex,
                                      _pBS_TXRU, _pUE_TXRU,
                                      _AntIndex_in_BS_TXRU, _AntIndex_in_UE_TXRU, _time_s);
            }else{
                Path_TimeH += sqrt(K / (1 + K)) *
                              m_pLOS_Subpath->CalcSubpath_TimeH_for_TXRUPair(
                                      BS_BeamIndex, UE_BeamIndex,
                                      _pBS_TXRU, _pUE_TXRU,
                                      _AntIndex_in_BS_TXRU, _AntIndex_in_UE_TXRU, _time_s);
            }
            //chty 1111 e
        }
    }
    
    return Path_TimeH;
}