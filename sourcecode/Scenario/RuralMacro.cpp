
#include "RuralMacro.h"

#include "PathLossRuralMacroLOS_Highfreq.h"
#include "PathLossRuralMacroNLOS_Highfreq.h"
#include "../ChannelModel/P.h"
#include "../ChannelModel/PathLossRuralMacroLOS_Highfreq.h"
#include"PathLossRuralMacroNLOS_Highfreq.h"
#include "PathLossRuralMacroO2I_Highfreq.h"
#include "../Parameters/Parameters.h"
#include"PathState.h"

using namespace cm;

RuralMacro::RuralMacro(){
    m_pDSMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.DSCorrDistM));
    m_pAODMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.ASDCorrDistM));
    m_pAOAMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.ASACorrDistM));
    m_pSFMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.SFCorrDistM));
    m_pEODMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.ESDCorrDistM));
    m_pEOAMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.ESACorrDistM));
}

RuralMacro::~RuralMacro(){
    
}

void RuralMacro::SetMSPara(MS* _pms){
    //只适用于低频RMA
    _pms->m_pRxNode->m_iSpecial = (xUniform_msconstruct(0, 1) < Parameters::Instance().MSS.DProbOutdoor) ? 0 : 1;
    if (_pms->m_pRxNode->m_iSpecial == 0) {
        _pms->m_pRxNode->m_dInCarLossDB = Parameters::Instance().MSS.bIsOutdoorInCar_notOnCar ?
                -1 * (xNormal_msconstruct(0, 5) + 9) : 0.0;
        _pms->m_pRxNode->m_SpeedSacleFactor = Parameters::Instance().MSS.DCarSpeedSacleFactor;
    } else {
        _pms->m_pRxNode->m_dInCarLossDB = 0.0;
        _pms->m_pRxNode->m_SpeedSacleFactor = 1.0;
    }
}

std::shared_ptr<PathLoss> RuralMacro::GetPathLossPtr(Tx& _tx, Rx& _rx, bool _bIsLOS){
    std::shared_ptr<PathLoss> result;
    static std::shared_ptr<PathLoss> pPLRMaLOS
            = std::shared_ptr<PathLoss > (new PathLossRuralMacroLOS_ModeB());
    static std::shared_ptr<PathLoss> pPLRMaNLOS
            = std::shared_ptr<PathLoss > (new PathLossRuralMacroNLOS_ModeB());//该处均只使用高频
    result = _bIsLOS ? pPLRMaLOS : pPLRMaNLOS;
    return result;
}

std::shared_ptr<PathLoss> RuralMacro::GetPathLossPtr(double _dInDoorDisM, bool _bIsLOS,
                                                     int _iIslowloss, double _dPenetrationSFdb){
    std::shared_ptr<PathLoss> result;
    result = std::shared_ptr<PathLoss > (
            new PathLossRuralMacroO2I_ModeB(_dInDoorDisM, _bIsLOS,
                                      _iIslowloss, _dPenetrationSFdb));//使用高频参数
    return result;
}

double RuralMacro::GenPos2Din(std::pair<int, int> pos) {
    double rand_d1 = xUniform_channel(0, 10);
    double rand_d2 = xUniform_channel(0, 10);
    
    return min(rand_d1, rand_d2);
}

bool RuralMacro::DecideLOS(double _dDisM_2D, bool _bIsMacro2UE, double _h_UT) {
    bool isLOS;
    double dPossLOS;
    if (_dDisM_2D <= 10) {
        isLOS = true;
    } else if (_dDisM_2D > 10) {
        dPossLOS = std::exp(-1 * (_dDisM_2D - 10.0) / 1000.0);
        isLOS = xUniform_channel() < dPossLOS;
    } else {
        assert(false);
    }

    return isLOS;
}

