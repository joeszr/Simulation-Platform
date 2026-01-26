///@file LinkMatrix.h
///@brief 定义channelmodel的接口类的函数
///@author wangfei

#include "P.h"
#include "AOGOmni.h"
#include "GaussianMap.h"
#include "BasicChannelState.h"
#include "SpaceChannelState.h"
#include "ChannelState.h"
#include "PathLossIndoorFactoryLOS.h"
#include "PathLossIndoorHotspotLOS.h"
#include "PathLossIndoorHotspotLOS_Highfreq.h"
#include "PathLossUrbanMicroLOS.h"
#include "PathLossUrbanMicroLOS_Highfreq.h"
#include "PathLossUrbanMacroLOS.h"
#include "PathLossUrbanMacroLOS_Highfreq.h"
#include "PathLossRuralMacroLOS.h"
#include "PathLossIndoorFactoryDHNLOS.h"
#include "PathLossIndoorHotspotNLOS.h"
#include "PathLossIndoorHotspotNLOS_Highfreq.h"
#include "PathLossUrbanMicroNLOS.h"
#include "PathLossUrbanMicroNLOS_Highfreq.h"
#include "PathLossUrbanMacroNLOS.h"
#include "PathLossUrbanMacroNLOS_Highfreq.h"
#include "PathLossRuralMacroNLOS.h"
#include "PathLossRuralMacroLOS_Highfreq.h"
#include "PathLossRuralMacroNLOS_Highfreq.h"
#include "PathLossRuralMacroO2I_Highfreq.h"
#include "PathLossUrbanMicroO2I.h"
#include "PathLossUrbanMicroO2I_Highfreq.h"
#include "PathLossUrbanMacroO2I.h"
#include "PathLossUrbanMacroO2I_Highfreq.h"
#include "PathLossRuralMacroO2I.h"
#include "PathLossPico2UE_UrbanMicroLOS.h"
#include "PathLossPico2UE_UrbanMicroNLOS.h"
#include "PathLossPico2UE_UrbanMicroO2I.h"
#include "Rx.h"
#include "Tx.h"
#include "LinkMatrix.h"
#include "../Parameters/Parameters.h"
#include "PathState.h"
#include "SubpathState.h"
#include "../RIS/RISMatrix.h"
///相位计算
//#include "RISCouplingloss.h"
#include "CTXRU.h"
#include <sys/time.h>
#include"./functions.h"
std::mutex LinkMatrix_Initialize_mutex;
using namespace cm;
//@threads
#include <thread>
#include <mutex>
unsigned int LinkMatrix::threadnum = 2*sysconf(_SC_NPROCESSORS_CONF);
//@threads
//extern double xUniform_channel(double _dmin, double _dmax);
//extern double xNormal_channel(double _dave, double _dstd);

LinkMatrix* LinkMatrix::m_pLM = nullptr;

