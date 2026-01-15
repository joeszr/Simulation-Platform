/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CTXRU.h
 * Author: root
 *
 * Created on 2018年6月12日, 下午2:22
 */

#pragma once
#include <memory>
namespace cm {

    class Antenna;
    class AntennaPanel;
    
    class CTXRU {
    protected:
        // 所在天线面板的左下点相对于天线局部坐标系(LCS)的相对偏移量, in lamda
        double m_Panel_H_Offset_lamda;
        double m_Panel_V_Offset_lamda;
        // TXRU的左下阵子相对于天线面板的左下点的相对偏移量, in lamda
        double m_H_Offset_lamda;
        double m_V_Offset_lamda;
        
        // 0/90, +45/-45 度极化
        double m_PolarAngle_RAD;   // 极化角度
        
    protected:
        int m_H_AntNumPerTXRU;
        int m_V_AntNumPerTXRU;
        
        double m_dH;
        double m_dV;
        
    public:
        void Build_BS_TXRU();
        
        void Build_UE_TXRU(int _type=1);
        void Build_RIS_TXRU();

        
    public:
        double Get_LCS_TXRU_LB_H_Offset_lamda_in_Antenna() const;
        
        double Get_LCS_TXRU_LB_V_Offset_lamda_in_Antenna() const;
        
    public:
        double Calc_LCS_AntElement_H_Offset_in_Antenna(int _AntIndex_in_TXRU) const;
        double Calc_LCS_AntElement_V_Offset_in_Antenna(int _AntIndex_in_TXRU) const;
        
    public:
        int Get_V_AntNumPerTXRU() const {
            return m_V_AntNumPerTXRU;
        }
        
        int Get_H_AntNumPerTXRU() const {
            return m_H_AntNumPerTXRU;
        }
        
        int Get_Total_AntNumPerTXRU() const {
            return m_H_AntNumPerTXRU * m_V_AntNumPerTXRU;
        }
        
        int GetTXRUIndex() const {
            return m_TXRU_Index;
        }
        
        double GetPolarAngle_RAD() const {
            return m_PolarAngle_RAD;
        }
        
    public:
        std::shared_ptr<AntennaPanel> GetFatherAntennaPanelPointer();
        
    public:
        itpp::cmat Calc_TXRU_Weight(int _BeamIndex);
        
        itpp::cmat Calc_TXRU_Phase(
            double _dElevationRAD_LCS, double _dAzimuthRAD_LCS) const;
        
        std::complex<double> CalcAggregateGain(
            double dAzimuthRAD_GCS, double dElevationRAD_GCS, 
            int _BeamIndex);
        
    public:        
        CTXRU(AntennaPanel* _pFatherAntennaPanel, int _Polar_Index,
            int _H_TXRU_Index, int _V_TXRU_Index, int _TXRU_Index);
        
        virtual ~CTXRU() = default;
    
    protected:
        AntennaPanel*  m_pFatherAntennaPanel;
        int m_Polar_Index;
        int m_H_TXRU_Index;
        int m_V_TXRU_Index;
    public:
        //chty 1111 b
        int GetHTxRUIndex() const {
            return m_H_TXRU_Index;
        }

        int GetVTxRUIndex() const {
            return m_V_TXRU_Index;
        }
        //chty 1111 e
    protected:
        int m_TXRU_Index;
        //chty 1111 b
    private:
        int m_AntennaPanelIndex;
        double m_H_Offset_in_Antenna;
        double m_V_Offset_in_Antenna;
    public:
        int GetPanelIndex() const{
            return m_AntennaPanelIndex;
        }
        //chty 1111 e
    };

}

