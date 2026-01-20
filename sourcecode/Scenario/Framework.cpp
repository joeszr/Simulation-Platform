#include "../ChannelModel/LinkMatrix.h"
#include "../NetworkDrive/MSManager.h"
#include "../NetworkDrive/BSManager.h"
#include "IndoorHotspot.h"
#include "RuralMacro.h"
#include "UrbanMacro.h"
#include "UrbanMicro.h"
#include "TwoLayerNetwork.h"
#include "OneLayerNetwork.h"
#include "IndoorNetwork.h"
#include "FrameRIS.h"
#include "../Statistician/Statistician.h"
#include "../Statistician/Observer.h"
#include "../NetworkDrive/Clock.h"
using namespace std;

Framework* Framework::m_pFramework = 0;
///构造函数
Framework::Framework(void){
    systemInitFinished=false;
}

///析构函数
Framework::~Framework(void){
}

void Framework::InitFrame(){
    if(!systemInitFinished){
        ////////////////////System Init////////////////////
        ChannelParameterInitialize(); //初始化信道参数
        ///添加基站
        BSManager::Instance().AddBSs();
        ///配置基站的位置，以及BTS的位置和天线方向
        DistributeBSs();
        PrintHead();
        cout << "ok!" << endl;
        systemInitFinished=true;
        
    }
    /////////////////////Drop Init/////////////////////
    //将时钟置回零点
    Clock::Instance().Reset();
    //添加MS
    MSManager::Instance().AddMS();
    DistributeMSs();
    cout << "DistributeMSs() Succeeded!!!!!!!" << endl;
    //添加RIS
    if (Parameters::Instance().BASIC.BISRIS){
        BSManager::Instance().AddRISs();
        BSManager::Instance().DistributeRISs();    
    }
    //初始化每个MS到各个BTS的路损和阴影衰落
    cm::LinkMatrix::Instance().Initialize();
    cout << "LinkMatrix Initialize() Succeeded!!!!!!!!!!!!!!!!!!!!!" << endl;
    //确定业务类型和确定主服务BTS，确定Geometry
    InitializeMSs();
    cout << "InitializeMSs() Succeeded!!!!!!!!!!!!!!!!!!" << endl;
    //初始化基站
    InitializeBSs();
    cout << "InitializeBSs() Succeeded!!!!!!!!!!!!!!!!!!" << endl;
    //建立MS和BTS的数据统计结构体
    Statistician::Instance().Initialize();
}