void LinkMatrix::Reset() {
    m_TxRx2CS.clear();
    m_TxRIS2CS.clear();
    m_RISRx2CS.clear();
    m_Pos2Din.clear();
}
///初始化函数清空之前的数据
void LinkMatrix::Initialize() {
    m_Pos2Din.clear();
    m_Pos2PentratinlossSF.clear();
    BasicChannelState::InitializeMap();
    // delete channelinfo b
    //20200413
//    if (!ChannelInfo::isOver) {
//        ChannelInfo& ci = ChannelInfo::Instance();
//        ci.Tx_Num = Tx::CountTx();
//        ci.Rx_Num = Rx::CountRx();
//        ci.resize(); //根据最新的Tx_Num和Rx_Num设置内部各种容器的大小
//        //设定Txs和Rxs
//        for (int i = 0; i < ci.Tx_Num; i++) {
//            Tx& tx = Tx::GetTx(i);
//            ci.Txs[i].ID = tx.GetTxID();
//            ci.Txs[i].X = tx.GetX();
//            ci.Txs[i].Y = tx.GetY();
//            ci.Txs[i].Height = tx.GetTxHeightM();
//            ci.Txs[i].AntNum = tx.GetAntNum();
//        }
//        for (int i = 0; i < ci.Rx_Num; i++) {
//            Rx& rx = Rx::GetRx(i);
//            ci.Rxs[i].ID = rx.GetRxID();
//            ci.Rxs[i].X = rx.GetX();
//            ci.Rxs[i].Y = rx.GetY();
//            ci.Rxs[i].Height = rx.GetRxHeightM();
//            ci.Rxs[i].AntNum = rx.GetAntNum();
//        }
//    }
    //@threads
//    if(Parameters::Instance().BASIC.BISMultiThread){
//        vector<std::thread> group;
//        for (int i = 0; i < threadnum; ++i) {
//            Rx& rx = Rx::GetRx(i);
//            //@threads
////        Initialize(rx);
//            group.emplace_back(std::thread(bind(&LinkMatrix::initialthread,this,i)));
//        }
//        //@threads
//        for (auto& onethread : group) {
//            onethread.join();
//        }
//    }
//    else{
        for (int i = 0; i < Rx::CountRx(); ++i) {
            Rx& rx = Rx::GetRx(i);
             Initialize(rx);
        }
//    }

    //@threads
// delete channelinfo b
//    if (!ChannelInfo::isOver) {
//        //此时所有的TxRx对的信道信息的初始化已经完成，而ChannelInfo并不需要t，而WorkSlot中才会更新t，计算h(t)
//        //此时可以对ChannelInfo进行输出
//        //校验 ChannelInfo::Instance().verifyFunction()
//        Observer::SetIsEnable(true);
//        ofstream& out = Observer::Print("channelInfo");
//        out << ChannelInfo::Instance() << endl;
//        out.close();
//        Observer::SetIsEnable(false);
//        ChannelInfo::isOver = true;
//    }
//
}
///初始化单个接收端信道
void LinkMatrix::Initialize(Rx& _rx) {
    int rxid = _rx.GetRxID(); //注意：下面两种方法二选一，获取接收端ID
    int NumOfRX = Rx::CountRx();
    int NumOfTX = Tx::CountTx();
    //初始化基站到RIS的信道

    for (int i = 0; i < Tx::CountTx(); ++i) {
        Tx& tx = Tx::GetTx(i);
        TxRxID txrxid = std::make_pair(tx.GetTxID(), rxid);
        m_TxRx2CS[txrxid] = cm::ChannelState();

        ChannelState& cs = m_TxRx2CS[txrxid];
        cs.Initialize(tx, _rx);
        cs.SetStrong();
        cout << "LargeScale Processing:"
             << setw(2) << "RXID:" << setw(2) << rxid << '/' << NumOfRX
             << setw(5) << "->" << setw(5)
             << "TXID:" << setw(2) << (tx.GetTxID()) << '/' << NumOfTX
             << endl;
        cs.InitStrongSCM();
        cout << "SmallScale Processing:"
             << setw(2) << "RXID:" << setw(2) << rxid << '/' << NumOfRX
             << setw(5) << "->" << setw(5)
             << "TXID:" << setw(2) << (tx.GetTxID()) << '/' << NumOfTX
             << endl;
    }


    //如果是低频
    if (Parameters::Instance().BASIC.IScenarioModel <= 3) { ////ITU场景，设置强链路个数
        std::vector<std::pair<double, int> > tempvec1;
        for (int i = 0; i < Tx::CountTx(); ++i) {
            Tx& tx = Tx::GetTx(i);
            TxRxID txrxid = std::make_pair(tx.GetTxID(), rxid);
            m_TxRx2CS[txrxid].m_bIsStrong = false;
            double dLinkLossDB = GetCouplingLossDB(tx, _rx);
            double dTxPower = (m_TxRx2CS[txrxid].m_BCS.IsMacroToUE()) ? Parameters::Instance().Macro.DL.DMaxTxPowerDbm : Parameters::Instance().SmallCell.LINK.DPicoMaxTxPowerDbm;
            double dReceivedPower = dLinkLossDB + dTxPower;
            tempvec1.emplace_back(std::make_pair(dReceivedPower, i));
            Observer::Print("LinkStrength")<< (tx.GetTxID())<<setw(20)<<rxid<<setw(20)<<dLinkLossDB<<endl;
        }
        Observer::Print("LinkStrength") << endl;
        if (Parameters::Instance().BASIC.INumBSs != 1) {
            std::partial_sort(tempvec1.begin(), tempvec1.begin() + P::s().FX.IStrongLinkNum, tempvec1.end(), std::greater<std::pair<double, int> >());
            for (int i = 0; i < P::s().FX.IStrongLinkNum; ++i) {
                int iTxID = tempvec1[i].second;
                TxRxID txrxid = std::make_pair(iTxID, rxid);
                m_TxRx2CS[txrxid].SetStrong();
            }
        } else {
            for (auto& temp_pair : tempvec1) {
                int iTxID = temp_pair.second;
                TxRxID txrxid = std::make_pair(iTxID, rxid);
                if (m_TxRx2CS[txrxid].m_bIsStrong) {
                    cout << txrxid.first << " " << txrxid.second << " == true" << endl;
                }
                m_TxRx2CS[txrxid].SetStrong();
            }
        }
    }
}
///计算RIS单设链路路径的损失函数
// double LinkMatrix::GetRISCouplingLossDB(Tx& _tx, RIS& _ris, Rx& _rx) {
//     TxRISID txrisid = std::make_pair(_tx.GetTxID(), _ris.GetRISID());
//     return L2DB(m_TxRIS2CS[txrisid].m_dCouplingLoss);  // 计算 RIS 路径损失
// }
///第二段链路 RIS→Rx 预分配（thread1）：多线程遍历所有 MS，为其服务小区内所有 RIS 创建 m_RISRx2CS 并调用



