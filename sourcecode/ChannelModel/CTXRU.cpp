/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../Parameters/Parameters.h"
#include "CTXRU.h"
#include "AntennaPanel.h"
#include "Antenna.h"


using namespace cm;

CTXRU::CTXRU(
        AntennaPanel* _pFatherAntennaPanel,
        int _Polar_Index, int _H_TXRU_Index, int _V_TXRU_Index, int _TXRU_Index) {
    
    m_pFatherAntennaPanel = _pFatherAntennaPanel;
    m_Polar_Index = _Polar_Index;
    m_H_TXRU_Index = _H_TXRU_Index;
    m_V_TXRU_Index = _V_TXRU_Index;
    m_TXRU_Index = _TXRU_Index;
    // 天线面板的左下点相对于天线局部坐标系(LCS)的相对偏移量, in lamda
    m_Panel_H_Offset_lamda=m_pFatherAntennaPanel->Get_LCS_Panel_LB_H_Offset_lamda_in_Antenna();
    m_Panel_V_Offset_lamda=m_pFatherAntennaPanel->Get_LCS_Panel_LB_V_Offset_lamda_in_Antenna();
}

void CTXRU::Build_RIS_TXRU() {
    m_dH = Parameters::Instance().RIS.DHAntSpace;
    m_dV = Parameters::Instance().RIS.DVAntSpace;

    int IHAntNumPerPanel = Parameters::Instance().RIS.IHAntNumPerPanel;
    int IVAntNumPerPanel = Parameters::Instance().RIS.IVAntNumPerPanel;

    int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.H_TXRU_DIV_NUM_PerPanel;
    int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.V_TXRU_DIV_NUM_PerPanel;

    m_H_AntNumPerTXRU = IHAntNumPerPanel / H_TXRU_DIV_NUM_PerPanel;
    m_V_AntNumPerTXRU = IVAntNumPerPanel / V_TXRU_DIV_NUM_PerPanel;

    int Polarize_Num = Parameters::Instance().RIS.Polarize_Num;

    m_H_Offset_lamda = m_H_TXRU_Index * m_H_AntNumPerTXRU * m_dH;
    m_V_Offset_lamda = m_V_TXRU_Index * m_V_AntNumPerTXRU * m_dV;

    if (Polarize_Num == 1) {
        m_PolarAngle_RAD = 0.0;
    } else if (Polarize_Num == 2) {
        m_PolarAngle_RAD = (m_Polar_Index == 0) ? M_PI / 2 : 0;
    } else {
        assert(false);
    }
}

void CTXRU::Build_BS_TXRU() {
    
    m_dH = Parameters::Instance().Macro.DHAntSpace;
    m_dV = Parameters::Instance().Macro.DVAntSpace;
    
    int IHAntNumPerPanel = Parameters::Instance().Macro.IHAntNumPerPanel;
    int IVAntNumPerPanel = Parameters::Instance().Macro.IVAntNumPerPanel;
    
    int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().Macro.H_TXRU_DIV_NUM_PerPanel;
    int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().Macro.V_TXRU_DIV_NUM_PerPanel;
    
    m_H_AntNumPerTXRU = IHAntNumPerPanel / H_TXRU_DIV_NUM_PerPanel;
    m_V_AntNumPerTXRU = IVAntNumPerPanel / V_TXRU_DIV_NUM_PerPanel;
    
    int Polarize_Num = Parameters::Instance().Macro.Polarize_Num;
       
    m_H_Offset_lamda = m_H_TXRU_Index * m_H_AntNumPerTXRU * m_dH;
    m_V_Offset_lamda = m_V_TXRU_Index * m_V_AntNumPerTXRU * m_dV;  
    
    if(Polarize_Num == 1) {
        m_PolarAngle_RAD = 0.0;
    } else if(Polarize_Num == 2) {
        m_PolarAngle_RAD = (m_Polar_Index == 0) ? M_PI / 4 : -M_PI / 4;
    } else {
        assert(false);
    }
    //chty 1111 b
    m_AntennaPanelIndex = m_pFatherAntennaPanel->GetPanelIndex();
    m_H_Offset_in_Antenna = m_H_Offset_lamda+ m_pFatherAntennaPanel->Get_LCS_Panel_LB_H_Offset_lamda_in_Antenna();;
    m_V_Offset_in_Antenna = m_V_Offset_lamda + m_pFatherAntennaPanel->Get_LCS_Panel_LB_V_Offset_lamda_in_Antenna();
    //chty 1111 e
}

