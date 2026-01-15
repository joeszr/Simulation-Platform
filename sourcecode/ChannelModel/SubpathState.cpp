/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file SubpathState.cpp
///@brief  SubpathState类定义
///
///包括构造函数，析构函数，初始化子径状态和WorkSlot函数的定义
///
///@author wangfei
#include "P.h"
#include "PathState.h"
#include "BasicChannelState.h"
#include "Rx.h"
#include "./functions.h"
#include "SubpathState.h"
#include "../Parameters/Parameters.h"
#include "Tx.h"
#include "LinkMatrix.h"

#include "CTXRU.h"
#include "AntennaPanel.h"
#include "Antenna.h"
#include "../NetworkDrive/Clock.h"

using namespace cm;
///构造函数

SubpathState::SubpathState() {




    /*
        int TX_PolarizeNum = Parameters::Instance().Macro.IPolarize + 1;
        int RX_PolarizeNum = Parameters::Instance().MSS.FirstBand.IPolarize + 1;
    
     */
    /*
        m_RxPanelID_2_cTempD.resize(P::s().RX.IHPanelNum, itpp::cmat(RX_PolarizeNum, TX_PolarizeNum));
     */
    /*
        m_cTempB_with_UEAntennaPanel.resize(P::s().RX.IHPanelNum, std::complex<double>(0.0, 0.0));
     */

    /*
        m_cRx_dH_Unit.resize(P::s().RX.IHPanelNum, std::complex<double>(0.0, 0.0));
        m_cRx_dV_Unit.resize(P::s().RX.IHPanelNum, std::complex<double>(0.0, 0.0));    
     */

    /*
        m_UE_PhiRAD_with_AntennaPanel.resize(P::s().RX.IHPanelNum, 0.0);
        m_UE_AOG_with_AntennaPanel.resize(P::s().RX.IHPanelNum, 0.0);
     */

    m_bIsLOS_Subpath = false;
    HaveAllocate =false;
}

std::complex<double> SubpathState::GetTempD_for_TXRUPair(
        std::shared_ptr<CTXRU>& _pBS_TXRU,
        std::shared_ptr<CTXRU>& _pUE_TXRU) {

    return m_TXRUPairID_2_cTempD[
            _pBS_TXRU->GetTXRUIndex()][ _pUE_TXRU->GetTXRUIndex()];
}

std::complex<double> SubpathState::CalcSubpath_TimeH_for_TXRUPair(
        int BS_BeamIndex, int UE_BeamIndex,
        std::shared_ptr<CTXRU>& _pBS_TXRU,
        std::shared_ptr<CTXRU>& _pUE_TXRU,
        int _AntIndex_in_BS_TXRU,
        int _AntIndex_in_UE_TXRU,
        double _time_s) {

    int _BS_AntennaPanelIndex
            = _pBS_TXRU->GetPanelIndex();
    int _UE_AntennaPanelIndex
            = _pUE_TXRU->GetPanelIndex();

    //chty 1111 b
    if(_time_s == 0.0){
        assert(false);
    }
    int BSTXRUIndex=_pBS_TXRU->GetTXRUIndex();
    int UETXRUIndex=_pUE_TXRU->GetTXRUIndex();
    double& lasttime = m_vdLastUpdate[_BS_AntennaPanelIndex][_UE_AntennaPanelIndex][BSTXRUIndex][UETXRUIndex];
    std::complex<double>& cSubpath_TimeH = m_vcSubpath_TimeH[_BS_AntennaPanelIndex][_UE_AntennaPanelIndex][BSTXRUIndex][UETXRUIndex];
    while(lasttime<_time_s){
        cSubpath_TimeH *= m_vcDelta_cSubpath_TimeH[_UE_AntennaPanelIndex];
        lasttime += m_dUpdateInterval;
    }
    //chty 1111 e

// delete channelinfo b
//    if (!ChannelInfo::isOver) {
//        Tx* pTx = this->m_pPathState->m_pBCS->m_pTx;
//        Rx* pRx = this->m_pPathState->m_pBCS->m_pRx;
//        ChannelInfo& ci = ChannelInfo::Instance();
//        AntPair& antPair = ci.H_TX_RX[pTx->GetTxID()][pRx->GetRxID()].H_TXRU_List[BSTXRUIndex][UETXRUIndex].H_Ant[_AntIndex_in_BS_TXRU][_AntIndex_in_UE_TXRU];
//
//        if (this->m_bIsLOS_Subpath) {
//            antPair.IsLOS = true;
//            antPair.C.real = cTempA.real();
//            antPair.C.imag = cTempA.imag();
//            antPair.D.real = m_cTempB_with_UEAntennaPanel[_UE_AntennaPanelIndex].real();
//            antPair.D.imag = m_cTempB_with_UEAntennaPanel[_UE_AntennaPanelIndex].imag();
//        } else {
//            //给A进行赋值
//            antPair.A[m_pPathState->m_Path_Index][m_Subpath_Index].real = cTempA.real();
//            antPair.A[m_pPathState->m_Path_Index][m_Subpath_Index].imag = cTempA.imag();
//            //给B进行赋值
//            antPair.B[m_pPathState->m_Path_Index][m_Subpath_Index].real = m_cTempB_with_UEAntennaPanel[_UE_AntennaPanelIndex].real();
//            antPair.B[m_pPathState->m_Path_Index][m_Subpath_Index].imag = m_cTempB_with_UEAntennaPanel[_UE_AntennaPanelIndex].imag();
//        }
//
//    }
    return cSubpath_TimeH;
}

