///@file ChannelState.cpp
///@brief  ChannelState类定义
///@author wangfei
//#include <boost/smart_ptr/make_shared.hpp>
#include <boost/make_shared.hpp>
#include "P.h"
#include "SpaceChannelState.h"
#include "ChannelState.h"
#include "Tx.h"
#include "Rx.h"
#include <vector>
#include "../Statistician/ChannelInfo.h"
#include "./functions.h"
#include "PathState.h"
boost::mutex csmtx;
using namespace cm;
///构造函数

ChannelState::ChannelState() {
    m_bIsStrong = false;
    m_pSCS = std::shared_ptr<SpaceChannelState > (nullptr);
}

void ChannelState::Initialize(Tx& _tx, Rx& _rx) {

    m_BCS.Initialize(_tx, _rx);

    //在这里初始化所有ChannelInfo中的容器大小，这里全部使用的是BCS的信息，而BCS已经完成了初始化
    Tx* pTx = this->m_BCS.m_pTx;
    Rx* pRx = this->m_BCS.m_pRx;
// delete channelinfo b
//    if (!ChannelInfo::isOver) {
//
//        //面板，端口，阵子邓信息在Tx/Rx的构造函数中完成了初始化
//        ChannelInfo& ci = ChannelInfo::Instance();
//        //获取收发端TXRU数目，只考虑第一个面板
//        TxRx_Pair& tx_rx = ci.H_TX_RX[pTx->GetTxID()][pRx->GetRxID()];
//        tx_rx.Tx_ID = pTx->GetTxID();
//        tx_rx.Rx_ID = pRx->GetRxID();
//        tx_rx.Tx_TXRU_Num = pTx->GetAntennaPointer()->GetFirstAntennaPanelPointer()->GetTXRU_Num();
//        tx_rx.Rx_TXRU_Num = pRx->GetAntennaPointer()->GetFirstAntennaPanelPointer()->GetTXRU_Num();
//
//        tx_rx.H_TXRU_List.clear();
//        tx_rx.H_TXRU_List.resize(tx_rx.Tx_TXRU_Num, vector<AntPair_List_per_TXRUPair>(tx_rx.Rx_TXRU_Num));
//
//        tx_rx.IsLOS = m_BCS.m_bIsLOS;
//        tx_rx.PL = sqrt(DB2L(m_BCS.m_PurePathLossDB));
//        tx_rx.K_R = DB2L(m_BCS.m_KFactorDB);
//        tx_rx.N = m_BCS.m_iNumOfPath;
//        tx_rx.path_delay_list.clear();
//        tx_rx.path_delay_list.resize(tx_rx.N);
//        tx_rx.path_powerPerRay_list.clear();
//        tx_rx.path_powerPerRay_list.resize(tx_rx.N);
//        //TODO初始化path_delay_list和path_power_list
//
//
//        for (int tx_TXRU_Index = 0; tx_TXRU_Index < tx_rx.Tx_TXRU_Num; tx_TXRU_Index++) {
//            for (int rx_TXRU_Index = 0; rx_TXRU_Index < tx_rx.Rx_TXRU_Num; rx_TXRU_Index++) {
//                AntPair_List_per_TXRUPair& antPairs = tx_rx.H_TXRU_List[tx_TXRU_Index][rx_TXRU_Index];
//                antPairs.Tx_TXRU_Index = tx_TXRU_Index;
//                antPairs.Rx_TXRU_Index = rx_TXRU_Index;
//                antPairs.Tx_Ant_Num = pTx->GetAntennaPointer()->GetFirstAntennaPanelPointer()->GetvTXRUs()[tx_TXRU_Index]->Get_Total_AntNumPerTXRU();
//                antPairs.Rx_Ant_Num = pRx->GetAntennaPointer()->GetFirstAntennaPanelPointer()->GetvTXRUs()[rx_TXRU_Index]->Get_Total_AntNumPerTXRU();
//                //vector<vector<AntPair> > H_Ant(antPairs.Tx_Ant_Num, vector<AntPair>(antPairs.Rx_Ant_Num));
//                antPairs.H_Ant.clear();
//                antPairs.H_Ant.resize(antPairs.Tx_Ant_Num, vector<AntPair>(antPairs.Rx_Ant_Num));
//                for (int s = 0; s < antPairs.Tx_Ant_Num; s++) {
//                    for (int u = 0; u < antPairs.Rx_Ant_Num; u++) {
//                        AntPair& antPair = antPairs.H_Ant[s][u];
//                        antPair.s = s;
//                        antPair.u = u;
//                        antPair.N = m_BCS.m_iNumOfPath;
//                        antPair.M = P::s().NumOfRayPerCluster;
//
//                        antPair.A.clear();
//                        antPair.A.resize(antPair.N, vector<Complex>(antPair.M));
//                        antPair.B.clear();
//                        antPair.B.resize(antPair.N, vector<Complex>(antPair.M));
//                    }
//                }
//            }
//        }
//    }
}

void ChannelState::Initialize(Tx& _tx, RIS& _ris) {
    m_BCS.SetLinkCategory_as_BS2RIS();
    Rx& _rx = _ris.GetRx();
    m_BCS.Initialize(_tx, _rx);
}


void ChannelState::Initialize(RIS& _ris, Rx& _rx) {
    m_BCS.SetLinkCategory_as_RIS2MS();
    Tx& _tx = _ris.GetTx();
    m_BCS.Initialize(_tx, _rx);
}

void ChannelState::WorkSlot(double _dTimeSec) const{
    if (m_bIsStrong) {
        m_pSCS->WorkSlot(_dTimeSec);
        /*
                }
         */
    }
}

void ChannelState::CalH() {
    m_pSCS->CalH();
}
void ChannelState::CalH(pair<int, int> beampair) {
    m_pSCS->CalH(beampair);
}
//void ChannelState::InitAllSCM() {
//    assert(!m_bIsStrong);
//    m_pSCS = std::shared_ptr<SpaceChannelState > (new SpaceChannelState(&m_BCS));
//    m_pSCS->Initialize();
//}

void ChannelState::InitStrongSCM() {
    if (m_bIsStrong) {
        m_pSCS = std::make_shared<SpaceChannelState > (&m_BCS);
        m_pSCS->Initialize();
    }
}

void ChannelState::SetStrong() {
//    assert(!m_bIsStrong);
    m_bIsStrong = true;
}
void ChannelState::InitSmallScale(){

    if(!SCSfinished){
        csmtx.lock();
        SCSfinished=1;
        csmtx.unlock();
        InitStrongSCM();    //初始化小尺度信道参数
        SetStrong();        //设置强链路
        CalH();             //计算H
    }

}