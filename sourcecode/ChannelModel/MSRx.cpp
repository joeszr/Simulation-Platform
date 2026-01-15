
#include "MSRx.h"
#include "AntennaOrientGain.h"
#include "P.h"
#include "../Parameters/Parameters.h"

using namespace cm;

///构造函数的实现
MSRxNode::MSRxNode(Point& _point) :  Rx(_point){
    Build_UE_Antenna();
    m_iSpecial = 0;
    m_iIsLowloss = -1;
    m_iFloorNum = 0;
    m_iTotalFloorNum = 0;
}

///拷贝构造函数的实现
MSRxNode::MSRxNode(const MSRxNode& _rx) : Rx(_rx){
    *this = _rx;
}

///虚析构函数的实现
MSRxNode::~MSRxNode(void) {
}


void MSRxNode::Build_UE_Antenna() {
    int H_Panel_Num = Parameters::Instance().MSS.IHPanelNum;
    int V_Panel_Num = Parameters::Instance().MSS.IVPanelNum;

    int _AntennaPanelNum = H_Panel_Num * V_Panel_Num;

    int _EtiltRADNum =
            Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iVUEBeamNum;
    int _EscanRADNum =
            Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHUEBeamNum;

    m_pAntenna = std::make_shared<Antenna>(
            _AntennaPanelNum, _EtiltRADNum, _EscanRADNum);

    m_pAntenna->m_vEtiltRAD =
            Parameters::Instance().Macro.ANALOGBEAM_CONFIG.vUEetiltRAD;
    m_pAntenna->m_vEscanRAD =
            Parameters::Instance().Macro.ANALOGBEAM_CONFIG.vUEescanRAD;

    m_pAntenna->m_MechanicalTiltRAD = 0.0;

    //20180626
    SetRxOrientRAD(xUniform_msconstruct(0, 2 * M_PI));

    for (int _V_Panel_Index = 0; _V_Panel_Index < V_Panel_Num; _V_Panel_Index++) {
        for (int _H_Panel_Index = 0; _H_Panel_Index < H_Panel_Num; _H_Panel_Index++) {

            int Panel_Index = _H_Panel_Index + _V_Panel_Index * H_Panel_Num;

            int H_TXRU_DIV_NUM_PerPanel =
                    Parameters::Instance().MSS.H_TXRU_DIV_NUM_PerPanel;
            int V_TXRU_DIV_NUM_PerPanel =
                    Parameters::Instance().MSS.V_TXRU_DIV_NUM_PerPanel;
            int Polarize_Num =
                    Parameters::Instance().MSS.Polarize_Num;

            int _TXRUNum =
                    H_TXRU_DIV_NUM_PerPanel * V_TXRU_DIV_NUM_PerPanel * Polarize_Num;


            std::shared_ptr<AntennaPanel> pAntennaPanel
                    = std::make_shared<AntennaPanel>(
                    m_pAntenna, _H_Panel_Index, _V_Panel_Index,
                    _TXRUNum, Panel_Index);
                int ue_type = 1;//UE参数类型
            pAntennaPanel->Build_UE_AntennaPanel(ue_type);

            m_pAntenna->m_vAntennaPanels[Panel_Index] = pAntennaPanel;
        }
    }

    m_pAntenna->m_ActiveTXRU_Num =
            m_pAntenna->GetFirstAntennaPanelPointer()->GetTXRU_Num();

    assert(m_pAntenna->SelfCheck());
}