void SubpathState::Initialize() {
    std::shared_ptr<cm::Antenna> pBSAntenna
            = m_pPathState->m_pBCS->m_pTx->GetAntennaPointer();
    std::shared_ptr<cm::Antenna> pUEAntenna
            = m_pPathState->m_pBCS->m_pRx->GetAntennaPointer();

    int BS_PanelNum = pBSAntenna->GetTotalAntennaPanel_Num();
    int UE_PanelNum = pUEAntenna->GetTotalAntennaPanel_Num();

    assert(BS_PanelNum > 0);

    // _BS_PanelIndex --> BS_dH/dV
    m_cBS_dH_Unit_withAntennaPanel.resize(BS_PanelNum, std::complex<double>(0.0, 0.0));
    m_cBS_dV_Unit_withAntennaPanel.resize(BS_PanelNum, std::complex<double>(0.0, 0.0));

    // _UE_PanelIndex --> UE_dH/dV
    m_cUE_dH_Unit_withAntennaPanel.resize(UE_PanelNum, std::complex<double>(0.0, 0.0));
    m_cUE_dV_Unit_withAntennaPanel.resize(UE_PanelNum, std::complex<double>(0.0, 0.0));

    // _UE_PanelIndex --> TempB
    m_cTempB_with_UEAntennaPanel.resize(UE_PanelNum, std::complex<double>(0.0, 0.0));

    // _BS/UE_PanelIndex --> PhiRAD
    m_BS_PhiRAD_with_AntennaPanel.resize(BS_PanelNum, 0.0);
    m_UE_PhiRAD_with_AntennaPanel.resize(UE_PanelNum, 0.0);

    // _BS/UE_PanelIndex --> AOG
    m_BS_AOG_with_AntennaPanel.resize(BS_PanelNum, 0.0);
    m_UE_AOG_with_AntennaPanel.resize(UE_PanelNum, 0.0);
    //chty 1111 b
    int BSPannelNum = m_pPathState->m_pBCS->m_pTx->GetPannelNum();
    int UEPannelNum = m_pPathState->m_pBCS->m_pRx->GetPannelNum();
    int BSTotalTxRUNum = m_pPathState->m_pBCS->m_pTx->GetTotalTxRUNum();//per panel
    int UETotalTxRUNum = m_pPathState->m_pBCS->m_pRx->GetTotalTxRUNum();//per panel
    m_bOnlyFirst = false;
    m_dUpdateInterval = Parameters::Instance().LINK_CTRL.Islot4Hupdate * Parameters::Instance().BASIC.DSlotDuration_ms/1000;
    if(!HaveAllocate){
        HaveAllocate=true;
        m_vcDelta_cSubpath_TimeH.resize(UEPannelNum);
        m_vcSubpath_TimeH.resize(BSPannelNum, vector<vector<vector<std::complex<double>>>>(UEPannelNum,
                                                                                           vector<vector<std::complex<double>>>(
                                                                                                   BSTotalTxRUNum,
                                                                                                   vector<std::complex<double>>(
                                                                                                           UETotalTxRUNum))));
        m_vdLastUpdate.resize(BSPannelNum, vector<vector<vector<double>>>(UEPannelNum,
                                                                          vector<vector<double>>(BSTotalTxRUNum,
                                                                                                 vector<double>(
                                                                                                         UETotalTxRUNum,
                                                                                                         0))));
    }
    //chty 1111 e
}

