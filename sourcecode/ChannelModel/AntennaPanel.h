/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AntennaPanel.h
 * Author: root
 *
 * Created on 2018年6月12日, 下午2:20
 */

#pragma once

#include "CTXRU.h"
#include "libfiles.h"

namespace cm {
    
    class Antenna;

    class AntennaPanel : public std::enable_shared_from_this<AntennaPanel> {
    
    protected:
        // 天线面板的左下点相对于天线局部坐标系(LCS)的相对偏移量, in lamda
        double m_Panel_H_Offset_lamda;
        double m_Panel_V_Offset_lamda;
        
        // in rad
        double m_Panel_Orient_Offset_RAD;
        
    protected:
        
        
        std::vector< std::shared_ptr<CTXRU> > m_vTXRUs;
        
        
        
    public:
        void Build_BS_AntennaPanel();
        void Build_RIS_AntennaPanel();
        void Build_UE_AntennaPanel(int _type);
        
    public:
        std::shared_ptr<Antenna> GetFatherAntennaPointer();
        
        std::vector< std::shared_ptr<CTXRU> >& GetvTXRUs() {
            return m_vTXRUs;
        }
        
        int GetPanelIndex() const {
            return m_Panel_Index;
        }
        
        double Get_LCS_Panel_LB_H_Offset_lamda_in_Antenna() const {
            return m_Panel_H_Offset_lamda;
        }
        
        double Get_LCS_Panel_LB_V_Offset_lamda_in_Antenna() const {
            return m_Panel_V_Offset_lamda;
        }
        
    public:
        std::shared_ptr<CTXRU> GetFirstTXRU();
        
//        std::shared_ptr<CTXRU> GetTXRU(int _Panel_Index);
        
        double GetTxRxOrientRAD() const;
        
        double GetMechanicalTiltRAD() const;
        
        int GetTXRU_Num() const;
        
    public:
        AntennaPanel(const std::shared_ptr<Antenna>& _pFatherAntenna,
                     const int& _H_Panel_Index, const int& _V_Panel_Index,
                     const int& _TXRUNum, const int& _Panel_Index);
        
        virtual ~AntennaPanel() = default;
        
    protected:
        std::weak_ptr<Antenna> m_pFatherAntenna;
        int m_H_Panel_Index;
        int m_V_Panel_Index;
        int m_TXRUNum;
        int m_Panel_Index;
        //chty 1111 b
    private:
        double m_MechanicalTiltRAD;
        //chty 1111 e
    };
}
