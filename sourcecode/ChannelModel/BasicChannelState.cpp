///@file BasicChannelState.cpp
///@brief  BasicChannelState类定义
///@author wangfei

#include "P.h"
#include "LinkMatrix.h"
#include "Tx.h"
#include "Rx.h"
#include "PathLoss.h"
#include "GaussianMap.h"
#include "AOGSector.h"
#include "./classholder.h"
#include "./WrapAround.h"
#include "./functions.h"
#include "BasicChannelState.h"
#include "../Parameters/Parameters.h"
#include "../Utility/Include.h"
#include "../Statistician/Observer.h"
#include "Antenna.h"

using namespace cm;
size_t cm::POSHash(const POS& pos){
    return std::hash<double>()(pos.first) ^ std::hash<double>()(pos.second);
}
size_t cm::KEYHash(const KEY& key){
    return POSHash(key.first) ^ POSHash(key.second);
}
// 移除全局变量 lm，避免静态初始化顺序问题
// 改为在需要时调用 LinkMatrix::Instance()
std::shared_ptr<GaussianMap> BasicChannelState::m_pDSMapLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAODMapLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAOAMapLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pSFMapLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pKMapLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pEODMapLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pEOAMapLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pDSMapNLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAODMapNLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAOAMapNLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pSFMapNLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pEODMapNLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pEOAMapNLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pDSMapO2I;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAODMapO2I;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAOAMapO2I;
std::shared_ptr<GaussianMap> BasicChannelState::m_pSFMapO2I;
std::shared_ptr<GaussianMap> BasicChannelState::m_pEODMapO2I;
std::shared_ptr<GaussianMap> BasicChannelState::m_pEOAMapO2I;
std::shared_ptr<GaussianMap> BasicChannelState::m_pDSMapPicoToUELOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAODMapPicoToUELOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAOAMapPicoToUELOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pSFMapPicoToUELOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pKMapPicoToUELOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pDSMapPicoToUENLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAODMapPicoToUENLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAOAMapPicoToUENLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pSFMapPicoToUENLOS;
std::shared_ptr<GaussianMap> BasicChannelState::m_pDSMapPicoToUEO2I;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAODMapPicoToUEO2I;
std::shared_ptr<GaussianMap> BasicChannelState::m_pAOAMapPicoToUEO2I;
std::shared_ptr<GaussianMap> BasicChannelState::m_pSFMapPicoToUEO2I;
SafeUnordered_map<KEY, bool> BasicChannelState::m_LOSORNLOS = SafeUnordered_map<KEY, bool>(100, KEYHash);
///构造函数

BasicChannelState::BasicChannelState() {
    m_dTxAntennaPatternDB = 0;
    m_d3DDistanceM = 0;
    /*
        m_dRxAntennaPatternDB = -31;
     */

    m_dRxAntennaPatternDB = -1.0;
    m_LoSLinkLossDB = -1.0;
}

///@brief 大尺度扩展参数的计算
///
///@param ITU ITU信道模型
///@param _tx BS的位置参数
///@param _rx MS的位置参数
///@return void