void SubpathState::Init_Step10() {
    m_Power = -1.0;
    if (!m_bIsLOS_Subpath) {
        //        m_Power = 1.0 / P::s().NumOfRayPerCluster;
        m_PhaseDegXX = random.xUniform_channel(0, 360);
        m_PhaseDegXY = random.xUniform_channel(0, 360);
        m_PhaseDegYX = random.xUniform_channel(0, 360);
        m_PhaseDegYY = random.xUniform_channel(0, 360);
    } else { // m_bIsLOS_Subpath == true
        if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA) {
            m_PhaseDegXX = random.xUniform_channel(0, 360);
            m_PhaseDegYY = m_PhaseDegXX;
        } else if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB) {
            double dWaveLength = P::s().FX.DWaveLength_Macro;
            m_PhaseDegXX = -2 * M_PI * m_pPathState->m_pBCS->m_d3DDistanceM / dWaveLength;
            m_PhaseDegYY = m_PhaseDegXX;
        } else {
            assert(false);
        }

        m_PhaseDegXY = 0.0;
        m_PhaseDegYX = 0.0;
    }
}

void SubpathState::Initialize_step11A() {

    Init_PhiRAD_and_AOG_new();

    InitUnitVector_withAntennaPanel_new();

    InitTempB_withAntennaPanel_new();

    InitTempD_withTXRUPair_new();
}

void SubpathState::InitUnitVector_withAntennaPanel_new() {

    std::shared_ptr<cm::Antenna> pBSAntenna
            = m_pPathState->m_pBCS->m_pTx->GetAntennaPointer();

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
            pBSAntenna->GetvAntennaPanels()) {

        m_cBS_dH_Unit_withAntennaPanel[pBSAntennaPanel->GetPanelIndex()] =
                M_J * (2.0 * M_PI * (
                std::sin(
                ELoSRAD_GCS2LCS(
                DEG2RAD(m_AODDeg), DEG2RAD(m_EODDeg),
                pBSAntennaPanel->GetTxRxOrientRAD(),
                pBSAntennaPanel->GetMechanicalTiltRAD(), 0))
                * std::sin(
                ALoSRAD_GCS2LCS(
                DEG2RAD(m_AODDeg), DEG2RAD(m_EODDeg),
                pBSAntennaPanel->GetTxRxOrientRAD(),
                pBSAntennaPanel->GetMechanicalTiltRAD(), 0))));

        m_cBS_dV_Unit_withAntennaPanel[pBSAntennaPanel->GetPanelIndex()] =
                M_J * (2.0 * M_PI * (
                std::cos(
                ELoSRAD_GCS2LCS(
                DEG2RAD(m_AODDeg), DEG2RAD(m_EODDeg),
                pBSAntennaPanel->GetTxRxOrientRAD(),
                pBSAntennaPanel->GetMechanicalTiltRAD(), 0))));
    }


    std::shared_ptr<cm::Antenna> pUEAntenna
            = m_pPathState->m_pBCS->m_pRx->GetAntennaPointer();

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pUEAntennaPanel,
            pUEAntenna->GetvAntennaPanels()) {

        m_cUE_dH_Unit_withAntennaPanel[pUEAntennaPanel->GetPanelIndex()] =
                M_J * (2.0 * M_PI * (
                std::sin(
                ELoSRAD_GCS2LCS(
                DEG2RAD(m_AOADeg), DEG2RAD(m_EOADeg),
                pUEAntennaPanel->GetTxRxOrientRAD(),
                pUEAntennaPanel->GetMechanicalTiltRAD(), 0))
                * std::sin(
                ALoSRAD_GCS2LCS(
                DEG2RAD(m_AOADeg), DEG2RAD(m_EOADeg),
                pUEAntennaPanel->GetTxRxOrientRAD(),
                pUEAntennaPanel->GetMechanicalTiltRAD(), 0))));

        m_cUE_dV_Unit_withAntennaPanel[pUEAntennaPanel->GetPanelIndex()] =
                M_J * (2.0 * M_PI * (
                std::cos(
                ELoSRAD_GCS2LCS(
                DEG2RAD(m_AOADeg), DEG2RAD(m_EOADeg),
                pUEAntennaPanel->GetTxRxOrientRAD(),
                pUEAntennaPanel->GetMechanicalTiltRAD(), 0))));
    }
}

