#include "RIS.h"
#include "../ChannelModel/AOGSector.h"

using namespace cm;

int RIS::riscounter = 0;

RIS::RIS(BTSID _btsid, int _iID) {
    m_ID = RISID(_btsid, _iID);
    riscounter++;
    dEOARAD_BTSRIS_GCS = 0;
    dAOARAD_BTSRIS_GCS = 0; 
//    ptx = std::shared_ptr<cm::TxNode>(new cm::TxNode(*this));//RIS TX
//    prx = std::shared_ptr<cm::RxNode>(new cm::RxNode(*this));//RIS RX
    //ptx = std::make_shared<cm::TxNode>(*this);   
    //prx = std::make_shared<cm::RxNode>(*this);
    m_pTxNode = std::shared_ptr<cm::RISTxNode>(new cm::RISTxNode(*this));
    m_pRxNode = std::shared_ptr<cm::RISRxNode>(new cm::RISRxNode(*this));
    Build_RIS_Antenna();   
//    prx->m_iSpecial = 0;
//    prx->m_iIsLowloss = -1;
//    prx->m_iFloorNum = 0;
//    prx->m_iTotalFloorNum = 0;
//    prx->m_dInCarLossDB=0;
    assert(m_pSelfAntenna != NULL);
    
    SetTxRx();
}

void RIS::Build_RIS_Antenna(){
    
    int H_Panel_Num = Parameters::Instance().RIS.IHPanelNum;
    int V_Panel_Num = Parameters::Instance().RIS.IVPanelNum;
    
    int _AntennaPanelNum = H_Panel_Num * V_Panel_Num;
    //修改
    int _EtiltRADNum = 
        Parameters::Instance().RIS.ANALOGBEAM_CONFIG.iVRISBeamNum;
    int _EscanRADNum = 
        Parameters::Instance().RIS.ANALOGBEAM_CONFIG.iHRISBeamNum;
  
    m_pSelfAntenna = std::make_shared<cm::Antenna>(
                    _AntennaPanelNum, _EtiltRADNum, _EscanRADNum);
    
    assert(m_pSelfAntenna != NULL);
    
    
    m_pSelfAntenna->m_vEtiltRAD = 
            Parameters::Instance().RIS.ANALOGBEAM_CONFIG.vRISetiltRAD;
    m_pSelfAntenna->m_vEscanRAD = 
            Parameters::Instance().RIS.ANALOGBEAM_CONFIG.vRISescanRAD;
    
    m_pSelfAntenna->m_MechanicalTiltRAD
        = cm::DEG2RAD(Parameters::Instance().RIS.DMechanicalTiltDeg);
    
    for(int _V_Panel_Index = 0; _V_Panel_Index < V_Panel_Num; _V_Panel_Index++) {
        for(int _H_Panel_Index = 0; _H_Panel_Index < H_Panel_Num; _H_Panel_Index++) {
             
            int Panel_Index = _H_Panel_Index + _V_Panel_Index * H_Panel_Num;
            
            int H_TXRU_DIV_NUM_PerPanel = 
                Parameters::Instance().RIS.H_TXRU_DIV_NUM_PerPanel;
            int V_TXRU_DIV_NUM_PerPanel = 
                Parameters::Instance().RIS.V_TXRU_DIV_NUM_PerPanel;
            int Polarize_Num = 
                Parameters::Instance().RIS.Polarize_Num;
            
            int _TXRUNum = 
                H_TXRU_DIV_NUM_PerPanel * V_TXRU_DIV_NUM_PerPanel * Polarize_Num;
    
    
            std::shared_ptr<cm::AntennaPanel> pAntennaPanel 
                = std::make_shared<cm::AntennaPanel>(
                    m_pSelfAntenna, _H_Panel_Index, _V_Panel_Index,
                    _TXRUNum, Panel_Index);

            pAntennaPanel->Build_RIS_AntennaPanel();

            m_pSelfAntenna->m_vAntennaPanels[Panel_Index] = pAntennaPanel;
        }
    }
    
    
    assert(m_pSelfAntenna != NULL);
    
    m_pSelfAntenna->m_ActiveTXRU_Num = m_pSelfAntenna->GetTotalTXRU_Num();
    
    assert(m_pSelfAntenna->SelfCheck());
    
}

void RIS::SetTxRx(){
    
//    ptx->m_pAntenna = m_pSelfAntenna;   
//    prx->m_pAntenna = m_pSelfAntenna;
    m_pTxNode->m_pAntenna = m_pSelfAntenna; 
    m_pRxNode->m_pAntenna = m_pSelfAntenna; 
///在BSManager::DistributeRISs()中设置
//  SetOrientRAD( _angle);
    SetTxRxHeightM( Parameters::Instance().RIS.DAntennaHeightM);
    SetTxRxAntNum(Parameters::Instance().RIS.ITotalAntNum);
    SetTxRxAntGainDB(Parameters::Instance().RIS.DAntennaGainDb);
    SetTxRxAOG();
    
    
    
}

RIS::~RIS(void) {
    --riscounter;
}

int RIS::CountRIS() {
    return riscounter;
}

RISID RIS::GetRISID() {
    return m_ID;
}

Tx& RIS::GetTx() {
    return *m_pTxNode;
}

Rx& RIS::GetRx() {
    return *m_pRxNode;
}

vector<MSID> RIS::GetActiveSet() {
    return m_ActiveSet;
}

void RIS::SetOrientRAD(double _angle){
    m_pTxNode->SetTxOrientRAD(_angle);
    m_pRxNode->SetRxOrientRAD(_angle);
}
void RIS::SetTxRxHeightM(double _dHeightM){
    m_pTxNode->SetTxHeightM(_dHeightM);
    m_pRxNode->SetRxHeightM(_dHeightM);
}
void RIS::SetTxRxAntNum(int _iAntNum){
    m_pTxNode->m_iAntNum = _iAntNum;
    //prx->m_iAntNum = _iAntNum;
}
void RIS::SetTxRxAntGainDB(double _AntGainDB){
    m_pTxNode->m_dTxAntGainDB = _AntGainDB;
    m_pRxNode->m_dRxAntGainDB = _AntGainDB;
}
void RIS::SetTxRxAOG(){
    m_pTxNode->m_pTxAOG = std::shared_ptr<cm::AntennaOrientGain > (
                        new cm::AOGSector(
                        Parameters::Instance().RIS.DH3DBBeamWidthDeg,
                        Parameters::Instance().RIS.DV3DBBeamWidthDeg,
                        Parameters::Instance().RIS.DHBackLossDB,
                        Parameters::Instance().RIS.DVBackLossDB,
                        Parameters::Instance().RIS.DHBackLossDB,
                        Parameters::Instance().RIS.DMechanicalTiltDeg));
    m_pRxNode->m_pRxAOG = m_pTxNode->m_pTxAOG;
}
    

double RIS::PhaseAdjust(){
    
}

void RIS::ConnectMS(MSID imsid){
    m_ActiveSet.push_back(imsid);
}