void BasicChannelState::InitializeSpread(ITU, Point& _tx, Rx& _rx) {
    itpp::mat r, y, x((P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL ? 7 : 5), 1);
    complex<double> position
            = complex<double>(_rx.GetX(), _rx.GetY()) + complex<double>(_tx.GetX(), _tx.GetY());
    if (IsMacroToUE()) {
        if (m_bIsLOS) {
            x(0, 0) = m_pDSMapLOS ->ReadMap(position);
            x(1, 0) = m_pAODMapLOS->ReadMap(position);
            x(2, 0) = m_pAOAMapLOS->ReadMap(position);
            x(3, 0) = m_pSFMapLOS ->ReadMap(position);
            x(4, 0) = m_pKMapLOS ->ReadMap(position);
            if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
                x(5, 0) = m_pEODMapLOS ->ReadMap(position);
                x(6, 0) = m_pEOAMapLOS ->ReadMap(position);
            }
            r = P::s().Macro2UE_LOS.R;
        } else {
            x(0, 0) = m_pDSMapNLOS ->ReadMap(position);
            x(1, 0) = m_pAODMapNLOS->ReadMap(position);
            x(2, 0) = m_pAOAMapNLOS->ReadMap(position);
            x(3, 0) = m_pSFMapNLOS ->ReadMap(position);
            x(4, 0) = 0;
            if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
                x(5, 0) = m_pEODMapNLOS ->ReadMap(position);
                x(6, 0) = m_pEOAMapNLOS ->ReadMap(position);
            }
            r = P::s().Macro2UE_NLOS.R;
        }

        if (_rx.GetSpecial() == 1 &&
                (P::s().IChannelModel_for_Scenario == P::UMI
                || P::s().IChannelModel_for_Scenario == P::UMA
                || P::s().IChannelModel_for_Scenario == P::RMA
                || P::s().IChannelModel_for_Scenario == P::_5GCM_TWO_LAYER)) {
            x(0, 0) = m_pDSMapO2I ->ReadMap(position);
            x(1, 0) = m_pAODMapO2I->ReadMap(position);
            x(2, 0) = m_pAOAMapO2I->ReadMap(position);
            x(3, 0) = m_pSFMapO2I ->ReadMap(position);
            x(4, 0) = 0;
            if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
                x(5, 0) = m_pEODMapO2I ->ReadMap(position);
                x(6, 0) = m_pEOAMapO2I ->ReadMap(position);
            }
            r = P::s().Macro2UE_O2I.R;
        }

        if (P::s().IChannelModel_for_Scenario == P::_5GCM_TWO_LAYER
                && _rx.GetSpecial() == 1) {
            x(0, 0) = m_pDSMapNLOS ->ReadMap(position);
            x(1, 0) = m_pAODMapNLOS->ReadMap(position);
            x(2, 0) = m_pAOAMapNLOS->ReadMap(position);
            x(3, 0) = m_pSFMapNLOS ->ReadMap(position);
            r = P::s().Macro2UE_NLOS.R;
        }

        y = r * x;

        if (m_bIsLOS) {
            m_DelaySpread = pow(10.0, P::s().Macro2UE_LOS.DelaySpreadSTD * y(0, 0) + P::s().Macro2UE_LOS.DelaySpreadAVE);
            m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_LOS.AODSpreadSTD * y(1, 0) + P::s().Macro2UE_LOS.AODSpreadAVE));
            m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_LOS.AOASpreadSTD * y(2, 0) + P::s().Macro2UE_LOS.AOASpreadAVE));
            m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Macro2UE_LOS.ShadowFadingSTD : 0.0) * y(3, 0);
            m_KFactorDB = P::s().Macro2UE_LOS.KFactorDBSTD * y(4, 0) + P::s().Macro2UE_LOS.KFactorDBAVE;
            if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
                m_EODSpreadDeg = std::min(52.0, pow(10.0, m_EODSpreadSTD_LogDeg * y(5, 0) + m_EODSpreadAVE_LogDeg));
                m_EOASpreadDeg = std::min(52.0, pow(10.0, P::s().Macro2UE_LOS.EOASpreadSTD * y(6, 0) + P::s().Macro2UE_LOS.EOASpreadAVE));
            }
        } else {
            m_DelaySpread = pow(10.0, P::s().Macro2UE_NLOS.DelaySpreadSTD * y(0, 0) + P::s().Macro2UE_NLOS.DelaySpreadAVE);
            m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_NLOS.AODSpreadSTD * y(1, 0) + P::s().Macro2UE_NLOS.AODSpreadAVE));
            m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_NLOS.AOASpreadSTD * y(2, 0) + P::s().Macro2UE_NLOS.AOASpreadAVE));
            m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Macro2UE_NLOS.ShadowFadingSTD : 0.0) * y(3, 0);
            if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
                m_EODSpreadDeg = std::min(52.0, pow(10.0, m_EODSpreadSTD_LogDeg * y(5, 0) + m_EODSpreadAVE_LogDeg));
                m_EOASpreadDeg = std::min(52.0, pow(10.0, P::s().Macro2UE_NLOS.EOASpreadSTD * y(6, 0) + P::s().Macro2UE_NLOS.EOASpreadAVE));
            }
        }

        if (_rx.GetSpecial() == 1 &&
                (P::s().IChannelModel_for_Scenario == P::UMI
                || P::s().IChannelModel_for_Scenario == P::UMA
                || P::s().IChannelModel_for_Scenario == P::RMA
                || P::s().IChannelModel_for_Scenario == P::_5GCM_TWO_LAYER)) {

            m_DelaySpread = pow(10.0, P::s().Macro2UE_O2I.DelaySpreadSTD * y(0, 0) + P::s().Macro2UE_O2I.DelaySpreadAVE);
            m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_O2I.AODSpreadSTD * y(1, 0) + P::s().Macro2UE_O2I.AODSpreadAVE));
            m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_O2I.AOASpreadSTD * y(2, 0) + P::s().Macro2UE_O2I.AOASpreadAVE));
            m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Macro2UE_O2I.ShadowFadingSTD : 0.0) * y(3, 0);
            if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {

                m_EOASpreadDeg = std::min(52.0, pow(10.0, P::s().Macro2UE_O2I.EOASpreadSTD * y(6, 0) + P::s().Macro2UE_O2I.EOASpreadAVE));
            }
        }

        if (P::s().IChannelModel_for_Scenario == P::_5GCM_TWO_LAYER
                && _rx.GetSpecial() == 1) {
            m_DelaySpread = pow(10.0, P::s().Macro2UE_NLOS.DelaySpreadSTD * y(0, 0) + P::s().Macro2UE_NLOS.DelaySpreadAVE);
            m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_NLOS.AODSpreadSTD * y(1, 0) + P::s().Macro2UE_NLOS.AODSpreadAVE));
            m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Macro2UE_NLOS.AOASpreadSTD * y(2, 0) + P::s().Macro2UE_NLOS.AOASpreadAVE));
            m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Macro2UE_NLOS.ShadowFadingSTD : 0.0) * y(3, 0);
        }
    } else {
        if (m_bIsLOS) {
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

        if (_rx.GetSpecial() == 1
                && P::s().IChannelModel_for_Scenario != P::InH) {
            x(0, 0) = m_pDSMapPicoToUEO2I ->ReadMap(position);
            x(1, 0) = m_pAODMapPicoToUEO2I->ReadMap(position);
            x(2, 0) = m_pAOAMapPicoToUEO2I->ReadMap(position);
            x(3, 0) = m_pSFMapPicoToUEO2I ->ReadMap(position);
            r = P::s().Pico2UE_O2I.R;
        }

        y = r * x;

        if (m_bIsLOS) {
            m_DelaySpread = pow(10.0, P::s().Pico2UE_LOS.DelaySpreadSTD * y(0, 0) + P::s().Pico2UE_LOS.DelaySpreadAVE);
            m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_LOS.AODSpreadSTD * y(1, 0) + P::s().Pico2UE_LOS.AODSpreadAVE));
            m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_LOS.AOASpreadSTD * y(2, 0) + P::s().Pico2UE_LOS.AOASpreadAVE));
            m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Pico2UE_LOS.ShadowFadingSTD : 0.0) * y(3, 0);
            m_KFactorDB = P::s().Pico2UE_LOS.KFactorDBSTD * y(4, 0) + P::s().Pico2UE_LOS.KFactorDBAVE;
        } else {
            m_DelaySpread = pow(10.0, P::s().Pico2UE_NLOS.DelaySpreadSTD * y(0, 0) + P::s().Pico2UE_NLOS.DelaySpreadAVE);
            m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_NLOS.AODSpreadSTD * y(1, 0) + P::s().Pico2UE_NLOS.AODSpreadAVE));
            m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_NLOS.AOASpreadSTD * y(2, 0) + P::s().Pico2UE_NLOS.AOASpreadAVE));
            m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Pico2UE_NLOS.ShadowFadingSTD : 0.0) * y(3, 0);
        }

        if (_rx.GetSpecial() == 1
                && P::s().IChannelModel_for_Scenario != P::InH) {
            m_DelaySpread = pow(10.0, P::s().Pico2UE_O2I.DelaySpreadSTD * y(0, 0) + P::s().Pico2UE_O2I.DelaySpreadAVE);
            m_AODSpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_O2I.AODSpreadSTD_LogDeg * y(1, 0) + P::s().Pico2UE_O2I.AODSpreadAVE));
            m_AOASpreadDeg = std::min(104.0, pow(10.0, P::s().Pico2UE_O2I.AOASpreadSTD_LogDeg * y(2, 0) + P::s().Pico2UE_O2I.AOASpreadAVE));
            m_ShadowFadingDB = (P::s().FX.IISShadowFadingUsed ? P::s().Pico2UE_O2I.ShadowFadingSTD : 0.0) * y(3, 0);
        }
    }
}
///@brief Initialize the basic channel m_state

