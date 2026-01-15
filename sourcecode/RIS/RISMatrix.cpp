#include "../ChannelModel/P.h"
#include "../ChannelModel/AOGOmni.h"
#include "../ChannelModel/AOGSector.h"
#include "../ChannelModel/Point.h"
#include "../ChannelModel/WrapAround.h"
#include "../ChannelModel/GaussianMap.h"
#include "../ChannelModel/BasicChannelState.h"
#include "../ChannelModel/SpaceChannelState.h"
#include "../ChannelModel/ChannelState.h"
#include "../Parameters/Parameters.h"
#include "../Statistician/Observer.h"
#include "../ChannelModel/PathState.h"
#include "../ChannelModel/SubpathState.h"
#include "../ChannelModel/functions.h"
#include "../ChannelModel/CTXRU.h"
#include "BS2RISPathLossUrbanMacroOutdoorLOS.h"
#include "BS2RISPathLossUrbanMacroOutdoorNLOS.h"
#include "RIS2MSPathLossUrbanMacroOutdoorLOS.h"
#include "RIS2MSPathLossUrbanMacroOutdoorNLOS.h"
#include "RISMatrix.h"
#include "RIS.h"
#include "../NetworkDrive/Clock.h"

using namespace cm;
extern LinkMatrix& lm;
extern double xUniform_channel(double _dmin, double _dmax);
extern double xNormal_channel(double _dave, double _dstd);

RISMatrix* RISMatrix::m_pRM = 0;

void RISMatrix::Reset() {
    m_TxRIS2CS.clear();
    m_RISRx2CS.clear();
    m_Pos2Din.clear();
}

//void RISMatrix::InitThread_RIS(vector<TxRISID>& vpTxRIS){
//    for (vector<TxRISID>::iterator it = vpTxRIS.begin(); it != vpTxRIS.end(); ++it) {
//        lm.m_TxRIS2CS[*it].InitStrongSCM();
//        lm.m_TxRIS2CS[*it].SetStrong();
//                //计算小尺度信道
//        lm.m_TxRIS2CS[*it].CalH();
//        //lm.m_TxRIS2CS[*it].DeleteSCS();
//    }
//}
void RISMatrix::InitThread_RIS(std::vector<MS*>& vpMs) {
    for (std::vector<MS*>::iterator it = vpMs.begin(); it != vpMs.end(); ++it) {
        // 获取服务基站的 ID，并通过该 ID 获取实际的基站对象
        BTSID btsid = (**it).GetAllServBTSs()[0];   // 获取第一个服务基站的 ID
        int rxid = (**it).GetRxID();  // 获取该 MS 的接收 ID

        // 获取基站对象
        BTS& bts = btsid.GetBTS();  // 通过 btsid 获取对应的基站对象

        // 获取使用的 RIS ID
        RISID RIS_used = (**it).m_vMainServRIS_Comp;
        int risid = RIS_used.GetTotalIndex();  // 获取 RIS 的索引

        // 构造 TxRISID 和 RISRxID
        TxRISID txrisid = std::make_pair(bts.GetTxID(), risid);  // 组合 txid 和 risid
        RISRxID risrxid = std::make_pair(risid, rxid);  // 组合 risid 和 rxid

        cout << "RISMatirx_ini" << setw(10) << (**it).GetID() << endl;

        // 使用 assert 确保强信号链路
        assert(lm.m_TxRIS2CS[txrisid].m_bIsStrong);

        // 初始化 Tx→RIS 和 RIS→Rx 的小尺度结构
        lm.m_TxRIS2CS[txrisid].InitSmallScale();
        lm.m_RISRx2CS[risrxid].InitSmallScale();

        // 可以取消注释执行后续计算
        // CalH_Total((**it));
    }
}


void RISMatrix::UpdateThread_RIS(std::vector<MS*>& vpMs) {
    int iTimeSec = Clock::Instance().GetTimeSec();
    for (std::vector<MS*>::iterator it = vpMs.begin(); it != vpMs.end(); ++it) {
        int rxid = (*it)->GetRxID();
        for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
            BTS &bts = btsid.GetBTS();
            Tx &tx = Tx::GetTx(btsid.GetTotalIndex());
            int txid = bts.GetTxID();
            TxRxID txrxid = std::make_pair(txid, rxid);

            if (Parameters::Instance().RIS.IS_SmallScale && lm.m_TxRx2CS[txrxid].m_bIsStrong) {
                AntennaPanel* pBest_BS_Panel =
                    lm.m_TxRx2CS[txrxid].m_pSCS->m_pBest_BS_Panel;
                AntennaPanel* pBest_UE_Panel =
                    lm.m_TxRx2CS[txrxid].m_pSCS->m_pBest_UE_Panel;
                int BS_BeamIndex = lm.m_TxRx2CS[txrxid].m_pSCS->GetStrongestBSBeamIndex(pBest_BS_Panel, pBest_UE_Panel);
                int UE_BeamIndex = lm.m_TxRx2CS[txrxid].m_pSCS->GetStrongestUEBeamIndex(pBest_BS_Panel, pBest_UE_Panel);
                for (int i = 0; i < bts.GetRISNum(); ++i) {
                    RIS &ris = bts.GetRIS(i);
                    RISID id = ris.GetRISID();
                    int risid = id.GetTotalIndex();

                    RISRxID risrxid = std::make_pair(ris.GetRISID().GetTotalIndex(), rxid);
                    pair<int, int> beampair2 = std::make_pair(0, UE_BeamIndex);
                    lm.m_RISRx2CS[risrxid].m_pSCS->UpdateH(iTimeSec,beampair2);
                    RISMatrix::Instance().CalH_Total(BS_BeamIndex,bts, ris, **it);
                }
            }
        }
    }
}