void Framework::PrintHead() {
    ///打印输出表的表头，即将统计数据的文件输出相应表头。统计类在生成类对象的时候，建立相应的输出文件流
    Statistician::Instance().PrintHead();
    { 
        Observer::Print("LargeScale")<<"txid"
        << setw(20) << "rxid"
        << setw(20) <<"m_PathLossDB"
        << setw(20) << "m_ShadowFadingDB"
        << setw(20) << "m_dTxAntennaPatternDB"
        << setw(20) << "m_dRxAntennaPatternDB"
        << setw(20) << "dInCarLossDB"
        << setw(20) << "m_LoSLinkLossDB"
        << setw(20) << "m_PurePathLossDB"
        << setw(20) << "m_bIsLOS"
        << setw(20) << "m_d3DDistanceM"
        << setw(20) << "m_d2DDistanceM"     
        << endl;
    }
    if(Parameters::Instance().BASIC.IRISCASE){
        Observer::Print("LargeScale1")<<"txid"
        << setw(20) << "rxid"
        << setw(20) <<"m_PathLossDB"
        << setw(20) << "m_ShadowFadingDB"
        << setw(20) << "m_dTxAntennaPatternDB"
        << setw(20) << "m_dRxAntennaPatternDB"
        << setw(20) << "dInCarLossDB"
        << setw(20) << "m_LoSLinkLossDB"
        << setw(20) << "m_PurePathLossDB"
        << setw(20) << "m_bIsLOS"
        << setw(20) << "m_d3DDistanceM"
        << setw(20) << "m_d2DDistanceM"
        << endl;
        Observer::Print("LargeScale2")<<"txid"
        << setw(20) << "rxid"
        << setw(20) <<"m_PathLossDB"
        << setw(20) << "m_ShadowFadingDB"
        << setw(20) << "m_dTxAntennaPatternDB"
        << setw(20) << "m_dRxAntennaPatternDB"
        << setw(20) << "dInCarLossDB"
        << setw(20) << "m_LoSLinkLossDB"
        << setw(20) << "m_PurePathLossDB"
        << setw(20) << "m_bIsLOS"
        << setw(20) << "m_d3DDistanceM"
        << setw(20) << "m_d2DDistanceM"
        << endl;
    }
    {
    Observer::Print("MSEBBRank1Decode") << "iTime" << setw(20)
            << "iHARQID" << setw(20)
            << "m_MSID" << setw(20)
            << "m_MainServBTS" << setw(20)
            << "iSBNum" << setw(20)
            << " iRank" << setw(20)
            << "iSendNum" << setw(20)
            << " iRankID " << setw(20)
            << "iMCS " << setw(20)
            << "dOlla" << setw(20)
            << "dRawSINRDB" << setw(20)
            << "dPostSINRDB" << setw(20)
            << "dBLER" << setw(20)
            << "bRight " << setw(20)
            << "AllocatedSB" << endl;
    }
    if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP || 
        Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP2) {
        Observer::Print("FTPPacket") << setw(20) << "ID"
            << setw(20) << "MSID"
            << setw(20) << "OriginSize"
            << setw(20) << "RemainderSize"
            << setw(20) << "TxSize"
            << setw(20) << "RightRxSize"
            << setw(20) << "IsFinished"
            << setw(20) << "IsCompleted"
            << setw(20) << "BornTime"
            << setw(20) << "FinishTime"
            << setw(20) << "Delay" << endl;
    }
    {
        Observer::Print("ITUCalibration") << "UEID"
            << setw(20) << "UEPosX"
            << setw(20) << "UEPosY"
            << setw(20) << "MSOrientDEG"
            << setw(20) << "UEOrientRAD"
            << setw(20) << "TotalFloorNum"
            << setw(20) << "FloorIndex"
            << setw(20) << "UEHeight"
            << setw(20) << "IsInH"
            << setw(20) << "isLowLoss"

            << setw(20) << "bsidMajor"
            << setw(20) << "btsidMajor"
            << setw(20) << "btsIndexMajor"
            << setw(20) << "BTSPosXMajor"
            << setw(20) << "BTSPosYMajor"
            << setw(20) << "BTSWrapPosXMajor"
            << setw(20) << "BTSWrapPosYMajor"

            << setw(20) << "UEWrapPosXMajor"
            << setw(20) << "UEWrapPosYMajor"
            << setw(20) << "2DDistanceMajor"
            << setw(20) << "3DDistanceMajor"

            << setw(20) << "IsStrongLinkMajor"
            << setw(20) << "IsLOSMajor"
            << setw(20) << "LoSAoDMajor"
            << setw(20) << "LoSEoDMajor"
            << setw(20) << "LoSAoAMajor"
            << setw(20) << "LoSEoAMajor"

            << setw(20) << "LinkLossDBMajor"
            << setw(20) << "PathLossDBMajor"
            << setw(20) << "ShadowFadeDBMajor"

            << setw(20) << "BTSBeamIndexMajor"
            << setw(20) << "MSBeamIndexMajor"
            << setw(20) << "PanelIndexMajor"

            << setw(20) << "RSRPMajor"
            << setw(20) << "CouplingLossMajor"
            << setw(20) << "SNRMajor"
            << setw(20) << "SINRMajor"
            << setw(20) << "InfPMajor"

            << setw(20) << "BS_V_beamDEGMajor"
            << setw(20) << "BS_H_beamDEGMajor"
            << setw(20) << "UE_V_beamDEGMajor"
            << setw(20) << "UE_H_beamDEGMajor"


            << setw(20) << "bsidAux"
            << setw(20) << "btsidAux"
            << setw(20) << "btsIndexAux"
            << setw(20) << "BTSPosXAux"
            << setw(20) << "BTSPosYAux"
            << setw(20) << "BTSWrapPosXAux"
            << setw(20) << "BTSWrapPosYAux"

            << setw(20) << "UEWrapPosXAux"
            << setw(20) << "UEWrapPosYAux"
            << setw(20) << "2DDistanceAux"
            << setw(20) << "3DDistanceAux"

            << setw(20) << "IsStrongLinkAux"
            << setw(20) << "IsLOSAux"
            << setw(20) << "LoSAoDAux"
            << setw(20) << "LoSEoDAux"
            << setw(20) << "LoSAoAAux"
            << setw(20) << "LoSEoAAux"

            << setw(20) << "LinkLossDBAux"
            << setw(20) << "PathLossDBAux"
            << setw(20) << "ShadowFadeDBAux"

            << setw(20) << "BTSBeamIndexAux"
            << setw(20) << "MSBeamIndexAux"
            << setw(20) << "PanelIndexAux"

            << setw(20) << "RSRPAux"
            << setw(20) << "CouplingLossAux"
            << setw(20) << "SNRAux"
            << setw(20) << "SINRAux"
            << setw(20) << "InfPAux"

            << setw(20) << "BS_V_beamDEGAux"
            << setw(20) << "BS_H_beamDEGAux"
            << setw(20) << "UE_V_beamDEGAux"
            << setw(20) << "UE_H_beamDEGAux"
            << endl;
    }
    if(Parameters::Instance().RIS.is_baseline){
        Observer::Print("RIS_LargeScale_Information_base") << "UEID"
            << setw(20) << "BTSID"
            << setw(20) << "BS_V_beam"
            << setw(20) << "BS_H_beam"
            << setw(20) << "UE_V_beam"
            << setw(20) << "UE_H_beam"
            << setw(20) << "RSRP"    
            << endl;
    }
       
    if(Parameters::Instance().RIS.is_BestBTS2MSBeam){
        Observer::Print("RIS_LargeScale_Information0") << "UEID"
            << setw(20) << "BTSID"
            << setw(20) << "RISID"
            << setw(20) << "BS_V_beam"
            << setw(20) << "BS_H_beam"
            << setw(20) << "UE_V_beam"
            << setw(20) << "UE_H_beam"
            << setw(20) << "RIS_V_beam"
            << setw(20) << "RIS_H_beam"
            << setw(20) << "RSRP"    
            << endl;
    }
    if(Parameters::Instance().RIS.is_DoubleBest){
        Observer::Print("RIS_LargeScale_Information1") << "UEID"
            << setw(20) << "BTSID"
            << setw(20) << "RISID"
            << setw(20) << "BS_V_beam"
            << setw(20) << "BS_H_beam"
            << setw(20) << "UE_V_beam"
            << setw(20) << "UE_H_beam"
            << setw(20) << "RSRP"    
            << endl;
    }
    if(Parameters::Instance().RIS.is_BestCase){
        Observer::Print("RIS_LargeScale_Information2") << "UEID"
            << setw(20) << "BTSID"
            << setw(20) << "RISID"
            << setw(20) << "BS_V_beam"
            << setw(20) << "BS_H_beam"
            << setw(20) << "UE_V_beam"
            << setw(20) << "UE_H_beam"
            << setw(20) << "RIS_V_beam"
            << setw(20) << "RIS_H_beam"
            << setw(20) << "RSRP"
            << endl;
    }
    if(Parameters::Instance().RIS.is_BestBeam){
        Observer::Print("RIS_LargeScale_Information3") << "UEID"
            << setw(20) << "BTSID"
            << setw(20) << "RISID"
            << setw(20) << "BS_V_beam"
            << setw(20) << "BS_H_beam"
            << setw(20) << "UE_V_beam"
            << setw(20) << "UE_H_beam"
            << setw(20) << "RIS_V_beam"
            << setw(20) << "RIS_H_beam"
            << setw(20) << "RSRP"
            << endl;
    }
    if(Parameters::Instance().RIS.is_New){
        Observer::Print("RIS_LargeScale_Information_New") << "UEID"
            << setw(20) << "BTSID"
            << setw(20) << "RISID"
            << setw(20) << "BS_V_beam"
            << setw(20) << "BS_H_beam"
            << setw(20) << "UE_V_beam"
            << setw(20) << "UE_H_beam"
            << setw(20) << "RSRP"
            << endl;
    }
    
}

