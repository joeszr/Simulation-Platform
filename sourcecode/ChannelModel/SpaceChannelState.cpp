/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file SpaceChannelState.cpp
///@brief  SpaceChannelState类定义
///
///包括构造函数，析构函数，空时信道的初始化和每个时隙的更新，初始化每条径的延时和AOD/AOA的函数
///以及子载波上快衰信道的更新函数的定义
///
///@author wangfei
#include "P.h"
#include "BasicChannelState.h"
#include "./Point.h"
#include "LinkMatrix.h"
#include "AntennaOrientGain.h"
#include "PathState.h"
#include "SubpathState.h"
#include "./Rx.h"
#include "./functions.h"
#include "SpaceChannelState.h"
#include "Tx.h"
#include "Antenna.h"
#include "AntennaPanel.h"
#include "CTXRU.h"
#include "../NetworkDrive/Clock.h"
using namespace cm;

///构造函数
//202601
SpaceChannelState::SpaceChannelState(BasicChannelState* _pBCS) {
    m_pBCS = _pBCS;

    m_vPath.resize(m_pBCS->m_iNumOfPath, PathState());
    for (unsigned i = 0; i < m_vPath.size(); ++i) {
        m_vPath[i].m_pBCS = _pBCS;

        m_vPath[i].Initialize();

        m_vPath[i].m_Path_Index = i;
    }

    m_vPath[0].SetFirstPath(true);


    m_dStrongestCouplingLoss_Linear = 0;

    //正常设置
    m_H_updated_period_ms = Parameters::Instance().LINK_CTRL.Islot4Hupdate*Parameters::Instance().BASIC.DSlotDuration_ms; //

    // 20180419
    m_LastUpdateTime_ms = -m_H_updated_period_ms;

    std::shared_ptr<cm::Antenna> pBSAntenna
            = m_pBCS->m_pTx->GetAntennaPointer();

    std::shared_ptr<cm::Antenna> pUEAntenna
            = m_pBCS->m_pRx->GetAntennaPointer();

    std::shared_ptr<cm::CTXRU> pBS_firstTXRU =
            pBSAntenna->GetFirstAntennaPanelPointer()->GetFirstTXRU();

    std::shared_ptr<cm::CTXRU> pUE_firstTXRU =
            pUEAntenna->GetFirstAntennaPanelPointer()->GetFirstTXRU();

//    m_TXRUPairID_2_FreqH.resize(
//            P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace,
//            CTXRUPairMatrix_per_subcarrier(
//            pBSAntenna->GetTotalTXRU_Num(),
//            pUEAntenna->GetTotalTXRU_Num(),
//            1,
//            1)
//            );
    m_TXRUPairID_2_FreqH_Matrix.resize(P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace,
                                itpp::cmat(pUEAntenna->GetTotalTXRU_Num(), pBSAntenna->GetTotalTXRU_Num()));
}

SpaceChannelState::~SpaceChannelState() {
    m_pBCS = nullptr;
}

//LinkMatrix::Initialize(_rx) -> m_BCS.Initialize(_tx, _rx) 

void SpaceChannelState::Initialize() {
    //////////////////step5//////////////////
    InitializePathDelay();
    //////////////////step6//////////////////
    InitializePathPower();

    //////////////////step7//////////////////
    InitializeAOD(ITU());
    InitializeAOA(ITU());
    InitializeEOD(ITU());
    InitializeEOA();
    switch (P::s().IChannelModel_for_Scenario) {
        case P::UMI:
        case P::UMA:
        case P::RMA:
        case P::InH:
        case P::_5GCM_TWO_LAYER:
            //InitializeEOD(ITU());
            //InitializeEOA();
            break;
        default:
            assert(false);
            break;
    }

    for (unsigned i = 0; i < m_vPath.size(); ++i) {
        //m_vPath[i].Initialize(ITU(), i);
        /////////Path初始化////////////////
        step8(i);
        step9(i);
        step10(i);
        step11_A(i);
        step11_B(i);

    }
    
    Tx* pTx = this->m_pBCS->m_pTx;
    Rx* pRx = this->m_pBCS->m_pRx;
    // delete channelinfo b
//    //面板，端口，阵子等信息在Tx/Rx的构造函数中完成了初始化
//    if (!ChannelInfo::isOver) {
//        ChannelInfo& ci = ChannelInfo::Instance();
//        //获取收发端TXRU数目，只考虑第一个面板
//        TxRx_Pair& tx_rx = ci.H_TX_RX[pTx->GetTxID()][pRx->GetRxID()];
//        for (int i = 0; i < tx_rx.N; i++) {
//            tx_rx.path_delay_list[i] = m_vPath[i].m_DelayQua;
//            //      void PathState::Init_Step9() {
//            //          m_PowerPerRay = sqrt(m_PowerNlos / P::s().NumOfRayPerCluster);//m_PowerNlos在InitializePathPower()完成赋值
//            //          ....
//            //      }
//            tx_rx.path_powerPerRay_list[i] = m_vPath[i].m_PowerPerRay;
//        }
//    }

    //CalculateRSRP_new();
    //20250114
    if(m_pBCS->m_iLinkCategory==0)
        CalculateRSRP_new();
    else if(m_pBCS->m_iLinkCategory==1||m_pBCS->m_iLinkCategory==2){
        CalculateRSRP_RIS();
    }

    //chty 1111 b
    m_vdTempFromDelayQua.resize(m_vPath.size(),vector<std::complex<double>>(P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace));
    for(auto n=0;n<m_vdTempFromDelayQua.size();n++){
        for(auto k=0;k<P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace;k++){
            m_vdTempFromDelayQua[n][k]= exp(-1.0 * M_J * 2.0 * M_PI * (double) (k * P::s().FX.ICarrierSampleSpace) * P::s().FX.DCarrierWidthHz * m_vPath[n].m_DelayQua);
//            m_vdTempFromDelayQua[n][k]=  m_vPath[n].m_DelayQua;
        }
    }
    m_vH.resize(Parameters::Instance().BASIC.ISCNum/ cm::P::s().FX.ICarrierSampleSpace);
    m_vD.resize(Parameters::Instance().BASIC.ISCNum/ cm::P::s().FX.ICarrierSampleSpace);
    m_vbIsLatest.resize(Parameters::Instance().BASIC.ISCNum/ cm::P::s().FX.ICarrierSampleSpace);
    //chty 1111 e
    if (Parameters::Instance().BASIC.DWorkingMode == Parameters::WorkingMode_Normal)
    {
        double time = Clock::Instance().GetTimeSec();
        UpdateH(time); //-> CalcFreqH -> m_vPath[n].CalcPath_TimeH_for_TXRUPair(pBS_TXRU, pUE_TXRU, j, i, _dTimeSec)
        //-> m_vSubpath[i].CalcSubpath_TimeH_for_TXRUPair(_pBS_TXRU, _pUE_TXRU,_AntIndex_in_BS_TXRU, _AntIndex_in_UE_TXRU, _time_s)
        //当UpdateH调用完成之后，所有的信道信息都已经完成初始化，而CalcFreqH(_dTimeSec)在WorkSlot(_dTimeSec)中被调用，用于计算h(t)
        //而ChannelInfo并不需要t，到此完成所有信道信息的统计，可以进行输出
    }
}
//20260115
void SpaceChannelState::CalH(){
    pair<int, int> beampair;
    if(m_pBCS->m_iLinkCategory==1){
        std::shared_ptr<cm::Antenna> pBSAntenna = m_pBCS->m_pTx->GetAntennaPointer();
        for(int BS_V_BeamIndex = 0; BS_V_BeamIndex < pBSAntenna->Get_V_BeamNum(); ++BS_V_BeamIndex)
        {
            for(int BS_H_BeamIndex = 0; BS_H_BeamIndex < pBSAntenna->Get_H_BeamNum(); ++BS_H_BeamIndex)
            {
                int BSBeamIndex = pBSAntenna->Get_CombBeamIndex(BS_V_BeamIndex, BS_H_BeamIndex);
                beampair = make_pair(BSBeamIndex,0);
                UpdateFreqH(0.0, beampair);//这里要计算多个beam的信道，如果使用UpdateH，在第一次计算后会更新m_LastUpdateTime_ms，导致后续beam无法计算
            }
        }
    }
    else if (m_pBCS->m_iLinkCategory == 2) {
        beampair = std::make_pair(
            0,
            GetStrongestUEBeamIndex(m_pBest_BS_Panel, m_pBest_UE_Panel)
        );
        UpdateH(0.0, beampair);
    }
}
//上面的（0，0）可能不太对
void SpaceChannelState::CalH(pair<int, int> beampair){
    UpdateH(0.0, beampair);
}

