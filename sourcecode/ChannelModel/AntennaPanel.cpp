/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../Parameters/Parameters.h"
#include "AntennaPanel.h"
#include "Antenna.h"

using namespace cm;

AntennaPanel::AntennaPanel(const std::shared_ptr<Antenna>& _pFatherAntenna,
                           const int& _H_Panel_Index, const int& _V_Panel_Index,
                           const int& _TXRUNum, const int& _Panel_Index) {

    m_pFatherAntenna = _pFatherAntenna;
    m_H_Panel_Index = _H_Panel_Index;
    m_V_Panel_Index = _V_Panel_Index;
    m_TXRUNum = _TXRUNum;
    m_Panel_Index = _Panel_Index;

    assert(m_Panel_Index >= 0 && m_Panel_Index < m_TXRUNum);

    assert(m_Panel_Index == m_H_Panel_Index + m_V_Panel_Index
            * Parameters::Instance().Macro.IHPanelNum);

    m_vTXRUs.resize(m_TXRUNum, std::shared_ptr<CTXRU>());
    //chty 1111 b a
    m_MechanicalTiltRAD = m_pFatherAntenna.lock()->GetMechanicalTiltRAD();
    //chty 1111 e
}

void AntennaPanel::Build_BS_AntennaPanel() {

    m_Panel_Orient_Offset_RAD = 0.0;

    int IHAntNumPerPanel = Parameters::Instance().Macro.IHAntNumPerPanel;
    int IVAntNumPerPanel = Parameters::Instance().Macro.IVAntNumPerPanel;

    double DHAntSpace = Parameters::Instance().Macro.DHAntSpace;
    double DVAntSpace = Parameters::Instance().Macro.DVAntSpace;

    double dH_Panel_Space = DHAntSpace;
    double dV_Panel_Space = DVAntSpace;

    //20180622
    m_Panel_H_Offset_lamda
            = m_H_Panel_Index * ((IHAntNumPerPanel - 1) * DHAntSpace + dH_Panel_Space);
    m_Panel_V_Offset_lamda
            = m_V_Panel_Index * ((IVAntNumPerPanel - 1) * DVAntSpace + dV_Panel_Space);

    int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().Macro.H_TXRU_DIV_NUM_PerPanel;
    int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().Macro.V_TXRU_DIV_NUM_PerPanel;

    int Polarize_Num = Parameters::Instance().Macro.Polarize_Num;

    for (int _V_TXRU_Index = 0; _V_TXRU_Index < V_TXRU_DIV_NUM_PerPanel; _V_TXRU_Index++) {
        for (int _H_TXRU_Index = 0; _H_TXRU_Index < H_TXRU_DIV_NUM_PerPanel; _H_TXRU_Index++) {
            for (int _Polar_Index = 0; _Polar_Index < Polarize_Num; _Polar_Index++) {

                int Total_TXRU_NUM_perPanel =
                        H_TXRU_DIV_NUM_PerPanel * V_TXRU_DIV_NUM_PerPanel * Polarize_Num;

                int TXRU_Index_inner_Pannel =
                        _V_TXRU_Index * H_TXRU_DIV_NUM_PerPanel * Polarize_Num
                        + _H_TXRU_Index * Polarize_Num
                        + _Polar_Index;

                int TXRU_Index = m_Panel_Index * Total_TXRU_NUM_perPanel
                        + TXRU_Index_inner_Pannel;

                std::shared_ptr<CTXRU> pTXRU
                        = std::make_shared<CTXRU>(
                        this, _Polar_Index,
                        _H_TXRU_Index, _V_TXRU_Index, TXRU_Index);

                pTXRU->Build_BS_TXRU();

                m_vTXRUs[TXRU_Index_inner_Pannel] = pTXRU;
            }
        }
    }
}