///信道模型参数初始化函数
void Framework::ChannelParameterInitialize() {
    cm::P::s().FX.DCarrierWidthHz = Parameters::Instance().BASIC.DCarrierWidthHz;
    cm::P::s().FX.DRadioFrequencyMHz_Macro = Parameters::Instance().Macro.LINK.DRadioFrequencyMHz;
    cm::P::s().FX.DRadioFrequencyMHz_Pico = Parameters::Instance().SmallCell.LINK.DRadioFrequencyMHz;
    cm::P::s().FX.DSampleRateHz = Parameters::Instance().BASIC.DSampleRateHz;
    cm::P::s().FX.DWaveLength_Macro = Parameters::Instance().Macro.LINK.DWaveLength;
    cm::P::s().FX.DWaveLength_Pico = Parameters::Instance().SmallCell.LINK.DWaveLength;
    cm::P::s().FX.ICarrierNum = Parameters::Instance().BASIC.ISCNum;
    cm::P::s().FX.ICarrierSampleSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace;
    cm::P::s().FX.IISFastFadingUsed = Parameters::Instance().LINK_CTRL.IISFastFadingUsed;
    cm::P::s().FX.IISShadowFadingUsed = Parameters::Instance().LINK_CTRL.IISShadowFadingUsed;
    cm::P::s().FX.IStrongLinkNum = Parameters::Instance().LINK_CTRL.IStrongLinkNum;
    cm::P::s().IMacroTopology = Parameters::Instance().BASIC.IMacroTopology;
    ///20260120：增加参数
    cm::P::s().SetChannelModel(Parameters::Instance().BASIC.IChannelModel_for_Scenario,
        Parameters::Instance().BASIC.IChannelModel_VariantMode );
}

