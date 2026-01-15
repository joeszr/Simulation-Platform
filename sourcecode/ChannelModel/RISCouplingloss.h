/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/*
 * File:   RISCouplingloss.h
 * Author: oumali
 *
 * Created on 2023年9月21日, 下午3:38
 */

#pragma once
#include "libfiles.h"
#include "P.h"
#include "AntennaPanel.h"
#include "../MobileStation/MS.h"
#include "../BaseStation/BTS.h"

namespace cm{
    class ChannelState;
    class RISCouplingloss{
        typedef std::pair<int, int> RISRxID;
        typedef std::map<RISRxID, ChannelState> RISRxCS;

        typedef std::pair<int, int> TxRISID;
        typedef std::map<TxRISID, ChannelState> TxRISCS;
        typedef std::map<int, double> MSRP;

        typedef std::pair<int, int> TxRxID;
        typedef std::map<TxRxID, ChannelState> TxRxCS;
    public:
        //RIS双波束功率分配注水算法，pair<信噪比的倒数，分配的功率>
        void waterfilling(std::vector<std::pair<double,double> > &a);
        double InitializeRIS_CouplingLoss_linear_Highfreq(BTS& bts,MS& ms,int PhaseCase);
        std::pair<BTSID,double> InitializeRIS_CouplingLoss_linear_Highfreq(MS& ms,int PhaseCase);
        //double InitializeRIS_CouplingLoss_linear_Highfreq_Mind(BTS& bts, MS& ms,int Bs_Beam_Index,int Ms_Beam_Index,int PhaseCase,RIS*ris);
        double InitializeRIS_CouplingLoss_linear_Highfreq(BTS& bts, MS& ms,int Bs_Beam_Index,int Ms_Beam_Index,int PhaseCase);
        double InitializeRIS_CouplingLoss_linear_beamscannning(BTS& bts,MS& ms);
        double InitializeRIS_CouplingLoss_linear_beamscannning(BTS& bts, MS& ms,int Bs_Beam_Index,int Ms_Beam_Index,int case_);
        double InitializeRIS_CouplingLoss_linear_Interference(BTS& bts,MS& ms);
        //使用特定相位
        void InitializeRIS_CouplingLoss_linear_specific_angle(BTS& bts,MS& ms,RISID risid);
        //返回离BTS最近的RIS
        RIS&Get_mind_RIS(BTS& bts,MS& ms);
    public:
        MSRP m_MSRP;
        static RISCouplingloss& Instance();
    private:
        static RISCouplingloss* m_pRC;
    public:
        void InitializeRIS_CouplingLoss_linear_recal(BTS& bts,MS& ms,RISID risid);
    };
}