void LinkMatrix::RISInitialize() {
    ms_finished = 0;
    link2_finished = 0;

    // （可选）TXRU 规模参数
    int IHAntNumPerPanel = Parameters::Instance().Macro.IHAntNumPerPanel;
    int IVAntNumPerPanel = Parameters::Instance().Macro.IVAntNumPerPanel;
    int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().Macro.H_TXRU_DIV_NUM_PerPanel;
    int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().Macro.V_TXRU_DIV_NUM_PerPanel;
    m_H_AntNumPerTXRU = IHAntNumPerPanel / H_TXRU_DIV_NUM_PerPanel;
    m_V_AntNumPerTXRU = IVAntNumPerPanel / V_TXRU_DIV_NUM_PerPanel;

    // 1) 初始化 Tx→RIS 并做小尺度：
    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        BTS& bts = btsid.GetBTS();
        Tx&  tx  = Tx::GetTx(btsid.GetTotalIndex()); // 三四是 Tx/Rx 体系
        for (int i = 0; i < bts.GetRISNum(); ++i) {
            RIS& ris = bts.GetRIS(i);
            int risid = ris.GetRISID().GetTotalIndex();
            TxRISID txrisid = std::make_pair(tx.GetTxID(), risid);

            m_TxRIS2CS[txrisid] = ChannelState();
            m_TxRIS2CS[txrisid].Initialize(tx, ris);
            m_TxRIS2CS[txrisid].InitStrongSCM();
            m_TxRIS2CS[txrisid].SetStrong();
            m_TxRIS2CS[txrisid].CalH();   // <<< 小尺度生成
        }
    }

    // 2) 预分配 RIS→MS（单线程安全简化）：
    for (int msIdx = 0; msIdx <= MSID::End().ToInt(); ++msIdx) {
        MS& ms  = MSID(msIdx).GetMS();
        RISInitialize_thread1(ms);  // 只建索引+小尺度
        // （可选）如果你后面要做波束/相位“策略”选择，可在这里调用 RISInitialize(ms)
    }

    // （可选）统计输出同一二
}

///20251117
void LinkMatrix::RISInitialize_thread1(MS& ms){
int rxid = ms.GetRxID();
Rx& rx = *ms.m_pRxNode;
for(BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
    BTS& bts = btsid.GetBTS();
    for (int i = 0; i < bts.GetRISNum(); ++i) {
        RIS& ris = bts.GetRIS(i);
        RISID id = ris.GetRISID();
        int risid = id.GetTotalIndex();
        RISRxID risrxid = std::make_pair(risid, rxid);
        // 为 RIS→Rx 建立 ChannelState 并 Initialize
        m_RISRx2CS[risrxid] = cm::ChannelState();
        m_RISRx2CS[risrxid].Initialize(ris, rx);
    }
}
// 线程计数（与 RISInitialize() 中的 while 栅栏配合）
link2_finished += 1;
}

LinkMatrix::LinkMatrix(): m_Pos2Din(100,pair_intHash),m_Pos2PentratinlossSF(100,pair_intHash){
}
//没用上
void LinkMatrix::WorkSlot(double _dTimeSec) {
    if (P::s().FX.IISFastFadingUsed != P::FASTFADING_USED) {
        return;
    }
    //cwq dahuawu
    for (int i = 0; i < Rx::CountRx(); ++i) {
        for (int j = 0; j < Tx::CountTx(); ++j) {
            TxRxID txrxid = std::make_pair(j, i);
            m_TxRx2CS[txrxid].WorkSlot(_dTimeSec);
        }
    }
}

//BTSEBBRank1Txer::Initialize

void LinkMatrix::WorkSlot(Rx& _rx, double _dTimeSec) {
    if (P::s().FX.IISFastFadingUsed != P::FASTFADING_USED) {
        return;
    }
//cwq dahuawu
    for (int i = 0; i < Tx::CountTx(); ++i) {
        TxRxID txrxid = std::make_pair(i, _rx.GetRxID());
        m_TxRx2CS[txrxid].WorkSlot(_dTimeSec);
    }
}
//20251112
///20260120
void LinkMatrix::WorkSlot_RIS(double t) {
    // Tx→RIS
    for (auto &kv : m_TxRIS2CS) kv.second.WorkSlot(t);
    // RIS→Rx
    for (auto &kv : m_RISRx2CS) kv.second.WorkSlot(t);
}


///信道链路损失和衰减计算
double LinkMatrix::GetCouplingLossDB(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    //******************方法1：适用于完整的信道初始化方法****************
    //    return L2DB(m_TxRx2CS[txrxid].m_pSCS->m_dStrongestEnergy);

    //******************方法2：适用于简化的信道初始化方法****************
    //    if (IsStrong(_tx, _rx)) {
    return L2DB(m_TxRx2CS[txrxid].m_pSCS->m_dStrongestCouplingLoss_Linear); //cwq dahuawu
    //    } else {
    //        return m_TxRx2CS[txrxid].m_BCS.m_LoSLinkLossDB;
    //    }

}

double LinkMatrix::GetLoSLinkLossDB(Tx& _tx, Rx& _rx) {
    return -1.0;

    /*
        TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
        return m_TxRx2CS[txrxid].m_BCS.m_LoSLinkLossDB;
     */
}

double LinkMatrix::GetShadowFadeDB(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_ShadowFadingDB;//cwq dahuawu
}

double LinkMatrix::Get2DDistanceM(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_d2DDistanceM;//cwq dahuawu
}

double LinkMatrix::Get3DDistanceM(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_d3DDistanceM;//cwq dahuawu
}