void SubpathState::InitTempB_withAntennaPanel_new() {
    std::shared_ptr<cm::Antenna> pUEAntenna
            = m_pPathState->m_pBCS->m_pRx->GetAntennaPointer();

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pUEAntennaPanel,
            pUEAntenna->GetvAntennaPanels()) {

        double dWaveLength;
        if (m_pPathState->m_pBCS->IsMacroToUE()) {
            dWaveLength = P::s().FX.DWaveLength_Macro;
        } else if (m_pPathState->m_pBCS->IsPicoToUE()) {
            dWaveLength = P::s().FX.DWaveLength_Pico;
        } else {
            assert(false);
        }

        Rx* pRx = m_pPathState->m_pBCS->m_pRx;
        double dVelocity = pRx->GetVelocityMPS();
        double dMoveDirecRAD = pRx->GetMoveDirecRAD();

        m_cTempB_with_UEAntennaPanel[pUEAntennaPanel->GetPanelIndex()] =
                M_J * 2.0 * M_PI / dWaveLength * dVelocity
                * std::sin(ELoSRAD_GCS2LCS(
                DEG2RAD(m_AOADeg), DEG2RAD(m_EOADeg),
                pUEAntennaPanel->GetTxRxOrientRAD(),
                0, 0))
                * std::cos(
                ALoSRAD_GCS2LCS(
                DEG2RAD(m_AOADeg), DEG2RAD(m_EOADeg),
                pUEAntennaPanel->GetTxRxOrientRAD(),
                0, 0)
                - (dMoveDirecRAD));
    }
}