void RuralMacro::ReadMapPos(Point& _tx, Rx& _rx, BasicChannelState* const bcs) {
    itpp::mat r, y, x(7, 1);
    complex<double> position
            = complex<double>(_rx.GetX(), _rx.GetY()) + complex<double>(_tx.GetX(), _tx.GetY());
    if (bcs->IsMacroToUE()) {
        if (bcs->m_bIsLOS) {
            x(0, 0) = m_pDSMapLOS ->ReadMap(position);
            x(1, 0) = m_pAODMapLOS->ReadMap(position);
            x(2, 0) = m_pAOAMapLOS->ReadMap(position);
            x(3, 0) = m_pSFMapLOS ->ReadMap(position);
            x(4, 0) = m_pKMapLOS  ->ReadMap(position);
            x(5, 0) = m_pEODMapLOS->ReadMap(position);
            x(6, 0) = m_pEOAMapLOS->ReadMap(position);
            r = P::s().Macro2UE_LOS.R;
        } else {
            x(0, 0) = m_pDSMapNLOS ->ReadMap(position);
            x(1, 0) = m_pAODMapNLOS->ReadMap(position);
            x(2, 0) = m_pAOAMapNLOS->ReadMap(position);
            x(3, 0) = m_pSFMapNLOS ->ReadMap(position);
            x(4, 0) = 0;
            x(5, 0) = m_pEODMapNLOS->ReadMap(position);
            x(6, 0) = m_pEOAMapNLOS->ReadMap(position);
            r = P::s().Macro2UE_NLOS.R;
        }

        if (_rx.GetSpecial() == 1) {
            x(0, 0) = m_pDSMapO2I ->ReadMap(position);
            x(1, 0) = m_pAODMapO2I->ReadMap(position);
            x(2, 0) = m_pAOAMapO2I->ReadMap(position);
            x(3, 0) = m_pSFMapO2I ->ReadMap(position);
            x(4, 0) = 0;
            x(5, 0) = m_pEODMapO2I ->ReadMap(position);
            x(6, 0) = m_pEOAMapO2I ->ReadMap(position);
            r = P::s().Macro2UE_O2I.R;
        }

        y = r * x;

        if (bcs->m_bIsLOS) {
            bcs->m_DelaySpread = pow(10.0, P::s().Macro2UE_LOS.DelaySpreadSTD * y(0, 0) + P::s().Macro2UE_LOS.DelaySpreadAVE);
            bcs->m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_LOS.AODSpreadSTD * y(1, 0) + P::s().Macro2UE_LOS.AODSpreadAVE));
            bcs->m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_LOS.AOASpreadSTD * y(2, 0) + P::s().Macro2UE_LOS.AOASpreadAVE));
            bcs->m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Macro2UE_LOS.ShadowFadingSTD : 0.0) * y(3, 0);
            bcs->m_KFactorDB = P::s().Macro2UE_LOS.KFactorDBSTD * y(4, 0) + P::s().Macro2UE_LOS.KFactorDBAVE;
            bcs->m_EODSpreadDeg = std::min(52.0, pow(10.0, bcs->m_EODSpreadSTD_LogDeg * y(5, 0) + bcs->m_EODSpreadAVE_LogDeg));
            bcs->m_EOASpreadDeg = std::min(52.0, pow(10.0, P::s().Macro2UE_LOS.EOASpreadSTD * y(6, 0) + P::s().Macro2UE_LOS.EOASpreadAVE));
        } else {
            bcs->m_DelaySpread = pow(10.0, P::s().Macro2UE_NLOS.DelaySpreadSTD * y(0, 0) + P::s().Macro2UE_NLOS.DelaySpreadAVE);
            bcs->m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_NLOS.AODSpreadSTD * y(1, 0) + P::s().Macro2UE_NLOS.AODSpreadAVE));
            bcs->m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_NLOS.AOASpreadSTD * y(2, 0) + P::s().Macro2UE_NLOS.AOASpreadAVE));
            bcs->m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Macro2UE_NLOS.ShadowFadingSTD : 0.0) * y(3, 0);
            bcs->m_EODSpreadDeg = std::min(52.0, pow(10.0, bcs->m_EODSpreadSTD_LogDeg * y(5, 0) + bcs->m_EODSpreadAVE_LogDeg));
            bcs->m_EOASpreadDeg = std::min(52.0, pow(10.0, P::s().Macro2UE_NLOS.EOASpreadSTD * y(6, 0) + P::s().Macro2UE_NLOS.EOASpreadAVE));
        }

        if (_rx.GetSpecial() == 1) {
            bcs->m_DelaySpread = pow(10.0, P::s().Macro2UE_O2I.DelaySpreadSTD * y(0, 0) + P::s().Macro2UE_O2I.DelaySpreadAVE);
            bcs->m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_O2I.AODSpreadSTD * y(1, 0) + P::s().Macro2UE_O2I.AODSpreadAVE));
            bcs->m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_O2I.AOASpreadSTD * y(2, 0) + P::s().Macro2UE_O2I.AOASpreadAVE));
            bcs->m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Macro2UE_O2I.ShadowFadingSTD : 0.0) * y(3, 0);
            bcs->m_EOASpreadDeg = std::min(52.0, pow(10.0, P::s().Macro2UE_O2I.EOASpreadSTD * y(6, 0) + P::s().Macro2UE_O2I.EOASpreadAVE));
        }
    } else {
        if (bcs->m_bIsLOS) {
            x(0, 0) = m_pDSMapPicoToUELOS ->ReadMap(position);
            x(1, 0) = m_pAODMapPicoToUELOS->ReadMap(position);
            x(2, 0) = m_pAOAMapPicoToUELOS->ReadMap(position);
            x(3, 0) = m_pSFMapPicoToUELOS ->ReadMap(position);
            x(4, 0) = m_pKMapPicoToUELOS ->ReadMap(position);
            r = P::s().Pico2UE_LOS.R;
        } else {
            x(0, 0) = m_pDSMapPicoToUENLOS ->ReadMap(position);
            x(1, 0) = m_pAODMapPicoToUENLOS->ReadMap(position);
            x(2, 0) = m_pAOAMapPicoToUENLOS->ReadMap(position);
            x(3, 0) = m_pSFMapPicoToUENLOS ->ReadMap(position);
            r = P::s().Pico2UE_NLOS.R;
        }
        if (_rx.GetSpecial() == 1) {
            x(0, 0) = m_pDSMapPicoToUEO2I ->ReadMap(position);
            x(1, 0) = m_pAODMapPicoToUEO2I->ReadMap(position);
            x(2, 0) = m_pAOAMapPicoToUEO2I->ReadMap(position);
            x(3, 0) = m_pSFMapPicoToUEO2I ->ReadMap(position);
            r = P::s().Pico2UE_O2I.R;
        }
        y = r * x;

        if (bcs->m_bIsLOS) {
            bcs->m_DelaySpread = pow(10.0, P::s().Pico2UE_LOS.DelaySpreadSTD * y(0, 0) + P::s().Pico2UE_LOS.DelaySpreadAVE);
            bcs->m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_LOS.AODSpreadSTD * y(1, 0) + P::s().Pico2UE_LOS.AODSpreadAVE));
            bcs->m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_LOS.AOASpreadSTD * y(2, 0) + P::s().Pico2UE_LOS.AOASpreadAVE));
            bcs->m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Pico2UE_LOS.ShadowFadingSTD : 0.0) * y(3, 0);
            bcs->m_KFactorDB = P::s().Pico2UE_LOS.KFactorDBSTD * y(4, 0) + P::s().Pico2UE_LOS.KFactorDBAVE;
        } else {
            bcs->m_DelaySpread = pow(10.0, P::s().Pico2UE_NLOS.DelaySpreadSTD * y(0, 0) + P::s().Pico2UE_NLOS.DelaySpreadAVE);
            bcs->m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_NLOS.AODSpreadSTD * y(1, 0) + P::s().Pico2UE_NLOS.AODSpreadAVE));
            bcs->m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_NLOS.AOASpreadSTD * y(2, 0) + P::s().Pico2UE_NLOS.AOASpreadAVE));
            bcs->m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Pico2UE_NLOS.ShadowFadingSTD : 0.0) * y(3, 0);
        }
        if (_rx.GetSpecial() == 1) {
            bcs->m_DelaySpread = pow(10.0, P::s().Pico2UE_O2I.DelaySpreadSTD * y(0, 0) + P::s().Pico2UE_O2I.DelaySpreadAVE);
            bcs->m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_O2I.AODSpreadSTD_LogDeg * y(1, 0) + P::s().Pico2UE_O2I.AODSpreadAVE));
            bcs->m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_O2I.AOASpreadSTD_LogDeg * y(2, 0) + P::s().Pico2UE_O2I.AOASpreadAVE));
            bcs->m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Pico2UE_O2I.ShadowFadingSTD : 0.0) * y(3, 0);
        }
    }
}