//
void RISMatrix::Initialize(){
    //初始化Tx-RIS
    //RIS_Update.clear();
    if(Parameters::Instance().BASIC.BRISMultiThread){
        int nThreads = thread::hardware_concurrency();
        int nTxRIS =Parameters::Instance().BASIC.IRISPerBTS*Parameters::Instance().BASIC.ITotalBTSNumPerBS * Parameters::Instance().BASIC.INumBSs;
        int ThreadNum = min(nTxRIS, nThreads);
        vector<vector<MS*> > vpRx(ThreadNum);

        for(MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid){
            MS& ms = msid.GetMS();
            MS* pms=&ms;
            vpRx[msid.ToInt()%ThreadNum].push_back(pms);
        }
        boost::thread_group initial;
        for (int j = 0; j < static_cast<int> (vpRx.size()); ++j){
            initial.create_thread(bind(&RISMatrix::InitThread_RIS, this, vpRx[j]));
        }
        initial.join_all();
        for(MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid){
            MS& ms = msid.GetMS();
            Initialize(ms);
        }
    }
    else{
        for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
            BTS& bts = btsid.GetBTS();
            Tx& tx = Tx::GetTx(btsid.GetTotalIndex());
            for (int i = 0; i < Parameters::Instance().BASIC.IRISPerBTS; ++i) {
                RIS& ris = bts.GetRIS(i);
                RISID id = ris.GetRISID();
                int risid = id.GetTotalIndex();
                TxRISID txrisid = std::make_pair(bts.GetTxID(), risid);
                if(lm.m_TxRIS2CS[txrisid].m_bIsStrong){
                    lm.m_TxRIS2CS[txrisid].InitStrongSCM();
                    //计算小尺度信道
                    cout<<"SCS_CalH1"<<setw(4)<<btsid.GetTotalIndex()<<setw(4)<<risid<<endl;
                    lm.m_TxRIS2CS[txrisid].CalH();
                }
            }
        }
        for(MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid){
            MS& ms = msid.GetMS();
            Initialize(ms);
        }
    }

}
//
void RISMatrix::Initialize(MS& ms){
    int rxid = ms.GetRxID();
    Rx& rx = *ms.m_pRxNode;
    std::vector<BTSID> v_ServeBTS = ms.GetAllServBTSs();
    if(v_ServeBTS.size() > 1){
        cout<<"RIS不支持MULTITRP"<<endl;
        assert(false);
    }
    //初始化RIS-Rx
//    RISID Serv=ms.m_vMainServRIS_Comp;
//    RIS ServRIS=Serv.GetRIS();
//    RISRxID risrxid = std::make_pair(Serv.GetTotalIndex(), rxid);
//    assert(lm.m_RISRx2CS[risrxid].m_bIsStrong);
//    lm.m_RISRx2CS[risrxid].InitStrongSCM();
//    cout<<"SCS_CalH2"<<setw(4)<<Serv.GetTotalIndex()<<setw(4)<<rxid<<endl;
//    lm.m_RISRx2CS[risrxid].CalH();

    //计算其他基站到服务RIS的强干扰矩阵
//    if(!ServRIS.mIs_Interf_Ini){
//        ServRIS.mIs_Interf_Ini=true;
//        RIS_Update.push_back(ServRIS);
//        for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
//            BTS& bts = btsid.GetBTS();
//            if(btsid==v_ServeBTS[0])
//                continue;
//            TxNode& tx = TxNode::GetTx(btsid.GetTotalIndex());
//            TxRISID txrisid = std::make_pair(bts.GetTxID(), Serv.GetTotalIndex());
//            lm.m_TxRIS2CS[txrisid] = ChannelState();
//            lm.m_TxRIS2CS[txrisid].Initialize(tx, ServRIS);
//            lm.m_TxRIS2CS[txrisid].InitStrongSCM();
//            lm.m_TxRIS2CS[txrisid].CalH();
//        }
//    }

    cout<<"RISMatirx_CalH_Total"<<setw(10)<<ms.GetID()<<endl;
    CalH_Total(ms);
    //lm.m_RISRx2CS[risrxid].DeleteSCS();

}