void SubpathState::InitTempD_withTXRUPair_new() {

    itpp::cmat temp3 = itpp::zeros_c(2, 2);

    if (!m_bIsLOS_Subpath) {
        temp3(0, 0) = exp(M_J * DEG2RAD(m_PhaseDegXX));
        temp3(1, 1) = exp(M_J * DEG2RAD(m_PhaseDegYY));

        temp3(0, 1) = sqrt(1 / m_pPathState->m_dXPD1)
                * exp(M_J * DEG2RAD(m_PhaseDegYX));

        temp3(1, 0) = sqrt(1 / m_pPathState->m_dXPD2)
                * exp(M_J * DEG2RAD(m_PhaseDegXY));
    } else {
        temp3(0, 0) = exp(M_J * DEG2RAD(m_PhaseDegXX));
        temp3(1, 1) = -1.0 * exp(M_J * DEG2RAD(m_PhaseDegYY));

        temp3(0, 1) = 0.0;
        temp3(1, 0) = 0.0;
    }

    std::shared_ptr<cm::Antenna> pBSAntenna
            = m_pPathState->m_pBCS->m_pTx->GetAntennaPointer();

    std::shared_ptr<cm::Antenna> pUEAntenna
            = m_pPathState->m_pBCS->m_pRx->GetAntennaPointer();

    // <_BS_TXRUIndex, _UE_TXRUIndex> --> TempD
    m_TXRUPairID_2_cTempD = vector<vector<std::complex<double>>>(
            pBSAntenna->GetTotalTXRU_Num(), vector<std::complex<double>>(pUEAntenna->GetTotalTXRU_Num(), 0));

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
            pBSAntenna->GetvAntennaPanels()) {

        BOOST_FOREACH(std::shared_ptr<CTXRU> pBS_TXRU,
                pBSAntennaPanel->GetvTXRUs()) {

            itpp::cmat temp2 = itpp::zeros_c(2, 1);

            double BS_PhiRAD = m_BS_PhiRAD_with_AntennaPanel[pBSAntennaPanel->GetPanelIndex()];
            double BS_AOG = m_BS_AOG_with_AntennaPanel[pBSAntennaPanel->GetPanelIndex()];
            double BS_PolarAngle_RAD = pBS_TXRU->GetPolarAngle_RAD();

            temp2(0, 0) =
                    cos(BS_PhiRAD) * sqrt(BS_AOG) * std::cos(BS_PolarAngle_RAD)
                    - sin(BS_PhiRAD) * sqrt(BS_AOG) * std::sin(BS_PolarAngle_RAD); //36.814

            temp2(1, 0) =
                    sin(BS_PhiRAD) * sqrt(BS_AOG) * std::cos(BS_PolarAngle_RAD)
                    + cos(BS_PhiRAD) * sqrt(BS_AOG) * std::sin(BS_PolarAngle_RAD); //36.814

            BOOST_FOREACH(std::shared_ptr<AntennaPanel> pUEAntennaPanel,
                    pUEAntenna->GetvAntennaPanels()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pUE_TXRU,
                        pUEAntennaPanel->GetvTXRUs()) {

                    itpp::cmat temp1 = itpp::zeros_c(1, 2);

                    double UE_PhiRAD = m_UE_PhiRAD_with_AntennaPanel[pUEAntennaPanel->GetPanelIndex()];
                    double UE_AOG = m_UE_AOG_with_AntennaPanel[pUEAntennaPanel->GetPanelIndex()];
                    double UE_PolarAngle_RAD = pUE_TXRU->GetPolarAngle_RAD();


                    temp1(0, 0) =
                            cos(UE_PhiRAD) * sqrt(UE_AOG) * std::cos(UE_PolarAngle_RAD)
                            - sin(UE_PhiRAD) * sqrt(UE_AOG) * std::sin(UE_PolarAngle_RAD); //36.814

                    temp1(0, 1) =
                            sin(UE_PhiRAD) * sqrt(UE_AOG) * std::cos(UE_PolarAngle_RAD)
                            + cos(UE_PhiRAD) * sqrt(UE_AOG) * std::sin(UE_PolarAngle_RAD); //36.814


                    m_TXRUPairID_2_cTempD[
                            pBS_TXRU->GetTXRUIndex()][ pUE_TXRU->GetTXRUIndex()] =
                            (temp1 * temp3 * temp2) (0, 0);
                }
            }
        }
    }
}