void RuralMacro::SetSCSPara(BasicChannelState* const bcs, double _dTxHeight, double _dRxHeight){
    //小尺度其他参数还没有更新
    if (bcs->m_bIsLOS) {
        bcs->m_EODSpreadAVE_LogDeg = std::max(
                -1.0,
                -1.0 * 0.17 * (bcs->m_d2DDistanceM / 1000)
                - 0.01 * (_dRxHeight - 1.5) + 0.22);
        bcs->m_EODSpreadSTD_LogDeg = 0.34;
        bcs->m_EODOFFSETRAD = 0;
    } else {
        bcs->m_EODSpreadAVE_LogDeg = std::max(
                -1.0, -1.0 * 0.19 * (bcs->m_d2DDistanceM / 1000)
                - 0.01 * (_dRxHeight - 1.5) + 0.28);
        bcs->m_EODSpreadSTD_LogDeg = 0.30;
        bcs->m_EODOFFSETRAD =
                std::atan((35 - 3.5) / bcs->m_d2DDistanceM)
                - std::atan((35 - 1.5) / bcs->m_d2DDistanceM);
    }
    
    ////////////////初始化小尺度计算中需要用的公共变量 //////////////////////////////
    if (bcs->IsMacroToUE()) {
        if (bcs->m_bIsLOS) {
            bcs->m_iNumOfPath = P::s().Macro2UE_LOS.NumOfCluster;
            bcs->m_delayScaling = P::s().Macro2UE_LOS.DelayScaling;
            bcs->m_sigma = P::s().Macro2UE_LOS.PerClusterShadowingSTDDB;
            bcs->m_dClusterASD = P::s().Macro2UE_LOS.ClusterASD;
            bcs->m_dClusterASA = P::s().Macro2UE_LOS.ClusterASA;
            bcs->m_dClusterESD = P::s().Macro2UE_LOS.ClusterESD;
            bcs->m_dClusterESA = P::s().Macro2UE_LOS.ClusterESA;
        } else {
            bcs->m_iNumOfPath = P::s().Macro2UE_NLOS.NumOfCluster;
            bcs->m_delayScaling = P::s().Macro2UE_NLOS.DelayScaling;
            bcs->m_sigma = P::s().Macro2UE_NLOS.PerClusterShadowingSTDDB;
            bcs->m_dClusterASD = P::s().Macro2UE_NLOS.ClusterASD;
            bcs->m_dClusterASA = P::s().Macro2UE_NLOS.ClusterASA;
            bcs->m_dClusterESD = P::s().Macro2UE_NLOS.ClusterESD;
            bcs->m_dClusterESA = P::s().Macro2UE_NLOS.ClusterESA;
        }
        if (bcs->m_pRx->GetSpecial() == 1) {
            bcs->m_iNumOfPath = P::s().Macro2UE_O2I.NumOfCluster;
            bcs->m_delayScaling = P::s().Macro2UE_O2I.DelayScaling;
            bcs->m_sigma = P::s().Macro2UE_O2I.PerClusterShadowingSTDDB;
            bcs->m_dClusterASD = P::s().Macro2UE_O2I.ClusterASD;
            bcs->m_dClusterASA = P::s().Macro2UE_O2I.ClusterASA;
            bcs->m_dClusterESD = P::s().Macro2UE_O2I.ClusterESD;
            bcs->m_dClusterESA = P::s().Macro2UE_O2I.ClusterESA;
        }
    } else {
        if (bcs->m_bIsLOS) {
            bcs->m_iNumOfPath = P::s().Pico2UE_LOS.NumOfCluster;
            bcs->m_delayScaling = P::s().Pico2UE_LOS.DelayScaling;
            bcs->m_sigma = P::s().Pico2UE_LOS.PerClusterShadowingSTDDB;
            bcs->m_dClusterASD = P::s().Pico2UE_LOS.ClusterASD;
            bcs->m_dClusterASA = P::s().Pico2UE_LOS.ClusterASA;
        } else {
            bcs->m_iNumOfPath = P::s().Pico2UE_NLOS.NumOfCluster;
            bcs->m_delayScaling = P::s().Pico2UE_NLOS.DelayScaling;
            bcs->m_sigma = P::s().Pico2UE_NLOS.PerClusterShadowingSTDDB;
            bcs->m_dClusterASD = P::s().Pico2UE_NLOS.ClusterASD;
            bcs->m_dClusterASA = P::s().Pico2UE_NLOS.ClusterASA;
        }
        if (bcs->m_pRx->GetSpecial() == 1) {
            bcs->m_iNumOfPath = P::s().Pico2UE_O2I.NumOfCluster;
            bcs->m_delayScaling = P::s().Pico2UE_O2I.DelayScaling;
            bcs->m_sigma = P::s().Pico2UE_O2I.PerClusterShadowingSTDDB;
            bcs->m_dClusterASD = P::s().Pico2UE_O2I.ClusterASD;
            bcs->m_dClusterASA = P::s().Pico2UE_O2I.ClusterASA;
        }
    }
    ////////////////////////SCS:m_C///////////////////////////////////
    double C_Azimuth, C_Elevation;
    switch (bcs->m_iNumOfPath) {
        case 4:
            C_Azimuth = 0.779;
            C_Elevation = 0.779;
            break;
        case 5:
            C_Azimuth = 0.860;
            C_Elevation = 0.860;
            break;
        case 8:
            C_Azimuth = 1.018;
            C_Elevation = 1.018;
            break;
        case 10:
            C_Azimuth = 1.090;
            C_Elevation = 1.090;
            break;
        case 11:
            C_Azimuth = 1.123;
            C_Elevation = 1.123;
            break;
        case 12:
            C_Azimuth = 1.146;
            C_Elevation = 1.104;
            break;
        case 14:
            C_Azimuth = 1.190;
            C_Elevation = 1.190;
            break;
        case 15:
            C_Azimuth = 1.211;
            C_Elevation = 1.211;
            break;
        case 16:
            C_Azimuth = 1.226;
            C_Elevation = 1.226;
            break;
        case 19:
            C_Azimuth = 1.273;
            C_Elevation = 1.184;
            break;
        case 20:
            C_Azimuth = 1.289;
            C_Elevation = 1.178;
            break;
        default:
            assert(false);
            break;
    }
    bcs->m_C_Azimuth = C_Azimuth;
    bcs->m_C_Elevation = C_Elevation;
}