void RISMatrix::WorkSlot(){
    int iTime = Clock::Instance().GetTimeSlot();
    if (Parameters::Instance().BASIC.BDL == true && DownOrUpLink(iTime) == 0){
        if(Parameters::Instance().BASIC.BRISMultiThread){
            int nThreads = thread::hardware_concurrency();
            int nTxRIS =Parameters::Instance().BASIC.IRISPerBTS*Parameters::Instance().BASIC.ITotalBTSNumPerBS * Parameters::Instance().BASIC.INumBSs;
            int ThreadNum = min(nTxRIS, nThreads);
            vector<vector<MS*> > vpRx(ThreadNum);
            //for(MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid){
            //    MS& ms = msid.GetMS();
            //    MS* pms=&ms;
            //    vpRx[msid.ToInt()%ThreadNum].push_back(pms);
            //}

            //boost::thread_group initial;
            //for (int j = 0; j < static_cast<int> (vpRx.size()); ++j){
            //    initial.create_thread(bind(&RISMatrix::UpdateThread_RIS, this, vpRx[j]));
            //}
            std::map<BTSID, int> btsToThread;
            int threadIndex = 0;
            // 遍历所有MS，按服务基站分组
            for(MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
                MS& ms = msid.GetMS();
                BTSID servingBTS = ms.GetAllServBTSs()[0];
                if(btsToThread.find(servingBTS) == btsToThread.end()) {
                    btsToThread[servingBTS] = threadIndex % ThreadNum;
                    threadIndex++;
                }
                vpRx[btsToThread[servingBTS]].push_back(&ms);
            }
            // 创建并启动线程组
            boost::thread_group initial;
            for (int j = 0; j < static_cast<int> (vpRx.size()); ++j) {
                if(!vpRx[j].empty()) {
                    initial.create_thread(bind(&RISMatrix::UpdateThread_RIS, this, vpRx[j]));
                }
            }
            initial.join_all();
        }
        else

        for(MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid){
            MS& ms = msid.GetMS();
            WorkSlot(ms);
        }
    }
}

void RISMatrix::WorkSlot(MS& ms){
    int iTimeSec = Clock::Instance().GetTimeSec();
    int rxid = ms.GetRxID();
    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        BTS &bts = btsid.GetBTS();
        Tx &tx = Tx::GetTx(btsid.GetTotalIndex());
        int txid = bts.GetTxID();
        TxRxID txrxid = std::make_pair(txid, rxid);

        AntennaPanel* pBest_BS_Panel =
     lm.m_TxRx2CS[txrxid].m_pSCS->m_pBest_BS_Panel;
        AntennaPanel* pBest_UE_Panel =
            lm.m_TxRx2CS[txrxid].m_pSCS->m_pBest_UE_Panel;
        int BS_BeamIndex = lm.m_TxRx2CS[txrxid].m_pSCS->GetStrongestBSBeamIndex(pBest_BS_Panel, pBest_UE_Panel);

        if (Parameters::Instance().RIS.IS_SmallScale && lm.m_TxRx2CS[txrxid].m_bIsStrong) {
            AntennaPanel* pBest_BS_Panel =
        lm.m_TxRx2CS[txrxid].m_pSCS->m_pBest_BS_Panel;
            AntennaPanel* pBest_UE_Panel =
                lm.m_TxRx2CS[txrxid].m_pSCS->m_pBest_UE_Panel;
            int UE_BeamIndex = lm.m_TxRx2CS[txrxid].m_pSCS->GetStrongestUEBeamIndex(pBest_BS_Panel, pBest_UE_Panel);
            for (int i = 0; i < bts.GetRISNum(); ++i) {
                RIS &ris = bts.GetRIS(i);
                RISID id = ris.GetRISID();
                int risid = id.GetTotalIndex();

                RISRxID risrxid = std::make_pair(ris.GetRISID().GetTotalIndex(), rxid);
                pair<int, int> beampair2 = std::make_pair(0, UE_BeamIndex);
                lm.m_RISRx2CS[risrxid].m_pSCS->UpdateH(iTimeSec,beampair2);
                RISMatrix::Instance().CalH_Total(BS_BeamIndex,bts, ris, ms);
            }
        }
    }
}