void SpaceChannelState::WorkSlot(double _dTimeSec) {

    if (Parameters::Instance().BASIC.DWorkingMode
            == Parameters::WorkingMode_Normal) {

        UpdateH(_dTimeSec);
    }
}

//增加RIS的WorkSlot
void SpaceChannelState::WorkSlot_RIS(double _dTimeSec, pair<int, int> beampair) {
    //更新信道
    if(Clock::Instance().GetTimeSlot()>0&&Parameters::Instance().RIS.IS_SmallScale ==true)
        UpdateH_RIS(_dTimeSec, beampair);
    else
        UpdateH(_dTimeSec, beampair);
}

///@brief Initialize the delay for each path.

void SpaceChannelState::InitializePathDelay() {

    std::vector <double> vDelay(m_pBCS->m_iNumOfPath);

    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        vDelay[i] = -1 * m_pBCS->m_delayScaling * m_pBCS->m_DelaySpread * log(random.xUniform_channel());
    }

    std::sort(vDelay.begin(), vDelay.end());

    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        m_vPath[i].m_DelayRaw = vDelay[i] - vDelay[0];
    }

    double Tc = 1.0 / P::s().FX.DSampleRateHz;
    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        m_vPath[i].m_DelayQua = (Tc / 16) * floor(m_vPath[i].m_DelayRaw / (Tc / 16.0) + 0.5);
    }
}
///@brief Initialize the power for each path.

void SpaceChannelState::InitializePathPower() {

    m_pBCS->m_iNumOfPath = static_cast<int> (m_vPath.size());
    std::vector <double> vPower(m_pBCS->m_iNumOfPath);

    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        vPower[i] = exp(-1.0 * m_vPath[i].m_DelayRaw * (m_pBCS->m_delayScaling - 1.0) / (m_pBCS->m_delayScaling * m_pBCS->m_DelaySpread)) * DB2L(-1.0 * random.xNormal_channel(0, m_pBCS->m_sigma));
    }

    double sum = std::accumulate(vPower.begin(), vPower.end(), 0.0);

    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        m_vPath[i].m_Power = vPower[i] / sum;
        m_vPath[i].m_PowerNlos = m_vPath[i].m_Power;
    }

    //LOS
    double k = DB2L(m_pBCS->m_KFactorDB);
    //    if ((P::s().IEnvironmentType == P::ENVIRONMENTTYPE_ITU_URBAN_MICRO || P::s().IEnvironmentType == P::ENVIRONMENTTYPE_ITU_URBAN_MACRO) && m_pBCS->m_pRx->GetSpecial() == 1) {
    //        //do nothing
    //    } else if (P::s().IEnvironmentType == P::ENVIRONMENTTYPE_HETNET && P::s().IHetnetType == P::IHetnet_Cluster && m_pBCS->m_pRx->GetSpecial() == 1) {
    //        //do nothing!
    //    } else
    if (m_pBCS->m_bIsLOS) {
        double D = 0.7705 - 0.0433 * m_pBCS->m_KFactorDB + 0.0002 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.000017 * pow(m_pBCS->m_KFactorDB, 3.0);
        for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
            m_vPath[i].m_Power *= 1 / (1 + k);
            m_vPath[i].m_DelayQua /= D;
        }
        m_vPath[0].m_Power += k / (1 + k);
    }

}
///@brief Initialize the angle of departure for ITU implementation

void SpaceChannelState::InitializeAOD(ITU) {
    double dMaxPathPower = 0;
    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        if (m_vPath[i].m_Power > dMaxPathPower) {
            dMaxPathPower = m_vPath[i].m_Power;
        }
    }

    double C = m_pBCS->m_C_Azimuth;
    switch (P::s().IChannelModel_for_Scenario) {
        case P::InH:
            if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA) {
                if (m_pBCS->m_bIsLOS) {
                    C *= (0.9275 + 0.0439 * m_pBCS->m_KFactorDB - 0.0071 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(m_pBCS->m_KFactorDB, 3.0));
                }

                for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
                    m_vPath[i].m_AODDeg_Path = -1 * m_pBCS->m_AODSpreadDeg * std::log(m_vPath[i].m_Power / dMaxPathPower) / C;
                }
            } else if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB) {
                if (m_pBCS->m_bIsLOS) {
                    C *= (1.1035 - 0.028 * m_pBCS->m_KFactorDB - 0.002 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0001 * pow(m_pBCS->m_KFactorDB, 3.0));
                }
                for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
                    m_vPath[i].m_AODDeg_Path = 2 * m_pBCS->m_AODSpreadDeg / 1.4 * sqrt(-1 * std::log(m_vPath[i].m_Power / dMaxPathPower)) / C;
                }
            }

            break;
        case P::UMI:
        case P::UMA:
        case P::RMA:
        case P::_5GCM_TWO_LAYER:
            if (m_pBCS->m_bIsLOS) {
                C *= (1.1035 - 0.028 * m_pBCS->m_KFactorDB - 0.002 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0001 * pow(m_pBCS->m_KFactorDB, 3.0));
            }
            for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
                m_vPath[i].m_AODDeg_Path = 2 * m_pBCS->m_AODSpreadDeg / 1.4 * sqrt(-1 * std::log(m_vPath[i].m_Power / dMaxPathPower)) / C;
            }
            break;
        default:
            assert(false);
    }

    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        m_vPath[i].m_AODDeg_Path = m_vPath[i].m_AODDeg_Path * (random.xUniform_channel() < 0.5 ? -1 : 1) + random.xNormal_channel(0, m_pBCS->m_AODSpreadDeg / 7.0) + RAD2DEG(m_pBCS->m_AODLOSRAD);
    }

    if (m_pBCS->m_bIsLOS) {
        for (int i = m_pBCS->m_iNumOfPath - 1; i >= 0; --i) {
            m_vPath[i].m_AODDeg_Path = m_vPath[i].m_AODDeg_Path - m_vPath[0].m_AODDeg_Path + RAD2DEG(m_pBCS->m_AODLOSRAD);
        }
    }
}
///@brief Initialize the angle of arrival for ITU implementation