void CTXRU::Build_UE_TXRU(int _type) {
    int Polarize_Num;
    if(_type==1){
        m_dH = Parameters::Instance().MSS.FirstBand.DHAntSpace;
        m_dV = Parameters::Instance().MSS.FirstBand.DVAntSpace;
        m_H_AntNumPerTXRU = Parameters::Instance().MSS.FirstBand.IHAntNumPerPanel / Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel;
        m_V_AntNumPerTXRU = Parameters::Instance().MSS.FirstBand.IVAntNumPerPanel / Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel;
        Polarize_Num=Parameters::Instance().MSS.FirstBand.Polarize_Num;
    }
    else if(_type==2){
        m_dH = Parameters::Instance().MSS.FirstBand.DHAntSpace_2;
        m_dV = Parameters::Instance().MSS.FirstBand.DVAntSpace_2;
        m_H_AntNumPerTXRU = Parameters::Instance().MSS.FirstBand.IHAntNumPerPanel_2 / Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel_2;
        m_V_AntNumPerTXRU = Parameters::Instance().MSS.FirstBand.IVAntNumPerPanel_2 / Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel_2;
        Polarize_Num=Parameters::Instance().MSS.FirstBand.Polarize_Num_2;
    }
    else {
        cout<<"wrong ue type-CTXRU"<<endl;
        assert(false);
    }
    m_H_Offset_lamda = m_H_TXRU_Index * m_H_AntNumPerTXRU * m_dH;
    m_V_Offset_lamda = m_V_TXRU_Index * m_V_AntNumPerTXRU * m_dV;  
    
    if(Polarize_Num == 1) {
        m_PolarAngle_RAD = 0.0;
    } else if(Polarize_Num == 2) {
        m_PolarAngle_RAD = (m_Polar_Index == 0) ? M_PI / 2 : 0;
    } else {
        assert(false);
    }
    //chty 1111 b
    m_AntennaPanelIndex=m_pFatherAntennaPanel->GetPanelIndex();
    m_H_Offset_in_Antenna = m_H_Offset_lamda+ m_pFatherAntennaPanel->Get_LCS_Panel_LB_H_Offset_lamda_in_Antenna();
    m_V_Offset_in_Antenna = m_V_Offset_lamda + m_pFatherAntennaPanel->Get_LCS_Panel_LB_V_Offset_lamda_in_Antenna();
    //chty 1111 e
}

AntennaPanel* CTXRU::GetFatherAntennaPanelPointer() {
    return m_pFatherAntennaPanel;
}

double CTXRU::Get_LCS_TXRU_LB_H_Offset_lamda_in_Antenna() const {
    //chty 1111 b
    return m_H_Offset_in_Antenna;
//    return m_H_Offset_lamda
//        + m_pFatherAntennaPanel->Get_LCS_Panel_LB_H_Offset_lamda_in_Antenna();
    //chty 1111 e
}

double CTXRU::Get_LCS_TXRU_LB_V_Offset_lamda_in_Antenna() const {
    //chty 1111 b
    return m_V_Offset_in_Antenna;
//    return m_V_Offset_lamda
//        + m_pFatherAntennaPanel->Get_LCS_Panel_LB_V_Offset_lamda_in_Antenna();
    //chty 1111 e
}

double CTXRU::Calc_LCS_AntElement_H_Offset_in_Antenna(int _AntIndex_in_TXRU) const {
	int H_Index_in_TXRU = _AntIndex_in_TXRU / m_V_AntNumPerTXRU;
    
    double LCS_AntElement_H_Offset = H_Index_in_TXRU * m_dH
        + m_Panel_H_Offset_lamda+m_H_Offset_lamda;
    
    return LCS_AntElement_H_Offset;
    
}
double CTXRU::Calc_LCS_AntElement_V_Offset_in_Antenna(int _AntIndex_in_TXRU) const {
	int V_Index_in_TXRU = _AntIndex_in_TXRU % m_V_AntNumPerTXRU;
    
    double LCS_AntElement_V_Offset = V_Index_in_TXRU * m_dV
        + m_Panel_V_Offset_lamda+m_V_Offset_lamda;
    
    return LCS_AntElement_V_Offset;
}

