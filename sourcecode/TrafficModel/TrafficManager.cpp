#include "TrafficManager.h"
#include "FTPModel.h"
#include "FTPModel2.h"
#include "FTPModel3.h"
#include "XRModel.h"
#include "XRmultiModel.h"
#include "MixedTraffic.h"

TrafficManager::TrafficManager() {
    switch (Parameters::Instance().TRAFFIC.ITrafficModel) {
        case Parameters::ITrafficModel_XR:
            m_pTrafficModel =std::shared_ptr<TrafficModel> (new MixedTraffic(1,0,0,0,0));
            break;
        case Parameters::ITrafficModel_XRmulti:
            m_pTrafficModel =std::shared_ptr<TrafficModel> (new MixedTraffic(0,1,0,0,0));
            break;
        case Parameters::ITrafficModel_FTP3:
            m_pTrafficModel =std::shared_ptr<TrafficModel> (new MixedTraffic(0,0,1,0,0));
            break;
        case Parameters::ITrafficModel_FullBuffer:
            m_pTrafficModel =std::shared_ptr<TrafficModel> (new MixedTraffic(0,0,0,1,0));
            break;
        case Parameters::DataBaseTraffic:
            m_pTrafficModel =std::shared_ptr<TrafficModel> (new MixedTraffic(0,0,0,0,1));
            break;
        case Parameters::MixedTraffic:
            m_pTrafficModel =std::shared_ptr<TrafficModel> (new MixedTraffic(0,0,0,0,1));
            break;
//        case Parameters::ITrafficModel_FTP:
//            m_pTrafficModel = std::shared_ptr<TrafficModel > (new FTPModel());
//            break;
//        case Parameters::ITrafficModel_FTP2:
//            m_pTrafficModel = std::shared_ptr<TrafficModel > (new FTPModel2(Parameters::Instance().TRAFFIC.DPacketSize, Parameters::Instance().TRAFFIC.DMacroLamda));
//            break;
//        case Parameters::ITrafficModel_FTP3:
//            m_pTrafficModel = std::shared_ptr<TrafficModel > (new FTPModel3(Parameters::Instance().TRAFFIC.DPacketSize, Parameters::Instance().TRAFFIC.DMacroLamda));
//            break;
//        case Parameters::ITrafficModel_XR:
//            m_pTrafficModel = std::shared_ptr<TrafficModel > (new XRModel());
//            break;
//        case Parameters::ITrafficModel_XRmulti:
//            m_pTrafficModel = std::shared_ptr<TrafficModel > (new XRmultiModel());
//            break;
//        case Parameters::MixedTraffic:
//            m_pTrafficModel =std::shared_ptr<TrafficModel> (new MixedTraffic());
//            break;
        default:
            cout << "The value of Parameters::Instance().TRAFFIC.ITrafficMode is uncorrect, please check it." << std::endl;
            assert(false);
            break;
    }
}

void TrafficManager::WorkSlot(){
    m_pTrafficModel->WorkSlot();
}

void TrafficManager::OutputTrafficInfo(){
    m_pTrafficModel->OutputTrafficInfo();
}