void SpaceChannelState::InitializeAOA(ITU) {
    double dMaxPathPower = 0;
    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        if (m_vPath[i].m_Power > dMaxPathPower) {
            dMaxPathPower = m_vPath[i].m_Power;
        }
    }

    double C = m_pBCS->m_C_Azimuth;
    switch (P::s().IChannelModel_for_Scenario) {
        case P::InH:
            if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA) {
                if (m_pBCS->m_bIsLOS) {
                    C *= (0.9275 + 0.0439 * m_pBCS->m_KFactorDB - 0.0071 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(m_pBCS->m_KFactorDB, 3.0));
                }

                for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
                    m_vPath[i].m_AOADeg_Path = -1 * m_pBCS->m_AOASpreadDeg * std::log(m_vPath[i].m_Power / dMaxPathPower) / C;
                }
            } else if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB) {
                if (m_pBCS->m_bIsLOS) {
                    C *= (1.1035 - 0.028 * m_pBCS->m_KFactorDB - 0.002 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0001 * pow(m_pBCS->m_KFactorDB, 3.0));
                }
                for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
                    m_vPath[i].m_AOADeg_Path = 2 * m_pBCS->m_AOASpreadDeg / 1.4 * sqrt(-1 * std::log(m_vPath[i].m_Power / dMaxPathPower)) / C;
                }
            }

            break;
        case P::UMI:
        case P::UMA:
        case P::RMA:
        case P::_5GCM_TWO_LAYER:
            if (m_pBCS->m_bIsLOS) {
                C *= (1.1035 - 0.028 * m_pBCS->m_KFactorDB - 0.002 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0001 * pow(m_pBCS->m_KFactorDB, 3.0));
            }
            for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
                m_vPath[i].m_AOADeg_Path = 2 * m_pBCS->m_AOASpreadDeg / 1.4 * sqrt(-1 * std::log(m_vPath[i].m_Power / dMaxPathPower)) / C;
            }
            break;
        default:
            assert(false);
    }

    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        m_vPath[i].m_AOADeg_Path = m_vPath[i].m_AOADeg_Path * (random.xUniform_channel() < 0.5 ? -1 : 1) + random.xNormal_channel(0, m_pBCS->m_AOASpreadDeg / 7.0) + RAD2DEG(m_pBCS->m_AOALOSRAD);
    }

    if (m_pBCS->m_bIsLOS) {
        for (int i = m_pBCS->m_iNumOfPath - 1; i >= 0; --i) {
            m_vPath[i].m_AOADeg_Path = m_vPath[i].m_AOADeg_Path - m_vPath[0].m_AOADeg_Path + RAD2DEG(m_pBCS->m_AOALOSRAD);
        }
    }
}
///@brief Initialize the angle of departure for ITU implementation

void SpaceChannelState::InitializeEOD(ITU) {
    double dMaxPathPower = 0;
    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        if (m_vPath[i].m_Power > dMaxPathPower) {
            dMaxPathPower = m_vPath[i].m_Power;

        }
    }

    double C = m_pBCS->m_C_Elevation;
    switch (P::s().IChannelModel_for_Scenario) {
        case P::InH:
            if (m_pBCS->m_bIsLOS) {
                if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)//20170328_SZX
                    C *= (1.35 + 0.0202 * m_pBCS->m_KFactorDB - 0.0077 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(m_pBCS->m_KFactorDB, 3.0));
                else
                    C *= (1.3086 + 0.0339 * m_pBCS->m_KFactorDB - 0.0077 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(m_pBCS->m_KFactorDB, 3.0));
            }
            for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
                m_vPath[i].m_EODDeg_Path = -1 * m_pBCS->m_EODSpreadDeg * std::log(m_vPath[i].m_Power / dMaxPathPower) / C;
            }
            break;
        case P::UMI:
        case P::UMA:
        case P::RMA:
        case P::_5GCM_TWO_LAYER:
            if (m_pBCS->m_bIsLOS) {
                if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)//20170328_SZX
                    C *= (1.35 + 0.0202 * m_pBCS->m_KFactorDB - 0.0077 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(m_pBCS->m_KFactorDB, 3.0));
                else
                    C *= (1.3086 + 0.0339 * m_pBCS->m_KFactorDB - 0.0077 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(m_pBCS->m_KFactorDB, 3.0));
            }
            for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
                m_vPath[i].m_EODDeg_Path = -1 * m_pBCS->m_EODSpreadDeg * std::log(m_vPath[i].m_Power / dMaxPathPower) / C;
            }
            break;
        default:
            assert(false);
    }

    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        m_vPath[i].m_EODDeg_Path = m_vPath[i].m_EODDeg_Path * (random.xUniform_channel() < 0.5 ? -1 : 1) + random.xNormal_channel(0, m_pBCS->m_EODSpreadDeg / 7.0) + RAD2DEG(m_pBCS->m_EODOFFSETRAD) + RAD2DEG(m_pBCS->m_EODLOSRAD);
    }

    if (m_pBCS->m_bIsLOS) {
        for (int i = m_pBCS->m_iNumOfPath - 1; i >= 0; --i) {
            //            m_vPath[i].m_EODDeg = m_vPath[i].m_EODDeg - m_vPath[0].m_EODDeg + RAD2DEG(m_pBCS->m_EODOFFSETRAD) + RAD2DEG(m_pBCS->m_EODLOSRAD);

            // 20171215
            m_vPath[i].m_EODDeg_Path = m_vPath[i].m_EODDeg_Path - m_vPath[0].m_EODDeg_Path + RAD2DEG(m_pBCS->m_EODLOSRAD);
        }
    }
}
///@brief Initialize the elevation angle of arrival for ITU implementation