///基站位置分布初始化
void Framework::StandardDistribute() {
    BSManager& bsm = BSManager::Instance();
    double x, y;
    vector<std::shared_ptr<BS> >& vpBS = bsm.GetvpBS();
    int iNumBSs = vpBS.size();
    ///7小区基站位置初始化
    if (iNumBSs == 7) {
        vpBS[0]->SetXY(0, 0);
        for (int i = 1; i <= 6; ++i) {
            x = Parameters::Instance().Macro.DSiteDistanceM * cos((i - 1) * M_PI / 3);
            y = Parameters::Instance().Macro.DSiteDistanceM * sin((i - 1) * M_PI / 3);
            vpBS[i]->SetXY(x, y);
        }
    }///19小区基站位置初始化
    else if (iNumBSs == 19) {
        ///0号BS位置初始化
        vpBS[0]->SetXY(0, 0);
        for (int i = 1; i <= 6; ++i) {
            ///1～6号BS位置初始化
            x = Parameters::Instance().Macro.DSiteDistanceM * cos((i - 1) * M_PI / 3);
            y = Parameters::Instance().Macro.DSiteDistanceM * sin((i - 1) * M_PI / 3);
            vpBS[i]->SetXY(x, y);
            ///7～12号BS位置初始化
            x = M_SQRT3 * Parameters::Instance().Macro.DSiteDistanceM * cos((i - 1) * M_PI / 3 + M_PI / 2);
            y = M_SQRT3 * Parameters::Instance().Macro.DSiteDistanceM * sin((i - 1) * M_PI / 3 + M_PI / 2);
            vpBS[i + 6]->SetXY(x, y);
            ///13～18号BS位置初始化
            x = 2 * Parameters::Instance().Macro.DSiteDistanceM * cos((i - 1) * M_PI / 3);
            y = 2 * Parameters::Instance().Macro.DSiteDistanceM * sin((i - 1) * M_PI / 3);
            vpBS[i + 12]->SetXY(x, y);
        }
    }
    else if(iNumBSs == 1){vpBS[0]->SetXY(0, 0);}
    else
        assert(false);
    for (int i = 0; i < iNumBSs; ++i) {
        BS& bs = bsm.GetBS(i);
        x = bs.GetX();
        y = bs.GetY();
        Observer::Print("BS_Position") << x << setw(20) << y << endl;
        for (int j = 0; j < Parameters::Instance().BASIC.IBTSPerBS; ++j) {
            BTS& bts = bs.GetBTS(j);
            bts.SetXY(x, y);
            bts.SetTxOrientRAD(j * 2 * M_PI / 3+M_PI / 6);
        }
    }
}

