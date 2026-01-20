#include "OneLayerNetwork.h"

#include "../NetworkDrive/Clock.h"
#include "../Parameters/Parameters.h"
#include "../DistributeMSAlgo/DistributeMSAlgo4indoor.h"
#include "../DistributeMSAlgo/DistributeMSAlgoCenterRectangle.h"
#include "../DistributeMSAlgo/DistributeMSAlgoDiamond.h"
#include "../DistributeMSAlgo/DistributeMSAlgoFile.h"
#include "../DistributeMSAlgo/DistributeMSAlgoHexagon.h"
#include "../DistributeMSAlgo/DistributeMSAlgoFix.h"


///构造函数
OneLayerNetwork::OneLayerNetwork(void){
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

///析构函数
OneLayerNetwork::~OneLayerNetwork(void){
}

///基站位置分布初始化
void OneLayerNetwork::DistributeBSs() {
    StandardDistribute();
}