bool BasicChannelState::IsUMAOrRMAOrUMI_Indoor_UE(cm::Rx& _rx) {

    return _rx.GetSpecial() == 1
            && (P::s().IChannelModel_for_Scenario == P::UMA
            || P::s().IChannelModel_for_Scenario == P::RMA
            || P::s().IChannelModel_for_Scenario == P::UMI);
}

void BasicChannelState::Initialize(cm::Tx& _tx, cm::Rx& _rx) {
    m_pTx = &_tx;
    m_pRx = &_rx;
    cm::LinkMatrix& lm = cm::LinkMatrix::Instance();
    double dFrequencyGHz = P::s().FX.DRadioFrequencyMHz_Macro * 1e-3;
    Point tx_wrap;

    tx_wrap = WrapAround::Instance().WrapTx(_rx, _tx);

    m_d2DDistanceM = Distance(_rx, tx_wrap);
    double dTxHeight = _tx.GetTxHeightM();
    double dRxHeight = _rx.GetRxHeightM();
    m_d3DDistanceM = sqrt(pow(m_d2DDistanceM, 2.0) + pow(abs(dTxHeight - dRxHeight), 2.0));
    m_dUE2BSTiltRAD = std::atan((dTxHeight - dRxHeight) / m_d2DDistanceM); //以垂直于Z轴为0度，向上为负角度，向下为正角度
    lm.SetPos2Din(_tx, _rx); //针对5GCM_Uma||ITU_UMA 室内用户

    if (IsUMAOrRMAOrUMI_Indoor_UE(_rx)) {
        std::pair<int, int> pos(_tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());
        double d2D_Outdoor_DistanceM = m_d2DDistanceM - lm.GetPos2Din(pos);
        m_bIsLOS = DecideLOS(d2D_Outdoor_DistanceM, _tx, _rx);
    } else {
        m_bIsLOS = DecideLOS(m_d2DDistanceM, _tx, _rx);
    }
    
    //20200413
    //ChannelInfo::Instance().H_TX_RX[_tx.GetTxID()][_rx.GetRxID()].IsLOS = m_bIsLOS;

    //////////////////step3//////////////////
    //20171218
    //    if ( P::s().IChannelModel_for_Scenario == P::InH )
    ////        && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA ))
    //        m_PathLossDB = lm.GetPathLossFun(_tx, _rx, m_bIsLOS)->Db(m_d2DDistanceM, dRxHeight);
    //    else
    m_PathLossDB = lm.GetPathLossFun(_tx, _rx, m_bIsLOS)->Db(m_d3DDistanceM, dRxHeight);

    //////////////////为step7做准备//////////////////
    Point orient1 = _rx - tx_wrap;
    //????std::arg(）输出的角度取值范围是（0～360）还是（-180～+180）？最好是（0～360），因为BS和UE以及其天线朝向为（0～360）
    double angle1RAD = std::arg(std::complex <double>(orient1.GetX(), orient1.GetY()));
    angle1RAD = ConvergeAngle(angle1RAD);
    m_AODLOSRAD = angle1RAD; //GCS

    Point orient2 = tx_wrap - _rx;
    double angle2RAD = std::arg(std::complex <double>(orient2.GetX(), orient2.GetY()));
    angle2RAD = ConvergeAngle(angle2RAD);
    m_AOALOSRAD = angle2RAD; //GCS
    //GCS
    m_EODLOSRAD = m_dUE2BSTiltRAD + M_PI / 2; //坐标系的Z轴为朝上的方向为0度
    //GCS
    m_EOALOSRAD = M_PI - m_EODLOSRAD;

    if (P::s().IChannelModel_for_Scenario == P::UMA
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA) {
        if (m_bIsLOS) {
            m_EODOFFSETRAD = 0;
            m_EODSpreadSTD_LogDeg = 0.4;
            m_EODSpreadAVE_LogDeg = std::max(-0.5, -1.0 * 2.1 * (m_d2DDistanceM / 1000) - 0.01 * (dRxHeight - 1.5) + 0.75);
        } else {
            m_EODOFFSETRAD = DEG2RAD(-1.0 * pow(10, -1.0 * 0.62 * log10(std::max(10.0, m_d2DDistanceM)) + 1.93 - 0.07 * (dRxHeight - 1.5)));
            m_EODSpreadSTD_LogDeg = 0.49;
            m_EODSpreadAVE_LogDeg = std::max(-0.5, -1.0 * 2.1 * (m_d2DDistanceM / 1000) - 0.01 * (dRxHeight - 1.5) + 0.9);
        }
    } else if (P::s().IChannelModel_for_Scenario == P::UMI
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA) {
        if (m_bIsLOS) {
            m_EODOFFSETRAD = 0;
            m_EODSpreadSTD_LogDeg = 0.4;
            m_EODSpreadAVE_LogDeg = std::max(-0.5,
                    -1.0 * 2.1 * (m_d2DDistanceM / 1000)
                    + 0.01 * abs(dRxHeight - dTxHeight) + 0.75);
        } else {
            m_EODOFFSETRAD =
                    DEG2RAD(-1.0 * pow(10,
                    -1.0 * 0.55 * log10(std::max(10.0, m_d2DDistanceM)) + 1.6));
            m_EODSpreadSTD_LogDeg = 0.6;
            m_EODSpreadAVE_LogDeg =
                    std::max(-0.5,
                    -1.0 * 2.1 * (m_d2DDistanceM / 1000)
                    + 0.01 * std::max(0.0, dRxHeight - dTxHeight) + 0.9);
        }
    } else if ((P::s().IChannelModel_for_Scenario == P::InH
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)) {//20180329SZX
        m_EODSpreadAVE_LogDeg = P::s().Macro2UE_LOS.EODSpreadAVE;
        m_EODSpreadSTD_LogDeg = P::s().Macro2UE_LOS.EODSpreadSTD;
        m_EODOFFSETRAD = 0;

    } else if (P::s().IChannelModel_for_Scenario == P::UMA
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB) {//UMA

        // 20171212
        double dModifiedFreqGHz
                = (dFrequencyGHz >= 6.0) ? dFrequencyGHz : 6.0;

        //O2I的参数怎么更新？,o2i 的地方直接采用这些参数
        //            assert(false);
        if (m_bIsLOS) {
            m_EODSpreadAVE_LogDeg = std::max(
                    -0.5, -1.0 * 2.1 * (m_d2DDistanceM / 1000) - 0.01 * (dRxHeight - 1.5) + 0.75);
            m_EODSpreadSTD_LogDeg = 0.4;
            m_EODOFFSETRAD = 0;
            /*
            
                        P::s().Macro2UE_LOS.EODSpreadAVE = m_EODSpreadAVE_LogDeg;
                        P::s().Macro2UE_LOS.EODSpreadSTD = m_EODSpreadSTD_LogDeg;  
            
                        P::s().Macro2UE_O2I.EODSpreadAVE = m_EODSpreadAVE_LogDeg;
                        P::s().Macro2UE_O2I.EODSpreadSTD = m_EODSpreadSTD_LogDeg;
             */

        } else {
            m_EODSpreadAVE_LogDeg = std::max(
                    -0.5, -1.0 * 2.1 * (m_d2DDistanceM / 1000) - 0.01 * (dRxHeight - 1.5) + 0.9);

            m_EODSpreadSTD_LogDeg = 0.49;

            /*
                        P::s().Macro2UE_NLOS.EODSpreadAVE = m_EODSpreadAVE_LogDeg;
                        P::s().Macro2UE_NLOS.EODSpreadSTD = m_EODSpreadSTD_LogDeg;  
            
                        P::s().Macro2UE_O2I.EODSpreadAVE = m_EODSpreadAVE_LogDeg;
                        P::s().Macro2UE_O2I.EODSpreadSTD = m_EODSpreadSTD_LogDeg;
             */

            double a = (208 * log10(dModifiedFreqGHz) - 782) / 1000.0;
            double b = 25.0;
            double c = -0.13 * log10(dModifiedFreqGHz) + 2.03;
            double e = 7.66 * log10(dModifiedFreqGHz) - 5.96;

            // 20171212
            double temp =
                    (a * log10(std::max(b, m_d2DDistanceM))
                    + c - 0.07 * (dRxHeight - 1.5));
            //            
            //            double temp = 
            //                (a * 1000.0 
            //                * log10(std::max(0.025, m_d2DDistanceM)) 
            //                + c);
            ///171124 36.873 42页 Table 7.5-7 ZoD offset
            //            double temp = ((0.208 * log10(dFrequencyGHz) - 0.782) * log10(std::max(25.0, m_d2DDistanceM)) - 0.13 * log10(dFrequencyGHz) + 2.03);
            m_EODOFFSETRAD = DEG2RAD(e - pow(10, temp));


        }


    } else if ((P::s().IChannelModel_for_Scenario == P::UMI
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB)) {//UMI
        //            assert(false); // O2I 的parameter
        if (m_bIsLOS) {
            m_EODSpreadAVE_LogDeg =
                    std::max(-0.21,
                    -1.0 * 14.8 * (m_d2DDistanceM / 1000)
                    + 0.01 * abs(dRxHeight - dTxHeight) + 0.83);
            m_EODSpreadSTD_LogDeg = 0.35;
            m_EODOFFSETRAD = 0;
        } else {
            m_EODSpreadAVE_LogDeg =
                    std::max(-0.5,
                    -1.0 * 3.1 * (m_d2DDistanceM / 1000)
                    + 0.01 * std::max(0.0, dRxHeight - dTxHeight) + 0.2);
            m_EODSpreadSTD_LogDeg = 0.35;
            m_EODOFFSETRAD =
                    DEG2RAD(-1.0 * pow(10,
                    -1.0 * 1.5 * log10(std::max(10.0, m_d2DDistanceM)) + 3.3));
        }

    } else if ((P::s().IChannelModel_for_Scenario == P::InH
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB)) { //for indoor
        m_EODSpreadAVE_LogDeg = P::s().Macro2UE_LOS.EODSpreadAVE;
        m_EODSpreadSTD_LogDeg = P::s().Macro2UE_LOS.EODSpreadSTD;
        m_EODOFFSETRAD = 0;
    } else if (P::s().IChannelModel_for_Scenario == P::RMA) {
        //        && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA )) {// for RMA
        //        assert(false);
        //小尺度其他参数还没有更新
        if (m_bIsLOS) {
            m_EODSpreadAVE_LogDeg = std::max(
                    -1.0,
                    -1.0 * 0.17 * (m_d2DDistanceM / 1000)
                    - 0.01 * (dRxHeight - 1.5) + 0.22);
            m_EODSpreadSTD_LogDeg = 0.34;
            m_EODOFFSETRAD = 0;

            /*
                        //20171218
                        P::s().Macro2UE_LOS.EODSpreadAVE = m_EODSpreadAVE_LogDeg;
                        P::s().Macro2UE_LOS.EODSpreadSTD = m_EODSpreadSTD_LogDeg;  
                        P::s().Macro2UE_LOS.EODoffsetAVE = m_EODOFFSETRAD;
             */



        } else {
            m_EODSpreadAVE_LogDeg = std::max(
                    -1.0, -1.0 * 0.19 * (m_d2DDistanceM / 1000)
                    - 0.01 * (dRxHeight - 1.5) + 0.28);
            m_EODSpreadSTD_LogDeg = 0.30;
            m_EODOFFSETRAD =
                    std::atan((35 - 3.5) / m_d2DDistanceM)
                    - std::atan((35 - 1.5) / m_d2DDistanceM);
            /*
            
                        //20171218
                        P::s().Macro2UE_NLOS.EODSpreadAVE = m_EODSpreadAVE_LogDeg;
                        P::s().Macro2UE_NLOS.EODSpreadSTD = m_EODSpreadSTD_LogDeg;  
                        P::s().Macro2UE_NLOS.EODoffsetAVE = m_EODOFFSETRAD;
            
                        P::s().Macro2UE_O2I.EODSpreadAVE = m_EODSpreadAVE_LogDeg;
                        P::s().Macro2UE_O2I.EODSpreadSTD = m_EODSpreadSTD_LogDeg;
            
             */
        }
    } else {
        assert(false);
    }

    /*
        if (IsMacroToUE()) {
            m_dTxAntennaPatternDB = _tx.GetTxAOGDB(ALoSRAD_GCS2LCS(m_AODLOSRAD, m_EODLOSRAD, _tx.GetTxOrientRAD(), DEG2RAD(Parameters::Instance().Macro.DMechanicalTiltDeg), 0)
                    , ELoSRAD_GCS2LCS(m_AODLOSRAD, m_EODLOSRAD, _tx.GetTxOrientRAD(), DEG2RAD(Parameters::Instance().Macro.DMechanicalTiltDeg), 0));
        } else {
            //这儿需要修改，目前只能适用于全向的Pico
            m_dTxAntennaPatternDB = Parameters::Instance().SmallCell.LINK.DPicoAntennaGainDbi;
        }
     */


    /*
        m_BCS_StrongestUEPanelIndex = 0;
        if(P::s().RX.IHPanelNum == 1){
            if(Parameters::Instance().Macro.LINK.DRadioFrequencyMHz > 6000){//使用HBF
                assert(false);
            
                for (int i = 0; i < 2; ++i) {//2：权宜之计
                    double dRxAntennaPatternDB = _rx.GetRxAOGDB((ALoSRAD_GCS2LCS(m_AOALOSRAD, m_EOALOSRAD, _rx.GetRxOrientRAD() + i * M_PI, 0, 0)),
                            ELoSRAD_GCS2LCS(m_AOALOSRAD, m_EOALOSRAD, _rx.GetRxOrientRAD() + i * M_PI, 0, 0));
                    if (dRxAntennaPatternDB > m_dRxAntennaPatternDB) {
                        m_dRxAntennaPatternDB = dRxAntennaPatternDB;
                        m_BCS_StrongestUEPanelIndex = i;
                    }
                }
                m_vdRxAntennaPatternDB.push_back(m_dRxAntennaPatternDB);
            }else if(Parameters::Instance().Macro.LINK.DRadioFrequencyMHz < 6000){
                    m_dRxAntennaPatternDB = _rx.GetRxAOGDB((ALoSRAD_GCS2LCS(m_AOALOSRAD, m_EOALOSRAD, _rx.GetRxOrientRAD(), 0, 0)),
                            ELoSRAD_GCS2LCS(m_AOALOSRAD, m_EOALOSRAD, _rx.GetRxOrientRAD(), 0, 0));
                    m_vdRxAntennaPatternDB.push_back(m_dRxAntennaPatternDB);
            }else
                assert(false);
        }else if(P::s().RX.IHPanelNum == 2){
            for (int i = 0; i < P::s().RX.IHPanelNum; ++i) {
                double dRxAntennaPatternDB = _rx.GetRxAOGDB((ALoSRAD_GCS2LCS(m_AOALOSRAD, m_EOALOSRAD, _rx.GetRxOrientRAD() + i * M_PI, 0, 0)),
                        ELoSRAD_GCS2LCS(m_AOALOSRAD, m_EOALOSRAD, _rx.GetRxOrientRAD() + i * M_PI, 0, 0));
                m_vdRxAntennaPatternDB.push_back(dRxAntennaPatternDB);
                if (dRxAntennaPatternDB > m_dRxAntennaPatternDB) {
                    m_dRxAntennaPatternDB = dRxAntennaPatternDB;
                    m_BCS_StrongestUEPanelIndex = i;
                }
            }
            m_vdRxAntennaPatternDB.push_back(m_dRxAntennaPatternDB);
        }else if(Parameters::Instance().Macro.LINK.DRadioFrequencyMHz < 6000){
                m_dRxAntennaPatternDB = _rx.GetRxAOGDB((ALoSRAD_GCS2LCS(m_AOALOSRAD, m_EOALOSRAD, _rx.GetRxOrientRAD(), 0, 0)),
                        ELoSRAD_GCS2LCS(m_AOALOSRAD, m_EOALOSRAD, _rx.GetRxOrientRAD(), 0, 0));
                m_vdRxAntennaPatternDB.push_back(m_dRxAntennaPatternDB);
        }else
            assert(false);
    }else if(P::s().RX.IHPanelNum == 2){
        for (int i = 0; i < P::s().RX.IHPanelNum; ++i) {
            double dRxAntennaPatternDB = _rx.GetRxAOGDB((ALoSRAD_GCS2LCS(m_AOALOSRAD, m_EOALOSRAD, _rx.GetRxOrientRAD() + i * M_PI, 0, 0)),
                    ELoSRAD_GCS2LCS(m_AOALOSRAD, m_EOALOSRAD, _rx.GetRxOrientRAD() + i * M_PI, 0, 0));
            m_vdRxAntennaPatternDB.push_back(dRxAntennaPatternDB);
            if (dRxAntennaPatternDB > m_dRxAntennaPatternDB) {
                m_dRxAntennaPatternDB = dRxAntennaPatternDB;
                m_BCS_StrongestUEPanelIndex = i;
            }
        }
     */
    //////////////////step4//////////////////
    InitializeSpread(ITU(), tx_wrap, _rx);

    //    m_dPhaseDegLOSXX = xUniform_channel(0, 360);
    //    m_dPhaseDegLOSYY = m_dPhaseDegLOSXX;
    //20171218
    //    if(P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA){
    //        m_dPhaseDegLOSXX = xUniform_channel(0, 360);
    //        m_dPhaseDegLOSYY = m_dPhaseDegLOSXX;
    //    }else if(P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB){
    //        double dWaveLength = P::s().FX.DWaveLength_Macro;
    //        m_dPhaseDegLOSXX = -2 * M_PI * m_d3DDistanceM / dWaveLength;
    //        m_dPhaseDegLOSYY = m_dPhaseDegLOSXX;
    //    }else{
    //        assert(false);
    //    }


    double dInCarLossDB = _rx.GetInCarLossDB();
    /*
        m_LoSLinkLossDB = m_PathLossDB + m_ShadowFadingDB + m_dTxAntennaPatternDB + m_dRxAntennaPatternDB
                + dInCarLossDB;
        m_PurePathLossDB = m_LoSLinkLossDB - m_dTxAntennaPatternDB - m_dRxAntennaPatternDB;
     */

    m_PurePathLossDB = m_PathLossDB + m_ShadowFadingDB + dInCarLossDB
            + Parameters::Instance().Macro.DOTA_dB
            + Parameters::Instance().Macro.DMSBodyLoss_dB
            + Parameters::Instance().Macro.DTransmissionLineLoss_dB;

    {
        //hyl 冗余
//        Observer::Print("LargeScale") << m_PathLossDB
//                << setw(20) << m_ShadowFadingDB
//                << setw(20) << m_dTxAntennaPatternDB
//                << setw(20) << m_dRxAntennaPatternDB
//                << setw(20) << dInCarLossDB
//                << setw(20) << m_LoSLinkLossDB
//                << setw(20) << m_PurePathLossDB << endl;
    }
    ////////////////初始化小尺度计算中需要用的公共变量 //////////////////////////////
    if (IsMacroToUE()) {
        m_iNumOfPath = P::s().Macro2UE_LOS.NumOfCluster;
        m_delayScaling = P::s().Macro2UE_LOS.DelayScaling;
        m_sigma = P::s().Macro2UE_LOS.PerClusterShadowingSTDDB;
        m_dClusterASD = P::s().Macro2UE_LOS.ClusterASD;
        m_dClusterASA = P::s().Macro2UE_LOS.ClusterASA;
        if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
            m_dClusterESD = P::s().Macro2UE_LOS.ClusterESD;
            m_dClusterESA = P::s().Macro2UE_LOS.ClusterESA;
        }
        if (m_pRx->GetSpecial() == 1
                && P::s().IChannelModel_for_Scenario != P::InH) {
            m_iNumOfPath = P::s().Macro2UE_O2I.NumOfCluster;
            m_delayScaling = P::s().Macro2UE_O2I.DelayScaling;
            m_sigma = P::s().Macro2UE_O2I.PerClusterShadowingSTDDB;
            m_dClusterASD = P::s().Macro2UE_O2I.ClusterASD;
            m_dClusterASA = P::s().Macro2UE_O2I.ClusterASA;
            m_dClusterESD = P::s().Macro2UE_O2I.ClusterESD;
            m_dClusterESA = P::s().Macro2UE_O2I.ClusterESA;
        }
        m_iTxAntennaNum = P::s().MacroTX.ITotalAntNum;

    } else {
        m_iNumOfPath = P::s().Pico2UE_LOS.NumOfCluster;
        m_delayScaling = P::s().Pico2UE_LOS.DelayScaling;
        m_sigma = P::s().Pico2UE_LOS.PerClusterShadowingSTDDB;
        m_dClusterASD = P::s().Pico2UE_LOS.ClusterASD;
        m_dClusterASA = P::s().Pico2UE_LOS.ClusterASA;
        if (m_pRx->GetSpecial() == 1
                && P::s().IChannelModel_for_Scenario != P::InH) {
            m_iNumOfPath = P::s().Pico2UE_O2I.NumOfCluster;
            m_delayScaling = P::s().Pico2UE_O2I.DelayScaling;
            m_sigma = P::s().Pico2UE_O2I.PerClusterShadowingSTDDB;
            m_dClusterASD = P::s().Pico2UE_O2I.ClusterASD;
            m_dClusterASA = P::s().Pico2UE_O2I.ClusterASA;
        }
        m_iTxAntennaNum = P::s().PicoTX.ITotalAntNum;
    }
    ////////////////////////SCS:m_C///////////////////////////////////
    switch (m_iNumOfPath) {
        case 4:
            m_C_Azimuth = 0.779;
            m_C_Elevation = 0.779;
            break;
        case 5:
            m_C_Azimuth = 0.860;
            m_C_Elevation = 0.860;
            break;
        case 8:
            m_C_Azimuth = 1.018;
            m_C_Elevation = 1.018;
            break;
        case 10:
            m_C_Azimuth = 1.090;
            m_C_Elevation = 1.090;
            break;
        case 11:
            m_C_Azimuth = 1.123;
            m_C_Elevation = 1.123;
            break;
        case 12:
            m_C_Azimuth = 1.146;
            m_C_Elevation = 1.104;
            break;
        case 14:
            m_C_Azimuth = 1.190;
            m_C_Elevation = 1.190;
            break;
        case 15:
            m_C_Azimuth = 1.211;
            m_C_Elevation = 1.211;
            break;
        case 16:
            m_C_Azimuth = 1.226;
            m_C_Elevation = 1.226;
            break;
        case 19:
            m_C_Azimuth = 1.273;
            m_C_Elevation = 1.184;
            break;
        case 20:
            m_C_Azimuth = 1.289;
            m_C_Elevation = 1.178;
            break;
        default:
            assert(false);
    }
    if (P::s().IChannelModel_for_Scenario == P::InH) {
        switch (m_iNumOfPath) {
            case 15:
                if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)//20180330szx
                {
                    m_C_Azimuth = 1.434;
                    m_C_Elevation = 1.1088;
                } else {
                    m_C_Azimuth = 1.211;
                    m_C_Elevation = 1.1088;
                }
                break;
            case 19:
                if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)//20180330szx
                {
                    m_C_Azimuth = 1.501;
                    m_C_Elevation = 1.1764;
                } else {
                    m_C_Azimuth = 1.273;
                    m_C_Elevation = 1.184;
                }

                break;

                // 20171127 
            case 8:
                m_C_Elevation = 0.889;
                break;
            case 10:
                m_C_Elevation = 0.957;
                break;
            case 11:
                m_C_Elevation = 1.031;
                break;
            case 12:
                m_C_Elevation = 1.104;
                break;
            case 20:
                m_C_Elevation = 1.178;
                break;
        }
    }
}