void RuralMacro::InitializeAOD(SpaceChannelState* pSCS){
    double dMaxPathPower = 0;
    BasicChannelState* pBCS = pSCS->m_pBCS;
    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        if (pSCS->m_vPath[i].m_Power > dMaxPathPower) {
            dMaxPathPower = pSCS->m_vPath[i].m_Power;
        }
    }

    double C = pBCS->m_C_Azimuth;
    if (pBCS->m_bIsLOS) {
        C *= (1.1035 - 0.028 * pBCS->m_KFactorDB - 0.002 * pow(pBCS->m_KFactorDB, 2.0) + 0.0001 * pow(pBCS->m_KFactorDB, 3.0));
    }
    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        pSCS->m_vPath[i].m_AODDeg_Path = 2 * pBCS->m_AODSpreadDeg / 1.4 * sqrt(-1 * log(pSCS->m_vPath[i].m_Power / dMaxPathPower)) / C;
    }

    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        pSCS->m_vPath[i].m_AODDeg_Path = pSCS->m_vPath[i].m_AODDeg_Path * 
                (xUniform_channel() < 0.5 ? -1 : 1) + xNormal_channel(0, pBCS->m_AODSpreadDeg / 7.0) + RAD2DEG(pBCS->m_AODLOSRAD);
    }

    if (pBCS->m_bIsLOS) {
        for (int i = pBCS->m_iNumOfPath - 1; i >= 0; --i) {
            pSCS->m_vPath[i].m_AODDeg_Path = pSCS->m_vPath[i].m_AODDeg_Path - pSCS->m_vPath[0].m_AODDeg_Path + RAD2DEG(pBCS->m_AODLOSRAD);
        }
    }
}