void RISMatrix::CalH_Total(BTS& _bts, RIS& _RIS, MS& ms){
//    for bsbeam
//        for uebeam
//            for risbeam
//                1\2\3
//                1+(2+3)

    RIS& ServeRIS = _RIS;
    BTS& ServeBTS = _bts;
    Tx& tx = *_bts.m_pTxNode;
    Rx& rx = *ms.m_pRxNode;
    TxRxID txrxid = std::make_pair(ServeBTS.GetTxID(), ms.GetRxID());
    TxRISID txrisid = std::make_pair(ServeBTS.GetTxID(),ServeRIS.GetRISID().GetTotalIndex());
    RISRxID risrxid = std::make_pair(ServeRIS.GetRISID().GetTotalIndex(),ms.GetRxID());
    std::shared_ptr<cm::Antenna> pBTSAntenna = tx.GetAntennaPointer();
    std::shared_ptr<cm::Antenna> pRISAntenna = ServeRIS.GetAntennaPointer();
    std::shared_ptr<cm::Antenna> pMSAntenna = rx.GetAntennaPointer();
    int BTS_TXRU_NUM=pBTSAntenna->GetTotalTXRU_Num();
    int RIS_TXRU_NUM=pRISAntenna->GetTotalTXRU_Num();
    int MS_TXRU_NUM=pMSAntenna->GetTotalTXRU_Num();
    int iFreSampleNum = P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace;
    std::vector<itpp::cmat> temp1_t(iFreSampleNum);
    std::vector<itpp::cmat> temp2_t(iFreSampleNum);
    std::vector<itpp::cmat> temp4_t(iFreSampleNum);
    for(int i=0;i<iFreSampleNum;++i){
        temp1_t[i]=itpp::zeros_c(RIS_TXRU_NUM, BTS_TXRU_NUM);
        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
        pBTSAntenna->GetvAntennaPanels()) {

            BOOST_FOREACH(std::shared_ptr<AntennaPanel> pRISAntennaPanel,
            pRISAntenna->GetvAntennaPanels()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pBS_TXRU,
                pBSAntennaPanel->GetvTXRUs()) {

                    BOOST_FOREACH(std::shared_ptr<CTXRU> pRIS_TXRU,
                    pRISAntennaPanel->GetvTXRUs()) {

                        pair<int, int> txrupair = std::make_pair(pBS_TXRU->GetTXRUIndex(), pRIS_TXRU->GetTXRUIndex());
                        temp1_t[i](pRIS_TXRU->GetTXRUIndex(),pBS_TXRU->GetTXRUIndex())=lm.m_TxRIS2CS[txrisid].m_pSCS->m_TXRUPairID_2_FreqH[txrupair][i];

                                }
                            }
                        }
                    }
        temp2_t[i]=itpp::zeros_c(MS_TXRU_NUM, RIS_TXRU_NUM);
        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pRISAntennaPanel,
        pRISAntenna->GetvAntennaPanels()) {

            BOOST_FOREACH(std::shared_ptr<AntennaPanel> pMSAntennaPanel,
            pMSAntenna->GetvAntennaPanels()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pRIS_TXRU,
                pRISAntennaPanel->GetvTXRUs()) {

                    BOOST_FOREACH(std::shared_ptr<CTXRU> pMS_TXRU,
                    pMSAntennaPanel->GetvTXRUs()) {
                        pair<int, int> txrupair = std::make_pair(pRIS_TXRU->GetTXRUIndex(), pMS_TXRU->GetTXRUIndex());
                        temp2_t[i](pMS_TXRU->GetTXRUIndex(),pRIS_TXRU->GetTXRUIndex())=lm.m_RISRx2CS[risrxid].m_pSCS->m_TXRUPairID_2_FreqH[txrupair][i];
                        }
                    }
                }
            }
        temp4_t[i]=itpp::zeros_c(MS_TXRU_NUM, BTS_TXRU_NUM);
        itpp::cmat RISphase=itpp::zeros_c(RIS_TXRU_NUM, RIS_TXRU_NUM);
        for(int i=0;i<RIS_TXRU_NUM;++i){
            //RISphase(i,i)=ms.tempR_PRIS(i);
            RISphase(i,i) = lm.m_TxRx2CS[txrxid].m_pSCS->tempRIS(i);
        }
        temp4_t[i]=temp2_t[i]*RISphase*temp1_t[i];



    }
    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
    pBTSAntenna->GetvAntennaPanels()) {

        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pMSAntennaPanel,
        pMSAntenna->GetvAntennaPanels()) {

            BOOST_FOREACH(std::shared_ptr<CTXRU> pBS_TXRU,
            pBSAntennaPanel->GetvTXRUs()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pMS_TXRU,
                pMSAntennaPanel->GetvTXRUs()) {
                    pair<int, int> txrupair = std::make_pair(pBS_TXRU->GetTXRUIndex(), pMS_TXRU->GetTXRUIndex());
                    lm.m_TxRx2CS[txrxid].m_pSCS->m_TXRUPairID_2_FreqH_RIS[txrupair].resize(iFreSampleNum);
                    for (int k = 0; k < iFreSampleNum; ++k) {
                        lm.m_TxRx2CS[txrxid].m_pSCS->m_TXRUPairID_2_FreqH_RIS[txrupair][k]= temp4_t[k](pMS_TXRU->GetTXRUIndex(),pBS_TXRU->GetTXRUIndex());
                    }
                }
            }
        }
    }

}
void RISMatrix::CalH_Total(int BS_BeamIndex, BTS& _bts, RIS& _RIS, MS& ms){

    RIS& ServeRIS = _RIS;
    BTS& ServeBTS = _bts;
    Tx& tx = *_bts.m_pTxNode;
    Rx& rx = *ms.m_pRxNode;
    TxRxID txrxid = std::make_pair(ServeBTS.GetTxID(), ms.GetRxID());
    TxRISID txrisid = std::make_pair(ServeBTS.GetTxID(),ServeRIS.GetRISID().GetTotalIndex());
    RISRxID risrxid = std::make_pair(ServeRIS.GetRISID().GetTotalIndex(),ms.GetRxID());
    std::shared_ptr<cm::Antenna> pBTSAntenna = tx.GetAntennaPointer();
    std::shared_ptr<cm::Antenna> pRISAntenna = ServeRIS.GetAntennaPointer();
    std::shared_ptr<cm::Antenna> pMSAntenna = rx.GetAntennaPointer();
    int BTS_TXRU_NUM=pBTSAntenna->GetTotalTXRU_Num();
    int RIS_TXRU_NUM=pRISAntenna->GetTotalTXRU_Num();
    int MS_TXRU_NUM=pMSAntenna->GetTotalTXRU_Num();
    int iFreSampleNum = P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace;
    std::vector<itpp::cmat> temp1_t(iFreSampleNum);
    std::vector<itpp::cmat> temp2_t(iFreSampleNum);
    std::vector<itpp::cmat> temp4_t(iFreSampleNum);
    for(int i=0;i<iFreSampleNum;++i){
        temp1_t[i]=itpp::zeros_c(RIS_TXRU_NUM, BTS_TXRU_NUM);
        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
                pBTSAntenna->GetvAntennaPanels()) {

            BOOST_FOREACH(std::shared_ptr<AntennaPanel> pRISAntennaPanel,
                    pRISAntenna->GetvAntennaPanels()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pBS_TXRU,
                        pBSAntennaPanel->GetvTXRUs()) {

                    BOOST_FOREACH(std::shared_ptr<CTXRU> pRIS_TXRU,
                            pRISAntennaPanel->GetvTXRUs()) {

                        pair<int, int> txrupair = std::make_pair(pBS_TXRU->GetTXRUIndex(), pRIS_TXRU->GetTXRUIndex());
                        temp1_t[i](pRIS_TXRU->GetTXRUIndex(),pBS_TXRU->GetTXRUIndex())=lm.m_TxRIS2CS[txrisid].m_pSCS->m_TXRUPairID_2_FreqH_BS2RIS[BS_BeamIndex][txrupair][i];
                    }
                }
            }
        }
        temp2_t[i]=itpp::zeros_c(MS_TXRU_NUM, RIS_TXRU_NUM);
        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pRISAntennaPanel,
                pRISAntenna->GetvAntennaPanels()) {

            BOOST_FOREACH(std::shared_ptr<AntennaPanel> pMSAntennaPanel,
                    pMSAntenna->GetvAntennaPanels()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pRIS_TXRU,
                        pRISAntennaPanel->GetvTXRUs()) {

                    BOOST_FOREACH(std::shared_ptr<CTXRU> pMS_TXRU,
                            pMSAntennaPanel->GetvTXRUs()) {
                        pair<int, int> txrupair = std::make_pair(pRIS_TXRU->GetTXRUIndex(), pMS_TXRU->GetTXRUIndex());
                        temp2_t[i](pMS_TXRU->GetTXRUIndex(),pRIS_TXRU->GetTXRUIndex())=lm.m_RISRx2CS[risrxid].m_pSCS->m_TXRUPairID_2_FreqH[txrupair][i];
                    }
                }
            }
        }
        temp4_t[i]=itpp::zeros_c(MS_TXRU_NUM, BTS_TXRU_NUM);
        itpp::cmat RISphase=itpp::zeros_c(RIS_TXRU_NUM, RIS_TXRU_NUM);
        for(int i=0;i<RIS_TXRU_NUM;++i){
            //RISphase(i,i)=ms.tempR_PRIS(i);
            RISphase(i,i) = lm.m_TxRx2CS[txrxid].m_pSCS->tempRIS(i);
        }
        temp4_t[i]=temp2_t[i]*RISphase*temp1_t[i];



    }
    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
            pBTSAntenna->GetvAntennaPanels()) {

        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pMSAntennaPanel,
                pMSAntenna->GetvAntennaPanels()) {

            BOOST_FOREACH(std::shared_ptr<CTXRU> pBS_TXRU,
                    pBSAntennaPanel->GetvTXRUs()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pMS_TXRU,
                        pMSAntennaPanel->GetvTXRUs()) {
                    pair<int, int> txrupair = std::make_pair(pBS_TXRU->GetTXRUIndex(), pMS_TXRU->GetTXRUIndex());
                    lm.m_TxRx2CS[txrxid].m_pSCS->m_TXRUPairID_2_FreqH_RIS[txrupair].resize(iFreSampleNum);
                    for (int k = 0; k < iFreSampleNum; ++k) {
                        lm.m_TxRx2CS[txrxid].m_pSCS->m_TXRUPairID_2_FreqH_RIS[txrupair][k]= temp4_t[k](pMS_TXRU->GetTXRUIndex(),pBS_TXRU->GetTXRUIndex());
                    }
                }
            }
        }
    }

}