double LinkMatrix::GetPathLossDB(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_PathLossDB;//cwq dahuawu
}

double LinkMatrix::GetPurePathLossDB(Tx& _tx, Rx& _rx) {
//    return -1.0;
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_PurePathLossDB;//cwq dahuawu
}

double LinkMatrix::GetDinM(Tx& _tx, Rx& _rx) {
    std::pair<int, int> pos(_tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());
    if (m_Pos2Din.find(pos) == m_Pos2Din.end()) {
        //cout<<"Error in LinkMatrix::GetDinM(Tx& _tx, Rx& _rx)!"<<endl;
        assert(false);
    }
    return m_Pos2Din[pos];
}

std::shared_ptr<PathLoss> LinkMatrix::GetPathLossFun(Tx& _tx, Rx& _rx, bool _bIsLOS) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    std::shared_ptr<PathLoss> result;

    if ((P::s().IChannelModel_for_Scenario == P::InH
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)) {
        static std::shared_ptr<PathLoss> pPLInHLOS
                = std::shared_ptr<PathLoss > (new PathLossIndoorHotspotLOS_ModeA());
        static std::shared_ptr<PathLoss> pPLInHNLOS
                = std::shared_ptr<PathLoss > (new PathLossIndoorHotspotNLOS_ModeA());
        result = _bIsLOS ? pPLInHLOS : pPLInHNLOS;
    } else if (P::s().IChannelModel_for_Scenario == P::InF) {
        static std::shared_ptr<PathLoss> pPLInFLOS
                = std::shared_ptr<PathLoss>(new PathLossIndoorFactoryLOS());
        static std::shared_ptr<PathLoss> pPLInFNLOS
                = std::shared_ptr<PathLoss>(new PathLossIndoorFactoryDHNLOS());
        result = _bIsLOS ? pPLInFLOS : pPLInFNLOS;
    } else if ((P::s().IChannelModel_for_Scenario == P::UMI
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)) {

        if (_rx.GetSpecial() == 0) {
            static std::shared_ptr<PathLoss> pPLUMiLOS
                    = std::shared_ptr<PathLoss > (new PathLossUrbanMicroLOS_ModeA());
            static std::shared_ptr<PathLoss> pPLUMiNLOS
                    = std::shared_ptr<PathLoss > (new PathLossUrbanMicroNLOS_ModeA());
            result = _bIsLOS ? pPLUMiLOS : pPLUMiNLOS;
        } else if (_rx.GetSpecial() == 1) {
            std::pair<int, int> pos(
                    _tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());

            SetPos2Din(_tx, _rx);

            result = std::shared_ptr<PathLoss > (
                    new PathLossUrbanMicroO2I_ModeA(m_Pos2Din[pos], _bIsLOS));
        } else {
            assert(false);
        }
    } else if ((P::s().IChannelModel_for_Scenario == P::UMA
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)) {

        if (_rx.GetSpecial() == 0) {
            static std::shared_ptr<PathLoss> pPLUMaLOS
                    = std::shared_ptr<PathLoss > (new PathLossUrbanMacroLOS_ModeA());
//            static std::shared_ptr<PathLoss> pPLUMaNLOS
//                    = std::shared_ptr<PathLoss > (new PathLossUrbanMacroNLOS_ModeA());
            static std::shared_ptr<PathLoss> pPLUMaNLOS
                    = std::shared_ptr<PathLoss > (new PathLossUrbanMacroNLOS_Highfreq());
            result = _bIsLOS ? pPLUMaLOS : pPLUMaNLOS;
        } else if (_rx.GetSpecial() == 1) {
            std::pair<int, int> pos(_tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());

            SetPos2Din(_tx, _rx);

            result = std::shared_ptr<PathLoss > (
                    new PathLossUrbanMacroO2I(m_Pos2Din[pos], _bIsLOS));
        } else {
            assert(false);
        }
    } else if ((P::s().IChannelModel_for_Scenario == P::RMA
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA)) {
        if (_rx.GetSpecial() == 0) {
            static std::shared_ptr<PathLoss> pPLRMaLOS
                    = std::shared_ptr<PathLoss > (new PathLossRuralMacroLOS_ModeA());
            static std::shared_ptr<PathLoss> pPLRMaNLOS
                    = std::shared_ptr<PathLoss > (new PathLossRuralMacroNLOS_ModeA());
            result = _bIsLOS ? pPLRMaLOS : pPLRMaNLOS;
        } else if (_rx.GetSpecial() == 1) {
            std::pair<int, int> pos(_tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());

            SetPos2Din(_tx, _rx);

            result = std::shared_ptr<PathLoss > (
                    new PathLossRuralMacroO2I(m_Pos2Din[pos], _bIsLOS));
        } else {
            assert(false);
        }
    } else if ((P::s().IChannelModel_for_Scenario == P::InH
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB)) {

        static std::shared_ptr<PathLoss> pPLInHLOS
                = std::shared_ptr<PathLoss > (new PathLossIndoorHotspotLOS_ModeB());
        static std::shared_ptr<PathLoss> pPLInHNLOS
                = std::shared_ptr<PathLoss > (new PathLossIndoorHotspotNLOS_ModeB());
        result = _bIsLOS ? pPLInHLOS : pPLInHNLOS;
    } else if ((P::s().IChannelModel_for_Scenario == P::UMI
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB)) {

        if (_rx.GetSpecial() == 0) {
            static std::shared_ptr<PathLoss> pPLUMiLOS
                    = std::shared_ptr<PathLoss > (new PathLossUrbanMicroLOS_ModeB());
            static std::shared_ptr<PathLoss> pPLUMiNLOS
                    = std::shared_ptr<PathLoss > (new PathLossUrbanMicroNLOS_ModeB());
            result = _bIsLOS ? pPLUMiLOS : pPLUMiNLOS;
        } else if (_rx.GetSpecial() == 1) {
            std::pair<int, int> pos(
                    _tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());

            SetPos2Din(_tx, _rx);

            if (m_Pos2PentratinlossSF.find(pos) == m_Pos2PentratinlossSF.end()) {
                //m_Pos2PentratinlossSF[pos] = xNormal_channel(0, 7);
                if (_rx.IsLowloss() == 1) {
                    //low loss
                    m_Pos2PentratinlossSF[pos] = random.xNormal_channel(0, 4.4);

                } else if (_rx.IsLowloss() == 0) {
                    //high loss

                    m_Pos2PentratinlossSF[pos] = random.xNormal_channel(0, 6.5);
                } else {
                    assert(false);
                }
            }
            result = std::shared_ptr<PathLoss > (
                    new PathLossUrbanMicroO2I_Highfreq(
                    m_Pos2Din[pos], _bIsLOS, _rx.IsLowloss(), m_Pos2PentratinlossSF[pos]));
        } else {
            assert(false);
        }
    } else if ((P::s().IChannelModel_for_Scenario == P::UMA
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB)) {

        if (_rx.GetSpecial() == 0) {
            static std::shared_ptr<PathLoss> pPLUMaLOS
                    = std::shared_ptr<PathLoss > (new PathLossUrbanMacroLOS_Highfreq());
            static std::shared_ptr<PathLoss> pPLUMaNLOS
                    = std::shared_ptr<PathLoss > (new PathLossUrbanMacroNLOS_Highfreq());
            result = _bIsLOS ? pPLUMaLOS : pPLUMaNLOS;
        } else if (_rx.GetSpecial() == 1) {
            std::pair<int, int> pos(
                    _tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());

            SetPos2Din(_tx, _rx);

            if (m_Pos2PentratinlossSF.find(pos) == m_Pos2PentratinlossSF.end()) {
                //m_Pos2PentratinlossSF[pos] = xNormal_channel(0, 7);
                if (_rx.IsLowloss() == 1) {
                    //low loss
                    m_Pos2PentratinlossSF[pos] = random.xNormal_channel(0, 4.4);

                } else if (_rx.IsLowloss() == 0) {
                    //high loss
                    m_Pos2PentratinlossSF[pos] = random.xNormal_channel(0, 6.5);
                } else {
                    assert(false);
                }
            }
            result = std::shared_ptr<PathLoss > (
                    new PathLossUrbanMacroO2I_Highfreq(m_Pos2Din[pos],
                    _bIsLOS,
                    _rx.IsLowloss(),
                    m_Pos2PentratinlossSF[pos]));
        } else {
            assert(false);
        }
    } else if ((P::s().IChannelModel_for_Scenario == P::RMA
            && P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB)) {
        //        cout << "Do not support 5GCM_RMa!" << endl;
        //        assert(false);
        //20171219

        if (_rx.GetSpecial() == 0) {
            static std::shared_ptr<PathLoss> pPLRMaLOS
                    = std::shared_ptr<PathLoss > (new PathLossRuralMacroLOS_ModeB());
            static std::shared_ptr<PathLoss> pPLRMaNLOS
                    = std::shared_ptr<PathLoss > (new PathLossRuralMacroNLOS_ModeB());
            result = _bIsLOS ? pPLRMaLOS : pPLRMaNLOS;
        } else if (_rx.GetSpecial() == 1) {
            std::pair<int, int> pos(
                    _tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());

            SetPos2Din(_tx, _rx);

            if (m_Pos2PentratinlossSF.find(pos) == m_Pos2PentratinlossSF.end()) {
                //m_Pos2PentratinlossSF[pos] = xNormal_channel(0, 7);
                if (_rx.IsLowloss() == 1) {
                    //low loss
                    m_Pos2PentratinlossSF[pos] = random.xNormal_channel(0, 4.4);

                } else if (_rx.IsLowloss() == 0) {
                    assert(false);
                    //high loss
                    m_Pos2PentratinlossSF[pos] = random.xNormal_channel(0, 6.5);
                } else {
                    assert(false);
                }
            }
            result = std::shared_ptr<PathLoss > (
                    new PathLossRuralMacroO2I_ModeB(m_Pos2Din[pos],
                    _bIsLOS,
                    _rx.IsLowloss(),
                    m_Pos2PentratinlossSF[pos]));
        } else {
            assert(false);
        }
    } else if (P::s().IChannelModel_for_Scenario == P::_5GCM_TWO_LAYER) {
        static std::shared_ptr<PathLoss> pPLUMaLOS = std::shared_ptr<PathLoss > (new PathLossUrbanMacroLOS_ModeA());
        static std::shared_ptr<PathLoss> pPLUMaNLOS = std::shared_ptr<PathLoss > (new PathLossUrbanMacroNLOS_ModeA());
        static std::shared_ptr<PathLoss> pPLPico2UE_UMiLOS = std::shared_ptr<PathLoss > (new PathLossPico2UE_UrbanMicroLOS());
        static std::shared_ptr<PathLoss> pPLPico2UE_UMiNLOS = std::shared_ptr<PathLoss > (new PathLossPico2UE_UrbanMicroNLOS());

        if (m_TxRx2CS[txrxid].m_BCS.IsMacroToUE()) { //cwq dahuawu
            result = _bIsLOS ? pPLUMaLOS : pPLUMaNLOS;
            if (_rx.GetSpecial() == 1) {
                std::pair<int, int> pos(_tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());
                if (m_Pos2Din.find(pos) == m_Pos2Din.end()) {
                    m_Pos2Din[pos] = random.xUniform_channel(0, 25);
                }
                result = std::shared_ptr<PathLoss > (new PathLossUrbanMacroO2I(m_Pos2Din[pos], _bIsLOS));
            }
        } else {
            result = _bIsLOS ? pPLPico2UE_UMiLOS : pPLPico2UE_UMiNLOS;
            if (_rx.GetSpecial() == 1) {
                std::pair<int, int> pos(_tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());
                if (m_Pos2Din.find(pos) == m_Pos2Din.end()) {
                    m_Pos2Din[pos] = random.xUniform_channel(0, min(25.0, m_TxRx2CS[txrxid].m_BCS.m_d3DDistanceM));//cwq dahuawu
                }
                result = std::shared_ptr<PathLoss > (new PathLossPico2UE_UrbanMicroO2I(m_Pos2Din[pos], _bIsLOS));
            }
        }
    }
    return result;
}