void SubpathState::Init_PhiRAD_and_AOG_new() {

    std::shared_ptr<cm::Antenna> pBSAntenna
            = m_pPathState->m_pBCS->m_pTx->GetAntennaPointer();

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
            pBSAntenna->GetvAntennaPanels()) {

        assert(m_BS_AOG_with_AntennaPanel.size() > 0);
        assert(pBSAntennaPanel->GetPanelIndex() >= 0);

        m_BS_AOG_with_AntennaPanel[pBSAntennaPanel->GetPanelIndex()] =
                DB2L(m_pPathState->m_pBCS->m_pTx->GetTxAOGDB(
                ALoSRAD_GCS2LCS(
                DEG2RAD(m_AODDeg),
                DEG2RAD(m_EODDeg),
                pBSAntennaPanel->GetTxRxOrientRAD(),
                pBSAntennaPanel->GetMechanicalTiltRAD(),
                0),
                ELoSRAD_GCS2LCS(
                DEG2RAD(m_AODDeg),
                DEG2RAD(m_EODDeg),
                pBSAntennaPanel->GetTxRxOrientRAD(),
                pBSAntennaPanel->GetMechanicalTiltRAD(),
                0)));

        m_BS_PhiRAD_with_AntennaPanel[pBSAntennaPanel->GetPanelIndex()] =
                GetPhiRAD_GCS(
                DEG2RAD(m_AODDeg),
                DEG2RAD(m_EODDeg),
                pBSAntennaPanel->GetTxRxOrientRAD(),
                pBSAntennaPanel->GetMechanicalTiltRAD(),
                0);
    }

    std::shared_ptr<cm::Antenna> pUEAntenna
            = m_pPathState->m_pBCS->m_pRx->GetAntennaPointer();

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pUEAntennaPanel,
            pUEAntenna->GetvAntennaPanels()) {

        m_UE_AOG_with_AntennaPanel[pUEAntennaPanel->GetPanelIndex()] =
                DB2L(m_pPathState->m_pBCS->m_pRx->GetRxAOGDB(
                ALoSRAD_GCS2LCS(
                DEG2RAD(m_AOADeg),
                DEG2RAD(m_EOADeg),
                pUEAntennaPanel->GetTxRxOrientRAD(),
                pUEAntennaPanel->GetMechanicalTiltRAD(),
                0),
                ELoSRAD_GCS2LCS(
                DEG2RAD(m_AOADeg),
                DEG2RAD(m_EOADeg),
                pUEAntennaPanel->GetTxRxOrientRAD(),
                pUEAntennaPanel->GetMechanicalTiltRAD(),
                0)));

        m_UE_PhiRAD_with_AntennaPanel[pUEAntennaPanel->GetPanelIndex()] =
                GetPhiRAD_GCS(
                DEG2RAD(m_AOADeg),
                DEG2RAD(m_EOADeg),
                pUEAntennaPanel->GetTxRxOrientRAD(),
                pUEAntennaPanel->GetMechanicalTiltRAD(),
                0);
    }
}

void SubpathState::Initialize_step11B() {

    m_AODDeg = m_pPathState->m_AODDeg_Path;
    m_EODDeg = m_pPathState->m_EODDeg_Path;

    m_AOADeg = m_pPathState->m_AOADeg_Path;
    m_EOADeg = m_pPathState->m_EOADeg_Path;

    Init_Step10();
    Initialize_step11A();
}

