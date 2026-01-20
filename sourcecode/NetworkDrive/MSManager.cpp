///@file  MSManager .cpp
///@brief 移动台管理类实现
///@author wangxiaozhou

#include "../Parameters/Parameters.h"
#include "../BaseStation/BTSID.h"
#include "../BaseStation/BS.h"
#include "../DistributeMSAlgo/DistributeMSAlgoCenterRectangle.h"
#include "../DistributeMSAlgo/DistributeMSAlgoDiamond.h"
#include "../DistributeMSAlgo/DistributeMSAlgoHexagon.h"
#include "../DistributeMSAlgo/DistributeMSAlgoFile.h"
#include "../DistributeMSAlgo/DistributeMSAlgo4indoor.h"
#include "../DistributeMSAlgo/DistributeMSAlgoForHetNet.h"
#include "Clock.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../ChannelModel/LinkMatrix.h"
#include "MSManager.h"
#include "BSManager.h"
#include <thread>
//chty 1103
#include "../ChannelModel/LinkMatrix.h"
//chty 1103
MSManager* MSManager::m_pMSM = nullptr;

MSManager::MSManager() {
    if (Parameters::Instance().BASIC.IScenarioModel ==  Parameters::SCENARIO_LowFreq_INDOOR
            ||Parameters::Instance().BASIC.IScenarioModel ==  Parameters::SCENARIO_HighFreq_INDOOR) {
        // For Indoor Hot Spot Case
        m_pDMSA = std::shared_ptr<DistributeMSAlgo > (new DistributeMSAlgo4indoor);
    } else if (Parameters::Instance().BASIC.IScenarioModel ==  Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER) {
        // For HetNet
        m_pDMSA = std::shared_ptr<DistributeMSAlgo > (new DistributeMSAlgoForHetNet);
    } else  {
        switch (Parameters::Instance().BASIC.IMsDistributeMode) {
            case Parameters::DistributeMS_Hexagonal:
                m_pDMSA = std::shared_ptr<DistributeMSAlgo > (new DistributeMSAlgoHexagon);
                break;
            case Parameters::DistributeMS_Diamond:
                m_pDMSA = std::shared_ptr<DistributeMSAlgo > (new DistributeMSAlgoDiamond);
                break;
            case Parameters::DistributeMS_CenterRetangle:
                m_pDMSA = std::shared_ptr<DistributeMSAlgo > (new DistributeMSAlgoCenterRectangle);
                break;
            case Parameters::DistributeMS_File:
                m_pDMSA = std::shared_ptr<DistributeMSAlgo > (new DistributeMSAlgoFile);
                break;
            default:
                assert(false);
                break;
        }
    }
    m_bExistMultiTRP = false;//20260119
}



MS& MSManager::GetMS(int _id) {
    return m_vMS[_id];
}

void MSManager::AddMS() {
    if(Parameters::Instance().BASIC.ifcoexist){
        for (int i = 0; i < floor(Parameters::Instance().BASIC.ITotalMSNum*Parameters::Instance().BASIC.type1MSRate+0.5); ++i) {
            m_vMS.emplace_back(MS(i,1));
        }
        for(int i=floor(Parameters::Instance().BASIC.ITotalMSNum*Parameters::Instance().BASIC.type1MSRate+0.5);i<Parameters::Instance().BASIC.ITotalMSNum;i++){
            m_vMS.emplace_back(MS(i,2));
        }
    }
    else{
        for (int i = 0; i < (Parameters::Instance().BASIC.ITotalMSNum); ++i) {
            m_vMS.emplace_back(MS(i));
        }
    }
}

