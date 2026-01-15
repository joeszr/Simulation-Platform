
#include "BSTx.h"
#include "AntennaOrientGain.h"
#include "P.h"
#include "../Parameters/Parameters.h"


using namespace cm;

BSTxNode::BSTxNode(Point& _point) : Tx(_point) {
    Build_BS_Antenna();
    assert(m_pAntenna != NULL);
}

BSTxNode::BSTxNode(const BSTxNode& _bstx) : Tx(_bstx){
    *this = _bstx;
}

BSTxNode::~BSTxNode(void) {
}

void BSTxNode::Build_BS_Antenna() {
    int H_Panel_Num = Parameters::Instance().Macro.IHPanelNum;
    int V_Panel_Num = Parameters::Instance().Macro.IVPanelNum;
    
    int _AntennaPanelNum = H_Panel_Num * V_Panel_Num;
    
    int _EtiltRADNum = 
        Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iVBSBeamNum;
    int _EscanRADNum = 
        Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHBSBeamNum;
  
    m_pAntenna = std::make_shared<Antenna>(
                    _AntennaPanelNum, _EtiltRADNum, _EscanRADNum);
    
    assert(m_pAntenna != NULL);
    
    //20180625
    m_pAntenna->m_vEtiltRAD = 
            Parameters::Instance().Macro.ANALOGBEAM_CONFIG.vBSetiltRAD;
    m_pAntenna->m_vEscanRAD = 
            Parameters::Instance().Macro.ANALOGBEAM_CONFIG.vBSescanRAD;
    
    m_pAntenna->m_MechanicalTiltRAD 
        = DEG2RAD(Parameters::Instance().Macro.DMechanicalTiltDeg);
    
    for(int _V_Panel_Index = 0; _V_Panel_Index < V_Panel_Num; _V_Panel_Index++) {
        for(int _H_Panel_Index = 0; _H_Panel_Index < H_Panel_Num; _H_Panel_Index++) {
             
            int Panel_Index = _H_Panel_Index + _V_Panel_Index * H_Panel_Num;
            
            int H_TXRU_DIV_NUM_PerPanel = 
                Parameters::Instance().Macro.H_TXRU_DIV_NUM_PerPanel;
            int V_TXRU_DIV_NUM_PerPanel = 
                Parameters::Instance().Macro.V_TXRU_DIV_NUM_PerPanel;
            int Polarize_Num = 
                Parameters::Instance().Macro.Polarize_Num;
            
            int _TXRUNum = 
                H_TXRU_DIV_NUM_PerPanel * V_TXRU_DIV_NUM_PerPanel * Polarize_Num;
    
    
            std::shared_ptr<AntennaPanel> pAntennaPanel 
                = std::make_shared<AntennaPanel>(
                    m_pAntenna, _H_Panel_Index, _V_Panel_Index,
                    _TXRUNum, Panel_Index);

            pAntennaPanel->Build_BS_AntennaPanel();

            m_pAntenna->m_vAntennaPanels[Panel_Index] = pAntennaPanel;
        }
    }
    
    
    assert(m_pAntenna != NULL);
    
    m_pAntenna->m_ActiveTXRU_Num = m_pAntenna->GetTotalTXRU_Num();
    
    assert(m_pAntenna->SelfCheck());
}