void SpaceChannelState::InitializeEOA() {
    double dMaxPathPower = 0;
    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        if (m_vPath[i].m_Power > dMaxPathPower) {
            dMaxPathPower = m_vPath[i].m_Power;
        }
    }

    double dmeanEOARAD;

    double C = m_pBCS->m_C_Elevation;
    switch (P::s().IChannelModel_for_Scenario) {
        case P::InH:
            if (m_pBCS->m_bIsLOS) {
                if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)//20170328_SZX
                    C *= (1.35 + 0.0202 * m_pBCS->m_KFactorDB - 0.0077 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(m_pBCS->m_KFactorDB, 3.0));
                else
                    C *= (1.3086 + 0.0339 * m_pBCS->m_KFactorDB - 0.0077 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(m_pBCS->m_KFactorDB, 3.0));
            }
            for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
                m_vPath[i].m_EOADeg_Path = -1 * m_pBCS->m_EOASpreadDeg * std::log(m_vPath[i].m_Power / dMaxPathPower) / C;
            }
            dmeanEOARAD = m_pBCS->m_EOALOSRAD;

            break;
        case P::UMI:
        case P::UMA:
        case P::RMA:
        case P::_5GCM_TWO_LAYER:

            if (m_pBCS->m_bIsLOS) {
                if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)//20170328_SZX
                    C *= (1.35 + 0.0202 * m_pBCS->m_KFactorDB - 0.0077 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(m_pBCS->m_KFactorDB, 3.0));
                else
                    C *= (1.3086 + 0.0339 * m_pBCS->m_KFactorDB - 0.0077 * pow(m_pBCS->m_KFactorDB, 2.0) + 0.0002 * pow(m_pBCS->m_KFactorDB, 3.0));
            }
            for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
                m_vPath[i].m_EOADeg_Path = -1 * m_pBCS->m_EOASpreadDeg * std::log(m_vPath[i].m_Power / dMaxPathPower) / C;
            }

            dmeanEOARAD = (m_pBCS->m_pRx->GetSpecial() == 0) ? m_pBCS->m_EOALOSRAD : (M_PI / 2);

            break;
        default:
            assert(false);
    }

    //    double dmeanEOARAD = (m_pBCS->m_pRx->GetSpecial() == 0) ? m_pBCS->m_EOALOSRAD : (M_PI / 2);

    for (int i = 0; i < m_pBCS->m_iNumOfPath; ++i) {
        m_vPath[i].m_EOADeg_Path = m_vPath[i].m_EOADeg_Path * (random.xUniform_channel() < 0.5 ? -1 : 1) + random.xNormal_channel(0, m_pBCS->m_EOASpreadDeg / 7.0) + RAD2DEG(dmeanEOARAD);
    }

    if (m_pBCS->m_bIsLOS) {
        for (int i = m_pBCS->m_iNumOfPath - 1; i >= 0; --i) {
            m_vPath[i].m_EOADeg_Path = m_vPath[i].m_EOADeg_Path - m_vPath[0].m_EOADeg_Path + RAD2DEG(dmeanEOARAD);
            // 20180104
            //            m_vPath[i].m_EOADeg_Path = m_vPath[i].m_EOADeg_Path - m_vPath[0].m_EOADeg_Path + RAD2DEG(m_pBCS->m_EOALOSRAD);
        }
    }
}

void SpaceChannelState::step8(int path_index) {
    m_vPath[path_index].Init_Step8();
}

void SpaceChannelState::step9(int path_index) {
    m_vPath[path_index].Init_Step9();
}

void SpaceChannelState::step10(int path_index) {
    int i = path_index;
    for (int j = 0; j < P::s().NumOfRayPerCluster; ++j) {
        m_vPath[i].m_vSubpath[j].Init_Step10();
    }
}

void SpaceChannelState::step11_A(int path_index) {
    int i = path_index;
    for (int j = 0; j < P::s().NumOfRayPerCluster; ++j) {
        m_vPath[i].m_vSubpath[j].Initialize_step11A();
    }
}

void SpaceChannelState::step11_B(int path_index) {

    m_vPath[path_index].Build_LOS_Subpath();

}

void SpaceChannelState::UpdateH(double _dTimeSec, pair<int, int> beampair) {
    double _dTime_ms = _dTimeSec * 1000;

    //20180419
    const double tolance = 0.001;

    if (_dTime_ms - m_LastUpdateTime_ms >= m_H_updated_period_ms - tolance) {
        //chty 1111 b
        for(auto i=0;i< m_vbIsLatest.size();i++) {
            m_vbIsLatest[i] = false;
        }
        //chty 1111 e
        CalcFreqH(_dTimeSec);
        m_LastUpdateTime_ms = _dTime_ms;
    }
}
//20260115
void SpaceChannelState::UpdateH_RIS(double _dTimeSec, pair<int, int> beampair) {
    double _dTime_ms = _dTimeSec * 1000;

    //20180419
    const double tolance = 0.001;

    if (_dTime_ms - m_LastUpdateTime_ms >= m_H_updated_period_ms - tolance) {
        CalcFreqH_RIS(_dTimeSec, beampair);
        m_LastUpdateTime_ms = _dTime_ms;
    }
}

//20260115,原地方是UpdateFreq_Ris
void SpaceChannelState::CalcFreqH_RIS(double _dTimeSec, pair<int, int> beampair) {

    int iFreSampleNum = P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace;

    vector<complex<double> > f(iFreSampleNum);
    itpp::vec t(m_pBCS->m_iNumOfPath);
    itpp::cvec h(m_pBCS->m_iNumOfPath);

    m_TXRUPairID_2_FreqH.clear();
    double dPathLoss = sqrt(DB2L(m_pBCS->m_PurePathLossDB));

    std::shared_ptr<cm::Antenna> pBSAntenna
            = m_pBCS->m_pTx->GetAntennaPointer();

    std::shared_ptr<cm::Antenna> pUEAntenna
            = m_pBCS->m_pRx->GetAntennaPointer();

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
                  pBSAntenna->GetvAntennaPanels()) {

                    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pUEAntennaPanel,
                                  pUEAntenna->GetvAntennaPanels()) {

                                    BOOST_FOREACH(std::shared_ptr<CTXRU> pBS_TXRU,
                                                  pBSAntennaPanel->GetvTXRUs()) {

                                                    BOOST_FOREACH(std::shared_ptr<CTXRU> pUE_TXRU,
                                                                  pUEAntennaPanel->GetvTXRUs()) {

                                                                    f = CalcFreqH1(_dTimeSec, pBS_TXRU, pUE_TXRU, beampair);

                                                                    pair<int, int> txrupair = make_pair(pBS_TXRU->GetTXRUIndex(), pUE_TXRU->GetTXRUIndex());
                                                                    m_TXRUPairID_2_FreqH1[txrupair].resize(iFreSampleNum);

                                                                    for (int k = 0; k < iFreSampleNum; ++k) {
                                                                        m_TXRUPairID_2_FreqH1[txrupair][k] = f[k] * dPathLoss+m_TXRUPairID_2_FreqH_RIS[txrupair][k];
                                                                    }
                                                                }
                                                }
                                }
                }
}