bool BasicChannelState::IsMacroToUE() const{
    if (!m_pTx) {
        cout << "发射机不存在！" << endl;
        assert(false);
    }
    if (P::s().IChannelModel_for_Scenario != P::_5GCM_TWO_LAYER)
        return true;

    if ((m_pTx->GetTxID()) % (Parameters::Instance().BASIC.ITotalBTSNumPerBS) < Parameters::Instance().BASIC.IBTSPerBS)
        return true;
    else
        return false;
}

bool BasicChannelState::IsPicoToUE() const{
    return (!IsMacroToUE());
}

Tx& BasicChannelState::GetItsMacro() const{
    if (IsMacroToUE()) {
        return *m_pTx;
    } else {
        int iTotalBTSNumPerSite = Parameters::Instance().BASIC.ITotalBTSNumPerBS;
        int iPicoNumPerMacro = Parameters::Instance().SmallCell.LINK.IPicoNumPerMacro;
        int i = m_pTx->GetTxID();
        int j = i % iTotalBTSNumPerSite;
        int m = i - j;
        int n = (j - 3) / iPicoNumPerMacro;
        int macroid = m + n;
        return Tx::GetTx(macroid);
    }
}

bool BasicChannelState::DecideLOS(double _dDisM_2D, Tx& _tx, Rx& _rx) {
    POS txpos = std::make_pair(_tx.GetX(), _tx.GetY());
    POS rxpos = std::make_pair(_rx.GetX(), _rx.GetY());
    KEY key = std::make_pair(txpos, rxpos);

    bool result;
    double dPossLOS;
    static std::mutex m;
    m.lock();
    if (m_LOSORNLOS.find(key) != m_LOSORNLOS.end()) {
        result = m_LOSORNLOS[key];
        m.unlock();
    } else {
        m.unlock();
        switch (P::s().IChannelModel_for_Scenario) {
            case P::InF:
            {
                double d_clutter = 2; //2m
                double r = random.xUniform(0.4, 0.9); //>=40，最大值是多少？
                double h_c = random.xUniform(0, 10); //0-10
                double h_UT = _rx.GetRxHeightM();
                double h_BS = _tx.GetTxHeightM();
                double k_subsce_SH_DH = -1 * (d_clutter / log(1 - r))*((h_BS - h_UT) / (h_c - h_UT));
                dPossLOS = std::exp(-1 * _dDisM_2D / k_subsce_SH_DH);
                result = random.xUniform_channel() < dPossLOS;
            }
                break;
            case P::InH:
                /*
                                if(P::s().IChannelModel_VariantMode 
                                    == P::ITU_ChannelModel_ModeA) {
                    
                                    if (_dDisM_2D <= 18) {
                                        result = true;
                                    } else if (_dDisM_2D >= 37) {
                                        result = xUniform_channel() < 0.5;
                                    } else {
                                        dPossLOS = std::exp(-1 * (_dDisM_2D - 18) / 27);
                                        result = xUniform_channel() < dPossLOS;
                                    }
                                } else if (P::s().IChannelModel_VariantMode 
                                    == P::ITU_ChannelModel_ModeB) {
                 */

                if (_dDisM_2D <= 5) {
                    result = true;
                } else if (_dDisM_2D <= 49) {
                    dPossLOS = std::exp(-1 * (_dDisM_2D - 5) / 70.8);
                    result = random.xUniform_channel() < dPossLOS;
                } else {
                    dPossLOS = std::exp(-1 * (_dDisM_2D - 49) / 211.7)*0.54;
                    result = random.xUniform_channel() < dPossLOS;
                }

                /*
                                } else {
                                    assert(false);
                                }
                 */

                break;

            case P::UMI:

                if (_dDisM_2D <= 18) {
                    result = true;
                } else if (_dDisM_2D > 18) {
                    dPossLOS = 18 / _dDisM_2D
                            + std::exp(-1 * _dDisM_2D / 36.0) * (1 - 18.0 / _dDisM_2D);
                    result = random.xUniform_channel() < dPossLOS;
                } else {
                    assert(false);
                }
                //                dPossLOS = std::min(18 / _dDisM_2D, 1.0) * (1 - std::exp(-1 * _dDisM_2D / 36)) + std::exp(-1 * _dDisM_2D / 36);
                //                result = xUniform_channel() < dPossLOS;
                break;
            case P::UMA:
                if (P::s().IChannelModel_VariantMode
                        == P::ITU_ChannelModel_ModeA) {

                    assert(
                            Parameters::Instance().LINK_CTRL.I2DOr3DChannel == Parameters::IS3DCHANNEL);
                    // 20171201
                    dPossLOS = cm::CalcPossLOS_for_UMA_4G(
                            _dDisM_2D, _rx.GetRxHeightM());

                    result = random.xUniform_channel() < dPossLOS;
                } else if (P::s().IChannelModel_VariantMode
                        == P::ITU_ChannelModel_ModeB) {

                    // 20171201
                    dPossLOS = cm::CalcPossLOS_for_UMA_5G_ITU(
                            _dDisM_2D, _rx.GetRxHeightM());

                    result = random.xUniform_channel() < dPossLOS;
                } else {
                    assert(false);
                }
                break;

            case P::RMA:
                if (_dDisM_2D <= 10) {
                    result = true;
                } else if (_dDisM_2D > 10) {
                    dPossLOS = std::exp(-1 * (_dDisM_2D - 10.0) / 1000.0);
                    result = random.xUniform_channel() < dPossLOS;
                } else {
                    assert(false);
                }
                break;
            case P::_5GCM_TWO_LAYER:
                if (IsMacroToUE()) {
                    dPossLOS = std::min(18.0 / _dDisM_2D, 1.0) * (1 - std::exp(-1 * _dDisM_2D / 63)) + std::exp(-1 * _dDisM_2D / 63);
                    result = random.xUniform_channel() < dPossLOS;
                } else {
                    ///@xlong   若此发射机为Pico，则Pico-MS的路损模型使用ITU-UMi
                    dPossLOS = std::min(18.0 / _dDisM_2D, 1.0) * (1 - std::exp(-1 * _dDisM_2D / 36)) + std::exp(-1 * _dDisM_2D / 36);
                    result = random.xUniform_channel() < dPossLOS;
                }
                break;
            default:
                assert(false);
        }
        std::mutex m1;
        m1.lock();
        m_LOSORNLOS[key] = result;
        m1.unlock();
    }

    return result;
}
//202511
void BasicChannelState::SetLinkCategory_as_BS2RIS(){
    m_iLinkCategory = 1;
}
void BasicChannelState::SetLinkCategory_as_RIS2MS(){
    m_iLinkCategory = 2;
}