void RISMatrix::CalH_Total(MS& ms){
    BTS& ServeBTS = ms.GetAllServBTSs()[0].GetBTS();
    RIS& ServeRIS = ServeBTS.GetRIS(0);
    Tx& tx = *ServeBTS.m_pTxNode;
    Rx& rx = *ms.m_pRxNode;
    TxRxID txrxid = std::make_pair(ServeBTS.GetTxID(), ms.GetRxID());
    TxRISID txrisid = std::make_pair(ServeBTS.GetTxID(),ServeRIS.GetRISID().GetTotalIndex());
    RISRxID risrxid = std::make_pair(ServeRIS.GetRISID().GetTotalIndex(),ms.GetRxID());
    std::shared_ptr<cm::Antenna> pBTSAntenna = tx.GetAntennaPointer();
    std::shared_ptr<cm::Antenna> pRISAntenna = ServeRIS.GetAntennaPointer();
    std::shared_ptr<cm::Antenna> pMSAntenna = rx.GetAntennaPointer();
    int BTS_TXRU_NUM=pBTSAntenna->GetTotalTXRU_Num();
    int RIS_TXRU_NUM=pRISAntenna->GetTotalTXRU_Num();
    int MS_TXRU_NUM=pMSAntenna->GetTotalTXRU_Num();
    int iFreSampleNum = P::s().FX.ICarrierNum / P::s().FX.ICarrierSampleSpace;
    //itpp::cmat temp1 = itpp::zeros_c(RIS_TXRU_NUM, BTS_TXRU_NUM);
    std::vector<itpp::cmat> temp1_t(iFreSampleNum);
    //itpp::cmat temp2 = itpp::zeros_c(MS_TXRU_NUM, RIS_TXRU_NUM);
    std::vector<itpp::cmat> temp2_t(iFreSampleNum);
    //std::vector<itpp::cmat> temp3_t(iFreSampleNum);
    std::vector<itpp::cmat> temp4_t(iFreSampleNum);
    for(int i=0;i<iFreSampleNum;++i){
        temp1_t[i]=itpp::zeros_c(RIS_TXRU_NUM, BTS_TXRU_NUM);
        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
        pBTSAntenna->GetvAntennaPanels()) {

            BOOST_FOREACH(std::shared_ptr<AntennaPanel> pRISAntennaPanel,
                    pRISAntenna->GetvAntennaPanels()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pBS_TXRU,
                pBSAntennaPanel->GetvTXRUs()) {

                    BOOST_FOREACH(std::shared_ptr<CTXRU> pRIS_TXRU,
                    pRISAntennaPanel->GetvTXRUs()) {
                        pair<int, int> txrupair = std::make_pair(pBS_TXRU->GetTXRUIndex(), pRIS_TXRU->GetTXRUIndex());
                        temp1_t[i](pRIS_TXRU->GetTXRUIndex(),pBS_TXRU->GetTXRUIndex())=lm.m_TxRIS2CS[txrisid].m_pSCS->m_TXRUPairID_2_FreqH[txrupair][i];

                    }
                }
            }
        }
        temp2_t[i]=itpp::zeros_c(MS_TXRU_NUM, RIS_TXRU_NUM);
        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pRISAntennaPanel,
        pRISAntenna->GetvAntennaPanels()) {

            BOOST_FOREACH(std::shared_ptr<AntennaPanel> pMSAntennaPanel,
            pMSAntenna->GetvAntennaPanels()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pRIS_TXRU,
                pRISAntennaPanel->GetvTXRUs()) {

                    BOOST_FOREACH(std::shared_ptr<CTXRU> pMS_TXRU,
                    pMSAntennaPanel->GetvTXRUs()) {
                        pair<int, int> txrupair = std::make_pair(pRIS_TXRU->GetTXRUIndex(), pMS_TXRU->GetTXRUIndex());
                        temp2_t[i](pMS_TXRU->GetTXRUIndex(),pRIS_TXRU->GetTXRUIndex())=lm.m_RISRx2CS[risrxid].m_pSCS->m_TXRUPairID_2_FreqH[txrupair][i];


                    }

                }

            }
        }