double SpaceChannelState::GetCouplingloss(pair<int,int> beampair) {
    double result;
    if(mBeampair2Couplingloss_Linear.find(beampair) != mBeampair2Couplingloss_Linear.end()){
        result = mBeampair2Couplingloss_Linear[beampair];
    } else {
        //2022.9.27
        //方案一：全存，方便删除弱干扰链路的多径信息
        cout << "出错！找不到该beam对应的couplingloss" << endl;
        assert(false);
        //方案二：
        //        result = CalculateRSRP_ABF(beampair);
    }
    return result;
}

int SpaceChannelState::GetStrongestBSBeamIndex(
        AntennaPanel* _pBS_Panel,
        AntennaPanel* _pUE_Panel) {

    return m_PanelPairID_2_StrongestBSBeamIndex(
            _pBS_Panel->GetPanelIndex(),
            _pUE_Panel->GetPanelIndex());
}

int SpaceChannelState::GetStrongestUEBeamIndex(
        AntennaPanel* _pBS_Panel,
        AntennaPanel* _pUE_Panel) {

    return m_PanelPairID_2_StrongestUEBeamIndex(
            _pBS_Panel->GetPanelIndex(),
            _pUE_Panel->GetPanelIndex());
}

//20180615

double SpaceChannelState::CalCouplingLoss_of_Beampair_using_36873_8_1_new(
        int BS_BeamIndex, int UE_BeamIndex,
        std::shared_ptr<cm::CTXRU>& pBS_TXRU,
        std::shared_ptr<cm::CTXRU>& pUE_TXRU
        ) {
    double dCouplingLoss = 0;
    for (int iPathIndex = 0; iPathIndex < m_pBCS->m_iNumOfPath; ++iPathIndex) {
        for (int iSubpathIndex = 0; iSubpathIndex < P::s().NumOfRayPerCluster; ++iSubpathIndex) {

            double tmpPower = m_vPath[iPathIndex].m_vSubpath[iSubpathIndex]
                    .CalSubpathCouplingLoss_of_Beampair_using_36873_8_1_new(
                    BS_BeamIndex, UE_BeamIndex,
                    pBS_TXRU, pUE_TXRU);

            dCouplingLoss += pow(m_vPath[iPathIndex].m_PowerPerRay, 2) * tmpPower;
        }
    }

    if (m_pBCS->m_bIsLOS) {
        double K = DB2L(m_pBCS->m_KFactorDB);
        dCouplingLoss *= (1.0 / (1.0 + K));

        double tmpPower = m_vPath[0].m_pLOS_Subpath
                ->CalSubpathCouplingLoss_of_Beampair_using_36873_8_1_new(
                BS_BeamIndex, UE_BeamIndex,
                pBS_TXRU, pUE_TXRU);

        dCouplingLoss += (K / (1 + K)) * tmpPower;
    }

    double dPathLoss = sqrt(DB2L(m_pBCS->m_PurePathLossDB));

    dCouplingLoss *= pow(dPathLoss, 2);

    return dCouplingLoss;
}

double SpaceChannelState::CalCouplingLoss_linear_u36873_w_BestBeamPair_4_BestPanelPair() {
    std::shared_ptr<cm::CTXRU> pBS_TXRU =
            m_pBest_BS_Panel->GetFirstTXRU();

    std::shared_ptr<cm::CTXRU> pUE_TXRU =
            m_pBest_UE_Panel->GetFirstTXRU();

    int BS_BeamIndex = GetStrongestBSBeamIndex(
            m_pBest_BS_Panel, m_pBest_UE_Panel);

    int UE_BeamIndex = GetStrongestUEBeamIndex(
            m_pBest_BS_Panel, m_pBest_UE_Panel);

    return CalCouplingLoss_of_Beampair_using_36873_8_1_new(
            BS_BeamIndex, UE_BeamIndex, pBS_TXRU, pUE_TXRU);
}

double SpaceChannelState::CalCouplingLoss_linear_u36873_w_RandomBSBeam_4_BestPanelPair() {

    std::shared_ptr<cm::CTXRU> pBS_TXRU =
            m_pBest_BS_Panel->GetFirstTXRU();

    std::shared_ptr<cm::CTXRU> pUE_TXRU =
            m_pBest_UE_Panel->GetFirstTXRU();

    int BS_BeamIndex =
            m_pBest_BS_Panel->GetFatherAntennaPointer()->GetRandomBeamIndex();

    int UE_BeamIndex = GetStrongestUEBeamIndex(
            m_pBest_BS_Panel, m_pBest_UE_Panel);

    return CalCouplingLoss_of_Beampair_using_36873_8_1_new(
            BS_BeamIndex, UE_BeamIndex, pBS_TXRU, pUE_TXRU);
}

