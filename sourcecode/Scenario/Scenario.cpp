
#include "Scenario.h"
#include "../Parameters/Parameters.h"
#include "UrbanMicro.h"
#include "UrbanMacro.h"
#include "RuralMacro.h"
#include "RISUrbanMacro.h"
#include "IndoorHotspot.h"
#include "HetNet.h"

using namespace cm;

Scenario* Scenario::m_pScene = 0;

///构造函数
Scenario::Scenario(void){
    
}

///析构函数
Scenario::~Scenario(void){
}

void Scenario::InitializeMap() {
    m_pDSMapLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_LOS.DSCorrDistM));
    m_pAODMapLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_LOS.ASDCorrDistM));
    m_pAOAMapLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_LOS.ASACorrDistM));
    m_pSFMapLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_LOS.SFCorrDistM));
    m_pKMapLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_LOS.KCorrDistM));

    m_pDSMapNLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_NLOS.DSCorrDistM));
    m_pAODMapNLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_NLOS.ASDCorrDistM));
    m_pAOAMapNLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_NLOS.ASACorrDistM));
    m_pSFMapNLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_NLOS.SFCorrDistM));

    m_pEODMapLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_LOS.ESDCorrDistM));
    m_pEOAMapLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_LOS.ESACorrDistM));
    m_pEODMapNLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_NLOS.ESDCorrDistM));
    m_pEOAMapNLOS = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_NLOS.ESACorrDistM));
    
    m_pDSMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.DSCorrDistM));
    m_pAODMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.ASDCorrDistM));
    m_pAOAMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.ASACorrDistM));
    m_pSFMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.SFCorrDistM));

    m_pEODMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.ESDCorrDistM));
    m_pEOAMapO2I = std::shared_ptr<GaussianMap > (new GaussianMap(P::s().Macro2UE_O2I.ESACorrDistM));
        
    m_LOSORNLOS.clear();
}

double Scenario::GenPos2Din(std::pair<int, int> pos){
    cout << "调用基类2D距离计算函数，出错" << endl;
    assert(false);
    return 0.0;
}

Scenario& Scenario::Instance() {
    if(m_pScene == 0){
        switch (Parameters::Instance().BASIC.IScenarioModel) {
            case Parameters::SCENARIO_LowFreq_INDOOR:
            case Parameters::SCENARIO_HighFreq_INDOOR:
                m_pScene = new IndoorHotspot;
                break;
            case Parameters::SCENARIO_LowFreq_RURAL_MACRO:
            case Parameters::SCENARIO_HighFreq_RURAL_MACRO:
                m_pScene = new RuralMacro;
                break;
            case Parameters::SCENARIO_LowFreq_URBAN_MACRO:
            case Parameters::SCENARIO_HighFreq_URBAN_MACRO:
                m_pScene = new UrbanMacro;
                break;
            case Parameters::SCENARIO_LowFreq_URBAN_MICRO:
            case Parameters::SCENARIO_HighFreq_URBAN_MICRO:
                m_pScene = new UrbanMicro;
                break;
            case Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER:
                m_pScene = new HetNet;
                break;
            case Parameters::SCENARIO_RIS:
                m_pScene = new RISUrbanMacro;
                break;                
            default:
                cout << "Framework Error!" << endl;
                assert(false);
                break;
        }
    }
    return *m_pScene;
}