void LinkMatrix::SetPos2Din(Tx& _tx, Rx& _rx) {
    if (_rx.GetSpecial() == 1) {
        std::pair<int, int> pos(_tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());
        if (m_Pos2Din.find(pos) == m_Pos2Din.end()) {
            //@threads
            double & d = m_Pos2Din[pos];
            if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA) {
                if (P::s().IChannelModel_for_Scenario == P::UMA
                        || P::s().IChannelModel_for_Scenario == P::UMI) {

                    if (P::s().FX.DRadioFrequencyMHz_Macro * 1e-3 <= 6.0) {
                        d = random.xUniform_channel(0, 25);
                    } else {
                        double rand_d1 = random.xUniform_channel(0, 25);
                        double rand_d2 = random.xUniform_channel(0, 25);
                        double d_in = min(rand_d1, rand_d2);
                        d = d_in;
                    }
                } else if (P::s().IChannelModel_for_Scenario == P::RMA) {
                    if (P::s().FX.DRadioFrequencyMHz_Macro * 1e-3 <= 6.0) {
                        d = random.xUniform_channel(0, 10);
                    } else {
                        double rand_d1 = random.xUniform_channel(0, 10);
                        double rand_d2 = random.xUniform_channel(0, 10);
                        double d_in = min(rand_d1, rand_d2);
                        d = d_in;
                    }
                }
            } else if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB) {
                if (P::s().IChannelModel_for_Scenario == P::UMA
                        || P::s().IChannelModel_for_Scenario == P::UMI) {

                    double rand_d1 = random.xUniform_channel(0, 25);
                    double rand_d2 = random.xUniform_channel(0, 25);
                    double d_in = min(rand_d1, rand_d2);
                    d = d_in;

                } else if (P::s().IChannelModel_for_Scenario == P::RMA) {

                    double rand_d1 = random.xUniform_channel(0, 10);
                    double rand_d2 = random.xUniform_channel(0, 10);
                    double d_in = min(rand_d1, rand_d2);
                    d = d_in;
                }
            } else {
                assert(false);
            }
            //@threads
        }
    }
}