double SubpathState::CalSubpathCouplingLoss_of_Beampair_using_36873_8_1_new(
        int BS_BeamIndex, int UE_BeamIndex,
        std::shared_ptr<cm::CTXRU>& pBS_TXRU,
        std::shared_ptr<cm::CTXRU>& pUE_TXRU) {

    // calc BS side
    double dEODRAD_GCS = DEG2RAD(m_EODDeg);
    double dAODRAD_GCS = DEG2RAD(m_AODDeg);

    complex<double> cBSAggregateGain = pBS_TXRU->CalcAggregateGain(
            dAODRAD_GCS, dEODRAD_GCS, BS_BeamIndex);

    // calc UE side
    double dEOARAD_GCS = DEG2RAD(m_EOADeg);
    double dAOARAD_GCS = DEG2RAD(m_AOADeg);

    complex<double> cUEAggregateGain = pUE_TXRU->CalcAggregateGain(
            dAOARAD_GCS, dEOARAD_GCS, UE_BeamIndex);

    std::complex<double> TempD
            = GetTempD_for_TXRUPair(pBS_TXRU, pUE_TXRU);

    double dEnergy = pow(abs(TempD * cBSAggregateGain * cUEAggregateGain), 2);

    return dEnergy;
}
//chty 1111 b
//old calculation steps, the name of the members of SubpathState is from here;
std::complex<double> SubpathState::InitialCalcSubpath_TimeH_for_TXRUPair(
        int BS_BeamIndex, int UE_BeamIndex,
        std::shared_ptr<CTXRU> &_pBS_TXRU,
        std::shared_ptr<CTXRU> &_pUE_TXRU,
        int _AntIndex_in_BS_TXRU,
        int _AntIndex_in_UE_TXRU,
        double _time_s) {

    int _BS_AntennaPanelIndex
            = _pBS_TXRU->GetPanelIndex();
    int _UE_AntennaPanelIndex
            = _pUE_TXRU->GetPanelIndex();
    std::complex<double> cTempD = GetTempD_for_TXRUPair(_pBS_TXRU, _pUE_TXRU);
    double BS_H_Offset_in_Antenna
            = _pBS_TXRU->Get_LCS_TXRU_LB_H_Offset_lamda_in_Antenna();
    double BS_V_Offset_in_Antenna
            = _pBS_TXRU->Get_LCS_TXRU_LB_V_Offset_lamda_in_Antenna();
    std::complex<double> cTemp4 = exp(
            BS_H_Offset_in_Antenna * m_cBS_dH_Unit_withAntennaPanel[_BS_AntennaPanelIndex]
            + BS_V_Offset_in_Antenna * m_cBS_dV_Unit_withAntennaPanel[_BS_AntennaPanelIndex]);
    double UE_H_Offset_in_Antenna
            = _pUE_TXRU->Get_LCS_TXRU_LB_H_Offset_lamda_in_Antenna();
    double UE_V_Offset_in_Antenna
            = _pUE_TXRU->Get_LCS_TXRU_LB_V_Offset_lamda_in_Antenna();
    std::complex<double> cTemp5 = exp(
            UE_H_Offset_in_Antenna * m_cUE_dH_Unit_withAntennaPanel[_UE_AntennaPanelIndex]
            + UE_V_Offset_in_Antenna * m_cUE_dV_Unit_withAntennaPanel[_UE_AntennaPanelIndex]);
    if (!m_bOnlyFirst) {
        m_bOnlyFirst=true;
        assert(_BS_AntennaPanelIndex==0);
        assert(_UE_AntennaPanelIndex==0);
        assert(_pBS_TXRU->GetTXRUIndex()==0);
        assert(_pUE_TXRU->GetTXRUIndex()==0);

        double dEODRAD_GCS = DEG2RAD(m_EODDeg);
        double dAODRAD_GCS = DEG2RAD(m_AODDeg);
        BS_AggregateGain = _pBS_TXRU->CalcAggregateGain(
                dAODRAD_GCS, dEODRAD_GCS, BS_BeamIndex);

        // calc UE side
        double dEOARAD_GCS = DEG2RAD(m_EOADeg);
        double dAOARAD_GCS = DEG2RAD(m_AOADeg);

        UE_AggregateGain = _pUE_TXRU->CalcAggregateGain(
                dAOARAD_GCS, dEOARAD_GCS, UE_BeamIndex);
    }


    std::complex<double> cTempA = cTempD * cTemp5 * cTemp4 * BS_AggregateGain * UE_AggregateGain;
    std::complex<double> cSubpath_TimeH = cTempA
                                          * std::exp(m_cTempB_with_UEAntennaPanel[_UE_AntennaPanelIndex] * (_time_s + 5));

    assert(_time_s == 0);
    m_vcDelta_cSubpath_TimeH[_UE_AntennaPanelIndex] = std::exp(m_cTempB_with_UEAntennaPanel[_UE_AntennaPanelIndex] * m_dUpdateInterval);
    int BS_TxRUIndex = _pBS_TXRU->GetTXRUIndex();
    int UE_TxRUIndex = _pUE_TXRU->GetTXRUIndex();
    m_vcSubpath_TimeH[_BS_AntennaPanelIndex][_UE_AntennaPanelIndex][BS_TxRUIndex][UE_TxRUIndex] = cSubpath_TimeH;

//    static std::mutex l;
//    l.lock();
//    int flag = 0;
//    if(isnan(cTempD.real())){
//        cout<<"cTempD wrong!!"<<cTempD<<endl<<endl;
//        flag=1;
//    }
//    if(isnan(BS_H_Offset_in_Antenna)){
//        cout<<"BS_H_Offset_in_Antenna wrong!!"<<BS_H_Offset_in_Antenna<<endl<<endl;
//        flag=1;
//    }
//    if(isnan(BS_V_Offset_in_Antenna)){
//        cout<<"BS_V_Offset_in_Antenna wrong!!"<<BS_V_Offset_in_Antenna<<endl<<endl;
//        flag =2;
//    }
//    if(isnan(UE_H_Offset_in_Antenna)){
//        cout<<"UE_H_Offset_in_Antenna wrong!!"<<UE_H_Offset_in_Antenna<<endl<<endl;
//        flag =3;
//    }
//    if(isnan(UE_V_Offset_in_Antenna)){
//        cout<<"UE_V_Offset_in_Antenna wrong!!"<<UE_V_Offset_in_Antenna<<endl<<endl;
//        flag=4;
//    }
//
//    if(isnan(m_cBS_dH_Unit_withAntennaPanel[_BS_AntennaPanelIndex].real())){
//        cout<<"m_cBS_dH_Unit_withAntennaPanel[_BS_AntennaPanelIndex] wrong!!"<<m_cBS_dH_Unit_withAntennaPanel[_BS_AntennaPanelIndex]<<endl<<endl;
//        flag =5;
//    }
//    if(isnan(m_cBS_dV_Unit_withAntennaPanel[_BS_AntennaPanelIndex].real())){
//        cout<<"m_cBS_dV_Unit_withAntennaPanel[_BS_AntennaPanelIndex] wrong!!"<<m_cBS_dV_Unit_withAntennaPanel[_BS_AntennaPanelIndex]<<endl<<endl;
//        flag =6;
//    }
//    if(isnan(m_cUE_dH_Unit_withAntennaPanel[_UE_AntennaPanelIndex].real())){
//        cout<<"m_cUE_dH_Unit_withAntennaPanel[_UE_AntennaPanelIndex] initial wrong!!"<<m_cUE_dH_Unit_withAntennaPanel[_UE_AntennaPanelIndex]<<endl<<endl;
//        flag =7;
//
//    }
//    if(isnan(m_cUE_dH_Unit_withAntennaPanel[_UE_AntennaPanelIndex].real())){
//        cout<<"m_cUE_dV_Unit_withAntennaPanel[_UE_AntennaPanelIndex] initial wrong!!"<<m_cUE_dV_Unit_withAntennaPanel[_UE_AntennaPanelIndex]<<endl<<endl;
//        flag =8;
//    }
//    if(isnan(BS_AggregateGain.real())){
//        cout<<"BS_AggregateGain initial wrong!!"<<BS_AggregateGain<<endl<<endl;
//        flag =9;
//    }
//    if(isnan(UE_AggregateGain.real())){
//        cout<<"BS_AggregateGain initial wrong!!"<<UE_AggregateGain<<endl<<endl;
//        flag =10;
//    }
//    if(flag!=0){
//        exit(-100);
//    }
//    l.unlock();




    Tx *pTx = this->m_pPathState->m_pBCS->m_pTx;
    Rx *pRx = this->m_pPathState->m_pBCS->m_pRx;

// delete channelinfo b
//    if (!ChannelInfo::isOver) {
//
//        ChannelInfo &ci = ChannelInfo::Instance();
//        AntPair &antPair = ci.H_TX_RX[pTx->GetTxID()][pRx->GetRxID()].H_TXRU_List[BS_TxRUIndex][UE_TxRUIndex].H_Ant[_AntIndex_in_BS_TXRU][_AntIndex_in_UE_TXRU];
//
//        if (this->m_bIsLOS_Subpath) {
//            antPair.IsLOS = true;
//            antPair.C.real = cTempA.real();
//            antPair.C.imag = cTempA.imag();
//            antPair.D.real = m_cTempB_with_UEAntennaPanel[_UE_AntennaPanelIndex].real();
//            antPair.D.imag = m_cTempB_with_UEAntennaPanel[_UE_AntennaPanelIndex].imag();
//        } else {
//            //给A进行赋值
//            antPair.A[m_pPathState->m_Path_Index][m_Subpath_Index].real = cTempA.real();
//            antPair.A[m_pPathState->m_Path_Index][m_Subpath_Index].imag = cTempA.imag();
//            //给B进行赋值
//            antPair.B[m_pPathState->m_Path_Index][m_Subpath_Index].real = m_cTempB_with_UEAntennaPanel[_UE_AntennaPanelIndex].real();
//            antPair.B[m_pPathState->m_Path_Index][m_Subpath_Index].imag = m_cTempB_with_UEAntennaPanel[_UE_AntennaPanelIndex].imag();
//        }
//
//    }
    return cSubpath_TimeH;
}
//chty 1111 e