//        temp3_t[i]=itpp::zeros_c(MS_TXRU_NUM, BTS_TXRU_NUM);
        temp4_t[i]=itpp::zeros_c(MS_TXRU_NUM, BTS_TXRU_NUM);
//        BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
//        pBTSAntenna->GetvAntennaPanels()) {
//
//            BOOST_FOREACH(std::shared_ptr<AntennaPanel> pMSAntennaPanel,
//            pMSAntenna->GetvAntennaPanels()) {
//
//                BOOST_FOREACH(std::shared_ptr<CTXRU> pBS_TXRU,
//                pBSAntennaPanel->GetvTXRUs()) {
//                    BOOST_FOREACH(std::shared_ptr<CTXRU> pMS_TXRU,
//                    pMSAntennaPanel->GetvTXRUs()) {
//                        pair<int, int> txrupair = std::make_pair(pBS_TXRU->GetTXRUIndex(), pMS_TXRU->GetTXRUIndex());
//                        temp3_t[i](pMS_TXRU->GetTXRUIndex(),pBS_TXRU->GetTXRUIndex())=lm.m_TxRx2CS[txrxid].m_pSCS->m_TXRUPairID_2_FreqH[txrupair][i];
//                    }
//                }
//            }
//
//        }
        //itpp::cmat temp3=temp2_t[i]*lm.m_TxRx2CS[txrxid].m_pSCS->tempRIS*temp1_t[i]+temp3_t[i];
        itpp::cmat RISphase=itpp::zeros_c(RIS_TXRU_NUM, RIS_TXRU_NUM);
        for(int i=0;i<RIS_TXRU_NUM;++i){
            //RISphase(i,i)=ms.tempR_PRIS(i);
            RISphase(i,i) = lm.m_TxRx2CS[txrxid].m_pSCS->tempRIS(i);
        }
        temp4_t[i]=temp2_t[i]*RISphase*temp1_t[i];