void MSManager::DistributeMSs() {
    // 20171204
    //chty 1103

//    int iAllMS = this->CountMS();
//    for (int ims = 0; ims < iAllMS; ims++) {
//        this->DistributeMSs(
//                this->GetMS(ims) );
//    }

    if(Parameters::Instance().BASIC.BISMultiThread) {
        vector<std::thread> group;
        for (int i = 0; i < cm::LinkMatrix::threadnum; ++i) {
            group.emplace_back(std::thread(bind(&MSManager::DistributeMSthread, this, i)));
        }
        for (auto &onethread: group) {
            onethread.join();
        }
    }else{
        int iAllMS = this->CountMS();
        for (int ims = 0; ims < iAllMS; ims++) {
            this->DistributeMSs(
                    this->GetMS(ims) );
        }
    }

    //chty 1103
    
//    m_pDMSA->DistributeMS(*this);
}

void MSManager::DistributeMSs(MS& _ms) {
    
//    m_pDMSA->DistributeMS(_ms);
    
    // 20171204
    m_pDMSA->DistributeMS_with_distance_check(_ms);
}

void MSManager::InitializeMSs() {
//    Observer::SetIsEnable(true);
//    Observer::Print("MacroMS")<<"MSID"<<setw(20)<<"MainServ"<<setw(20)<<"GeometryDB"<<setw(20)<<"PathLossDB"<<setw(20)<<"LinkLossDB"<<setw(20)<<"PurePathLossDB"<<setw(20)<<"ShadowFadeDB"<<setw(20)<<"TxGainDB"<<setw(20)<<"DistanceM"<<endl;
//    Observer::Print("PicoMS")<<"MSID"<<setw(20)<<"MainServ"<<setw(20)<<"GeometryDB"<<setw(20)<<"PathLossDB"<<setw(20)<<"LinkLossDB"<<setw(20)<<"PurePathLossDB"<<setw(20)<<"ShadowFadeDB"<<setw(20)<<"TxGainDB"<<setw(20)<<"DistanceM"<<endl;
    
//@threads
//    if(Parameters::Instance().BASIC.BISMultiThread){
//        cout<<"Initializing MSs"<<endl;
//            vector<std::thread> group;
//            for (vector<MS>::iterator it = m_vMS.begin(); it != m_vMS.end(); ++it) {
//                //@threads
////        while (!(*it).Initialize()) {
////            //assert(false);
////            this->DistributeMSs(*it);
////            cm::LinkMatrix::Instance().Initialize(*it);
////        }
//                group.emplace_back(std::thread(&MSManager::reInitialze, std::ref(*it)));
//            }
//            for (auto it = group.begin(); it != group.end(); ++it) {
//                it->join();
//            }
//    }
//    else{
        for (vector<MS>::iterator it = m_vMS.begin(); it != m_vMS.end(); ++it) {
            reInitialze(*it);
        }
//    }
//@threads
}

void MSManager::WorkSlot() {
    if (!Parameters::Instance().BASIC.BISMultiThread) {
        for (auto& ms : m_vMS) {
            ms.WorkSlot();
        }
    }
}

void MSManager::Reset() {
    for (int i = 0; i < MSManager::Instance().CountMS(); ++i) {
        m_vMS[i].Reset();
    }
    m_vMS.clear();
}

MSManager& MSManager::Instance() {
    if (!m_pMSM) {
        m_pMSM = new MSManager;
    }
    return *m_pMSM;
}

int MSManager::CountMS()const {
    return static_cast<int> (m_vMS.size());
}


//@threads
void MSManager::reInitialze(MS& ms){
    while (!ms.Initialize()) {
        MSManager::Instance().DistributeMSs(ms);
        cm::LinkMatrix::Instance().Initialize(ms);
    }
}
//@threads
//chty 1103
void MSManager::DistributeMSthread(int n){
    static int iAllMS = this->CountMS();
    for (int i = 0; i < iAllMS; ++i) {
        if (i % (cm::LinkMatrix::threadnum) != n)continue;
        DistributeMSs(GetMS(i));
    }
}
///20260119
vector<std::shared_ptr<MS> >& MSManager::GetpMS(){
    return v_pMS;
}