void SpaceChannelState::CalculateRSRP_new() {
    std::shared_ptr<cm::Antenna> pBSAntenna
            = m_pBCS->m_pTx->GetAntennaPointer();

    std::shared_ptr<cm::Antenna> pUEAntenna
            = m_pBCS->m_pRx->GetAntennaPointer();

    // <_BS_TXRUIndex, _UE_TXRUIndex> --> TempD
    m_PanelPairID_2_StrongestBSBeamIndex = itpp::ones_i(
            pBSAntenna->GetTotalAntennaPanel_Num(),
            pUEAntenna->GetTotalAntennaPanel_Num()) * (-1);
    m_PanelPairID_2_StrongestUEBeamIndex = itpp::ones_i(
            pBSAntenna->GetTotalAntennaPanel_Num(),
            pUEAntenna->GetTotalAntennaPanel_Num()) * (-1);

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
            pBSAntenna->GetvAntennaPanels()) {

        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pUEAntennaPanel,
                pUEAntenna->GetvAntennaPanels()) {

            double StrongestCouplingLoss_Linear_per_PanelPair = 0;

            std::shared_ptr<cm::CTXRU> pBS_TXRU0 =
                    pBSAntennaPanel->GetFirstTXRU();
            std::shared_ptr<cm::CTXRU> pUE_TXRU0 =
                    pUEAntennaPanel->GetFirstTXRU();

            for (int BS_V_BeamIndex = 0; BS_V_BeamIndex < pBSAntenna->Get_V_BeamNum(); ++BS_V_BeamIndex) {
                for (int BS_H_BeamIndex = 0; BS_H_BeamIndex < pBSAntenna->Get_H_BeamNum(); ++BS_H_BeamIndex) {
                    for (int UE_V_BeamIndex = 0; UE_V_BeamIndex < pUEAntenna->Get_V_BeamNum(); ++UE_V_BeamIndex) {
                        for (int UE_H_BeamIndex = 0; UE_H_BeamIndex < pUEAntenna->Get_H_BeamNum(); ++UE_H_BeamIndex) {

                            // using 36.873 8.1-1
                            int BSBeamIndex = pBSAntenna->Get_CombBeamIndex(
                                    BS_V_BeamIndex, BS_H_BeamIndex);
                            int UEBeamIndex = pUEAntenna->Get_CombBeamIndex(
                                    UE_V_BeamIndex, UE_H_BeamIndex);

                            //20180625
                            double dCouplingLoss =
                                    CalCouplingLoss_of_Beampair_using_36873_8_1_new(
                                    BSBeamIndex, UEBeamIndex,
                                    pBS_TXRU0, pUE_TXRU0);
                            if (dCouplingLoss > StrongestCouplingLoss_Linear_per_PanelPair) {
                                StrongestCouplingLoss_Linear_per_PanelPair = dCouplingLoss;

                                m_PanelPairID_2_StrongestBSBeamIndex(
                                        pBSAntennaPanel->GetPanelIndex(),
                                        pUEAntennaPanel->GetPanelIndex()) = BSBeamIndex;
                                m_PanelPairID_2_StrongestUEBeamIndex(
                                        pBSAntennaPanel->GetPanelIndex(),
                                        pUEAntennaPanel->GetPanelIndex()) = UEBeamIndex;
                            }

                            if (dCouplingLoss > m_dStrongestCouplingLoss_Linear) {
                                m_dStrongestCouplingLoss_Linear = dCouplingLoss;

                                m_pBest_BS_Panel = pBSAntennaPanel.get();
                                m_pBest_UE_Panel = pUEAntennaPanel.get();
                            }
                        }
                    }
                }
            }
        }
    }
}

//itpp::cmat SpaceChannelState::GetH_for_subcarrier_and_TXRUPair(
//        int _scid,
//        std::shared_ptr<CTXRU> _pBS_TXRU,
//        std::shared_ptr<CTXRU> _pUE_TXRU) {
//
//    int _BS_TXRUIndex = _pBS_TXRU->GetTXRUIndex();
//    int _UE_TXRUIndex = _pUE_TXRU->GetTXRUIndex();
//
//    // <itpp::cmat > --> H1 dim(RxAntNum_per_TXRU, TxAntNum_per_TXRU)
//    // itpp::Mat<itpp::cmat > -->  X1 = <_BS_TXRUIndex, _UE_TXRUIndex> -> H1
//    // std::vector < itpp::Mat<itpp::cmat > > --> scid --> X1
//    return m_TXRUPairID_2_FreqH[_scid](_BS_TXRUIndex, _UE_TXRUIndex);
//}

void SpaceChannelState::UpdateFreqH(double _dTimeSec, pair<int, int> beampair) {
    //频域信道响应，子载波数量
    int iFreSampleNum = P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace;

    vector<complex<double> > f(iFreSampleNum);
    itpp::vec t(m_pBCS->m_iNumOfPath);
    itpp::cvec h(m_pBCS->m_iNumOfPath);

    m_TXRUPairID_2_FreqH.clear();
    double dPathLoss = sqrt(DB2L(m_pBCS->m_PurePathLossDB));

    std::shared_ptr<cm::Antenna> pBSAntenna
            = m_pBCS->m_pTx->GetAntennaPointer();

    std::shared_ptr<cm::Antenna> pUEAntenna
            = m_pBCS->m_pRx->GetAntennaPointer();

    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
            pBSAntenna->GetvAntennaPanels()) {

        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pUEAntennaPanel,
                pUEAntenna->GetvAntennaPanels()) {

            BOOST_FOREACH(std::shared_ptr<CTXRU> pBS_TXRU,
                    pBSAntennaPanel->GetvTXRUs()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pUE_TXRU,
                        pUEAntennaPanel->GetvTXRUs()) {

                    ///20251202
                    std::fill(f.begin(), f.end(), std::complex<double>(0.0, 0.0));

                    int iPathNum = m_pBCS->m_iNumOfPath;

                    // 2. 遍历所有路径，计算叠加
                    for (int n = 0; n < iPathNum; ++n) {
                        // 计算时域信道系数 (针对给定的 beampair)
                        std::complex<double> ChannelMat_Element =
                                m_vPath[n].CalcPath_TimeH_for_TXRUPair(
                                beampair.first,  // BS Beam Index
                                beampair.second, // UE Beam Index
                                pBS_TXRU, pUE_TXRU,
                                0, 0, // j, i (天线索引，此处均为0)
                                _dTimeSec);

                        // 转换到频域并累加
                        for (int k = 0; k < iFreSampleNum; ++k) {
                            f[k] += ChannelMat_Element * m_vdTempFromDelayQua[n][k];
                        }
                    }

                    pair<int, int> txrupair = make_pair(pBS_TXRU->GetTXRUIndex(), pUE_TXRU->GetTXRUIndex());
                    m_TXRUPairID_2_FreqH[txrupair].resize(iFreSampleNum);

                    for (int k = 0; k < iFreSampleNum; ++k) {
                        m_TXRUPairID_2_FreqH[txrupair][k] = f[k] * dPathLoss;
                    }
                    if(m_pBCS->m_iLinkCategory==1){
                        m_TXRUPairID_2_FreqH_BS2RIS[beampair.first][txrupair].resize(iFreSampleNum);
                        m_TXRUPairID_2_FreqH_BS2RIS[beampair.first][txrupair] = m_TXRUPairID_2_FreqH[txrupair];
                    }
                }
            }
        }
    }
}