//        if(ms.GetID().ToInt()==0){
//            if(i<10){
//                for(int m=0;m<MS_TXRU_NUM;++m){
//                    for(int n=0;n<BTS_TXRU_NUM;++n){
//                        Observer::Print((string)"Hd"+to_string(i)) << temp3(m,n) << endl;
//                        Observer::Print((string)"H"+to_string(i)) << temp4_t[i](m,n) << endl;
//
//                    }
//                }
//            }
//        }


    }
    BOOST_FOREACH(std::shared_ptr<AntennaPanel> pBSAntennaPanel,
        pBTSAntenna->GetvAntennaPanels()) {

            BOOST_FOREACH(std::shared_ptr<AntennaPanel> pMSAntennaPanel,
            pMSAntenna->GetvAntennaPanels()) {

                BOOST_FOREACH(std::shared_ptr<CTXRU> pBS_TXRU,
                pBSAntennaPanel->GetvTXRUs()) {
                    BOOST_FOREACH(std::shared_ptr<CTXRU> pMS_TXRU,
                    pMSAntennaPanel->GetvTXRUs()) {
                        pair<int, int> txrupair = std::make_pair(pBS_TXRU->GetTXRUIndex(), pMS_TXRU->GetTXRUIndex());
                        lm.m_TxRx2CS[txrxid].m_pSCS->m_TXRUPairID_2_FreqH_RIS[txrupair].resize(iFreSampleNum);
                        for (int k = 0; k < iFreSampleNum; ++k) {

                            lm.m_TxRx2CS[txrxid].m_pSCS->m_TXRUPairID_2_FreqH_RIS[txrupair][k]= temp4_t[k](pMS_TXRU->GetTXRUIndex(),pBS_TXRU->GetTXRUIndex());
                        }
                    }

                }
            }
        }

}


//
RISMatrix::RISMatrix(void) {
}
//
RISMatrix::~RISMatrix(void) {
}
//
//void RISMatrix::WorkSlot(RIS& _ris, double _dTimeSec) {
//}
//
//double RISMatrix::GetCouplingLossDB(TxNode& _tx, RIS& _ris) {
//    TxRISID txrisid = std::make_pair(_tx.GetTxID(), _ris.GetRISID().GetTotalIndex());
//    return L2DB(m_TxRIS2CS[txrisid].m_pSCS->CalCouplingLoss_linear_u36873_w_BestBeamPair_4_BestPanelPair());
//}
//
//double RISMatrix::GetCouplingLossDB(RIS& _ris, RxNode& _rx) {
//    RISRxID risrxid = std::make_pair(_ris.GetRISID().GetTotalIndex(), _rx.GetRxID());
//    return L2DB(m_RISRx2CS[risrxid].m_pSCS->m_dStrongestCouplingLoss_Linear);
//}
//
//
//double RISMatrix::GetShadowFadeDB(Tx& _tx, RIS& _ris) {
//    TxRISID txrisid = std::make_pair(_tx.GetTxID(), _ris.GetRISID().GetTotalIndex());
//    return m_TxRIS2CS[txrisid].m_BCS.m_ShadowFadingDB;
//}
//
//double RISMatrix::GetShadowFadeDB(RIS& _ris, Rx& _rx) {
//    RISRxID risrxid = std::make_pair(_ris.GetRISID().GetTotalIndex(), _rx.GetRxID());
//    return m_RISRx2CS[risrxid].m_BCS.m_ShadowFadingDB;
//}
//
//
//double RISMatrix::Get2DDistanceM(Tx& _tx, RIS& _ris) {
//    TxRISID txrisid = std::make_pair(_tx.GetTxID(), _ris.GetRISID().GetTotalIndex());
//    return m_TxRIS2CS[txrisid].m_BCS.m_d2DDistanceM;
//}
//
//double RISMatrix::Get2DDistanceM(RIS& _ris, Rx& _rx) {
//    RISRxID risrxid = std::make_pair(_ris.GetRISID().GetTotalIndex(), _rx.GetRxID());
//    return m_RISRx2CS[risrxid].m_BCS.m_d2DDistanceM;
//}
//
//
//double RISMatrix::Get3DDistanceM(Tx& _tx, RIS& _ris) {
//    TxRISID txrisid = std::make_pair(_tx.GetTxID(), _ris.GetRISID().GetTotalIndex());
//    return m_TxRIS2CS[txrisid].m_BCS.m_d3DDistanceM;
//}
//
//double RISMatrix::Get3DDistanceM(RIS& _ris, Rx& _rx) {
//    RISRxID risrxid = std::make_pair(_ris.GetRISID().GetTotalIndex(), _rx.GetRxID());
//    return m_RISRx2CS[risrxid].m_BCS.m_d3DDistanceM;
//}
//
//
//double RISMatrix::GetPathLossDB(Tx& _tx, RIS& _ris) {
//    TxRISID txrisid = std::make_pair(_tx.GetTxID(), _ris.GetRISID().GetTotalIndex());
//    return m_TxRIS2CS[txrisid].m_BCS.m_PathLossDB;
//}
//
//double RISMatrix::GetPathLossDB(RIS& _ris, Rx& _rx) {
//    RISRxID risrxid = std::make_pair(_ris.GetRISID().GetTotalIndex(), _rx.GetRxID());
//    return m_RISRx2CS[risrxid].m_BCS.m_PathLossDB;
//}
//
//
//boost::shared_ptr<PathLoss> RISMatrix::GetPathLossFun(Tx& _tx, RIS& _ris, bool m_bIsLOS) {
//    boost::shared_ptr<PathLoss> result;
//    if (m_bIsLOS)
//        result = boost::shared_ptr<PathLoss > (new BS2RISPathLossUrbanMacroOutdoorLOS());
//    else
//        result = boost::shared_ptr<PathLoss > (new BS2RISPathLossUrbanMacroOutdoorNLOS());
//
//    return result;
//}
//
//boost::shared_ptr<PathLoss> RISMatrix::GetPathLossFun(RIS& _ris, Rx& _rx, bool m_bIsLOS) {
//    RISRxID risrxid = std::make_pair(_ris.GetRISID().GetTotalIndex(), _rx.GetRxID());
//    boost::shared_ptr<PathLoss> result;
//    if (m_bIsLOS)
//        result = boost::shared_ptr<PathLoss > (new RIS2MSPathLossUrbanMacroOutdoorLOS());
//    else
//        result = boost::shared_ptr<PathLoss > (new RIS2MSPathLossUrbanMacroOutdoorNLOS());
//
//    return result;
//}


