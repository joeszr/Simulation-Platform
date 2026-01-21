///@file Tx.cpp
///@brief  Tx类定义
///

#include "Tx.h"
#include "AntennaOrientGain.h"
#include "P.h"
#include "../Parameters/Parameters.h"


using namespace cm;

int Tx::txcounter = 0;
std::unordered_map<int, Tx*> Tx::m_mTxID2PTx;

Tx::Tx() {
    m_iTxID = txcounter++;
    m_mTxID2PTx[m_iTxID] = this;
    m_dTxHeight = 0;
    Build_BS_Antenna();
    assert(m_pAntenna != nullptr);
}



Tx::Tx(const Tx& _tx) {
    *this = _tx;
    m_mTxID2PTx[m_iTxID] = this;
    ++txcounter;
}

Tx::Tx(Point& _point) {
    SetX(_point.GetX());
    SetY(_point.GetY());
    m_iTxID = txcounter++;
    m_mTxID2PTx[m_iTxID] = this;
    m_dTxHeight = 0;
    Build_BS_Antenna();
    assert(m_pAntenna != nullptr);
}

Tx::~Tx() {
    --txcounter;
}

void Tx::Build_BS_Antenna() {
    int H_Panel_Num = Parameters::Instance().Macro.IHPanelNum;
    int V_Panel_Num = Parameters::Instance().Macro.IVPanelNum;
    
    int _AntennaPanelNum = H_Panel_Num * V_Panel_Num;
    //chty 1111 b
    m_PannelNum=_AntennaPanelNum;
    //chty 1111 e
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
    //chty 1111 b
    m_HTxRUNum = Parameters::Instance().Macro.H_TXRU_DIV_NUM_PerPanel;
    m_VTxRUNum = Parameters::Instance().Macro.V_TXRU_DIV_NUM_PerPanel;
    m_TotalTxRUNum = m_VTxRUNum * m_HTxRUNum * Parameters::Instance().Macro.Polarize_Num;
    //chty 1111 e
    
    assert(m_pAntenna != NULL);
    
    m_pAntenna->m_ActiveTXRU_Num = m_pAntenna->GetTotalTXRU_Num();
    
    assert(m_pAntenna->SelfCheck());
}

double Tx::GetTxAOGDB(double _dAngleRAD, double _dDownTiltRAD) {
    double dTxAOGDB = m_pTxAOG->Db(_dAngleRAD, _dDownTiltRAD);
    return m_dTxAntGainDB + dTxAOGDB;
}

int Tx::CountTx() {
    return txcounter;
}

Tx& Tx::GetTx(int _iTxID) {
    return *m_mTxID2PTx[_iTxID];
}

int Tx::GetTxID() const{
    return m_iTxID;
}

double Tx::GetTxOrientRAD() const{
    return m_dTxOrientRAD;
}

void Tx::SetTxOrientRAD(double _dRAD) {
    m_dTxOrientRAD = _dRAD;
    
    assert(m_pAntenna != NULL);
    
    m_pAntenna->SetTxRxOrientRAD(m_dTxOrientRAD);
}

int Tx::GetAntNum() const{
    return m_iAntNum;
}

double Tx::GetTxHeightM() const{
    return m_dTxHeight;
}

void Tx::SetTxHeightM(double _dHeightM){
    m_dTxHeight = _dHeightM;
}