void RuralMacro::InitializeAOA(SpaceChannelState* pSCS){
    double dMaxPathPower = 0;
    BasicChannelState* pBCS = pSCS->m_pBCS;
    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        if (pSCS->m_vPath[i].m_Power > dMaxPathPower) {
            dMaxPathPower = pSCS->m_vPath[i].m_Power;
        }
    }

    double C = pBCS->m_C_Azimuth;
    if (pBCS->m_bIsLOS) {
        C *= (1.1035 - 0.028 * pBCS->m_KFactorDB - 0.002 * pow(pBCS->m_KFactorDB, 2.0) + 0.0001 * pow(pBCS->m_KFactorDB, 3.0));
    }
    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        pSCS->m_vPath[i].m_AOADeg_Path = 2 * pBCS->m_AOASpreadDeg / 1.4 * sqrt(-1 * log(pSCS->m_vPath[i].m_Power / dMaxPathPower)) / C;
    }

    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        pSCS->m_vPath[i].m_AOADeg_Path = pSCS->m_vPath[i].m_AOADeg_Path * 
                (xUniform_channel() < 0.5 ? -1 : 1) + xNormal_channel(0, pBCS->m_AOASpreadDeg / 7.0) + RAD2DEG(pBCS->m_AOALOSRAD);
    }

    if (pBCS->m_bIsLOS) {
        for (int i = pBCS->m_iNumOfPath - 1; i >= 0; --i) {
            pSCS->m_vPath[i].m_AOADeg_Path = pSCS->m_vPath[i].m_AOADeg_Path - pSCS->m_vPath[0].m_AOADeg_Path + RAD2DEG(pBCS->m_AOALOSRAD);
        }
    }
}