void BasicChannelState::InitializeMap() {
    m_pDSMapLOS = std::make_shared<GaussianMap>(P::s().Macro2UE_LOS.DSCorrDistM);
    m_pAODMapLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_LOS.ASDCorrDistM);
    m_pAOAMapLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_LOS.ASACorrDistM);
    m_pSFMapLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_LOS.SFCorrDistM);
    m_pKMapLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_LOS.KCorrDistM);

    m_pDSMapNLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_NLOS.DSCorrDistM);
    m_pAODMapNLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_NLOS.ASDCorrDistM);
    m_pAOAMapNLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_NLOS.ASACorrDistM);
    m_pSFMapNLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_NLOS.SFCorrDistM);

    if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
        m_pEODMapLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_LOS.ESDCorrDistM);
        m_pEOAMapLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_LOS.ESACorrDistM);
        m_pEODMapNLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_NLOS.ESDCorrDistM);
        m_pEOAMapNLOS = std::make_shared<GaussianMap > (P::s().Macro2UE_NLOS.ESACorrDistM);
    }
    if (((P::s().IChannelModel_for_Scenario == P::UMI
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)
            || (P::s().IChannelModel_for_Scenario == P::RMA
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)
            || (P::s().IChannelModel_for_Scenario == P::UMA
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)
            || (P::s().IChannelModel_for_Scenario == P::RMA
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB)
            || (P::s().IChannelModel_for_Scenario == P::UMA
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB)
            || (P::s().IChannelModel_for_Scenario == P::UMI
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB))) {
        m_pDSMapO2I = std::make_shared<GaussianMap > (P::s().Macro2UE_O2I.DSCorrDistM);
        m_pAODMapO2I = std::make_shared<GaussianMap > (P::s().Macro2UE_O2I.ASDCorrDistM);
        m_pAOAMapO2I = std::make_shared<GaussianMap > (P::s().Macro2UE_O2I.ASACorrDistM);
        m_pSFMapO2I = std::make_shared<GaussianMap > (P::s().Macro2UE_O2I.SFCorrDistM);
        if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
            m_pEODMapO2I = std::make_shared<GaussianMap > (P::s().Macro2UE_O2I.ESDCorrDistM);
            m_pEOAMapO2I = std::make_shared<GaussianMap > (P::s().Macro2UE_O2I.ESACorrDistM);
        }
    }

    if (P::s().IChannelModel_for_Scenario == P::_5GCM_TWO_LAYER) {
        m_pDSMapPicoToUELOS = std::make_shared<GaussianMap > (P::s().Pico2UE_LOS.DSCorrDistM);
        m_pAODMapPicoToUELOS = std::make_shared<GaussianMap > (P::s().Pico2UE_LOS.ASDCorrDistM);
        m_pAOAMapPicoToUELOS = std::make_shared<GaussianMap > (P::s().Pico2UE_LOS.ASACorrDistM);
        m_pSFMapPicoToUELOS = std::make_shared<GaussianMap > (P::s().Pico2UE_LOS.SFCorrDistM);
        m_pKMapPicoToUELOS = std::make_shared<GaussianMap > (P::s().Pico2UE_LOS.KCorrDistM);

        m_pDSMapPicoToUENLOS = std::make_shared<GaussianMap > (P::s().Pico2UE_NLOS.DSCorrDistM);
        m_pAODMapPicoToUENLOS = std::make_shared<GaussianMap > (P::s().Pico2UE_NLOS.ASDCorrDistM);
        m_pAOAMapPicoToUENLOS = std::make_shared<GaussianMap > (P::s().Pico2UE_NLOS.ASACorrDistM);
        m_pSFMapPicoToUENLOS = std::make_shared<GaussianMap > (P::s().Pico2UE_NLOS.SFCorrDistM);

        m_pDSMapPicoToUEO2I = std::make_shared<GaussianMap > (P::s().Pico2UE_O2I.DSCorrDistM);
        m_pAODMapPicoToUEO2I = std::make_shared<GaussianMap > (P::s().Pico2UE_O2I.ASDCorrDistM);
        m_pAOAMapPicoToUEO2I = std::make_shared<GaussianMap > (P::s().Pico2UE_O2I.ASACorrDistM);
        m_pSFMapPicoToUEO2I = std::make_shared<GaussianMap > (P::s().Pico2UE_O2I.SFCorrDistM);
    }

    m_LOSORNLOS.clear();
}