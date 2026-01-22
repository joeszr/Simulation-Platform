#include "FrameRIS.h"

#include "Framework.h"
#include "../NetworkDrive/Clock.h"
#include "../ChannelModel/LinkMatrix.h"
#include "RIS/RISMatrix.h"
#include "../Parameters/Parameters.h"
#include "../Statistician/Statistician.h"
#include "../DistributeMSAlgo/DistributeMSAlgo4indoor.h"
#include "../DistributeMSAlgo/DistributeMSAlgoCenterRectangle.h"
#include "../DistributeMSAlgo/DistributeMSAlgoDiamond.h"
#include "../DistributeMSAlgo/DistributeMSAlgoFile.h"
#include "../DistributeMSAlgo/DistributeMSAlgoHexagon.h"
#include "../DistributeMSAlgo/DistributeMSAlgoFix.h"
#include"NetworkDrive/MSManager.h"
 ///构造函数
FrameRIS::FrameRIS(void){
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
FrameRIS::~FrameRIS(void){
}

///基站位置分布初始化
void FrameRIS::DistributeBSs() {
    StandardDistribute();
}
//用户初始化
//void FrameRIS::InitializeMSs(){
//    MSManager& msm = MSManager::Instance();
//    vector<std::shared_ptr<MS> >& v_MS = msm.GetvMS();
//    for (auto it = v_MS.begin(); it != v_MS.end(); ++it) {
//        (*it)->Initialize_for_RIS();
//    }
//}

void FrameRIS::InitFrame(){
    ////////////////////System Init////////////////////
    if(!Framework::systemInitFinished){
        ChannelParameterInitialize(); //初始化信道参数
        cout<<"RISInit"<<endl;
        ///添加基站
        BSManager::Instance().AddBSs();
        ///配置基站的位置，以及BTS的位置和天线方向
        DistributeBSs();
        PrintHead();
        cout << "ok!" << endl;

    }
    /////////////////////Drop Init/////////////////////
    //将时钟置回零点
    Clock::Instance().Reset();
    //添加MS
    MSManager::Instance().AddMS();
    //添加RIS
    if(!Framework::systemInitFinished){
        BSManager::Instance().AddRISs();
        BSManager::Instance().DistributeRISs();
        systemInitFinished=true;
    }
    //添加用户MS
    DistributeMSs();
    cout << "DistributeMSs() Succeeded!!!!!!!" << endl;
    //初始化每个MS到各个BTS的路损和阴影衰落
    cm::LinkMatrix::Instance().Initialize();
    cout << "LinkMatrix Initialize() Succeeded!!!!!!!!!!!!!!!!!!!!!" << endl;

    InitializeMSs();
    cout << "InitializeMSs() Succeeded!!!!!!!!!!!!!!!!!!" << endl;
    if(Parameters::Instance().BASIC.BISRIS){//添加RIS情况下
        //初始化RIS
        cm::LinkMatrix::Instance().RISInitialize();
        cout << "RIS Initialize() Succeeded!!!!!!!!!!!!!!!!!!!!!" << endl;
    }
    //初始化基站
    InitializeBSs();
    cout << "InitializeBSs() Succeeded!!!!!!!!!!!!!!!!!!" << endl;
    //建立MS和BTS的数据统计结构体
    Statistician::Instance().Initialize();

    /*
    //确定业务类型和确定主服务BTS，确定Geometry
    InitializeMSs();
    cout << "InitializeMSs() Succeeded!!!!!!!!!!!!!!!!!!" << endl;
    //初始化RIS大尺度链路
    cm::LinkMatrix::Instance().RISInitialize();
    if(Parameters::Instance().RIS.IS_SmallScale==true||Parameters::Instance().BASIC.ISlotPerDrop>0){
        if(Parameters::Instance().BASIC.BISRIS){
            cm::RISMatrix::Instance().Initialize();
        }
       //初始化基站
        InitializeBSs();
        cout << "InitializeBSs() Succeeded!!!!!!!!!!!!!!!!!!" << endl;
        //建立MS和BTS的数据统计结构体
        Statistician::Instance().Initialize();
    }*/
}

void FrameRIS::PrintHead() {
    // 调用基类的PrintHead方法
    Framework::PrintHead();
}