double LinkMatrix::GetPos2Din(std::pair<int, int> & _pos) {
    return m_Pos2Din[_pos];
}
///setPos2Pentratinloss
bool LinkMatrix::IsStrong(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_bIsStrong;
}

bool LinkMatrix::IsLOS(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_bIsLOS;      //cwq dahuawu
}

LinkMatrix& LinkMatrix::Instance() {
    if (!m_pLM) {
        m_pLM = new LinkMatrix;
    }
    return *m_pLM;
}

double LinkMatrix::GetESD(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_EODSpreadDeg;//cwq dahuawu

}

double LinkMatrix::GetESA(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_EOASpreadDeg;//cwq dahuawu
}

double LinkMatrix::GetLoSAoDRAD(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_AODLOSRAD;//cwq dahuawu
}

double LinkMatrix::GetLoSEoDRAD(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_EODLOSRAD;//cwq dahuawu
}

double LinkMatrix::GetLoSAoARAD(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_AOALOSRAD;//cwq dahuawu
}

double LinkMatrix::GetLoSEoARAD(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    return m_TxRx2CS[txrxid].m_BCS.m_EOALOSRAD;//cwq dahuawu
}

bool LinkMatrix::GetPathInfo(Tx& _tx, Rx& _rx, std::vector<double>& _vPathPower, std::vector<double>& _vSubPathPower, std::vector<double>& _vPathAoD, std::vector<double>& _vSubPathAoD, std::vector<double>& _vPathEoD, std::vector<double>& _vSubPathEoD, std::vector<double>& _vPathEoA, std::vector<double>& _vSubPathEoA) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    if (!m_TxRx2CS[txrxid].m_pSCS) {
        if (m_TxRx2CS[txrxid].m_BCS.m_pRx->GetSpecial() == 0 && m_TxRx2CS[txrxid].m_BCS.m_bIsLOS) {
            double kR = DB2L(m_TxRx2CS[txrxid].m_BCS.m_KFactorDB);
            double dPathPower = kR / (1 + kR);
            double dPathAoDDeg = RAD2DEG(m_TxRx2CS[txrxid].m_BCS.m_AODLOSRAD);
            double dPathEoDDeg = RAD2DEG(m_TxRx2CS[txrxid].m_BCS.m_EODLOSRAD);
            double dPathEoADeg = RAD2DEG(m_TxRx2CS[txrxid].m_BCS.m_EOALOSRAD);
            _vPathPower.push_back(dPathPower);
            _vSubPathPower.push_back(dPathPower);
            _vPathAoD.push_back(dPathAoDDeg);
            _vSubPathAoD.push_back(dPathAoDDeg);
            _vPathEoD.push_back(dPathEoDDeg);
            _vSubPathEoD.push_back(dPathEoDDeg);
            _vPathEoA.push_back(dPathEoADeg);
            _vSubPathEoA.push_back(dPathEoADeg);
        } else {
            _vPathPower.push_back(0);
            _vSubPathPower.push_back(0);
            _vPathAoD.push_back(0);
            _vSubPathAoD.push_back(0);
            _vPathEoD.push_back(0);
            _vSubPathEoD.push_back(0);
            _vPathEoA.push_back(0);
            _vSubPathEoA.push_back(0);
        }
        int iclusterNum = static_cast<int> (m_TxRx2CS[txrxid].m_pSCS->m_vPath.size());
        for (int k = 0; k < iclusterNum; ++k) {
            double dPathPower = m_TxRx2CS[txrxid].m_pSCS->m_vPath[k].m_PowerNlos;
            double kR = DB2L(m_TxRx2CS[txrxid].m_BCS.m_KFactorDB);
            if (m_TxRx2CS[txrxid].m_BCS.m_pRx->GetSpecial() == 0 && m_TxRx2CS[txrxid].m_BCS.m_bIsLOS) {
                dPathPower *= 1 / (1 + kR);
            }
            double dPathAoDDeg = m_TxRx2CS[txrxid].m_pSCS->m_vPath[k].m_AODDeg_Path;
            double dPathEoDDeg = m_TxRx2CS[txrxid].m_pSCS->m_vPath[k].m_EODDeg_Path;
            double dPathEoADeg = m_TxRx2CS[txrxid].m_pSCS->m_vPath[k].m_EOADeg_Path;
            _vPathPower.push_back(dPathPower);
            _vPathAoD.push_back(dPathAoDDeg);
            _vPathEoD.push_back(dPathEoDDeg);
            _vPathEoA.push_back(dPathEoADeg);

            int isubpathNum = static_cast<int> ((m_TxRx2CS[txrxid].m_pSCS->m_vPath)[k].m_vSubpath.size());
            for (int l = 0; l < isubpathNum; ++l) {
                double dSubPathPower = pow((m_TxRx2CS[txrxid].m_pSCS->m_vPath)[k].m_PowerPerRay, 2);
                double kR = DB2L(m_TxRx2CS[txrxid].m_BCS.m_KFactorDB);
                if (m_TxRx2CS[txrxid].m_BCS.m_pRx->GetSpecial() == 0 && m_TxRx2CS[txrxid].m_BCS.m_bIsLOS) {
                    dSubPathPower *= 1 / (1 + kR);
                }
                double dSubPathAoDDeg = ((m_TxRx2CS[txrxid].m_pSCS->m_vPath)[k].m_vSubpath[l].m_AODDeg);
                double dSubPathEoDDeg = ((m_TxRx2CS[txrxid].m_pSCS->m_vPath)[k].m_vSubpath[l].m_EODDeg);
                double dSubPathEoADeg = ((m_TxRx2CS[txrxid].m_pSCS->m_vPath)[k].m_vSubpath[l].m_EOADeg);
                _vSubPathPower.push_back(dSubPathPower);
                _vSubPathAoD.push_back(dSubPathAoDDeg);
                _vSubPathEoD.push_back(dSubPathEoDDeg);
                _vSubPathEoA.push_back(dSubPathEoADeg);
            }
        }
        return true;
    } else {
        return false;
    }
}