std::complex<double> CTXRU::CalcAggregateGain(
    double dAzimuthRAD_GCS, double dElevationRAD_GCS, 
    int _BeamIndex) {
    
    //std::shared_ptr<AntennaPanel> _pAntennaPanel = GetFatherAntennaPanelPointer();
    
    double dElevationRAD_LCS = ELoSRAD_GCS2LCS(
        dAzimuthRAD_GCS, dElevationRAD_GCS,
        m_pFatherAntennaPanel->GetTxRxOrientRAD(),
        m_pFatherAntennaPanel->GetMechanicalTiltRAD(),
        0);

    double dAzimuthRAD_LCS = ALoSRAD_GCS2LCS(
        dAzimuthRAD_GCS, dElevationRAD_GCS,
        m_pFatherAntennaPanel->GetTxRxOrientRAD(),
        m_pFatherAntennaPanel->GetMechanicalTiltRAD(),
        0);   
    
    itpp::cmat mPhase = Calc_TXRU_Phase(
        dElevationRAD_LCS, dAzimuthRAD_LCS);
    
    itpp::cmat mWeight = Calc_TXRU_Weight(_BeamIndex);
    
    itpp::cmat TMP = mWeight * mPhase;
    
    std::complex<double> cAggregateGain = TMP(0, 0);
    
    return cAggregateGain;
}

itpp::cmat CTXRU::Calc_TXRU_Weight(int _BeamIndex) {
    std::shared_ptr<Antenna> pAntenna =
            m_pFatherAntennaPanel->GetFatherAntennaPointer();

    double EscanRAD = pAntenna->GetEscanRAD(_BeamIndex);
    double EtiltRAD = pAntenna->GetEtiltRAD(_BeamIndex);
    
    // mMatrix_old = [h0v0, h0v1, h0v2, ..., h1v0, h1v1, h1v2, ...], 标准
    itpp::cmat mMatrix = itpp::zeros_c(
            1 , m_H_AntNumPerTXRU * m_V_AntNumPerTXRU );
    
    
    //2208sincos
    double temp_a = 1 / sqrt(m_H_AntNumPerTXRU * m_V_AntNumPerTXRU) ;
    double temp_b = m_dV * sin(EtiltRAD);
    double temp_c = m_dH * cos(EtiltRAD) * sin(EscanRAD);
    
    for (int h = 0; h < m_H_AntNumPerTXRU; ++h){
        for (int v = 0; v < m_V_AntNumPerTXRU; ++v){
            complex<double> mtemp = temp_a * exp(2.0 * M_PI * (v * temp_b - h *  temp_c) * M_J);
            
            mMatrix(0, GetPanelWeightIndex(
                    h, v, m_H_AntNumPerTXRU, m_V_AntNumPerTXRU) ) = mtemp;   
        }
    }
    return mMatrix;
}

itpp::cmat CTXRU::Calc_TXRU_Phase(
    double _dElevationRAD_LCS, double _dAzimuthRAD_LCS) const{
    
    ///注意：该函数中_detiltRAD=90代表垂直于Z轴方向；_descanRAD代表和水平维主瓣方向的角度差值
    itpp::cmat mMatrix = itpp::zeros_c( 
        m_H_AntNumPerTXRU * m_V_AntNumPerTXRU, 1);
    
    
    //2208sincos
    double temp_cos = cos(_dElevationRAD_LCS);
    double temp_sin = sin(_dElevationRAD_LCS) * sin(_dAzimuthRAD_LCS);  
    
    for (int h = 0; h < m_H_AntNumPerTXRU; ++h){
        for (int v = 0; v < m_V_AntNumPerTXRU; ++v){
            
            //20180625
            complex<double> mtemp = 
                exp(2.0 * M_PI * (
                    (m_V_Offset_lamda + v * m_dV) * temp_cos
                    + (m_H_Offset_lamda + h * m_dH) * temp_sin ) * M_J);
            
//            mMatrix(m*_iN_V+n,0)=mtemp;
            // 20171129
            mMatrix( GetPanelWeightIndex(
                    h, v, m_H_AntNumPerTXRU, m_V_AntNumPerTXRU), 0 ) = mtemp;
        }
    }
     return mMatrix;
    
}

