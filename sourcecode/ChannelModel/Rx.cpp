/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file Rx.cpp
///@brief 定义了和MS相关的一些函数
///
///@author wangfei
#include "Rx.h"
#include "AntennaOrientGain.h"
#include "P.h"
#include "../Parameters/Parameters.h"
#include "../SafeMap.h"
using namespace cm;

int Rx::rxcounter = 0;
SafeMap<int,Rx*> Rx::m_mRxID2PRx;

///构造函数的实现
Rx::Rx() {
    m_iSpecial = 0;
    //zhengyi high freq
    m_iIsLowloss = -1;
    m_dInCarLossDB = 0;
    m_iRxID = rxcounter++;
    m_mRxID2PRx[m_iRxID] = this;
    m_dRxHeight = P::s().RX.DAntennaHeightM;
    m_iTotalFloorNum = 0;
    m_iFloorNum = 0;
    type=1;
   // std::cout<<"rxcounter1 "<<rxcounter<<std::endl;
    
    
    Build_UE_Antenna();
}
Rx::Rx(int _type) {
    m_iSpecial = 0;
    //zhengyi high freq
    m_iIsLowloss = -1;
    m_dInCarLossDB = 0;
    m_iRxID = rxcounter++;
    m_mRxID2PRx[m_iRxID] = this;
    m_dRxHeight = P::s().RX.DAntennaHeightM;
    m_iTotalFloorNum = 0;
    m_iFloorNum = 0;
    type=_type;
   // std::cout<<"rxcounter1 "<<rxcounter<<std::endl;
    
    
    Build_UE_Antenna();
}
///拷贝构造函数的实现
Rx::Rx(const Rx& _rx) {
    *this = _rx;
    m_mRxID2PRx[m_iRxID] = this;
    ++rxcounter;
   // std::cout<<"rxcounter2  "<<rxcounter<<std::endl;
}
///虚析构函数的实现

Rx::~Rx() {
    --rxcounter;
}

void Rx::Build_UE_Antenna() {
    int H_Panel_Num,V_Panel_Num;
    if(type==1){
        H_Panel_Num = Parameters::Instance().MSS.FirstBand.IHPanelNum;
        V_Panel_Num = Parameters::Instance().MSS.FirstBand.IVPanelNum;
    }
    else if(type==2){
        H_Panel_Num = Parameters::Instance().MSS.FirstBand.IHPanelNum_2;
        V_Panel_Num = Parameters::Instance().MSS.FirstBand.IVPanelNum_2;
    }
    int _AntennaPanelNum = H_Panel_Num * V_Panel_Num;
    //chty 1111 b
    m_PannelNum=_AntennaPanelNum;
    //chty 1111 e
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
    SetRxOrientRAD(random.xUniform_msconstruct(0, 2 * M_PI));
    
    if(type==1){
        //chty 1111 b
        m_HTxRUNum = Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel;
        m_VTxRUNum = Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel;
        m_TotalTxRUNum = m_VTxRUNum * m_HTxRUNum * Parameters::Instance().MSS.FirstBand.Polarize_Num;
        //chty 1111 e
        for (int _V_Panel_Index = 0; _V_Panel_Index < V_Panel_Num; _V_Panel_Index++) {
            for (int _H_Panel_Index = 0; _H_Panel_Index < H_Panel_Num; _H_Panel_Index++) {

                int Panel_Index = _H_Panel_Index + _V_Panel_Index * H_Panel_Num;

                int H_TXRU_DIV_NUM_PerPanel =
                        Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel;
                int V_TXRU_DIV_NUM_PerPanel =
                        Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel;
                int Polarize_Num =
                        Parameters::Instance().MSS.FirstBand.Polarize_Num;

                int _TXRUNum =
                        H_TXRU_DIV_NUM_PerPanel * V_TXRU_DIV_NUM_PerPanel * Polarize_Num;


                std::shared_ptr<AntennaPanel> pAntennaPanel
                        = std::make_shared<AntennaPanel>(
                        m_pAntenna, _H_Panel_Index, _V_Panel_Index,
                        _TXRUNum, Panel_Index);

                pAntennaPanel->Build_UE_AntennaPanel(type);

                m_pAntenna->m_vAntennaPanels[Panel_Index] = pAntennaPanel;
            }
        }
    }
    else if(type==2){
        //chty 1111 b
        m_HTxRUNum = Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel_2;
        m_VTxRUNum = Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel_2;
        m_TotalTxRUNum = m_VTxRUNum * m_HTxRUNum * Parameters::Instance().MSS.FirstBand.Polarize_Num_2;
        //chty 1111 e
        for (int _V_Panel_Index = 0; _V_Panel_Index < V_Panel_Num; _V_Panel_Index++) {
            for (int _H_Panel_Index = 0; _H_Panel_Index < H_Panel_Num; _H_Panel_Index++) {

                int Panel_Index = _H_Panel_Index + _V_Panel_Index * H_Panel_Num;

                int H_TXRU_DIV_NUM_PerPanel =
                        Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel_2;
                int V_TXRU_DIV_NUM_PerPanel =
                        Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel_2;
                int Polarize_Num =
                        Parameters::Instance().MSS.FirstBand.Polarize_Num_2;

                int _TXRUNum =
                        H_TXRU_DIV_NUM_PerPanel * V_TXRU_DIV_NUM_PerPanel * Polarize_Num;


                std::shared_ptr<AntennaPanel> pAntennaPanel
                        = std::make_shared<AntennaPanel>(
                        m_pAntenna, _H_Panel_Index, _V_Panel_Index,
                        _TXRUNum, Panel_Index);
                pAntennaPanel->Build_UE_AntennaPanel(type);

                m_pAntenna->m_vAntennaPanels[Panel_Index] = pAntennaPanel;
            }
        }
    }

    m_pAntenna->m_ActiveTXRU_Num = 
            m_pAntenna->GetFirstAntennaPanelPointer()->GetTXRU_Num();
    
    assert(m_pAntenna->SelfCheck());
}

///获取Rx的天线方向性增益
///@param _dAngleRAD 角度值
///@return 天线方向性增益
double Rx::GetRxAOGDB(double _dAngleRAD, double _dDownTiltRAD) const{
    double dRxAOGDB = m_pRxAOG->Db(_dAngleRAD, _dDownTiltRAD);
    return m_dRxAntGainDB + dRxAOGDB;
}
//double Rx::GetRxAOGDB(double _dAngleRAD) {
//    double dRxAOGDB = m_pRxAOG->Db(_dAngleRAD,0);
//    return m_dRxAntGainDB + dRxAOGDB;
//}
///记录Rx的数目
int Rx::CountRx() {
    return rxcounter;
}

///获得Rx的引用
Rx& Rx::GetRx(int _iRxID) {
    return *m_mRxID2PRx[_iRxID];
}

void Rx::SetRxOrientRAD( double _rad ){
    m_dRxOrientRAD = _rad;
    
    m_pAntenna->SetTxRxOrientRAD(m_dRxOrientRAD);
}