/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */

#include "RISCouplingloss.h"
#include "ChannelState.h"
#include "BasicChannelState.h"
#include "SpaceChannelState.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../Statistician/Observer.h"

//RIS beam scanning
using namespace cm;
extern LinkMatrix& lm;
RISCouplingloss* RISCouplingloss::m_pRC = 0;
boost::shared_mutex riscl_mutex;
///计算 RIS 元素加相位后的等效两跳增益，中找到 耦合增益最大 的那一个，结果写回linkMatrix和MS
double RISCouplingloss::InitializeRIS_CouplingLoss_linear_beamscannning(BTS& bts, MS& ms){
    TxRxID txrxid = std::make_pair(bts.GetTxID(), ms.GetRxID());
    Tx& tx = Tx::GetTx(bts.GetTxID());
    Rx& rx = *ms.m_pRxNode;
    double dCouplingLoss_RIS_Linear = 0.0;
    for (int i = 0; i < Parameters::Instance().BASIC.IRISPerBTS; ++i) {
           RIS& ris = bts.GetRIS(i);
           int rxid = ms.GetRxID();
           RISID id = ris.GetRISID();
           int risid = id.GetTotalIndex();
           RISRxID risrxid = std::make_pair(risid, rxid);
           TxRISID txrisid = std::make_pair(bts.GetTxID(), risid);
           std::shared_ptr<AntennaPanel> _pAntennaPanel = ris.GetAntennaPointer()->GetFirstAntennaPanelPointer();
           std::shared_ptr<AntennaPanel> _txAntennaPanel= tx.GetAntennaPointer()->GetFirstAntennaPanelPointer();
           std::shared_ptr<cm::CTXRU> pTx_TXRU = _txAntennaPanel->GetFirstTXRU();
           double dEOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EOALOSRAD;
           double dAOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AOALOSRAD;
           double dEODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EODLOSRAD;
           double dAODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AODLOSRAD;

           double dElevationRAD_TxRIS_LCS = ELoSRAD_GCS2LCS(
                    dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dElevationRAD_TxRIS_LCS>=0&&M_PI>=dElevationRAD_TxRIS_LCS);

           double dAzimuthRAD_TxRIS_LCS = ALoSRAD_GCS2LCS(
                    dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dAzimuthRAD_TxRIS_LCS>=-M_PI&&M_PI>=dAzimuthRAD_TxRIS_LCS);

           double dEODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_EODLOSRAD;
           double dAODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_AODLOSRAD;
//           double random1 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;
//           double random2 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;

           double dElevationRAD_RISRx_LCS = ELoSRAD_GCS2LCS(
                    dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           //Observer::Print("ElevationRAD_RISRx") <<  dElevationRAD_RISRx_LCS << endl;
           assert(dElevationRAD_RISRx_LCS>=0&&M_PI>=dElevationRAD_RISRx_LCS);
           double dAzimuthRAD_RISRx_LCS = ALoSRAD_GCS2LCS(
                    dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           //Observer::Print("AzimuthRAD_RISRx") <<  dAzimuthRAD_RISRx_LCS << endl;
           assert(dAzimuthRAD_RISRx_LCS>=-M_PI&&M_PI>=dAzimuthRAD_RISRx_LCS);
           double m_dH = Parameters::Instance().RIS.DHAntSpace;
           double m_dV = Parameters::Instance().RIS.DVAntSpace;
           int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.H_TXRU_DIV_NUM_PerPanel;
//            int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.V_TXRU_DIV_NUM_PerPanel;

           std::shared_ptr<cm::Antenna> pTxAntenna = tx.GetAntennaPointer();
           std::shared_ptr<cm::Antenna> pRxAntenna = rx.GetAntennaPointer();
           std::shared_ptr<cm::Antenna> pRisAntenna = ris.GetAntennaPointer();
           double dCouplingLoss_oneRIS = 0.0;
           complex<double> Alpha1Alpha2 = 0.0;
           complex<double> dCouplingLoss_RIS_temp1 = 0.0;
           complex<double> dCouplingLoss_RIS_temp3=0.0;
           double dCouplingLoss_RIS_temp2 = 0.0;
           pair<int,int> RIS_BtsBeamIndex_temp;
           int Ris_Polarize_Num =Parameters::Instance().RIS.Polarize_Num;
           int Index_tmp;
           int RIS_H_Index;
           int RIS_V_Index;
           int Best_RIS_H_Index;
           int Best_RIS_V_Index;


           for (int Bts_V_BeamIndex = 0; Bts_V_BeamIndex < pTxAntenna->Get_V_BeamNum(); ++Bts_V_BeamIndex) {
                for (int Bts_H_BeamIndex = 0; Bts_H_BeamIndex < pTxAntenna->Get_H_BeamNum(); ++Bts_H_BeamIndex) {
                    int BtsBeamIndex = pTxAntenna->Get_CombBeamIndex(Bts_V_BeamIndex, Bts_H_BeamIndex);
                    double EscanRAD = pTxAntenna->GetEscanRAD(BtsBeamIndex);
                    double EtiltRAD = pTxAntenna->GetEtiltRAD(BtsBeamIndex);
                    complex<double> cTxAggregateGain = pTx_TXRU->CalcAggregateGain(
                    dAODRAD_TxRIS_GCS, dEODRAD_TxRIS_GCS, BtsBeamIndex);
                    for (int Ms_V_BeamIndex = 0; Ms_V_BeamIndex < pRxAntenna->Get_V_BeamNum(); ++Ms_V_BeamIndex) {
                        for (int Ms_H_BeamIndex = 0; Ms_H_BeamIndex < pRxAntenna->Get_H_BeamNum(); ++Ms_H_BeamIndex) {
                            for (int Ris_V_BeamIndex = 0; Ris_V_BeamIndex < pRisAntenna->Get_V_BeamNum(); ++Ris_V_BeamIndex) {
                                for (int Ris_H_BeamIndex = 0; Ris_H_BeamIndex < pRisAntenna->Get_H_BeamNum(); ++Ris_H_BeamIndex) {
                                    int RisBeamIndex = pRisAntenna->Get_CombBeamIndex(Ris_V_BeamIndex,Ris_H_BeamIndex);
                                    dCouplingLoss_RIS_temp1 = 0.0;
                                    dCouplingLoss_RIS_temp3=0.0;
                                    int MsBeamIndex = pRxAntenna->Get_CombBeamIndex(Ms_V_BeamIndex, Ms_H_BeamIndex);
//                                    std::shared_ptr<AntennaPanel> _pBest_BS_Panel =
//                                        m_TxRx2CS[txrxid].m_pSCS->m_pBest_Tx_Panel;
//                                    std::shared_ptr<AntennaPanel> _pBest_UE_Panel =
//                                        m_TxRx2CS[txrxid].m_pSCS->m_pBest_Rx_Panel;
//                                    int BtsBeamIndex = m_TxRx2CS[txrxid].m_pSCS->GetStrongestTxBeamIndex(_pBest_BS_Panel,_pBest_UE_Panel);
                                    double dAzimuthRAD_RISRx_LCS_beam = pRisAntenna->GetEscanRAD(RisBeamIndex);
                                    double dElevationRAD_RISRx_LCS_beam = pRisAntenna->GetEtiltRAD(RisBeamIndex);

                                    for(int i =0;i<ris.GetAntennaPointer()->GetTotalTXRU_Num();i++){
                                        Index_tmp=i/Ris_Polarize_Num;
                                        RIS_H_Index= Index_tmp%H_TXRU_DIV_NUM_PerPanel;
                                        RIS_V_Index= Index_tmp/H_TXRU_DIV_NUM_PerPanel;
                                        Alpha1Alpha2 = lm.m_TxRIS2CS[txrisid].m_BCS.Alpha_for_CouplingLoss(BtsBeamIndex,i)*lm.m_RISRx2CS[risrxid].m_BCS.Alpha_for_CouplingLoss(MsBeamIndex,i);

//                                      double dAzimuthRAD_RISRx_LCS_beam = pRisAntenna->GetEscanRAD(Ris_H_BeamIndex)-dSpaceHRISBeamRAD/2;//+(-dAzimuthRAD_TxRIS_LCS)-floor((-dAzimuthRAD_TxRIS_LCS)/dSpaceHRISBeamRAD)*dSpaceHRISBeamRAD;
//                                      double dElevationRAD_RISRx_LCS_beam = pRisAntenna->GetEtiltRAD(Ris_V_BeamIndex)-dSpaceVRISBeamRAD/2+(M_PI/2.0-dElevationRAD_TxRIS_LCS)-floor((M_PI/2.0-dElevationRAD_TxRIS_LCS)/dSpaceVRISBeamRAD)*dSpaceVRISBeamRAD;
//                                      double dAzimuthRAD_TxRIS_LCS_beam = pTxAntenna->GetEscanRAD(BtsBeamIndex);
//                                      double dElevationRAD_TxRIS_LCS_beam = pTxAntenna->GetEtiltRAD(BtsBeamIndex);

//                                      double phase =std::arg(exp(-1.0*(2.0 * M_PI * ((i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_TxRIS_LCS_beam-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_TxRIS_LCS_beam-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS_beam)+(i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_RISRx_LCS_beam-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_RISRx_LCS_beam-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS_beam)))* M_J));
                                        //根据天线方向图最新phase
                                        std::complex<double> phase4=exp(-2.0 * M_PI * (RIS_V_Index * m_dV * cos(dElevationRAD_RISRx_LCS_beam)
                                        +RIS_H_Index * m_dH * sin(dElevationRAD_RISRx_LCS_beam) * sin(dAzimuthRAD_RISRx_LCS_beam)) * M_J);
                                        //配平ris-a的phase,ris-d的phase
                                        //std::complex<double> phase3 =exp(1.0*(2.0 * M_PI * (RIS_V_Index * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)- RIS_H_Index * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)+RIS_V_Index * m_dV * sin(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI)- RIS_H_Index * m_dH * cos(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS)))* M_J);
                                        dCouplingLoss_RIS_temp1 += (Alpha1Alpha2*phase4);
                                        //dCouplingLoss_RIS_temp3 += (Alpha1Alpha2*phase1);
                                    }
                                    double abs1=abs(dCouplingLoss_RIS_temp1);
                                    //double abs3=abs(dCouplingLoss_RIS_temp3);
                                    if(abs(dCouplingLoss_RIS_temp1)>dCouplingLoss_RIS_temp2){
                                        dCouplingLoss_RIS_temp2 = abs(dCouplingLoss_RIS_temp1);
                                        RIS_BtsBeamIndex_temp = make_pair(BtsBeamIndex,MsBeamIndex);
                                        Best_RIS_H_Index=Ris_H_BeamIndex;
                                        Best_RIS_V_Index=Ris_V_BeamIndex;
                                    }
                                }
                            }
                        }
                    }
                }
            }
           dCouplingLoss_oneRIS = pow(dCouplingLoss_RIS_temp2,2);
           dCouplingLoss_oneRIS *= DB2L(lm.m_TxRIS2CS[txrisid].m_BCS.m_PurePathLossDB)*DB2L(lm.m_RISRx2CS[risrxid].m_BCS.m_PurePathLossDB);
                if( dCouplingLoss_oneRIS>dCouplingLoss_RIS_Linear){
    ///选取最大的dCouplingLoss_OneRIS
                    dCouplingLoss_RIS_Linear = dCouplingLoss_oneRIS;
                    lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex = RIS_BtsBeamIndex_temp;
                    ms.m_vMainServRIS3 = id;
                    ms.Best_RIS_H3=Best_RIS_H_Index;
                    ms.Best_RIS_V3=Best_RIS_V_Index;
                    //m_TxRx2CS[txrxid].m_BCS.m_risindex=i;
                }

    }
    return dCouplingLoss_RIS_Linear;
}
///波束扫描，固定BS和UE波束
double RISCouplingloss::InitializeRIS_CouplingLoss_linear_beamscannning(BTS& bts, MS& ms,int Bs_Beam_Index,int Ms_Beam_Index,int case_){
    cout << "Beam_Index:"<< setw(2) << Bs_Beam_Index<<endl;
    double dCouplingLoss_RIS_Linear = 0.0;
    TxRxID txrxid = std::make_pair(bts.GetTxID(), ms.GetRxID());
    Tx& tx = Tx::GetTx(bts.GetTxID());
    Rx& rx = *ms.m_pRxNode;
    for (int i = 0; i < Parameters::Instance().BASIC.IRISPerBTS; ++i) {
           RIS& ris = bts.GetRIS(i);
           int rxid = ms.GetRxID();
           RISID id = ris.GetRISID();
           int risid = id.GetTotalIndex();
           RISRxID risrxid = std::make_pair(risid, rxid);
           //m_RISRx2CS[risrxid];
           TxRISID txrisid = std::make_pair(bts.GetTxID(), risid);
           //m_TxRIS2CS[txrisid];

           std::shared_ptr<AntennaPanel> _pAntennaPanel = ris.GetAntennaPointer()->GetFirstAntennaPanelPointer();
           std::shared_ptr<AntennaPanel> _txAntennaPanel= tx.GetAntennaPointer()->GetFirstAntennaPanelPointer();
           std::shared_ptr<cm::CTXRU> pTx_TXRU = _txAntennaPanel->GetFirstTXRU();
           double dEOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EOALOSRAD;
           double dAOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AOALOSRAD;
           double dEODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EODLOSRAD;
           double dAODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AODLOSRAD;
           double dElevationRAD_TxRIS_LCS = ELoSRAD_GCS2LCS(
                    dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dElevationRAD_TxRIS_LCS>=0&&M_PI>=dElevationRAD_TxRIS_LCS);

           double dAzimuthRAD_TxRIS_LCS = ALoSRAD_GCS2LCS(
                    dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dAzimuthRAD_TxRIS_LCS>=-M_PI&&M_PI>=dAzimuthRAD_TxRIS_LCS);

               double dEODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_EODLOSRAD;
           double dAODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_AODLOSRAD;
//           double random1 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;
//           double random2 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;

           double dElevationRAD_RISRx_LCS = ELoSRAD_GCS2LCS(
                    dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dElevationRAD_RISRx_LCS>=0&&M_PI>=dElevationRAD_RISRx_LCS);
           double dAzimuthRAD_RISRx_LCS = ALoSRAD_GCS2LCS(
                    dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dAzimuthRAD_RISRx_LCS>=-M_PI&&M_PI>=dAzimuthRAD_RISRx_LCS);
           double m_dH = Parameters::Instance().RIS.DHAntSpace;
           double m_dV = Parameters::Instance().RIS.DVAntSpace;
           int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.H_TXRU_DIV_NUM_PerPanel;
//            int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.V_TXRU_DIV_NUM_PerPanel;

           std::shared_ptr<cm::Antenna> pTxAntenna = tx.GetAntennaPointer();
           std::shared_ptr<cm::Antenna> pRxAntenna = rx.GetAntennaPointer();
           std::shared_ptr<cm::Antenna> pRisAntenna = ris.GetAntennaPointer();
           double dCouplingLoss_oneRIS = 0.0;
           complex<double> Alpha1Alpha2 = 0.0;
           complex<double> dCouplingLoss_RIS_temp1 = 0.0;
           double dCouplingLoss_RIS_temp2 = 0.0;
           int Index_tmp;
           int RIS_H_Index;
           int RIS_V_Index;
           int Best_RIS_H_Index;
           int Best_RIS_V_Index;
           int Ris_Polarize_Num =Parameters::Instance().RIS.Polarize_Num;
           //complex<double> cTxAggregateGain = pTx_TXRU->CalcAggregateGain(
           //dAODRAD_TxRIS_GCS, dEODRAD_TxRIS_GCS, Bs_Beam_Index);
           //std::complex<double> angle2 = std::arg(cTxAggregateGain);
           //angle2=exp((-1)*angle2* M_J);
           for (int Ris_V_BeamIndex = 0; Ris_V_BeamIndex < pRisAntenna->Get_V_BeamNum(); ++Ris_V_BeamIndex) {
                for (int Ris_H_BeamIndex = 0; Ris_H_BeamIndex < pRisAntenna->Get_H_BeamNum(); ++Ris_H_BeamIndex) {

//                            std::shared_ptr<AntennaPanel> _pBest_BS_Panel =
//                                m_TxRx2CS[txrxid].m_pSCS->m_pBest_Tx_Panel;
//                            std::shared_ptr<AntennaPanel> _pBest_UE_Panel =
//                                m_TxRx2CS[txrxid].m_pSCS->m_pBest_Rx_Panel;

//                            int BtsBeamIndex = m_TxRx2CS[txrxid].m_pSCS->GetStrongestTxBeamIndex(_pBest_BS_Panel, _pBest_UE_Panel);
                            int RisBeamIndex = pRisAntenna->Get_CombBeamIndex(Ris_V_BeamIndex,Ris_H_BeamIndex);
                            dCouplingLoss_RIS_temp1=0;
                            for(int i =0;i<ris.GetAntennaPointer()->GetTotalTXRU_Num();i++){
                                Alpha1Alpha2 = lm.m_TxRIS2CS[txrisid].m_BCS.Alpha_for_CouplingLoss(Bs_Beam_Index,i)*lm.m_RISRx2CS[risrxid].m_BCS.Alpha_for_CouplingLoss(Ms_Beam_Index,i);
                                //double angle = std::arg(Alpha1Alpha2);

                                        double dAzimuthRAD_RISRx_LCS_beam = pRisAntenna->GetEscanRAD(RisBeamIndex);
                                        double dElevationRAD_RISRx_LCS_beam = pRisAntenna->GetEtiltRAD(RisBeamIndex);
                                         Index_tmp=i/Ris_Polarize_Num;
                                         RIS_H_Index=Index_tmp%H_TXRU_DIV_NUM_PerPanel;
                                         RIS_V_Index=Index_tmp/H_TXRU_DIV_NUM_PerPanel;
                                         //std::complex<double> phase1 =exp(1.0*(2.0 * M_PI * (RIS_V_Index * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)- RIS_H_Index * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)+RIS_V_Index * m_dV * sin(dElevationRAD_RISRx_LCS_beam-1.0/2.0 *M_PI)- RIS_H_Index * m_dH * cos(dElevationRAD_RISRx_LCS_beam-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS_beam)))* M_J);
                                         std::complex<double> phase4=exp(-2.0 * M_PI * (RIS_V_Index * m_dV * cos(dElevationRAD_RISRx_LCS_beam)
                                               +RIS_H_Index * m_dH * sin(dElevationRAD_RISRx_LCS_beam) * sin(dAzimuthRAD_RISRx_LCS_beam)) * M_J);
                                         //std::complex<double> phase2=phase1;
                                        if(case_==2){
                                            double angle = std::arg(phase4);
//按angle的区间，将相位量化到四个值
                                            if(0.0>angle&&angle>=-1.0/2.0 * M_PI){
                                                phase4 = exp(1.0/2.0 * M_PI * M_J);
                                            }else if(angle>= -M_PI&&angle<-1.0/2.0 * M_PI){
                                                phase4 = exp( M_PI * M_J);
                                            }else if(angle>=1.0/2.0 *M_PI&&angle< M_PI){
                                                phase4 = exp((-1)*( 1.0/2.0 *M_PI * M_J));
                                            }else if(angle> 0.0&&angle< 1.0/2.0 *M_PI){
                                                phase4 = exp((-1)*( 0.0 *M_PI * M_J));
                                            }
                                            else if(angle==0.0){
                                                phase4 = exp((-1)*( 0.0 *M_PI * M_J));
                                            }else if(angle==M_PI){
                                                phase4 = exp((-1)*( M_PI * M_J));
                                            }else{
                                                assert(false);
                                            }
                                        }
                                        Alpha1Alpha2*=phase4;

                                dCouplingLoss_RIS_temp1 += Alpha1Alpha2;
                            }
                            if(abs(dCouplingLoss_RIS_temp1)>dCouplingLoss_RIS_temp2){
                                dCouplingLoss_RIS_temp2 = abs(dCouplingLoss_RIS_temp1);
                                Best_RIS_H_Index=Ris_H_BeamIndex;
                                Best_RIS_V_Index=Ris_V_BeamIndex;
                            }
                        }
            }
           dCouplingLoss_oneRIS = pow(dCouplingLoss_RIS_temp2,2);
           dCouplingLoss_oneRIS *= DB2L(lm.m_TxRIS2CS[txrisid].m_BCS.m_PurePathLossDB)*DB2L(lm.m_RISRx2CS[risrxid].m_BCS.m_PurePathLossDB);
                if( dCouplingLoss_oneRIS>dCouplingLoss_RIS_Linear){
                    dCouplingLoss_RIS_Linear = dCouplingLoss_oneRIS;
                    ms.m_vMainServRIS0 = id;
                    ms.Best_RIS_V0=Best_RIS_V_Index;
                    ms.Best_RIS_H0=Best_RIS_H_Index;
                }
    }
    return dCouplingLoss_RIS_Linear;
}
///高频建模
double RISCouplingloss::InitializeRIS_CouplingLoss_linear_Highfreq(BTS& bts, MS& ms,int PhaseCase){
    double dCouplingLoss_RIS_Linear = 0.0;
    TxRxID txrxid = std::make_pair(bts.GetTxID(), ms.GetRxID());
    Tx& tx = Tx::GetTx(bts.GetTxID());
    Rx& rx = *ms.m_pRxNode;
    RISRxID best_RISRxID;
    //itpp::cmat tempR = itpp::zeros_c(bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num(), bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num());
    //std::vector<std::complex<double>> tempR (bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num(),1.0);
    for (int i = 0; i < Parameters::Instance().BASIC.IRISPerBTS; ++i) {
           RIS& ris = bts.GetRIS(i);
           int rxid = ms.GetRxID();
           RISID id = ris.GetRISID();
           int risid = id.GetTotalIndex();
           RISRxID risrxid = std::make_pair(risid, rxid);
           TxRISID txrisid = std::make_pair(bts.GetTxID(), risid);
           std::shared_ptr<AntennaPanel> _pAntennaPanel = ris.GetAntennaPointer()->GetFirstAntennaPanelPointer();
           std::shared_ptr<AntennaPanel> _txAntennaPanel= tx.GetAntennaPointer()->GetFirstAntennaPanelPointer();
           std::shared_ptr<cm::CTXRU> pTx_TXRU = _txAntennaPanel->GetFirstTXRU();
           double dEOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EOALOSRAD;
           double dAOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AOALOSRAD;
           double dEODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EODLOSRAD;
           double dAODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AODLOSRAD;
           double dElevationRAD_TxRIS_LCS = ELoSRAD_GCS2LCS(
                    dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dElevationRAD_TxRIS_LCS>=0&&M_PI>=dElevationRAD_TxRIS_LCS);

           double dAzimuthRAD_TxRIS_LCS = ALoSRAD_GCS2LCS(
                    dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dAzimuthRAD_TxRIS_LCS>=-M_PI&&M_PI>=dAzimuthRAD_TxRIS_LCS);

           double dEODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_EODLOSRAD;
           double dAODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_AODLOSRAD;
//           double random1 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;
//           double random2 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;

           double dElevationRAD_RISRx_LCS = ELoSRAD_GCS2LCS(
                    dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dElevationRAD_RISRx_LCS>=0&&M_PI>=dElevationRAD_RISRx_LCS);
           double dAzimuthRAD_RISRx_LCS = ALoSRAD_GCS2LCS(
                    dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dAzimuthRAD_RISRx_LCS>=-M_PI&&M_PI>=dAzimuthRAD_RISRx_LCS);
           double m_dH = Parameters::Instance().RIS.DHAntSpace;
           double m_dV = Parameters::Instance().RIS.DVAntSpace;
           int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.H_TXRU_DIV_NUM_PerPanel;
//            int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.V_TXRU_DIV_NUM_PerPanel;

           std::shared_ptr<cm::Antenna> pTxAntenna = tx.GetAntennaPointer();
           std::shared_ptr<cm::Antenna> pRxAntenna = rx.GetAntennaPointer();
           std::shared_ptr<cm::Antenna> pRisAntenna = ris.GetAntennaPointer();
           double dCouplingLoss_oneRIS = 0.0;
           complex<double> Alpha1Alpha2 = 0.0;
           complex<double> dCouplingLoss_RIS_temp1 = 0.0;
           double dCouplingLoss_RIS_temp2 = 0.0;
           pair<int,int> RIS_BtsBeamIndex_temp;
           //itpp::cmat tempR_PRIS = itpp::zeros_c(bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num(), bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num());
           //std::vector<std::complex<double>> tempR_PRIS (bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num(),1.0);
           int Index_tmp;
           int RIS_H_Index;
           int RIS_V_Index;
           int Ris_BeamIndex;
           int Ris_Polarize_Num =Parameters::Instance().RIS.Polarize_Num;
           //生成ris随机波束index
           for (int Bts_V_BeamIndex = 0; Bts_V_BeamIndex < pTxAntenna->Get_V_BeamNum(); ++Bts_V_BeamIndex) {
                for (int Bts_H_BeamIndex = 0; Bts_H_BeamIndex < pTxAntenna->Get_H_BeamNum(); ++Bts_H_BeamIndex) {
                    int BtsBeamIndex = pTxAntenna->Get_CombBeamIndex(Bts_V_BeamIndex, Bts_H_BeamIndex);
                    complex<double> cTxAggregateGain = pTx_TXRU->CalcAggregateGain(
                    dAODRAD_TxRIS_GCS, dEODRAD_TxRIS_GCS, BtsBeamIndex);
                    std::complex<double> angle2 = std::arg(cTxAggregateGain);
                    angle2=exp((-1)*angle2* M_J);
                    for (int Ms_V_BeamIndex = 0; Ms_V_BeamIndex < pRxAntenna->Get_V_BeamNum(); ++Ms_V_BeamIndex) {
                        for (int Ms_H_BeamIndex = 0; Ms_H_BeamIndex < pRxAntenna->Get_H_BeamNum(); ++Ms_H_BeamIndex) {

                            int MsBeamIndex = pRxAntenna->Get_CombBeamIndex(Ms_V_BeamIndex, Ms_H_BeamIndex);

//                            std::shared_ptr<AntennaPanel> _pBest_BS_Panel =
//                                m_TxRx2CS[txrxid].m_pSCS->m_pBest_Tx_Panel;
//                            std::shared_ptr<AntennaPanel> _pBest_UE_Panel =
//                                m_TxRx2CS[txrxid].m_pSCS->m_pBest_Rx_Panel;

//                            int BtsBeamIndex = m_TxRx2CS[txrxid].m_pSCS->GetStrongestTxBeamIndex(_pBest_BS_Panel, _pBest_UE_Panel);
                            dCouplingLoss_RIS_temp1=0;
                            //tempR.clear();
                            for(int i =0;i<ris.GetAntennaPointer()->GetTotalTXRU_Num();i++){

                                Alpha1Alpha2 = lm.m_TxRIS2CS[txrisid].m_BCS.Alpha_for_CouplingLoss(BtsBeamIndex,i)*lm.m_RISRx2CS[risrxid].m_BCS.Alpha_for_CouplingLoss(MsBeamIndex,i);
//                                if(PhaseCase==4&&i==1)
//                                    Observer::Print("CouplingLoss1") << bts.GetTxID() << setw(20) << rxid << setw(20) << abs(Alpha1Alpha2) << endl;
                                double angle = std::arg(Alpha1Alpha2);
                                std::complex<double> phase;
                                switch (PhaseCase){
                                    //随机相位
                                    case 0:{
                                        double randomphase = xUniform_distributems(-1, 1);
                                        phase = exp(randomphase*M_PI * M_J);
                                        break;
                                    }
                                    case 1:{
                                        complex<double> mtemp =angle2*exp(2.0 * M_PI * ((i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)+(i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS)) * M_J);
                                        phase = mtemp;
                                        assert(abs(mtemp-exp((-1)*angle* M_J))<0.01);
                                        break;
                                    }
                                    case 2:{
                                        phase= exp((-1)*angle* M_J);
//                                        Alpha1Alpha2 *= exp((-1)*0* M_J);
                                        break;
                                    }
                                    //2bit量化
//                                    case 3:{
//                                        double phase =std::arg(exp(-1.0*(2.0 * M_PI * ((i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)+(i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS)))* M_J));
//                                        if(0.0>phase&&phase>=-1.0/2.0 * M_PI){
//                                            Alpha1Alpha2 *= exp(1.0/2.0 * M_PI * M_J);
//                                        }else if(phase>= -M_PI&&phase<-1.0/2.0 * M_PI){
//                                            Alpha1Alpha2 *= exp( M_PI * M_J);
//                                        }else if(phase>= 1.0/2.0 *M_PI&&phase< M_PI){
//                                            Alpha1Alpha2 *= exp((-1)*( 1.0/2.0 *M_PI * M_J));
//                                        }else if(phase> 0.0&&phase< 1.0/2.0 *M_PI){
//                                            Alpha1Alpha2 *= exp((-1)*( 0.0 *M_PI * M_J));
//                                        }
//                                        else if(phase==0.0){
//                                            Alpha1Alpha2 *= exp((-1)*( 0.0 *M_PI * M_J));
//                                        }else if(phase==M_PI){
//                                            Alpha1Alpha2 *= exp((-1)*( M_PI * M_J));
//                                        }else{
//                                            assert(false);
//                                        }
//                                        break;
//                                    }
                                    //1bit
                                    case 3:{
                                        //double phase =std::arg(angle2*exp(-1.0*(2.0 * M_PI * ((i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)+(i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS)))* M_J));
                                        if(0.0>angle&&angle>=-1.0/2.0 * M_PI){
                                            phase = exp(0.0 * M_PI * M_J);
                                        }else if(angle>= -M_PI&&angle<-1.0/2.0 * M_PI){
                                            phase *= exp((-1)*( M_PI * M_J));
                                        }else if(angle>= 1.0/2.0 *M_PI&&angle< M_PI){
                                            phase *= exp((-1)*( M_PI * M_J));
                                        }else if(angle> 0.0&&angle< 1.0/2.0 *M_PI){
                                            phase *= exp((-1)*( 0.0 *M_PI * M_J));
                                        }
                                        else if(angle==0.0){
                                            phase *= exp((-1)*( 0.0 *M_PI * M_J));
                                        }else if(angle==M_PI){
                                            phase *= exp((-1)*( M_PI * M_J));
                                        }else{
                                            assert(false);
                                        }
                                        break;
                                    }
                                    //2bit量化
                                    case 4:{
        //                                Observer::Print("phase") <<angle<<endl;
                                        if(0.0>angle&&angle>=-1.0/2.0 * M_PI){
                                            phase = exp(1.0/2.0 * M_PI * M_J);
                                        }else if(angle>= -M_PI&&angle<-1.0/2.0 * M_PI){
                                            phase = exp( M_PI * M_J);
                                        }else if(angle>=1.0/2.0 *M_PI&&angle< M_PI){
                                            phase = exp((-1)*( 1.0/2.0 *M_PI * M_J));
                                        }else if(angle> 0.0&&angle< 1.0/2.0 *M_PI){
                                            phase = exp((-1)*( 0.0 *M_PI * M_J));
                                        }
                                        else if(angle==0.0){
                                            phase = exp((-1)*( 0.0 *M_PI * M_J));
                                        }else if(angle==M_PI){
                                            phase = exp((-1)*( M_PI * M_J));
                                        }else{
                                            assert(false);
                                        }
                                        break;
                                    }
//                                    case 5:{
//                                        dElevationRAD_RISRx_LCS = (M_PI-dElevationRAD_TxRIS_LCS)+floor((dElevationRAD_RISRx_LCS-(M_PI-dElevationRAD_TxRIS_LCS-M_PI/36.0))/(M_PI/18.0))*(M_PI/18.0);
//                                        dAzimuthRAD_RISRx_LCS = floor((dAzimuthRAD_RISRx_LCS-(-M_PI/36.0))/(M_PI/18.0))*(M_PI/18.0);
//        //                                dElevationRAD_RISRx_LCS = (M_PI-dElevationRAD_TxRIS_LCS)+floor((dElevationRAD_RISRx_LCS-(M_PI-dElevationRAD_TxRIS_LCS-M_PI/18.0))/(M_PI/9.0))*(M_PI/9.0);
//        //                                dAzimuthRAD_RISRx_LCS = floor((dAzimuthRAD_RISRx_LCS-(-M_PI/18.0))/(M_PI/9.0))*(M_PI/9.0);
//                                        double phase =std::arg(angle2*exp(-1.0*(2.0 * M_PI * ((i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)+(i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS)))* M_J));
//                                        if(0.0>phase&&phase>=-1.0/2.0 * M_PI){
//                                            Alpha1Alpha2 *= exp(1.0/2.0 * M_PI * M_J);
//                                        }else if(phase>= -M_PI&&phase<-1.0/2.0 * M_PI){
//                                            Alpha1Alpha2 *= exp( M_PI * M_J);
//                                        }else if(phase>= 1.0/2.0 *M_PI&&phase< M_PI){
//                                            Alpha1Alpha2 *= exp((-1)*( 1.0/2.0 *M_PI * M_J));
//                                        }else if(phase> 0.0&&phase< 1.0/2.0 *M_PI){
//                                            Alpha1Alpha2 *= exp((-1)*( 0.0 *M_PI * M_J));
//                                        }
//                                        else if(phase==0.0){
//                                            Alpha1Alpha2 *= exp((-1)*( 0.0 *M_PI * M_J));
//                                        }else if(phase==M_PI){
//                                            Alpha1Alpha2 *= exp((-1)*( M_PI * M_J));
//                                        }else{
//                                            assert(false);
//                                        }
//                                        break;
//                                    }
                                    //参数错误
                                    default:{
                                        assert(false);
                                        break;
                                    }
                                }
                                Alpha1Alpha2 *= phase;
                                dCouplingLoss_RIS_temp1 += Alpha1Alpha2;
                                //tempR[i]=phase;
                            }
                            if(abs(dCouplingLoss_RIS_temp1)>dCouplingLoss_RIS_temp2){
                                dCouplingLoss_RIS_temp2 = abs(dCouplingLoss_RIS_temp1);
                                RIS_BtsBeamIndex_temp = make_pair(BtsBeamIndex,MsBeamIndex);
                                //tempR_PRIS = tempR;
                            }
                        }
                    }
                }
            }
           dCouplingLoss_oneRIS = pow(dCouplingLoss_RIS_temp2,2);
           dCouplingLoss_oneRIS *= DB2L(lm.m_TxRIS2CS[txrisid].m_BCS.m_PurePathLossDB)*DB2L(lm.m_RISRx2CS[risrxid].m_BCS.m_PurePathLossDB);

            if( dCouplingLoss_oneRIS>dCouplingLoss_RIS_Linear){
                dCouplingLoss_RIS_Linear = dCouplingLoss_oneRIS;
                //lm.m_TxRx2CS[txrxid].m_pSCS->tempRIS = tempR_PRIS;
                lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex2 = RIS_BtsBeamIndex_temp;
                ms.m_vMainServRIS2 = id;
                lm.m_TxRx2CS[txrxid].m_BCS.m_risindex=i;
            }
    }
    return dCouplingLoss_RIS_Linear;
}
///高频，BS和UE波束固定
std::pair<BTSID,double> RISCouplingloss::InitializeRIS_CouplingLoss_linear_Highfreq(MS& ms,int PhaseCase){
    int NumOfBTS = Parameters::Instance().BASIC.ITotalBTSNumPerBS * Parameters::Instance().BASIC.INumBSs;
    int NumOfMS = NumOfBTS * Parameters::Instance().BASIC.DNumMSPerBTS;
    Rx& rx = *ms.m_pRxNode;
    int rxid = ms.GetRxID();
    double dTxPower=Parameters::Instance().Macro.DL.DMaxTxPowerDbm;
    double dBest_ReceivedPower_BestCase=-200.0;
    double dBest_BTS_MS_Power_Case;
    double dBest_BTS_RIS_Power_Case;
    int txid_BestCase;
    int BS_TXRUNum = Parameters::Instance().Macro.H_TXRU_DIV_NUM_PerPanel * Parameters::Instance().Macro.V_TXRU_DIV_NUM_PerPanel * Parameters::Instance().Macro.Polarize_Num;
    BTSID btsid_case;
    RISID dBest_RISID_BTS;
    int dBest_BS_BeamIndex_Case;
    int dBest_UE_BeamIndex_Case;
    int Ris_Best_BeamIndex;
    //itpp::cmat tempR = itpp::zeros_c(bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num(), bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num());
    itpp::cmat tempR = itpp::zeros_c(Parameters::Instance().RIS.H_TXRU_DIV_NUM_PerPanel*Parameters::Instance().RIS.V_TXRU_DIV_NUM_PerPanel, 1);
    for(BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        BTS& bts = btsid.GetBTS();
        itpp::cmat tempR_PBTS =itpp::zeros_c(bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num(), 1);
        Tx& tx = Tx::GetTx(btsid.GetTotalIndex());
        int txid = bts.GetTxID();
        TxRxID txrxid = std::make_pair(txid, rxid);
        double dCouplingLoss_RIS_Linear = 0.0;
        int Ris_Best_BeamIndex_PBTS;
        for (int i = 0; i < Parameters::Instance().BASIC.IRISPerBTS; ++i) {
               RIS& ris = bts.GetRIS(i);
               int rxid = ms.GetRxID();
               RISID id = ris.GetRISID();
               int risid = id.GetTotalIndex();
               RISRxID risrxid = std::make_pair(risid, rxid);
               TxRISID txrisid = std::make_pair(bts.GetTxID(), risid);
               std::shared_ptr<AntennaPanel> _pAntennaPanel = ris.GetAntennaPointer()->GetFirstAntennaPanelPointer();
               std::shared_ptr<AntennaPanel> _txAntennaPanel= tx.GetAntennaPointer()->GetFirstAntennaPanelPointer();
               std::shared_ptr<cm::CTXRU> pTx_TXRU = _txAntennaPanel->GetFirstTXRU();
               double dEOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EOALOSRAD;
               double dAOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AOALOSRAD;
               double dEODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EODLOSRAD;
               double dAODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AODLOSRAD;
               double dElevationRAD_TxRIS_LCS = ELoSRAD_GCS2LCS(
                        dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                        _pAntennaPanel->GetTxRxOrientRAD(),
                        _pAntennaPanel->GetMechanicalTiltRAD(),
                        0);
               assert(dElevationRAD_TxRIS_LCS>=0&&M_PI>=dElevationRAD_TxRIS_LCS);

               double dAzimuthRAD_TxRIS_LCS = ALoSRAD_GCS2LCS(
                        dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                        _pAntennaPanel->GetTxRxOrientRAD(),
                        _pAntennaPanel->GetMechanicalTiltRAD(),
                        0);
               assert(dAzimuthRAD_TxRIS_LCS>=-M_PI&&M_PI>=dAzimuthRAD_TxRIS_LCS);

               double dEODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_EODLOSRAD;
               double dAODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_AODLOSRAD;
    //           double random1 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;
    //           double random2 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;

               double dElevationRAD_RISRx_LCS = ELoSRAD_GCS2LCS(
                        dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                        _pAntennaPanel->GetTxRxOrientRAD(),
                        _pAntennaPanel->GetMechanicalTiltRAD(),
                        0);
               assert(dElevationRAD_RISRx_LCS>=0&&M_PI>=dElevationRAD_RISRx_LCS);
               double dAzimuthRAD_RISRx_LCS = ALoSRAD_GCS2LCS(
                        dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                        _pAntennaPanel->GetTxRxOrientRAD(),
                        _pAntennaPanel->GetMechanicalTiltRAD(),
                        0);
               assert(dAzimuthRAD_RISRx_LCS>=-M_PI&&M_PI>=dAzimuthRAD_RISRx_LCS);
               double m_dH = Parameters::Instance().RIS.DHAntSpace;
               double m_dV = Parameters::Instance().RIS.DVAntSpace;
               int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.H_TXRU_DIV_NUM_PerPanel;
    //            int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.V_TXRU_DIV_NUM_PerPanel;

               std::shared_ptr<cm::Antenna> pTxAntenna = tx.GetAntennaPointer();
               std::shared_ptr<cm::Antenna> pRxAntenna = rx.GetAntennaPointer();
               std::shared_ptr<cm::Antenna> pRisAntenna = ris.GetAntennaPointer();
               double dCouplingLoss_oneRIS = 0.0;
               complex<double> Alpha1Alpha2 = 0.0;
               complex<double> dCouplingLoss_RIS_temp1 = 0.0;
               double dCouplingLoss_RIS_temp2 = 0.0;
               pair<int,int> RIS_BtsBeamIndex_temp;
               //itpp::cmat tempR_PRIS = itpp::zeros_c(bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num(), bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num());
               itpp::cmat tempR_PRIS =itpp::zeros_c(bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num(), 1);
               int Index_tmp;
               int RIS_H_Index;
               int RIS_V_Index;
               int Ris_Best_BeamIndex_PRIS;
               int Ris_Polarize_Num =Parameters::Instance().RIS.Polarize_Num;
               //生成ris随机波束index
               for (int Bts_V_BeamIndex = 0; Bts_V_BeamIndex < pTxAntenna->Get_V_BeamNum(); ++Bts_V_BeamIndex) {
                    for (int Bts_H_BeamIndex = 0; Bts_H_BeamIndex < pTxAntenna->Get_H_BeamNum(); ++Bts_H_BeamIndex) {
                        int BtsBeamIndex = pTxAntenna->Get_CombBeamIndex(Bts_V_BeamIndex, Bts_H_BeamIndex);
                        complex<double> cTxAggregateGain = pTx_TXRU->CalcAggregateGain(
                        dAODRAD_TxRIS_GCS, dEODRAD_TxRIS_GCS, BtsBeamIndex);
                        std::complex<double> angle2 = std::arg(cTxAggregateGain);
                        angle2=exp((-1)*angle2* M_J);
                        for (int Ms_V_BeamIndex = 0; Ms_V_BeamIndex < pRxAntenna->Get_V_BeamNum(); ++Ms_V_BeamIndex) {
                            for (int Ms_H_BeamIndex = 0; Ms_H_BeamIndex < pRxAntenna->Get_H_BeamNum(); ++Ms_H_BeamIndex) {

                                int MsBeamIndex = pRxAntenna->Get_CombBeamIndex(Ms_V_BeamIndex, Ms_H_BeamIndex);

    //                            std::shared_ptr<AntennaPanel> _pBest_BS_Panel =
    //                                m_TxRx2CS[txrxid].m_pSCS->m_pBest_Tx_Panel;
    //                            std::shared_ptr<AntennaPanel> _pBest_UE_Panel =
    //                                m_TxRx2CS[txrxid].m_pSCS->m_pBest_Rx_Panel;

    //                            int BtsBeamIndex = m_TxRx2CS[txrxid].m_pSCS->GetStrongestTxBeamIndex(_pBest_BS_Panel, _pBest_UE_Panel);
                                if(PhaseCase==5||PhaseCase==6){
                                    for (int Ris_V_BeamIndex = 0; Ris_V_BeamIndex < pRisAntenna->Get_V_BeamNum(); ++Ris_V_BeamIndex) {
                                        for (int Ris_H_BeamIndex = 0; Ris_H_BeamIndex < pRisAntenna->Get_H_BeamNum(); ++Ris_H_BeamIndex) {
                                            int RisBeamIndex = pRisAntenna->Get_CombBeamIndex(Ris_V_BeamIndex,Ris_H_BeamIndex);
                                            dCouplingLoss_RIS_temp1=0;
                                            for(int i =0;i<ris.GetAntennaPointer()->GetTotalTXRU_Num();i++){
                                                Alpha1Alpha2 = lm.m_TxRIS2CS[txrisid].m_BCS.Alpha_for_CouplingLoss(BtsBeamIndex,i)*lm.m_RISRx2CS[risrxid].m_BCS.Alpha_for_CouplingLoss(MsBeamIndex,i);
                                                double dAzimuthRAD_RISRx_LCS_beam = pRisAntenna->GetEscanRAD(RisBeamIndex);
                                                double dElevationRAD_RISRx_LCS_beam = pRisAntenna->GetEtiltRAD(RisBeamIndex);
                                                Index_tmp=i/Ris_Polarize_Num;
                                                RIS_H_Index=Index_tmp%H_TXRU_DIV_NUM_PerPanel;
                                                RIS_V_Index=Index_tmp/H_TXRU_DIV_NUM_PerPanel;
                                                std::complex<double> phase1=exp(-2.0 * M_PI * (RIS_V_Index * m_dV * cos(dElevationRAD_RISRx_LCS_beam)
                                                +RIS_H_Index * m_dH * sin(dElevationRAD_RISRx_LCS_beam) * sin(dAzimuthRAD_RISRx_LCS_beam)) * M_J);
                                                if(PhaseCase==6){
                                                    double angle = std::arg(phase1);
                                                    if(0.0>angle&&angle>=-1.0/2.0 * M_PI){
                                                        phase1 = exp(1.0/2.0 * M_PI * M_J);
                                                    }else if(angle>= -M_PI&&angle<-1.0/2.0 * M_PI){
                                                        phase1 = exp( M_PI * M_J);
                                                    }else if(angle>=1.0/2.0 *M_PI&&angle< M_PI){
                                                        phase1 = exp((-1)*( 1.0/2.0 *M_PI * M_J));
                                                    }else if(angle> 0.0&&angle< 1.0/2.0 *M_PI){
                                                        phase1 = exp((-1)*( 0.0 *M_PI * M_J));
                                                    }
                                                    else if(angle==0.0){
                                                        phase1 = exp((-1)*( 0.0 *M_PI * M_J));
                                                    }else if(angle==M_PI){
                                                        phase1 = exp((-1)*( M_PI * M_J));
                                                    }else{
                                                        assert(false);
                                                    }
                                                }
                                                Alpha1Alpha2*=phase1;
                                                dCouplingLoss_RIS_temp1 += Alpha1Alpha2;
                                                tempR(i,0)=phase1;
                                            }
                                            if(abs(dCouplingLoss_RIS_temp1)>dCouplingLoss_RIS_temp2){
                                                Ris_Best_BeamIndex_PRIS=RisBeamIndex;
                                                dCouplingLoss_RIS_temp2 = abs(dCouplingLoss_RIS_temp1);
                                                RIS_BtsBeamIndex_temp = make_pair(BtsBeamIndex,MsBeamIndex);
                                                tempR_PRIS = tempR;
                                            }

                                        }
                                    }
                                }
                                else{
                                    Ris_Best_BeamIndex_PRIS=0;
                                    dCouplingLoss_RIS_temp1=0;
                                    tempR.clear();
                                    for(int i =0;i<ris.GetAntennaPointer()->GetTotalTXRU_Num();i++){

                                        Alpha1Alpha2 = lm.m_TxRIS2CS[txrisid].m_BCS.Alpha_for_CouplingLoss(BtsBeamIndex,i)*lm.m_RISRx2CS[risrxid].m_BCS.Alpha_for_CouplingLoss(MsBeamIndex,i);
        //                                if(PhaseCase==4&&i==1)
        //                                    Observer::Print("CouplingLoss1") << bts.GetTxID() << setw(20) << rxid << setw(20) << abs(Alpha1Alpha2) << endl;
                                        double angle = std::arg(Alpha1Alpha2);
                                        std::complex<double> phase;
                                        switch (PhaseCase){
                                            //随机相位
                                            case 0:{
                                                double randomphase = xUniform_distributems(-1, 1);
                                                phase = exp(randomphase*M_PI * M_J);
                                                break;
                                            }
                                            case 1:{
                                                complex<double> mtemp =angle2*exp(2.0 * M_PI * ((i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)+(i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS)) * M_J);
                                                phase = mtemp;
                                                assert(abs(mtemp-exp((-1)*angle* M_J))<0.01);
                                                break;
                                            }
                                            case 2:{
                                                phase= exp((-1)*angle* M_J);
        //                                        Alpha1Alpha2 *= exp((-1)*0* M_J);
                                                break;
                                            }
                                            //2bit量化
        //                                    case 3:{
        //                                        double phase =std::arg(exp(-1.0*(2.0 * M_PI * ((i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)+(i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS)))* M_J));
        //                                        if(0.0>phase&&phase>=-1.0/2.0 * M_PI){
        //                                            Alpha1Alpha2 *= exp(1.0/2.0 * M_PI * M_J);
        //                                        }else if(phase>= -M_PI&&phase<-1.0/2.0 * M_PI){
        //                                            Alpha1Alpha2 *= exp( M_PI * M_J);
        //                                        }else if(phase>= 1.0/2.0 *M_PI&&phase< M_PI){
        //                                            Alpha1Alpha2 *= exp((-1)*( 1.0/2.0 *M_PI * M_J));
        //                                        }else if(phase> 0.0&&phase< 1.0/2.0 *M_PI){
        //                                            Alpha1Alpha2 *= exp((-1)*( 0.0 *M_PI * M_J));
        //                                        }
        //                                        else if(phase==0.0){
        //                                            Alpha1Alpha2 *= exp((-1)*( 0.0 *M_PI * M_J));
        //                                        }else if(phase==M_PI){
        //                                            Alpha1Alpha2 *= exp((-1)*( M_PI * M_J));
        //                                        }else{
        //                                            assert(false);
        //                                        }
        //                                        break;
        //                                    }
                                            //1bit
                                            case 3:{
                                                //double phase =std::arg(angle2*exp(-1.0*(2.0 * M_PI * ((i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)+(i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS)))* M_J));
                                                if(0.0>angle&&angle>=-1.0/2.0 * M_PI){
                                                    phase = exp(0.0 * M_PI * M_J);
                                                }else if(angle>= -M_PI&&angle<-1.0/2.0 * M_PI){
                                                    phase *= exp((-1)*( M_PI * M_J));
                                                }else if(angle>= 1.0/2.0 *M_PI&&angle< M_PI){
                                                    phase *= exp((-1)*( M_PI * M_J));
                                                }else if(angle> 0.0&&angle< 1.0/2.0 *M_PI){
                                                    phase *= exp((-1)*( 0.0 *M_PI * M_J));
                                                }
                                                else if(angle==0.0){
                                                    phase *= exp((-1)*( 0.0 *M_PI * M_J));
                                                }else if(angle==M_PI){
                                                    phase *= exp((-1)*( M_PI * M_J));
                                                }else{
                                                    assert(false);
                                                }
                                                break;
                                            }
                                            //2bit量化
                                            case 4:{
                //                                Observer::Print("phase") <<angle<<endl;
                                                if(0.0>angle&&angle>=-1.0/2.0 * M_PI){
                                                    phase = exp(1.0/2.0 * M_PI * M_J);
                                                }else if(angle>= -M_PI&&angle<-1.0/2.0 * M_PI){
                                                    phase = exp( M_PI * M_J);
                                                }else if(angle>=1.0/2.0 *M_PI&&angle< M_PI){
                                                    phase = exp((-1)*( 1.0/2.0 *M_PI * M_J));
                                                }else if(angle> 0.0&&angle< 1.0/2.0 *M_PI){
                                                    phase = exp((-1)*( 0.0 *M_PI * M_J));
                                                }
                                                else if(angle==0.0){
                                                    phase = exp((-1)*( 0.0 *M_PI * M_J));
                                                }else if(angle==M_PI){
                                                    phase = exp((-1)*( M_PI * M_J));
                                                }else{
                                                    assert(false);
                                                }
                                                break;
                                            }
                                            //参数错误
                                            default:{
                                                assert(false);
                                                break;
                                            }
                                        }
                                        Alpha1Alpha2 *= phase;
                                        dCouplingLoss_RIS_temp1 += Alpha1Alpha2;
                                        tempR(i,0)=phase;
                                    }
                                    if(abs(dCouplingLoss_RIS_temp1)>dCouplingLoss_RIS_temp2){
                                        dCouplingLoss_RIS_temp2 = abs(dCouplingLoss_RIS_temp1);
                                        RIS_BtsBeamIndex_temp = make_pair(BtsBeamIndex,MsBeamIndex);
                                        tempR_PRIS = tempR;
                                    }
                                }
                            }
                        }
                    }
                }
               dCouplingLoss_oneRIS = pow(dCouplingLoss_RIS_temp2,2);
               dCouplingLoss_oneRIS *= DB2L(lm.m_TxRIS2CS[txrisid].m_BCS.m_PurePathLossDB)*DB2L(lm.m_RISRx2CS[risrxid].m_BCS.m_PurePathLossDB);

                if( dCouplingLoss_oneRIS>dCouplingLoss_RIS_Linear){
                    dCouplingLoss_RIS_Linear = dCouplingLoss_oneRIS;
                    tempR_PBTS = tempR_PRIS;
                    lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex = RIS_BtsBeamIndex_temp;
                    dBest_RISID_BTS = id;
                    Ris_Best_BeamIndex_PBTS=Ris_Best_BeamIndex_PRIS;
                    lm.m_TxRx2CS[txrxid].m_BCS.m_risindex=i;
                }
        }
        double dReceivedPower_RIS_2 = dTxPower + L2DB(dCouplingLoss_RIS_Linear);
        double dReceivedPower_RIS_Linear_2 = DB2L(dReceivedPower_RIS_2);
        double dReceivedPower_BTS2MS_case = dTxPower + L2DB(lm.m_TxRx2CS[txrxid].m_pSCS->GetCouplingloss(lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex));
        double dReceivedPower_BTS2MS_case_Linear = DB2L(dReceivedPower_BTS2MS_case);
        double dReceivedPower_Linear_2 = dReceivedPower_BTS2MS_case_Linear + dReceivedPower_RIS_Linear_2;
        double dReceivedPower_2 = L2DB(dReceivedPower_Linear_2);
        if(dReceivedPower_2>dBest_ReceivedPower_BestCase){
            dBest_ReceivedPower_BestCase=dReceivedPower_2;
            btsid_case=btsid;
            txid_BestCase=txid;
            ms.tempR_PRIS = tempR_PBTS;
            dBest_BTS_MS_Power_Case=dReceivedPower_BTS2MS_case;
            dBest_BTS_RIS_Power_Case=dReceivedPower_RIS_2;
            dBest_BS_BeamIndex_Case=lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex.first;
            dBest_UE_BeamIndex_Case=lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex.second;
            ms.m_vMainServRIS2=dBest_RISID_BTS;
            Ris_Best_BeamIndex=Ris_Best_BeamIndex_PBTS;
        }
              cout << "RIS_BestCase"<<PhaseCase<<":"
            << setw(2) << "MSID:" << setw(2) << rxid << '/' << NumOfMS
            << setw(5) << "->" << setw(5)
            << "BTSID:" << setw(2) << (bts.GetTxID()) << '/' << NumOfBTS
            << endl;
    }
    riscl_mutex.lock();
    Observer::Print("BTS_MS2") <<dBest_BTS_MS_Power_Case<<endl;
    Observer::Print("RIS_MS2") <<dBest_BTS_RIS_Power_Case<<endl;
    Observer::Print("RSRP2") <<dBest_ReceivedPower_BestCase<< endl;
    Observer::Print("RIS_LargeScale_Information2") << rxid
    <<setw(20) << txid_BestCase
    <<setw(20) << ms.m_vMainServRIS2.GetTotalIndex()
    <<setw(20) <<dBest_BS_BeamIndex_Case/Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHBSBeamNum
    <<setw(20) <<dBest_BS_BeamIndex_Case%Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHBSBeamNum
    <<setw(20) <<dBest_UE_BeamIndex_Case/Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHUEBeamNum
    <<setw(20) <<dBest_UE_BeamIndex_Case%Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHUEBeamNum
    <<setw(20) <<Ris_Best_BeamIndex/Parameters::Instance().RIS.ANALOGBEAM_CONFIG.iHRISBeamNum
    <<setw(20) <<Ris_Best_BeamIndex%Parameters::Instance().RIS.ANALOGBEAM_CONFIG.iHRISBeamNum
    <<setw(20) << dBest_ReceivedPower_BestCase<<endl;
    riscl_mutex.unlock();
    std::pair<BTSID,double> out=std::make_pair(btsid_case,dBest_ReceivedPower_BestCase);
    return out;
}
///选择最佳基站和RIS
double RISCouplingloss::InitializeRIS_CouplingLoss_linear_Highfreq(BTS& bts, MS& ms,int Bs_Beam_Index,int Ms_Beam_Index,int PhaseCase){
    double dCouplingLoss_RIS_Linear = 0.0;
    TxRxID txrxid = std::make_pair(bts.GetTxID(), ms.GetRxID());
    Tx& tx = Tx::GetTx(bts.GetTxID());
    Rx& rx = *ms.m_pRxNode;
    RISRxID best_RISRxID;
    itpp::cmat tempR_PRIS = itpp::zeros_c(bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num(), 1);
    for (int i = 0; i < bts.GetRISNum(); ++i) {
       if(Parameters::Instance().BASIC.ISubScenarioModel==301 && bts.GetTxID()>2) continue;
       RIS& ris = bts.GetRIS(i);
       int rxid = ms.GetRxID();
       RISID id = ris.GetRISID();
       int risid = id.GetTotalIndex();
       RISRxID risrxid = std::make_pair(risid, rxid);
       TxRISID txrisid = std::make_pair(bts.GetTxID(), risid);
       std::shared_ptr<AntennaPanel> _pAntennaPanel = ris.GetAntennaPointer()->GetFirstAntennaPanelPointer();
       std::shared_ptr<AntennaPanel> _txAntennaPanel= tx.GetAntennaPointer()->GetFirstAntennaPanelPointer();
       std::shared_ptr<cm::CTXRU> pTx_TXRU = _txAntennaPanel->GetFirstTXRU();
       double dEOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EOALOSRAD;
       double dAOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AOALOSRAD;
       double dEODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EODLOSRAD;
       double dAODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AODLOSRAD;
       double dElevationRAD_TxRIS_LCS = ELoSRAD_GCS2LCS(
                dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                _pAntennaPanel->GetTxRxOrientRAD(),
                _pAntennaPanel->GetMechanicalTiltRAD(),
                0);
       assert(dElevationRAD_TxRIS_LCS>=0&&M_PI>=dElevationRAD_TxRIS_LCS);

       double dAzimuthRAD_TxRIS_LCS = ALoSRAD_GCS2LCS(
                dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                _pAntennaPanel->GetTxRxOrientRAD(),
                _pAntennaPanel->GetMechanicalTiltRAD(),
                0);
       assert(dAzimuthRAD_TxRIS_LCS>=-M_PI&&M_PI>=dAzimuthRAD_TxRIS_LCS);

       double dEODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_EODLOSRAD;
       double dAODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_AODLOSRAD;
//           double random1 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;
//           double random2 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;

       double dElevationRAD_RISRx_LCS = ELoSRAD_GCS2LCS(
                dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                _pAntennaPanel->GetTxRxOrientRAD(),
                _pAntennaPanel->GetMechanicalTiltRAD(),
                0);
       assert(dElevationRAD_RISRx_LCS>=0&&M_PI>=dElevationRAD_RISRx_LCS);
       double dAzimuthRAD_RISRx_LCS = ALoSRAD_GCS2LCS(
                dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                _pAntennaPanel->GetTxRxOrientRAD(),
                _pAntennaPanel->GetMechanicalTiltRAD(),
                0);
       assert(dAzimuthRAD_RISRx_LCS>=-M_PI&&M_PI>=dAzimuthRAD_RISRx_LCS);
       double m_dH = Parameters::Instance().RIS.DHAntSpace;
       double m_dV = Parameters::Instance().RIS.DVAntSpace;
       int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.H_TXRU_DIV_NUM_PerPanel;
//            int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.V_TXRU_DIV_NUM_PerPanel;

       std::shared_ptr<cm::Antenna> pTxAntenna = tx.GetAntennaPointer();
       std::shared_ptr<cm::Antenna> pRxAntenna = rx.GetAntennaPointer();
       std::shared_ptr<cm::Antenna> pRisAntenna = ris.GetAntennaPointer();
       double dCouplingLoss_oneRIS = 0.0;
       complex<double> Alpha1Alpha2 = 0.0;
       complex<double> dCouplingLoss_RIS_temp1 = 0.0;
       double dCouplingLoss_RIS_temp2 = 0.0;
       pair<int,int> RIS_BtsBeamIndex_temp;
       int Index_tmp;
       int RIS_H_Index;
       int RIS_V_Index;
       int Ris_BeamIndex;
       int Ris_Polarize_Num =Parameters::Instance().RIS.Polarize_Num;
       //生成ris随机波束index
        int BtsBeamIndex = Bs_Beam_Index;
        complex<double> cTxAggregateGain = pTx_TXRU->CalcAggregateGain(dAODRAD_TxRIS_GCS, dEODRAD_TxRIS_GCS, BtsBeamIndex);
        std::complex<double> angle2 = std::arg(cTxAggregateGain);
        angle2=exp((-1)*angle2* M_J);
        int MsBeamIndex = Ms_Beam_Index;

        dCouplingLoss_RIS_temp1=0;
        itpp::cmat tempR = itpp::zeros_c(bts.GetRIS(0).GetAntennaPointer()->GetTotalTXRU_Num(), 1);
        for(int i =0;i<ris.GetAntennaPointer()->GetTotalTXRU_Num();i++){
            Alpha1Alpha2 = lm.m_TxRIS2CS[txrisid].m_BCS.Alpha_for_CouplingLoss(BtsBeamIndex,i)*lm.m_RISRx2CS[risrxid].m_BCS.Alpha_for_CouplingLoss(MsBeamIndex,i);
            double angle = std::arg(Alpha1Alpha2);
            std::complex<double> phase;
            switch (PhaseCase){
                //随机相位
                case 0:{
                    double randomphase = xUniform_distributems(-1, 1);
                    phase = exp(randomphase*M_PI * M_J);
                    break;
                }
                case 1:{
                    phase = angle2*exp(2.0 * M_PI * (    (i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)
                                                        -(i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)
                                                        +(i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI)
                                                        -(i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS) ) * M_J);
                    //assert(abs(mtemp-exp((-1)*angle* M_J))<0.01);
                    break;
                }
                case 2:{
                    phase = exp((-1)*angle* M_J);
//                                        Alpha1Alpha2 *= exp((-1)*0* M_J);
                    break;
                }
               case 3:{
                    if(0.0>angle&&angle>=-1.0/2.0 * M_PI){
                         phase = exp(0.0 * M_PI * M_J);
                     }else if(angle>= -M_PI&&angle<-1.0/2.0 * M_PI){
                         phase = exp((-1)*( M_PI * M_J));
                     }else if(angle>= 1.0/2.0 *M_PI&&angle< M_PI){
                         phase = exp((-1)*( M_PI * M_J));
                     }else if(angle> 0.0&&angle< 1.0/2.0 *M_PI){
                         phase = exp((-1)*( 0.0 *M_PI * M_J));
                     }
                     else if(angle==0.0){
                         phase = exp((-1)*( 0.0 *M_PI * M_J));
                     }else if(angle==M_PI){
                         phase = exp((-1)*( M_PI * M_J));
                     }else{
                         assert(false);
                     }
                     break;
                }
                //2bit量化
                case 4:{
        //                                Observer::Print("phase") <<angle<<endl;
                    if(0.0>angle&&angle>=-1.0/2.0 * M_PI){
                        phase = exp(1.0/2.0 * M_PI * M_J);
                    }else if(angle>= -M_PI&&angle<-1.0/2.0 * M_PI){
                        phase = exp( M_PI * M_J);
                    }else if(angle>=1.0/2.0 *M_PI&&angle< M_PI){
                        phase = exp((-1)*( 1.0/2.0 *M_PI * M_J));
                    }else if(angle> 0.0&&angle< 1.0/2.0 *M_PI){
                        phase = exp((-1)*( 0.0 *M_PI * M_J));
                    }
                    else if(angle==0.0){
                        phase = exp((-1)*( 0.0 *M_PI * M_J));
                    }else if(angle==M_PI){
                        phase= exp((-1)*( M_PI * M_J));
                    }else{
                        assert(false);
                    }
                    break;
                }
//                    case 5:{
//                        dElevationRAD_RISRx_LCS = (M_PI-dElevationRAD_TxRIS_LCS)+floor((dElevationRAD_RISRx_LCS-(M_PI-dElevationRAD_TxRIS_LCS-M_PI/36.0))/(M_PI/18.0))*(M_PI/18.0);
//                        dAzimuthRAD_RISRx_LCS = floor((dAzimuthRAD_RISRx_LCS-(-M_PI/36.0))/(M_PI/18.0))*(M_PI/18.0);
////                                dElevationRAD_RISRx_LCS = (M_PI-dElevationRAD_TxRIS_LCS)+floor((dElevationRAD_RISRx_LCS-(M_PI-dElevationRAD_TxRIS_LCS-M_PI/18.0))/(M_PI/9.0))*(M_PI/9.0);
////                                dAzimuthRAD_RISRx_LCS = floor((dAzimuthRAD_RISRx_LCS-(-M_PI/18.0))/(M_PI/9.0))*(M_PI/9.0);
//                        double phase =std::arg(angle2*exp(-1.0*(2.0 * M_PI * ((i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_TxRIS_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_TxRIS_LCS)+(i/H_TXRU_DIV_NUM_PerPanel) * m_dV * sin(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI)- (i%H_TXRU_DIV_NUM_PerPanel) * m_dH * cos(dElevationRAD_RISRx_LCS-1.0/2.0 *M_PI) * sin(dAzimuthRAD_RISRx_LCS)))* M_J));
//                        if(0.0>phase&&phase>=-1.0/2.0 * M_PI){
//                            Alpha1Alpha2 *= exp(1.0/2.0 * M_PI * M_J);
//                        }else if(phase>= -M_PI&&phase<-1.0/2.0 * M_PI){
//                            Alpha1Alpha2 *= exp( M_PI * M_J);
//                        }else if(phase>= 1.0/2.0 *M_PI&&phase< M_PI){
//                            Alpha1Alpha2 *= exp((-1)*( 1.0/2.0 *M_PI * M_J));
//                        }else if(phase> 0.0&&phase< 1.0/2.0 *M_PI){
//                            Alpha1Alpha2 *= exp((-1)*( 0.0 *M_PI * M_J));
//                        }
//                        else if(phase==0.0){
//                            Alpha1Alpha2 *= exp((-1)*( 0.0 *M_PI * M_J));
//                        }else if(phase==M_PI){
//                            Alpha1Alpha2 *= exp((-1)*( M_PI * M_J));
//                        }else{
//                            assert(false);
//                        }
//                        break;
//                    }
                //参数错误
                default:{
                    assert(false);
                    break;
                }
            }
            Alpha1Alpha2*=phase;
            dCouplingLoss_RIS_temp1 += Alpha1Alpha2;
            //记录相位矩阵
            tempR(i,0) = phase;
        }
        if(abs(dCouplingLoss_RIS_temp1)>dCouplingLoss_RIS_temp2){
            dCouplingLoss_RIS_temp2 = abs(dCouplingLoss_RIS_temp1);
            RIS_BtsBeamIndex_temp = make_pair(BtsBeamIndex,MsBeamIndex);
            //记录相位矩阵
            tempR_PRIS = tempR;
        }

       dCouplingLoss_oneRIS = pow(dCouplingLoss_RIS_temp2,2);
       dCouplingLoss_oneRIS *= DB2L(lm.m_TxRIS2CS[txrisid].m_BCS.m_PurePathLossDB)*DB2L(lm.m_RISRx2CS[risrxid].m_BCS.m_PurePathLossDB);

        if( dCouplingLoss_oneRIS>dCouplingLoss_RIS_Linear){
            dCouplingLoss_RIS_Linear = dCouplingLoss_oneRIS;
            lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex = RIS_BtsBeamIndex_temp;
            lm.m_TxRx2CS[txrxid].m_pSCS->tempRIS = tempR_PRIS;//记录相位矩阵
            ms.m_vMainServRIS_New = id;
        }
    }
    return dCouplingLoss_RIS_Linear;
}

///计算RIS对MS干扰
double RISCouplingloss::InitializeRIS_CouplingLoss_linear_Interference(BTS& bts,MS& ms){
    double dCouplingLoss_RIS_Linear = 0.0;
    TxRxID txrxid = std::make_pair(bts.GetTxID(), ms.GetRxID());
    Tx tx = Tx::GetTx(bts.GetTxID());
    Rx& rx = *ms.m_pRxNode;
    RISRxID best_RISRxID;
    lm.m_TxRx2CS[txrxid].m_BCS.m_couplingloss_ris.resize(Parameters::Instance().BASIC.IRISPerBTS);
    for (int i = 0; i < bts.GetRISNum(); ++i) {
           RIS& ris = bts.GetRIS(i);
           int rxid = ms.GetRxID();
           RISID id = ris.GetRISID();
           int risid = id.GetTotalIndex();
           RISRxID risrxid = std::make_pair(risid, rxid);
           TxRISID txrisid = std::make_pair(bts.GetTxID(), risid);

           std::shared_ptr<AntennaPanel> _pAntennaPanel = ris.GetAntennaPointer()->GetFirstAntennaPanelPointer();
           std::shared_ptr<AntennaPanel> _txAntennaPanel= tx.GetAntennaPointer()->GetFirstAntennaPanelPointer();
           std::shared_ptr<cm::CTXRU> pTx_TXRU = _txAntennaPanel->GetFirstTXRU();
           double dEOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EOALOSRAD;
           double dAOARAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AOALOSRAD;
           double dEODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_EODLOSRAD;
           double dAODRAD_TxRIS_GCS = lm.m_TxRIS2CS[txrisid].m_BCS.m_AODLOSRAD;
           double dElevationRAD_TxRIS_LCS = ELoSRAD_GCS2LCS(
                    dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dElevationRAD_TxRIS_LCS>=0&&M_PI>=dElevationRAD_TxRIS_LCS);

           double dAzimuthRAD_TxRIS_LCS = ALoSRAD_GCS2LCS(
                    dAOARAD_TxRIS_GCS, dEOARAD_TxRIS_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dAzimuthRAD_TxRIS_LCS>=-M_PI&&M_PI>=dAzimuthRAD_TxRIS_LCS);

           double dEODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_EODLOSRAD;
           double dAODRAD_RISRx_GCS = lm.m_RISRx2CS[risrxid].m_BCS.m_AODLOSRAD;
//           double random1 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;
//           double random2 = xUniform_distributems(-1, 1)*1.0/18.0 * M_PI;

           double dElevationRAD_RISRx_LCS = ELoSRAD_GCS2LCS(
                    dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dElevationRAD_RISRx_LCS>=0&&M_PI>=dElevationRAD_RISRx_LCS);
           double dAzimuthRAD_RISRx_LCS = ALoSRAD_GCS2LCS(
                    dAODRAD_RISRx_GCS, dEODRAD_RISRx_GCS,
                    _pAntennaPanel->GetTxRxOrientRAD(),
                    _pAntennaPanel->GetMechanicalTiltRAD(),
                    0);
           assert(dAzimuthRAD_RISRx_LCS>=-M_PI&&M_PI>=dAzimuthRAD_RISRx_LCS);
           double m_dH = Parameters::Instance().RIS.DHAntSpace;
           double m_dV = Parameters::Instance().RIS.DVAntSpace;
           int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.H_TXRU_DIV_NUM_PerPanel;
//            int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().RIS.V_TXRU_DIV_NUM_PerPanel;

           std::shared_ptr<cm::Antenna> pTxAntenna = tx.GetAntennaPointer();
           std::shared_ptr<cm::Antenna> pRxAntenna = rx.GetAntennaPointer();
           std::shared_ptr<cm::Antenna> pRisAntenna = ris.GetAntennaPointer();
           double dCouplingLoss_oneRIS = 0.0;
           complex<double> Alpha1Alpha2 = 0.0;
           complex<double> dCouplingLoss_RIS_temp1 = 0.0;
           double dCouplingLoss_RIS_temp2 = 0.0;
           pair<int,int> RIS_BtsBeamIndex_temp;
           int Index_tmp;
           int RIS_H_Index;
           int RIS_V_Index;
           int Ris_Polarize_Num =Parameters::Instance().RIS.Polarize_Num;
           int Ris_BeamIndex=pRisAntenna->GetRandomBeamIndex();
           //生成ris随机波束index
            int BtsBeamIndex = pTxAntenna->GetRandomBeamIndex();

            int MsBeamIndex = pRxAntenna->GetRandomBeamIndex();

//                            std::shared_ptr<AntennaPanel> _pBest_BS_Panel =
//                                m_TxRx2CS[txrxid].m_pSCS->m_pBest_Tx_Panel;
//                            std::shared_ptr<AntennaPanel> _pBest_UE_Panel =
//                                m_TxRx2CS[txrxid].m_pSCS->m_pBest_Rx_Panel;

//                            int BtsBeamIndex = m_TxRx2CS[txrxid].m_pSCS->GetStrongestTxBeamIndex(_pBest_BS_Panel, _pBest_UE_Panel);
            dCouplingLoss_RIS_temp1=0;
            for (int i = 0; i < ris.GetAntennaPointer()->GetTotalTXRU_Num(); i++) {

                Alpha1Alpha2 = lm.m_TxRIS2CS[txrisid].m_BCS.Alpha_for_CouplingLoss(BtsBeamIndex, i) * lm.m_RISRx2CS[risrxid].m_BCS.Alpha_for_CouplingLoss(MsBeamIndex, i);
                //                                if(PhaseCase==4&&i==1)
                //                                    Observer::Print("CouplingLoss1") << bts.GetTxID() << setw(20) << rxid << setw(20) << abs(Alpha1Alpha2) << endl;
                double angle = std::arg(Alpha1Alpha2);
                //計算該bts中所有ris的干扰
                double dAzimuthRAD_RISRx_LCS_beam = pRisAntenna->GetEscanRAD(Ris_BeamIndex);
                double dElevationRAD_RISRx_LCS_beam = pRisAntenna->GetEtiltRAD(Ris_BeamIndex);
                Index_tmp = i / Ris_Polarize_Num;
                RIS_H_Index = Index_tmp % H_TXRU_DIV_NUM_PerPanel;
                RIS_V_Index = Index_tmp / H_TXRU_DIV_NUM_PerPanel;
                std::complex<double> phase1 = exp(-2.0 * M_PI * (RIS_V_Index * m_dV * cos(dElevationRAD_RISRx_LCS_beam)
                        + RIS_H_Index * m_dH * sin(dElevationRAD_RISRx_LCS_beam) * sin(dAzimuthRAD_RISRx_LCS_beam)) * M_J);
                std::complex<double> phase2 = phase1;
                Alpha1Alpha2 *= phase2;
                dCouplingLoss_RIS_temp1 += Alpha1Alpha2;
            }

           dCouplingLoss_RIS_temp2 = abs(dCouplingLoss_RIS_temp1);
           dCouplingLoss_oneRIS = pow(dCouplingLoss_RIS_temp2,2);
           dCouplingLoss_oneRIS *= DB2L(lm.m_TxRIS2CS[txrisid].m_BCS.m_PurePathLossDB)*DB2L(lm.m_RISRx2CS[risrxid].m_BCS.m_PurePathLossDB);

            lm.m_TxRx2CS[txrxid].m_BCS.m_couplingloss_ris[i]=dCouplingLoss_oneRIS;
            dCouplingLoss_RIS_Linear+=dCouplingLoss_oneRIS;
    }
    return dCouplingLoss_RIS_Linear;
}

///对比某个指定RIS，比较两种RIS配置对MS效果
void RISCouplingloss::InitializeRIS_CouplingLoss_linear_specific_angle(BTS& bts,MS& ms,RISID risid){
    complex<double> dCouplingLoss_RIS_temp1 = 0.0;
    complex<double> dCouplingLoss_RIS_temp2 = 0.0;

    TxRxID txrxid = std::make_pair(bts.GetTxID(), ms.GetRxID());
    Tx& tx = Tx::GetTx(bts.GetTxID());
    Rx& rx = *ms.m_pRxNode;
    int rxid = ms.GetRxID();
    RIS ris=risid.GetRIS();
    double dTxPower=Parameters::Instance().Macro.DL.DMaxTxPowerDbm;
    RISRxID risrxid = std::make_pair(risid.GetTotalIndex(), rxid);
    TxRISID txrisid = std::make_pair(bts.GetTxID(), risid.GetTotalIndex());
    int BtsBeamIndex=lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex.first;
    int MsBeamIndex =lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex.second;
    complex<double> Alpha1Alpha2 = 0.0;
    for (int i = 0; i < ris.GetAntennaPointer()->GetTotalTXRU_Num(); i++) {
        Alpha1Alpha2 = lm.m_TxRIS2CS[txrisid].m_BCS.Alpha_for_CouplingLoss(BtsBeamIndex, i) * lm.m_RISRx2CS[risrxid].m_BCS.Alpha_for_CouplingLoss1(MsBeamIndex, i);
        double angle = std::arg(Alpha1Alpha2);
        complex<double> phase= exp((-1)*angle* M_J);
        Alpha1Alpha2*=sqrt(DB2L(lm.m_RISRx2CS[risrxid].m_BCS.TXRU2LL[std::make_pair(i,0)]));
        complex<double> Alpha1Alpha2_f = Alpha1Alpha2*ms.tempR_PRIS(i,0);
        complex<double> Alpha1Alpha2_n = Alpha1Alpha2*phase;
        dCouplingLoss_RIS_temp1+=Alpha1Alpha2_f;
        dCouplingLoss_RIS_temp2+=Alpha1Alpha2_n;

    }
    double dCouplingLoss1=pow(abs(dCouplingLoss_RIS_temp1),2);
    dCouplingLoss1 *= DB2L(lm.m_TxRIS2CS[txrisid].m_BCS.m_PurePathLossDB);
    double dCouplingLoss_ris1=dTxPower+L2DB(dCouplingLoss1);
    double dCouplingLoss2=pow(abs(dCouplingLoss_RIS_temp2),2);
    dCouplingLoss2 *= DB2L(lm.m_TxRIS2CS[txrisid].m_BCS.m_PurePathLossDB);
    double dCouplingLoss_ris2=dTxPower+L2DB(dCouplingLoss2);
    double dReceivedPower_BTS2MS_case = dTxPower + L2DB(lm.m_TxRx2CS[txrxid].m_pSCS->GetCouplingloss(lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex));
    double dReceivedPower_BTS2MS_case_Linear = DB2L(dReceivedPower_BTS2MS_case);
    const double T_dCouplingLoss1=dReceivedPower_BTS2MS_case_Linear+DB2L(dCouplingLoss_ris1);
    const double T_dCouplingLoss2=dReceivedPower_BTS2MS_case_Linear+DB2L(dCouplingLoss_ris2);
    riscl_mutex.lock();
    Observer::Print("BTS-MS4")<<dReceivedPower_BTS2MS_case<<endl;
    Observer::Print("RIS-MS4")<<dCouplingLoss_ris1<<endl;
    Observer::Print("RIS-MS5")<<dCouplingLoss_ris2<<endl;
    Observer::Print("RSRP4")<<L2DB(T_dCouplingLoss1)<<endl;
    Observer::Print("RSRP5")<<L2DB(T_dCouplingLoss2)<<endl;
    riscl_mutex.unlock();




}


void RISCouplingloss::InitializeRIS_CouplingLoss_linear_recal(BTS& bts,MS& ms,RISID risid){
    complex<double> dCouplingLoss_RIS_temp1 = 0.0;
    TxRxID txrxid = std::make_pair(bts.GetTxID(), ms.GetRxID());
    Tx& tx = Tx::GetTx(bts.GetTxID());
    Rx& rx = *ms.m_pRxNode;
    int rxid = ms.GetRxID();
    RIS ris=risid.GetRIS();
    double dTxPower=Parameters::Instance().Macro.DL.DMaxTxPowerDbm;
    RISRxID risrxid = std::make_pair(risid.GetTotalIndex(), rxid);
    TxRISID txrisid = std::make_pair(bts.GetTxID(), risid.GetTotalIndex());
    int BtsBeamIndex=lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex.first;
    int MsBeamIndex =lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex.second;
    complex<double> Alpha1Alpha2 = 0.0;
    for (int i = 0; i < ris.GetAntennaPointer()->GetTotalTXRU_Num(); i++) {
        Alpha1Alpha2 = lm.m_TxRIS2CS[txrisid].m_BCS.Alpha_for_CouplingLoss(BtsBeamIndex, i) * lm.m_RISRx2CS[risrxid].m_BCS.Alpha_for_CouplingLoss(MsBeamIndex, i);
        double angle = std::arg(Alpha1Alpha2);
        complex<double> phase= exp((-1)*angle* M_J);
        complex<double> Alpha1Alpha2 = Alpha1Alpha2*phase;
        dCouplingLoss_RIS_temp1+=Alpha1Alpha2;
    }
    double dCouplingLoss=pow(abs(dCouplingLoss_RIS_temp1),2);
    dCouplingLoss *= DB2L(lm.m_TxRIS2CS[txrisid].m_BCS.m_PurePathLossDB)*DB2L(lm.m_RISRx2CS[risrxid].m_BCS.m_PurePathLossDB);
    double dCouplingLoss_ris=dTxPower+L2DB(dCouplingLoss);
    double dReceivedPower_BTS2MS_case = dTxPower + L2DB(lm.m_TxRx2CS[txrxid].m_pSCS->GetCouplingloss(lm.m_TxRx2CS[txrxid].m_BCS.RIS_BtsBeamIndex));
    double dReceivedPower_BTS2MS_case_Linear = DB2L(dReceivedPower_BTS2MS_case);
    const double T_dCouplingLoss=dReceivedPower_BTS2MS_case_Linear+DB2L(dCouplingLoss_ris);
    Observer::Print("RSRP_S")<<L2DB(T_dCouplingLoss)<<endl;
}

///扫描这个 BS 下的所有 RIS，计算每个 RIS 到 MS 的距离，找到距离最小的RIS
RIS& RISCouplingloss::Get_mind_RIS(BTS& bts,MS& ms){
    double RIS_MS_dismin=3*Parameters::Instance().Macro.DSiteDistanceM;
    int i_;
    for (int i = 0; i < Parameters::Instance().BASIC.IRISPerBTS; ++i) {
        RIS& ris = bts.GetRIS(i);
        double RIS_MS_dis=Distance(ris,ms);
        if(RIS_MS_dis<RIS_MS_dismin){
            RIS_MS_dismin=RIS_MS_dis;
            i_=i;
        }
    }
    RIS& ris = bts.GetRIS(i_);
    return ris;

}

void RISCouplingloss::waterfilling(std::vector<std::pair<double,double> > &a)
{
    int K=a.size();
    int P=1;
    double allo_set=P;
    double MAX=a[0].first;
    double ii=0;
    double sum=0.0;
    for (int i=0;i<K;++i)
    {
        allo_set+=a[i].first;
        if(a[i].first>MAX) {
            MAX = a[i].first;
            ii = i;
        }
    }
    double level=allo_set/K;
    while(MAX>=level)
    {
        MAX=-1;
        a[ii].second=0;
        level=(level*K-a[ii].first)/K;
        a[ii].first=-1;
        for (int i=0;i<K;++i)
        {
            if(a[i].first>MAX) {
                MAX = a[i].first;
                ii = i;
            }
        }
        if(MAX==-1) {
            a[ii].second = 1;
            break;
        }


    }
    for (int i=0;i<K;++i)
    {
        if(a[i].first!=-1)
            a[i].second=level-a[i].first;
        sum+=a[i].second;

    }
    for (int i=0;i<K;++i)
    {
        if(a[i].first!=-1&&a[i].second!=0)
            a[i].second=a[i].second/sum;
    }
//    cout<<a[0].second<<endl;
//    cout<<a[1].second<<endl;
}

RISCouplingloss& RISCouplingloss::Instance() {
    if (m_pRC == 0) {
        m_pRC = new RISCouplingloss;
    }
    return *m_pRC;
}