//void RISMatrix::SetPos2Din(Tx& _tx, Rx& _rx) {
//    if (_rx.GetSpecial() == 1) {
//        std::pair<int, int> pos(_tx.GetX() + _rx.GetX(), _tx.GetY() + _rx.GetY());
//        if (m_Pos2Din.find(pos) == m_Pos2Din.end()) {
//
//            if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeA) {
//                if (P::s().IChannelModel_for_Scenario == P::UMA
//                        || P::s().IChannelModel_for_Scenario == P::UMI) {
//
//                    if (P::s().FX.DRadioFrequencyMHz_Macro * 1e-3 <= 6.0) {
//                        m_Pos2Din[pos] = xUniform_channel(0, 25);
//                    } else {
//                        double rand_d1 = xUniform_channel(0, 25);
//                        double rand_d2 = xUniform_channel(0, 25);
//                        double d_in = min(rand_d1, rand_d2);
//                        m_Pos2Din[pos] = d_in;
//                    }
//                } else if (P::s().IChannelModel_for_Scenario == P::RMA) {
//                    if (P::s().FX.DRadioFrequencyMHz_Macro * 1e-3 <= 6.0) {
//                        m_Pos2Din[pos] = xUniform_channel(0, 10);
//                    } else {
//                        double rand_d1 = xUniform_channel(0, 10);
//                        double rand_d2 = xUniform_channel(0, 10);
//                        double d_in = min(rand_d1, rand_d2);
//                        m_Pos2Din[pos] = d_in;
//                    }
//                }
//            } else if (P::s().IChannelModel_VariantMode == P::ITU_ChannelModel_ModeB) {
//                if (P::s().IChannelModel_for_Scenario == P::UMA
//                        || P::s().IChannelModel_for_Scenario == P::UMI) {
//
//                    double rand_d1 = xUniform_channel(0, 25);
//                    double rand_d2 = xUniform_channel(0, 25);
//                    double d_in = min(rand_d1, rand_d2);
//                    m_Pos2Din[pos] = d_in;
//
//                } else if (P::s().IChannelModel_for_Scenario == P::RMA) {
//
//                    double rand_d1 = xUniform_channel(0, 10);
//                    double rand_d2 = xUniform_channel(0, 10);
//                    double d_in = min(rand_d1, rand_d2);
//                    m_Pos2Din[pos] = d_in;
//                }
//            } else {
//                assert(false);
//            }
//        }
//    }
//}

//double RISMatrix::GetPos2Din(std::pair<int, int> & _pos) {
//    return m_Pos2Din[_pos];
//}

//bool RISMatrix::IsStrong(Tx& _tx, Rx& _rx) {
//    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
//    return m_TxRx2CS[txrxid].m_bIsStrong;
//}
//
//bool RISMatrix::IsLOS(Tx& _tx, Rx& _rx) {
//    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
//    return m_TxRx2CS[txrxid].m_BCS.m_bIsLOS;
//}

RISMatrix& RISMatrix::Instance() {
    if (m_pRM == 0) {
        m_pRM = new RISMatrix;
    }
    return *m_pRM;
}

//double RISMatrix::GetLoSAoDRAD(Tx& _tx, Rx& _rx) {
//    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
//    return m_TxRx2CS[txrxid].m_BCS.m_AODLOSRAD;
//}
//
//double RISMatrix::GetLoSEoDRAD(Tx& _tx, Rx& _rx) {
//    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
//    return m_TxRx2CS[txrxid].m_BCS.m_EODLOSRAD;
//}
//
//double RISMatrix::GetLoSAoARAD(Tx& _tx, Rx& _rx) {
//    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
//    return m_TxRx2CS[txrxid].m_BCS.m_AOALOSRAD;
//}
//
//double RISMatrix::GetLoSEoARAD(Tx& _tx, Rx& _rx) {
//    TxRxID txrxid = std::make_pair(_tx.GetTxID(), _rx.GetRxID());
//    return m_TxRx2CS[txrxid].m_BCS.m_EOALOSRAD;
//}