std::complex<double> SpaceChannelState::GetH_after_ABF(int _scid,
        std::shared_ptr<cm::CTXRU> _pBS_TXRU,
        std::shared_ptr<cm::CTXRU> _pUE_TXRU,
        int _iBSBeamIndex, int _iUEBeamIndex) {

//    itpp::cmat BS_TXRU_Weight = _pBS_TXRU->Calc_TXRU_Weight(_iBSBeamIndex);
//    itpp::cmat UE_TXRU_Weight = _pUE_TXRU->Calc_TXRU_Weight(_iUEBeamIndex);


//    itpp::cmat CTEMP = UE_TXRU_Weight * FreqH_4_TXRUPair * BS_TXRU_Weight;


//    itpp::cmat FreqH_4_TXRUPair = GetH_for_subcarrier_and_TXRUPair(
//            _scid, _pBS_TXRU, _pUE_TXRU);
//    itpp::cmat CTEMP = FreqH_4_TXRUPair;
//    return CTEMP(0, 0);

    int _BS_TXRUIndex = _pBS_TXRU->GetTXRUIndex();
    int _UE_TXRUIndex = _pUE_TXRU->GetTXRUIndex();
    return m_TXRUPairID_2_FreqH_Matrix[_scid](_UE_TXRUIndex, _BS_TXRUIndex);
}

itpp::cmat SpaceChannelState::GetH_after_ABF_for_all_active_TXRU_Pairs_RISIntf_BestPanel(int _scid){
    itpp::cmat H_after_ABF = itpp::zeros_c(
            m_pBest_UE_Panel->GetTXRU_Num(),
            m_pBest_BS_Panel->GetTXRU_Num());
    vector< std::shared_ptr<CTXRU> > vBS_TXRUs =
            m_pBest_BS_Panel->GetvTXRUs();
    vector< std::shared_ptr<CTXRU> > vUE_TXRUs =
            m_pBest_UE_Panel->GetvTXRUs();

    for (int i = 0; i < m_pBest_UE_Panel->GetTXRU_Num(); i++) {
        std::shared_ptr<CTXRU> pUE_TXRU = vUE_TXRUs[i];

        for (int j = 0; j < m_pBest_BS_Panel->GetTXRU_Num(); j++) {
            std::shared_ptr<CTXRU> pBS_TXRU = vBS_TXRUs[j];
            //            complex<double> H_after_ABF_with_BestBeams =
            //                GetH_after_ABF_with_BestBeams(_scid, pBS_TXRU, pUE_TXRU);
            complex<double> H_after_ABF_with_BestBeams_RIS =
                GetH_after_ABF_with_BestBeams_RISonly(_scid, pBS_TXRU, pUE_TXRU);
            H_after_ABF(i, j) = H_after_ABF_with_BestBeams_RIS;

        }
    }
}

std::complex<double> SpaceChannelState::GetH_after_ABF_with_BestBeams(
        int _scid,
        std::shared_ptr<cm::CTXRU> _pBS_TXRU,
        std::shared_ptr<cm::CTXRU> _pUE_TXRU) {

    AntennaPanel* _pBS_Panel =
            _pBS_TXRU->GetFatherAntennaPanelPointer().get();
    AntennaPanel* _pUE_Panel =
            _pUE_TXRU->GetFatherAntennaPanelPointer().get();

    int StrongestBSBeamIndex = GetStrongestBSBeamIndex(
            _pBS_Panel, _pUE_Panel);
    int StrongestUEBeamIndex = GetStrongestUEBeamIndex(
            _pBS_Panel, _pUE_Panel);

    return GetH_after_ABF(_scid, _pBS_TXRU, _pUE_TXRU,
            StrongestBSBeamIndex, StrongestUEBeamIndex);

}

complex<double> SpaceChannelState::GetH_after_ABF_with_BestBeams_RISonly(
        int _scid,
        std::shared_ptr<cm::CTXRU> _pBS_TXRU,
        std::shared_ptr<cm::CTXRU> _pUE_TXRU) {

    pair<int, int> txrupair = make_pair(_pBS_TXRU->GetTXRUIndex(), _pUE_TXRU->GetTXRUIndex());
    //std::map<std::pair<int, int>, std::vector<std::complex<double> > > H = m_TXRUPairID_2_FreqH;
    complex<double> CTEMP =  m_TXRUPairID_2_FreqH_RIS[txrupair][_scid];

    return CTEMP;
}

itpp::cmat SpaceChannelState::GetH_after_ABF_for_all_active_TXRU_Pairs(
        int scid) {
    //chty 1111 b
    if(m_vbIsLatest[scid]){
        return m_vH[scid];
    }else{
        cmat c = GetH_after_ABF_for_active_TXRU_Pairs_per_PanelPair(
                scid, m_pBest_BS_Panel, m_pBest_UE_Panel);
        m_vH[scid] = c;
        m_vbIsLatest[scid] =true;

//        cmat mU, mD;
//        vec vS;
//        svd(c, mU, vS, mD);
//        m_vD[scid] = mD;

        return c;
    }
    //chty 1111 e
}
itpp::cmat SpaceChannelState::Get_mD_after_ABF_for_all_active_TXRU_Pairs(
        int scid) {
    //chty 1111 b
    assert(m_vbIsLatest[scid]);
    return m_vD[scid];
}

itpp::cmat SpaceChannelState::GetH_after_ABF_for_active_TXRU_Pairs_per_PanelPair(
        int _scid,
        AntennaPanel* _pBS_Panel,
        AntennaPanel* _pUE_Panel) {


//    itpp::cmat H_after_ABF = itpp::zeros_c(
//            _pUE_Panel->GetTXRU_Num(),
//            _pBS_Panel->GetTXRU_Num());
//
//    std::vector< std::shared_ptr<CTXRU> > vBS_TXRUs =
//            _pBS_Panel->GetvTXRUs();
//    std::vector< std::shared_ptr<CTXRU> > vUE_TXRUs =
//            _pUE_Panel->GetvTXRUs();
//
//    for (int i = 0; i < _pUE_Panel->GetTXRU_Num(); i++) {
//        std::shared_ptr<CTXRU> pUE_TXRU = vUE_TXRUs[i];
//
//        for (int j = 0; j < _pBS_Panel->GetTXRU_Num(); j++) {
//            std::shared_ptr<CTXRU> pBS_TXRU = vBS_TXRUs[j];
//
//            std::complex<double> H_after_ABF_with_BestBeams =
//                    GetH_after_ABF_with_BestBeams(_scid, pBS_TXRU, pUE_TXRU);
//
//            H_after_ABF(i, j) = H_after_ABF_with_BestBeams;
//        }
//    }
    int BS_TXRU_Begin = _pBS_Panel->GetFirstTXRU()->GetTXRUIndex();
    int BS_TXRU_End = BS_TXRU_Begin + _pBS_Panel->GetTXRU_Num() - 1;
    int UE_TXRU_Begin = _pUE_Panel->GetFirstTXRU()->GetTXRUIndex();
    int UE_TXRU_End = UE_TXRU_Begin + _pUE_Panel->GetTXRU_Num() - 1;
    itpp::cmat temp = m_TXRUPairID_2_FreqH_Matrix[_scid].get_cols(BS_TXRU_Begin, BS_TXRU_End);
    itpp::cmat H_after_ABF = temp.get_rows(UE_TXRU_Begin, UE_TXRU_End);
    return H_after_ABF;
}