void Framework::DistributeMSs(){
    MSManager& msm = MSManager::Instance();
    int iAllMS = msm.CountMS();
    for (int ims = 0; ims < iAllMS; ims++) {
        m_pDMSA->DistributeMS_with_distance_check(msm.GetMS(ims));
    }
}

void Framework::DistributeMSs(vector<int> _vmsid) {
    MSManager& msm = MSManager::Instance();
    for (int i = 0; i<static_cast<int> (_vmsid.size()); ++i) {
        m_pDMSA->DistributeMS_with_distance_check(msm.GetMS(_vmsid[i]));
    }
}

void Framework::ConstructPicoGroups(){
    
}

void Framework::InitializeMSs(){
    MSManager& msm = MSManager::Instance();
    vector<std::shared_ptr<MS> >& v_MS = msm.GetvMS();
    for (auto it = v_MS.begin(); it != v_MS.end(); ++it) {
        while (!(*it)->Initialize()) {
            m_pDMSA->DistributeMS_with_distance_check(**it);
            cm::LinkMatrix::Instance().Initialize(*(*it)->m_pRxNode);
        }
        if(!Parameters::Instance().BASIC.BISRIS)
            Observer::Print("new_MS_Position") << (*it)->GetX() << setw(20) << (*it)->GetY() << endl;
    }
}

void Framework::InitializeMSs(vector<int> _vmsid) {
    MSManager& msm = MSManager::Instance();
    for (int i = 0; i<static_cast<int> (_vmsid.size()); ++i) {
        MS& ms = msm.GetMS(_vmsid[i]);
        while (!ms.Initialize()) {
            m_pDMSA->DistributeMS_with_distance_check(ms);
            cm::LinkMatrix::Instance().Initialize(*ms.m_pRxNode);
        }
        if (ms.GetAllServBTSs().size() > 1)
            msm.GetExistMultiTRP() = true;
    }
}

///基站初始化
void Framework::InitializeBSs() {
    BSManager& bsm = BSManager::Instance();
    vector<std::shared_ptr<BS> >& vpBS = bsm.GetvpBS();
    for (auto it = vpBS.begin(); it != vpBS.end(); ++it) {
        (*it)->Initialize();
    }
}

Framework& Framework::Instance() {
    if(m_pFramework == 0){
        switch(Parameters::Instance().BASIC.IScenarioModel){
            case Parameters::SCENARIO_LowFreq_INDOOR:
            case Parameters::SCENARIO_HighFreq_INDOOR:
                m_pFramework = new IndoorNetwork;
                break;
            case Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER:
                m_pFramework = new TwoLayerNetwork;
                break;
            case Parameters::SCENARIO_RIS:
                m_pFramework = new FrameRIS;
                break;
                
            default:
                m_pFramework = new OneLayerNetwork;
                break;
        }
    }
    return *m_pFramework;
}