void RuralMacro::InitializeEOD(SpaceChannelState* pSCS){
    double dMaxPathPower = 0;
    BasicChannelState* pBCS = pSCS->m_pBCS;
    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        if (pSCS->m_vPath[i].m_Power > dMaxPathPower) {
            dMaxPathPower = pSCS->m_vPath[i].m_Power;

        }
    }

    double C = pBCS->m_C_Elevation;
    if (pBCS->m_bIsLOS) {
        C *= (1.3086 + 0.0339 * pBCS->m_KFactorDB - 0.0077 * pow(pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(pBCS->m_KFactorDB, 3.0));
    }
    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        pSCS->m_vPath[i].m_EODDeg_Path = -1 * pBCS->m_EODSpreadDeg * log(pSCS->m_vPath[i].m_Power / dMaxPathPower) / C;
    }

    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        pSCS->m_vPath[i].m_EODDeg_Path = pSCS->m_vPath[i].m_EODDeg_Path * 
                (xUniform_channel() < 0.5 ? -1 : 1) + xNormal_channel(0, pBCS->m_EODSpreadDeg / 7.0) + RAD2DEG(pBCS->m_EODOFFSETRAD) + RAD2DEG(pBCS->m_EODLOSRAD);
    }

    if (pBCS->m_bIsLOS) {
        for (int i = pBCS->m_iNumOfPath - 1; i >= 0; --i) {
            // 20171215
            pSCS->m_vPath[i].m_EODDeg_Path = pSCS->m_vPath[i].m_EODDeg_Path - pSCS->m_vPath[0].m_EODDeg_Path + RAD2DEG(pBCS->m_EODLOSRAD);
        }
    }
}

void RuralMacro::InitializeEOA(SpaceChannelState* pSCS){
    double dMaxPathPower = 0;
    BasicChannelState* pBCS = pSCS->m_pBCS;
    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        if (pSCS->m_vPath[i].m_Power > dMaxPathPower) {
            dMaxPathPower = pSCS->m_vPath[i].m_Power;
        }
    }
    
    double dmeanEOARAD;
    double C = pBCS->m_C_Elevation;
    if (pBCS->m_bIsLOS) {
        C *= (1.3086 + 0.0339 * pBCS->m_KFactorDB - 0.0077 * pow(pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(pBCS->m_KFactorDB, 3.0));
    }
    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        pSCS->m_vPath[i].m_EOADeg_Path = -1 * pBCS->m_EOASpreadDeg * log(pSCS->m_vPath[i].m_Power / dMaxPathPower) / C;
    }

    dmeanEOARAD = (pBCS->m_pRx->GetSpecial() == 0) ? pBCS->m_EOALOSRAD : (M_PI / 2);

    for (int i = 0; i < pBCS->m_iNumOfPath; ++i) {
        pSCS->m_vPath[i].m_EOADeg_Path = pSCS->m_vPath[i].m_EOADeg_Path * 
                (xUniform_channel() < 0.5 ? -1 : 1) + xNormal_channel(0, pBCS->m_EOASpreadDeg / 7.0) + RAD2DEG(dmeanEOARAD);
    }

    if (pBCS->m_bIsLOS) {
        for (int i = pBCS->m_iNumOfPath - 1; i >= 0; --i) {
            pSCS->m_vPath[i].m_EOADeg_Path = pSCS->m_vPath[i].m_EOADeg_Path - pSCS->m_vPath[0].m_EOADeg_Path + RAD2DEG(dmeanEOARAD);
        }
    }
}