//20260124
void SpaceChannelState::StrongBeam(map<pair<int, int>, double> &mBeam2Couplingloss){

    vector<pair<pair<int,int> ,double> > beam(mBeam2Couplingloss.begin(),mBeam2Couplingloss.end());
    //排序
    sort(beam.begin(),beam.end(),bigger);

    //方案二：全存
    for(vector<pair<pair<int,int> ,double> >::iterator it = beam.begin();it != beam.end(); ++it){
        mBeampair2Couplingloss_Linear.insert(*it);
    }
}

void SpaceChannelState::CalcFreqH(double _dTimeSec) {
    vector<std::complex<double>> f(P::s().FX.ICarrierNum);
    vector<double> t(m_pBCS->m_iNumOfPath);
    vector<std::complex<double>> h(m_pBCS->m_iNumOfPath);

    double dPathLoss = sqrt(DB2L(m_pBCS->m_PurePathLossDB));
    int iPathNum = m_pBCS->m_iNumOfPath;
    int iFreSampleNum = P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace;

    std::shared_ptr<cm::Antenna> pBSAntenna
            = m_pBCS->m_pTx->GetAntennaPointer();

    std::shared_ptr<cm::Antenna> pUEAntenna
            = m_pBCS->m_pRx->GetAntennaPointer();

    BOOST_FOREACH(std::shared_ptr<AntennaPanel>& pBSAntennaPanel,
            pBSAntenna->GetvAntennaPanels()) {

        BOOST_FOREACH(std::shared_ptr<AntennaPanel>& pUEAntennaPanel,
                pUEAntenna->GetvAntennaPanels()) {

            int BS_BeamIndex = m_PanelPairID_2_StrongestBSBeamIndex(pBSAntennaPanel->GetPanelIndex(),pUEAntennaPanel->GetPanelIndex());
            int UE_BeamIndex = m_PanelPairID_2_StrongestUEBeamIndex(pBSAntennaPanel->GetPanelIndex(),pUEAntennaPanel->GetPanelIndex());

            BOOST_FOREACH(std::shared_ptr<CTXRU>& pBS_TXRU,
                    pBSAntennaPanel->GetvTXRUs()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU>& pUE_TXRU,
                        pUEAntennaPanel->GetvTXRUs()) {

                    for (int i = 0; i < 1; ++i) {
                        for (int j = 0; j < 1; ++j) {
                            //chty 1115 b
//                            for (int n = 0; n < m_pBCS->m_iNumOfPath; ++n) {
                            for (int n = 0; n < iPathNum; ++n) {
                            //chty 1115 e
                                std::complex<double> ChannelMat_Element =
                                        m_vPath[n].CalcPath_TimeH_for_TXRUPair(
                                        BS_BeamIndex, UE_BeamIndex,
                                        pBS_TXRU, pUE_TXRU, j, i, _dTimeSec);

                                t[n] = m_vPath[n].m_DelayQua;
                                h[n] = ChannelMat_Element;
                            }
                            for (int k = 0; k < iFreSampleNum; ++k) {
                                f[k] = 0;
                                for (int n = 0; n < m_pBCS->m_iNumOfPath; ++n) {
                                    //chty 1111 b
//                                    f[k] += h[n] * exp(-1.0 * M_J * 2.0 * M_PI * (double) (k * P::s().FX.ICarrierSampleSpace) * P::s().FX.DCarrierWidthHz * t[n]);
                                    f[k]  += h[n] * m_vdTempFromDelayQua[n][k];
                                    //chty 1111 e
                                }
                            }
                            for (int k = 0; k < iFreSampleNum; ++k) {
                                // <itpp::cmat > --> H1 dim(RxAntNum_per_TXRU, TxAntNum_per_TXRU)
                                // itpp::Mat<itpp::cmat > -->  X1 = <_BS_TXRUIndex, _UE_TXRUIndex> -> H1   
                                // std::vector < itpp::Mat<itpp::cmat > > --> scid --> X1                               
//                                m_TXRUPairID_2_FreqH[k]
//                                        (pBS_TXRU->GetTXRUIndex(), pUE_TXRU->GetTXRUIndex())
//                                        (i, j) = f[k] * dPathLoss;
                                m_TXRUPairID_2_FreqH_Matrix[k](pUE_TXRU->GetTXRUIndex(), pBS_TXRU->GetTXRUIndex())
                                         = f[k] * dPathLoss;
                            }
                        }
                    }
                    //Observer::Print("AAAAAAAA") << P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace <<"\t"<< pBS_TXRU->GetTXRUIndex() << "\t" << pUE_TXRU->GetTXRUIndex() << "\t" << endl;

                }
            }
        }
    }
}

//20260115
vector<complex<double> > SpaceChannelState::CalcFreqH1(double _dTimeSec,
                                                      std::shared_ptr<cm::CTXRU> _pBS_TXRU,
                                                      std::shared_ptr<cm::CTXRU> _pUE_TXRU, pair<int,int> beampair){

    int iFreSampleNum = P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace;

    vector<complex<double> > f(iFreSampleNum);
    vector<double> t(m_pBCS->m_iNumOfPath);
    itpp::cvec h(m_pBCS->m_iNumOfPath);

    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 1; ++j) {
            for (int n = 0; n < m_pBCS->m_iNumOfPath; ++n) {
                complex<double> ChannelMat_Element =
                        m_vPath[n].CalcPath_TimeH_for_TXRUPair(
                                beampair.first, beampair.second,
                                _pBS_TXRU, _pUE_TXRU,j,i, _dTimeSec);

                t[n] = m_vPath[n].m_DelayQua;
                h[n] = ChannelMat_Element;
                //Observer::Print("h")<<h[n]<<endl;
            }
        }
    }
    for (int k = 0; k < iFreSampleNum; ++k) {
        f[k] = 0;
        for (int n = 0; n < m_pBCS->m_iNumOfPath; ++n) {
            f[k] += h[n] *
                    exp(-1.0 * M_J * 2.0 * M_PI * (double) (k * P::s().FX.ICarrierSampleSpace) *
                        P::s().FX.DCarrierWidthHz * t[n]);
        }
        //Observer::Print("f")<<f[k]<<endl;
    }


    return f;
}

//chty 1111 b
void SpaceChannelState::UpdateH4AllSC() {
    for(auto scid=0; scid<m_vH.size();scid++){
        m_vH[scid] = GetH_after_ABF_for_active_TXRU_Pairs_per_PanelPair(
                scid, m_pBest_BS_Panel, m_pBest_UE_Panel);
    }
}
//chty 1111 e