int LinkMatrix::GetStrongestUEPanelIndex(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());

    return m_TxRx2CS[txrxid].m_pSCS->m_pBest_UE_Panel->GetPanelIndex();//cwq dahuawu
}

int LinkMatrix::GetStrongestBSBeamIndex(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    /*
        return m_TxRx2CS[txrxid].m_pSCS->m_iStrongestBSBeamIndex;
     */
//cwq dahuawu
    AntennaPanel* _pBest_BS_Panel =
            m_TxRx2CS[txrxid].m_pSCS->m_pBest_BS_Panel;
    AntennaPanel* _pBest_UE_Panel =
            m_TxRx2CS[txrxid].m_pSCS->m_pBest_UE_Panel;

    return m_TxRx2CS[txrxid].m_pSCS->GetStrongestBSBeamIndex(
            _pBest_BS_Panel, _pBest_UE_Panel);
}

int LinkMatrix::GetStrongestUEBeamIndex(Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    /*
        return m_TxRx2CS[txrxid].m_pSCS->m_iStrongestUEBeamIndex;
     */
//cwq dahuawu
    AntennaPanel* _pBest_BS_Panel =
            m_TxRx2CS[txrxid].m_pSCS->m_pBest_BS_Panel;
    AntennaPanel* _pBest_UE_Panel =
            m_TxRx2CS[txrxid].m_pSCS->m_pBest_UE_Panel;

    return m_TxRx2CS[txrxid].m_pSCS->GetStrongestUEBeamIndex(
            _pBest_BS_Panel, _pBest_UE_Panel);
}

