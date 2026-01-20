#include "IndoorNetwork.h"
#include "../NetworkDrive/Clock.h"
#include "../Parameters/Parameters.h"
#include "../DistributeMSAlgo/DistributeMSAlgo4indoor.h"
#include "../DistributeMSAlgo/DistributeMSAlgoCenterRectangle.h"
#include "../DistributeMSAlgo/DistributeMSAlgoDiamond.h"
#include "../DistributeMSAlgo/DistributeMSAlgoFile.h"
#include "../DistributeMSAlgo/DistributeMSAlgoHexagon.h"
#include "../DistributeMSAlgo/DistributeMSAlgoFix.h"


///构造函数
IndoorNetwork::IndoorNetwork(void){
    //子类构造函数最后调用，覆盖OneLayer的值
    //For Indoor Hot Spot Case 
    m_pDMSA = std::shared_ptr<DistributeMSAlgo > (new DistributeMSAlgo4indoor);
}

///析构函数
IndoorNetwork::~IndoorNetwork(void){
}

///基站位置分布初始化
void IndoorNetwork::DistributeBSs() {
    BSManager& bsm = BSManager::Instance();
    double x, y;
    vector<std::shared_ptr<BS> >& vpBS = bsm.GetvpBS();
    int iNumBSs = vpBS.size();
    if (iNumBSs == 2) {
        assert(Parameters::Instance().BASIC.IBTSPerBS == 1);
        vpBS[0]->SetXY(-30, 0);
        vpBS[1]->SetXY(30, 0);
        for (int i = 0; i < iNumBSs; ++i) {
            BS& bs = bsm.GetBS(i);
            double x = bs.GetX();
            double y = bs.GetY();
            BTS& bts = bs.GetBTS(0);
            bts.SetXY(x, y);
            bts.SetTxOrientRAD(0);
            bts.SetTxHeightM(Parameters::Instance().Macro.DAntennaHeightM);
        }
    }///4基站(1 floor)矩形拓扑初始化
    else if (iNumBSs == 4) {
        assert(Parameters::Instance().BASIC.IBTSPerBS == 1);
        vpBS[0]->SetXY(-45, 0);
        vpBS[1]->SetXY(-15, 0);
        vpBS[2]->SetXY(15, 0);
        vpBS[3]->SetXY(45, 0);
        for (int i = 0; i < iNumBSs; ++i) {
            BS& bs = bsm.GetBS(i);
            double x = bs.GetX();
            double y = bs.GetY();
            BTS& bts = bs.GetBTS(0);
            bts.SetXY(x, y);
            bts.SetTxOrientRAD(0);
            bts.SetTxHeightM(Parameters::Instance().Macro.DAntennaHeightM);
        }
    }//hf 12 bs indoor open and mixed office
    else if (iNumBSs == 12) {
        for (int i = 0; i != 6; ++i)
            vpBS[i]->SetXY(-50 + i * 20, 10);
        for (int i = 6; i != 12; ++i)
            vpBS[i]->SetXY(50 - (i - 6)*20, -10);
        for (int i = 0; i < iNumBSs; ++i) {
            BS& bs = bsm.GetBS(i);
            double x = bs.GetX();
            double y = bs.GetY();
            if (Parameters::Instance().BASIC.IBTSPerBS == 3) {
                for (int j = 0; j < Parameters::Instance().BASIC.IBTSPerBS; ++j) {
                    BTS& bts = bs.GetBTS(j);
                    bts.SetXY(x, y);
                    bts.SetTxOrientRAD(j * 2 * M_PI / 3); // 要不要+30 ？section 7.8
                    bts.SetTxHeightM(Parameters::Instance().Macro.DAntennaHeightM);
                }
            } else if (Parameters::Instance().BASIC.IBTSPerBS == 1) {
                BTS& bts = bs.GetBTS(0);
                bts.SetXY(x, y);
                bts.SetTxOrientRAD(0);
                bts.SetTxHeightM(Parameters::Instance().Macro.DAntennaHeightM);
            } else {
                cout << "Error in BSManager::DistributeBSs()!" << endl;
            }
        }
    }
}

void IndoorNetwork::SetMSPara(){
    int MSCount = MSManager::Instance().CountMS();
    vector<std::shared_ptr<MS> >& v_pms = MSManager::Instance().GetpMS();
    
    for(int i = 0; i < MSCount; ++i){
        v_pms[i]->m_pRxNode->m_iSpecial = 1;
        v_pms[i]->m_pRxNode->m_dInCarLossDB = 0;
    }
}