void AntennaPanel::Build_UE_AntennaPanel(int _type) {
    int IHPanelNum;
           IHPanelNum = Parameters::Instance().MSS.FirstBand.IHPanelNum;

//    if (_type==1){
//       IHPanelNum = Parameters::Instance().MSS.FirstBand.IHPanelNum;
//    }
//    else if(_type==2){
//       IHPanelNum = Parameters::Instance().MSS.FirstBand.IHPanelNum_2;
//    }
    if (IHPanelNum == 1) {
        m_Panel_Orient_Offset_RAD = 0.0;
    } else if (IHPanelNum == 2) {
        m_Panel_Orient_Offset_RAD = (m_H_Panel_Index == 0) ? 0.0 : M_PI;
    } else {
        assert(false);
    }

    m_Panel_H_Offset_lamda = 0.0;
    m_Panel_V_Offset_lamda = 0.0;
    int H_TXRU_DIV_NUM_PerPanel;
    int V_TXRU_DIV_NUM_PerPanel; 
    int Polarize_Num;
    if(_type==1) {
        Polarize_Num = Parameters::Instance().MSS.FirstBand.Polarize_Num;
        H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel;
        V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel;
                
    }
    else if(_type==2) {
        Polarize_Num = Parameters::Instance().MSS.FirstBand.Polarize_Num_2;
        H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel_2;
        V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel_2;
    }
    else{
        cout<<"wrong ue type-AntennaPanel"<<endl;
        assert(false);
    }
    for (int _V_TXRU_Index = 0; _V_TXRU_Index < V_TXRU_DIV_NUM_PerPanel; _V_TXRU_Index++) {
        for (int _H_TXRU_Index = 0; _H_TXRU_Index < H_TXRU_DIV_NUM_PerPanel; _H_TXRU_Index++) {
            for (int _Polar_Index = 0; _Polar_Index < Polarize_Num; _Polar_Index++) {

                int Total_TXRU_NUM_perPanel =
                        H_TXRU_DIV_NUM_PerPanel * V_TXRU_DIV_NUM_PerPanel * Polarize_Num;

                int TXRU_Index_inner_Pannel =
                        _V_TXRU_Index * H_TXRU_DIV_NUM_PerPanel * Polarize_Num
                        + _H_TXRU_Index * Polarize_Num
                        + _Polar_Index;

                assert(TXRU_Index_inner_Pannel >= 0
                        && TXRU_Index_inner_Pannel < m_TXRUNum);

                int TXRU_Index = m_Panel_Index * Total_TXRU_NUM_perPanel
                        + TXRU_Index_inner_Pannel;


                std::shared_ptr<CTXRU> pTXRU
                        = std::make_shared<CTXRU>(
                        this, _Polar_Index,
                        _H_TXRU_Index, _V_TXRU_Index, TXRU_Index);

                pTXRU->Build_UE_TXRU(_type);

                m_vTXRUs[TXRU_Index_inner_Pannel] = pTXRU;
            }
        }
    }
}

void AntennaPanel::Build_RIS_AntennaPanel() {

    m_Panel_Orient_Offset_RAD = 0.0;

    int IHAntNumPerPanel = Parameters::Instance().RIS.IHAntNumPerPanel;
    int IVAntNumPerPanel = Parameters::Instance().RIS.IVAntNumPerPanel;

    double DHAntSpace = Parameters::Instance().RIS.DHAntSpace;
    double DVAntSpace = Parameters::Instance().RIS.DVAntSpace;

    double dH_Panel_Space = DHAntSpace;
    double dV_Panel_Space = DVAntSpace;

    //20180622
    m_Panel_H_Offset_lamda
        = m_H_Panel_Index * ((IHAntNumPerPanel - 1) * DHAntSpace + dH_Panel_Space);
    m_Panel_V_Offset_lamda
        = m_V_Panel_Index * ((IVAntNumPerPanel - 1) * DVAntSpace + dV_Panel_Space);

    int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.H_TXRU_DIV_NUM_PerPanel;
    int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.V_TXRU_DIV_NUM_PerPanel;

    int Polarize_Num = Parameters::Instance().RIS.Polarize_Num;

    for(int _V_TXRU_Index = 0; _V_TXRU_Index < V_TXRU_DIV_NUM_PerPanel; _V_TXRU_Index++) {
        for(int _H_TXRU_Index = 0; _H_TXRU_Index < H_TXRU_DIV_NUM_PerPanel; _H_TXRU_Index++) {
            for(int _Polar_Index = 0; _Polar_Index < Polarize_Num; _Polar_Index++) {

                int Total_TXRU_NUM_perPanel =
                    H_TXRU_DIV_NUM_PerPanel * V_TXRU_DIV_NUM_PerPanel * Polarize_Num;

                int TXRU_Index_inner_Pannel =
                    _V_TXRU_Index * H_TXRU_DIV_NUM_PerPanel * Polarize_Num
                    + _H_TXRU_Index * Polarize_Num
                    + _Polar_Index;

                int TXRU_Index =  m_Panel_Index * Total_TXRU_NUM_perPanel
                    + TXRU_Index_inner_Pannel;

                std::shared_ptr<CTXRU> pTXRU
                    = std::make_shared<CTXRU>(
                        shared_from_this().get(), _Polar_Index,
                        _H_TXRU_Index, _V_TXRU_Index, TXRU_Index);

                pTXRU->Build_RIS_TXRU();

                m_vTXRUs[TXRU_Index_inner_Pannel] = pTXRU;
            }
        }
    }
}

std::shared_ptr<CTXRU> AntennaPanel::GetFirstTXRU() {
    return m_vTXRUs[0];
}

/*
std::shared_ptr<CTXRU> AntennaPanel::GetTXRU(int _Panel_Index) {
    return m_vTXRUs[_Panel_Index];
}
 */

int AntennaPanel::GetTXRU_Num() const{
    assert(m_TXRUNum == m_vTXRUs.size());
    return m_TXRUNum;
}

std::shared_ptr<Antenna> AntennaPanel::GetFatherAntennaPointer() {
    return m_pFatherAntenna.lock();
}

double AntennaPanel::GetTxRxOrientRAD() const {
    return m_pFatherAntenna.lock()->GetTxRxOrientRAD() + m_Panel_Orient_Offset_RAD;
}

double AntennaPanel::GetMechanicalTiltRAD() const {
    //chty 1111 b
//    return m_pFatherAntenna.lock()->GetMechanicalTiltRAD();
    return m_MechanicalTiltRAD;
//chty 1111 e
}