double LinkMatrix::GetCouplingLoss_linear_u36873_w_BestBeamPair_4_BestPanelPair(
        Tx& _tx, Rx& _rx) {

    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    //cwq dahuawu
    return m_TxRx2CS[txrxid].m_pSCS->CalCouplingLoss_linear_u36873_w_BestBeamPair_4_BestPanelPair();
}

double LinkMatrix::GetCouplingLoss_linear_u36873_w_RandomBSBeam_4_BestPanelPair(
        Tx& _tx, Rx& _rx) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    //cwq dahuawu
    return m_TxRx2CS[txrxid].m_pSCS->CalCouplingLoss_linear_u36873_w_RandomBSBeam_4_BestPanelPair();

}

itpp::cmat LinkMatrix::GetFadingMat_wABF_for_active_TXRU_Pairs_per_PanelPair(
        Tx& _tx, Rx& _rx, int _scid,
        AntennaPanel* _pBS_Panel,
        AntennaPanel* _pUE_Pane) {

    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    //cwq dahuawu
    return m_TxRx2CS[txrxid].m_pSCS->
            GetH_after_ABF_for_active_TXRU_Pairs_per_PanelPair(
            _scid / P::s().FX.ICarrierSampleSpace,
            _pBS_Panel, _pUE_Pane);
}

itpp::cmat LinkMatrix::GetFadingMat_wABF_for_all_active_TXRU_Pairs(
        Tx& _tx, Rx& _rx, int _scid) {
    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    //cwq dahuawu
    return m_TxRx2CS[txrxid].m_pSCS->
            GetH_after_ABF_for_all_active_TXRU_Pairs(
            _scid / P::s().FX.ICarrierSampleSpace);
}
itpp::cmat LinkMatrix::Get_mD_wABF_for_all_active_TXRU_Pairs(
        Tx& _tx, Rx& _rx, int _scid) {

    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    //cwq dahuawu
    return m_TxRx2CS[txrxid].m_pSCS->
            Get_mD_after_ABF_for_all_active_TXRU_Pairs(
            _scid / P::s().FX.ICarrierSampleSpace);
}

itpp::cmat LinkMatrix::GetFadingMat_wABF_for_all_active_TXRU_Pairs2(
        Tx& _tx, Rx& _rx, int _scid,
        const std::vector<std::pair<int, std::vector<int> > >& _vUEPanelAndvBSBeam) {

    assert(Parameters::Instance().Macro.DL.bEnable_BSAnalogBF == false);

    return GetFadingMat_wABF_for_all_active_TXRU_Pairs(_tx, _rx, _scid);
}
///用处
const itpp::cmat LinkMatrix::GetFadingMat_wABF_for_all_active_TXRU_Pairs_RISIntf(
     Tx& _tx, Rx& _rx, int _scid) {

    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
    assert(m_TxRx2CS.count(txrxid) != 0);
    itpp::cmat&& H = m_TxRx2CS[txrxid].m_pSCS->
    ///用处
            GetH_after_ABF_for_all_active_TXRU_Pairs_RISIntf_BestPanel(
            _scid / P::s().FX.ICarrierSampleSpace);
    return H;
    //    return m_TxRx2CS[txrxid].m_pSCS->
    //            GetH_after_ABF_for_all_active_TXRU_Pairs(
    //            _scid / P::s().FX.ICarrierSampleSpace);
}

///线程初始化函数
void LinkMatrix::initialthread(int n) {
    for (int i = 0; i < Rx::CountRx(); ++i) {
        if (i % threadnum != n)continue;
        Rx &rx = Rx::GetRx(i);
        Initialize(rx);
    }
}

