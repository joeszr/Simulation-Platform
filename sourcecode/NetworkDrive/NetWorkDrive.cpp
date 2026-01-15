///@file NetWorkDrive.cpp
///@brief 系统驱动模块
///@author wangxiaozhou

#include "../Parameters/Parameters.h"
#include "Clock.h"
#include "../BaseStation/BTSID.h"
#include "../MobileStation/MSID.h"
#include "BSManager.h"
#include "MSManager.h"
#include "../Statistician/Statistician.h"
#include "../BaseStation/BTS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../TrafficModel/TrafficManager.h"
#include "NetWorkDrive.h"
#include "../BaseStation/CodeBookFactory.h"
#include "../Statistician/Directory.h"
#include "../Statistician/Statistics.h"
//@threads
#include"Thread_control.h"

NetWorkDrive* NetWorkDrive::m_pNetWorkDrive = nullptr;
int G_PacketID = 0;

int G_ICurDrop = -1;

///仿真过程启动的入口点函数


void NetWorkDrive::SimulateRun() {
    time(&begin);
    SystemInitialize();
    simthread();
    //CodeBookFactory();
    SystemEnd();
}

///信道模型参数初始化函数

void NetWorkDrive::ChannelParameterInitialize() {
    cm::P::s().MacroTX.DAntennaHeightM = Parameters::Instance().Macro.DAntennaHeightM;
    cm::P::s().MacroTX.DHAntSpace = Parameters::Instance().Macro.DHAntSpace;
    cm::P::s().MacroTX.DSiteDistanceM = Parameters::Instance().Macro.DSiteDistanceM;
    cm::P::s().MacroTX.ITotalAntNum = Parameters::Instance().Macro.ITotalAntNum;
    //    cm::P::s().MacroTX.IPolarizedType = Parameters::Instance().Macro.IPolarize;
    cm::P::s().MacroTX.DVAntSpace = Parameters::Instance().Macro.DVAntSpace;
    cm::P::s().MacroTX.IHTotalAntNum = Parameters::Instance().Macro.IHTotalAntNum;
    ///
    cm::P::s().PicoTX.ITotalAntNum = Parameters::Instance().SmallCell.LINK.IPicoAntennaNum;
    ///
    cm::P::s().RX.DAntennaHeightM = Parameters::Instance().MSS.DAntennaHeightM;
    cm::P::s().RX.IHPanelNum = Parameters::Instance().MSS.FirstBand.IHPanelNum;
    cm::P::s().RX.ITotalAntNumPerPanel = Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel;
    cm::P::s().RX.ITotalAntNum = Parameters::Instance().MSS.FirstBand.ITotalAntNum;
    //    cm::P::s().RX.IPolarizedType = Parameters::Instance().MSS.FirstBand.IPolarize;
    cm::P::s().RX.DHAntSpace = Parameters::Instance().MSS.FirstBand.DHAntSpace;
    cm::P::s().RX.DVAntSpace = Parameters::Instance().MSS.FirstBand.DVAntSpace;
    cm::P::s().RX.IHTotalAntNumPerPanel = Parameters::Instance().MSS.FirstBand.IHAntNumPerPanel * Parameters::Instance().MSS.FirstBand.Polarize_Num;
    cm::P::s().RX.IHAntNumPerPanel = Parameters::Instance().MSS.FirstBand.IHAntNumPerPanel;

    ///
    cm::P::s().FX.DCarrierWidthHz = Parameters::Instance().BASIC.DCarrierWidthHz;
    cm::P::s().FX.DRadioFrequencyMHz_Macro = Parameters::Instance().Macro.LINK.DRadioFrequencyMHz;
    cm::P::s().FX.DRadioFrequencyMHz_Pico = Parameters::Instance().SmallCell.LINK.DRadioFrequencyMHz;
    cm::P::s().FX.DSampleRateHz = Parameters::Instance().BASIC.DSampleRateHz;
    cm::P::s().FX.DWaveLength_Macro = Parameters::Instance().Macro.LINK.DWaveLength;
    cm::P::s().FX.DWaveLength_Pico = Parameters::Instance().SmallCell.LINK.DWaveLength;
    cm::P::s().FX.ICarrierNum = Parameters::Instance().BASIC.ISCNum;
    cm::P::s().FX.ICarrierSampleSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace;
    cm::P::s().FX.IISFastFadingUsed = Parameters::Instance().LINK_CTRL.IISFastFadingUsed;
    cm::P::s().FX.I2DOr3DChannel = Parameters::Instance().LINK_CTRL.I2DOr3DChannel;
    cm::P::s().FX.IISShadowFadingUsed = Parameters::Instance().LINK_CTRL.IISShadowFadingUsed;
    cm::P::s().FX.IStrongLinkNum = Parameters::Instance().LINK_CTRL.IStrongLinkNum;
    cm::P::s().IMacroTopology = Parameters::Instance().BASIC.IMacroTopology;
    cm::P::s().MacroTX.DDownTiltDeg = Parameters::Instance().Macro.DMechanicalTiltDeg;
    ///
    cm::P::s().SetChannelModel(
            Parameters::Instance().BASIC.IChannelModel_for_Scenario,
            Parameters::Instance().BASIC.IChannelModel_VariantMode);
}

///系统初始化过程

void NetWorkDrive::SystemInitialize() {
    cout << "System is initialized.....";
    if (Parameters::Instance().BASIC.IDLORUL == Parameters::DL) {
        LinkLevelInterface::Instance_DL();
    } 
    else if(Parameters::Instance().BASIC.IDLORUL == Parameters::UL) {
        LinkLevelInterface::Instance_UL();
    }
    else if(Parameters::Instance().BASIC.IDLORUL == Parameters::ULandDL){
        LinkLevelInterface::Instance_UL();
        LinkLevelInterface::Instance_DL();
    }
    ChannelParameterInitialize(); //初始化信道参数
    Statistician::Instance().PrintHead();
    cout << "ok!" << endl;
    Observer::SetIsEnable(true);
    PrintHead();
}

///@brief 系统结束函数
///
///完成程序结束之前的清理工作

void NetWorkDrive::SystemEnd() {
    Statistician::Instance().PreProcess();
    Statistician::Instance().PrintTable();
//    TrafficManager::Instance().OutputTrafficInfo();
}
///Drop初始化过程函数

void NetWorkDrive::DropInitialize() {
    //将时钟置回零点
    Clock::Instance().Reset();
    //撒Pico
    BSManager::Instance().DistributePicos_HetNet();
    //添加MS
    MSManager::Instance().AddMS();
    if(G_ICurDrop == 1)
    {
        //建立MS和BTS的数据统计结构体
        Statistician::Instance().Initialize();
    }
    Statistics::Instance().Initialize();
    MSManager::Instance().DistributeMSs();
    cout << "DistributeMSs() Succeeded!!!!!!!" << endl;
    //初始化每个MS到各个BTS的路损和阴影衰落
    cm::LinkMatrix::Instance().Initialize();
    cout << "LinkMatrix Initialize() Succeeded!!!!!!!!!!!!!!!!!!!!!" << endl;
    time(&end);
    cout << "initialization " << end - begin << endl;
    //确定业务类型和确定主服务BTS，确定Geometry
    MSManager::Instance().InitializeMSs();
    cout << "InitializeMSs() Succeeded!!!!!!!!!!!!!!!!!!" << endl;
    //初始化基站
    BSManager::Instance().InitializeBSs();
    cout << "InitializeBSs() Succeeded!!!!!!!!!!!!!!!!!!" << endl;
    time(&end);
    cout << "Re initialization " << end - begin << endl;

}

///时隙仿真流程，完成每个时隙的工作

void NetWorkDrive::SlotRun() {
    cm::LinkMatrix::Instance().WorkSlot(Clock::Instance().GetTimeSec());
    int iSlot = Clock::Instance().GetTimeSlot();
    TrafficManager::Instance().WorkSlot();

    if(DownOrUpLink(iSlot)==0||DownOrUpLink(iSlot)==2){
        BSManager::Instance().WorkSlot();
        MSManager::Instance().WorkSlot();
    }
    else if(DownOrUpLink(iSlot)==1){
        MSManager::Instance().WorkSlot();
        BSManager::Instance().WorkSlot();
    }
    else{
        assert(false);
    }
}

///@brief 返回唯一的系统驱动对象实例
///
///Instance()函数是实现Singleton模式的组成部分之一。在第一次调用时初始化唯一的一个指向NetWorkDrive对
///象的指针。函数返回NetWorkDrive类对象的引用而不是指针是为了避免返回的指针被用户做delete操作。

NetWorkDrive & NetWorkDrive::Instance() {
    if (!m_pNetWorkDrive)
        m_pNetWorkDrive = new NetWorkDrive;
    return *m_pNetWorkDrive;
}

///@brief Drop结束函数
///
///完成Drop结束后的清理，统计和输出数据，并清理MS，BTS,linkmatrix等

void NetWorkDrive::DropEnd() {
//    Statistician::Instance().PreProcess();
//    Statistician::Instance().PrintTable();

    ///对统计信息进行打印
    Statistics::Instance().PrintHead();
    Statistics::Instance().PreProcess();
    Statistics::Instance().PrintTable();
    //@threads
    long timestart,timeend1,timeend2,timeend3,timeend4,timeend5;
    cout<<endl;
    time(&timestart);
    Thread_control::Instance().Reset();
    time(&timeend1);
    cout<<timeend1-timestart<<endl;
    cm::LinkMatrix::Instance().Reset();
//    time(&timeend2);
//    cout<<timeend2-timeend1<<endl;
    MSManager::Instance().Reset();
//    time(&timeend3);
//    cout<<timeend3-timeend2<<endl;
    BSManager::Instance().Reset();
//    time(&timeend4);
//    cout<<timeend4-timeend3<<endl;
//    Statistician::Instance().Reset();
//    time(&timeend5);
//    cout<<timeend5-timeend4<<endl;
}

void NetWorkDrive::PrintHead(){
    Observer::Print("Scheduling_Record") << "Time"
                <<setw(20) << "BSID"
                <<setw(20) << "MSID"
                <<setw(20) << "DLorUL"
                <<setw(20) << "HARQID"
                <<setw(20) << "Rank"
                <<setw(20) << "MCS"
                <<setw(20) << "SendNum"
                <<setw(20) << "TBSizeKbit"
                <<setw(20) << "Transmint_slot"
                <<setw(20) << "PxSCH_RB"
                <<setw(20) << "PDCCH_RB"<<endl;
    //hyl 冗余
//    Observer::Print("CSI_Record_period")<<"Time"
//            << setw(20) << "BSID"
//            << setw(20) << "ThroughputKbps"
//            << setw(20) << "BLERTx1"
//            << setw(20) << "BLERTx2"
//            << setw(20) << "BLERTx3"
//            << setw(20) << "BLERTx4"
//            << setw(20) << "NMSE"
//            << setw(20) << "NMSE_cuda"
//            << setw(20) << "cossim"
//            << setw(20) << "time_cost"
//            << setw(20) << "feedback_bits"
//            <<endl;
//    Observer::Print("CSI_Record_total")<<"Time"
//                                        << setw(20) << "BSID"
//                                        << setw(20) << "ThroughputKbps"
//                                        << setw(20) << "BLERTx1"
//                                        << setw(20) << "BLERTx2"
//                                        << setw(20) << "BLERTx3"
//                                        << setw(20) << "BLERTx4"
//                                        << setw(20) << "NMSE"
//                                        << setw(20) << "NMSE_cuda"
//                                        << setw(20) << "cossim"
//                                        << setw(20) << "time_cost"
//                                        <<endl;
//    Observer::Print("QoS_Record_before")<<"Time"
//                               << setw(20) << "BSID"
//                               << setw(20) << "MSID"
//                               << setw(20) << "LC0"
//                               << setw(20) << "LC1"
//                               << setw(20) << "LC2"
//                               << setw(20) << "LC3" <<endl;
//    Observer::Print("QoS_Record_after")<<"Time"
//                                        << setw(20) << "BSIDfffff"
//                                        << setw(20) << "MSID"
//                                        << setw(20) << "LC0"
//                                        << setw(20) << "LC1"
//                                        << setw(20) << "LC2"
//                                        << setw(20) << "LC3" <<endl;
    Observer::Print("SR_Record")<<"Drop"
                    << setw(20) << "BSID"
                    << setw(20) << "MSNum"
                    << setw(20) << "RBUseRate"
                    << setw(20) << "SR_Period"
                    << setw(20) << "SR_RBNum" << endl;

    Observer::Print("MS_buffer_record")<<"Time"
                    << setw(20) << "BSID"
                    << setw(20) << "MSID"
                    << setw(20) << "LCG0"
                    << setw(20) << "LCG1"
                    << setw(20) << "LCG2"
                    << setw(20) << "LCG3" <<endl;
    Observer::Print("BS_buffer_record")<<"Time"
                   << setw(20) << "BSID"
                   << setw(20) << "MSID"
                   << setw(20) << "LCG0"
                   << setw(20) << "LCG1"
                   << setw(20) << "LCG2"
                   << setw(20) << "LCG3" <<endl;
    //hyl 冗余
//    Observer::Print("RI_record")<< "Time"
//                    << setw(20) << "BSID"
//                    << setw(20) << "MSID"
//                    << setw(20) << "Rank"
//                    << setw(20) << "SINR"
//                    << setw(20) << "MCS"
//                    << setw(20) << "Capacity"<<endl;
    Observer::Print("DRXrecord")<< "BSID"
                    << setw(20) << "MSID"
                    << setw(20) << "Frame"
                    << setw(20) << "Slot"
                    << setw(20) << "OnDuration"
                    << setw(20) << "Inactivity"
                    << setw(20) << "DRXCycleState"
                    << setw(20) << "PDCCH"
                    << setw(20) << "DRXState"<<endl;
    if (Parameters::Instance().BASIC.IDLORUL == Parameters::UL||Parameters::Instance().BASIC.IDLORUL == Parameters::ULandDL) {

        //hyl 冗余
//        Observer::Print("MSDecode_UL") << "Time"
//                                       << setw(20) << "BTSID"
//                                       << setw(20) << "MSID"
//                                       << setw(20) << "HARQID"
//                                       << setw(20) << "TBSize"
//                                       << setw(20) << "SendNum"
//                                       << setw(20) << "MCS[0]"
//                                       << setw(20) << "MCS[1]"
//                                       << setw(20) << "SINRDB"
//                                       << setw(20) << "BLER"
//                                       << setw(20) << "ACKNAK" << endl;
//
//        Observer::Print("TPCRecord") << "Time"
//                                         << setw(20) << "MSID"
//                                         << setw(20) << "PathLoss"
//                                         << setw(20) << "SINR"
//                                         << setw(20) << "Point"
//                                         << setw(20) << "TPC"<<endl;
        Observer::Print("TxPowerRecord") << "Time"
                                         << setw(20) << "BTSID"
                                         << setw(20) << "MSID"
                                         << setw(20) << "TxPower_dBm"
                                         << setw(20) << "f_dB"<<endl;
        Observer::Print("PHRRecord") <<"Time"
                                    <<setw(20)<<"BTSID"
                                    <<setw(20)<<"MSID"
                                    <<setw(20)<<"IsTriggerrd"
                                    <<setw(20)<<"Trigger_Reason"
                                    <<setw(20)<<"PHR_Index"<<endl;
        //hyl 冗余
//        Observer::Print("SRlog") <<"Time"
//                                 <<setw(20)<<"BTSID"
//                                 <<setw(20)<<"MSID"
//                                 <<setw(20)<<"state"<<endl;
    }

    {
        Observer::Print("SchMes") << "BSID"
                                             << setw(20) << "Drop"
                                             << setw(20) << "Frame"
                                             << setw(20) << "Slot"
                                             << setw(20) << "DLorUL"
                                             << setw(20) << "MSID"
                                             << setw(20) << "HARQID"
                                             << setw(20) << "Rank"
                                             << setw(20) << "SendNum"
                                             << setw(20) << "SINR"
                                             << setw(20) << "PreSINR"
                                             << setw(20) << "InLoopSINR"
                                             << setw(20) << "MCSLevel"
                                             << setw(20) << "MCSOrder"
                                             << setw(20) << "BLER"
                                             << setw(20) << "A/N"
                                             << setw(20) << "TBSize"
                                             << setw(20) << "SBNum"
                                             << setw(20) << "SBID"
                                             << setw(20) << "TPC"
                                            << setw(20) << "CCELevel"
                                            //<< setw(20) << "CCESB"
                                             << endl;
    }
//hyl 冗余
//	{
//		Observer::Print("SchMes_WarmUpslot") << "BSID"
//            << setw(20) << "Frame"
//            << setw(20) << "Slot"
//			<< setw(20) << "DLorUL"
//			<< setw(20) << "MSID"
//			<< setw(20) << "HARQID"
//			<< setw(20) << "Rank"
//			<< setw(20) << "SINR"
//			<< setw(20) << "MCSLevel"
//			<< setw(20) << "MCSOrder"
//			<< setw(20) << "BLER"
//			<< setw(20) << "A/N"
//			<< setw(20) << "TBSize"
//			<< setw(20) << "SBNum"
//			<< setw(20) << "SBID"
//			<< setw(20) << "TPC"
//			<< endl;
//	}

//    {
//        Observer::Print("MUschedularTest") << "Pre  " << "Time"
//                                           << setw(20) << "BTSID"
//                                           << setw(20) << "sbid"
//                                           << setw(20) << "msid"
//                                           << setw(20) << "RateTemp"
//                                           << setw(20) << "MCS_Rank0"
//                                           << setw(20) << "iUsedSBNum"
//                                           << setw(20) << "PacketKbitTemp"
//                                           << setw(20) << "MCS_Rank0"
//                                           << endl;
//    }
//
//    {
//        Observer::Print("LargeScale") << "m_PathLossDB"
//                                      << setw(20) << "m_ShadowFadingDB"
//                                      << setw(20) << "m_dTxAntennaPatternDB"
//                                      << setw(20) << "m_dRxAntennaPatternDB"
//                                      << setw(20) << "dInCarLossDB"
//                                      << setw(20) << "m_LoSLinkLossDB"
//                                      << setw(20) << "m_PurePathLossDB" << endl;
//    }
//
//    {
//        Observer::Print("SmallScale") << "m_dGeometryDB_SNR"
//                                      << setw(20) << "m_dGeometryDB_SINR" << endl;
//    }

    //    Observer::Print("MSofLoss") << "MSID" << setw(20)
    //            << "bIsServBTS" << setw(20)
    //            << "bIsStrong" << setw(20)
    //            << "bIsLoS" << setw(20)
    //            << "BTSID" << setw(20)
    //            << "LinkLossDB" << setw(20)
    //            << "OneLinkLossDB" << endl;

//    Observer::Print("MacroMS") << "MSID" << setw(20) << "MainServ" << setw(20) << "GeometryDB" << setw(20) << "PathLossDB" << setw(20) << "LinkLossDB" << setw(20) << "PurePathLossDB" << setw(20) << "ShadowFadeDB" << setw(20) << "TxGainDB" << setw(20) << "DistanceM" << endl;
    //    Observer::Print("PicoMS") << "MSID" << setw(20) << "MainServ" << setw(20) << "GeometryDB" << setw(20) << "PathLossDB" << setw(20) << "LinkLossDB" << setw(20) << "PurePathLossDB" << setw(20) << "ShadowFadeDB" << setw(20) << "TxGainDB" << setw(20) << "DistanceM" << endl;
//    Observer::Print("TotalMS") << "MSID" << setw(20) << "MainServ" << setw(20) << "GeometryDB" << setw(20) << "PathLossDB" << setw(20) << "LinkLossDB" << setw(20) << "PurePathLossDB" << setw(20) << "ShadowFadeDB" << setw(20) << "TxGainDB" << setw(20) << "DistanceM" << endl;
//    Observer::Print("MSEBBRank1Decode") << "iTime" << setw(20)
//                                        << "iHARQID" << setw(20)
//                                        << "m_MSID" << setw(20)
//                                        << "m_MainServBTS" << setw(20)
//                                        << "iSBNum" << setw(20)
//                                        << " iRank" << setw(20)
//                                        << "iSendNum" << setw(20)
//                                        << " iRankID " << setw(20)
//                                        << "iMCS " << setw(20)
//                                        << "dOlla" << setw(20)
//                                        << "dPostSINRDB" << setw(20)
//                                        << "dBLER" << setw(20)
//                                        << "bRight " << setw(20)
//                                        << "dTBSize" << setw(20)
//                                        << "AllocatedSB" << setw(20)
//                                        << "k1_slot" << setw(20)
//                                        << "LatestTransTime" << setw(20)
//                                        << "PacketID" << setw(20)
//                                        << "PacketSize"<< setw(20)
//                                        << "spectrumefficiency"
//                                        << endl;


    //    Observer::Print("MS.MCS=0") << "Time" << setw(20) << "msid" << setw(20) << "BTSID" << setw(20) << "MCS" << setw(20) << "CompetorLisSize" << endl;
    if (Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP
        || Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP2
        || Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_FTP3
        || Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::ITrafficModel_XR
           || Parameters::Instance().TRAFFIC.ITrafficModel == Parameters::MixedTraffic) {

        //hyl 冗余
//        Observer::Print("XRPacketErrorRate") << setw(20) << "Time"
//                                             << setw(20) << "MSID"
//                                             << setw(20) << "BTSID"
//                                             << setw(20) << "PacketNum_Wrong"
//                                             << setw(20) << "PacketNum_Right"
//                                             << setw(20) << "PacketErrorRate" << endl;
//
//
//
//		Observer::Print("FTPPacket") << setw(20)<<"ID"
//			<< setw(20) << "LogTime"
//			<< setw(20) << "MSID"
//			<< setw(20) << "BTSID"
//			<< setw(20) << "OriginSize"
//			<< setw(20) << "RemainderSize"
//			<< setw(20) << "TxSize"
//			<< setw(20) << "RightRxSize"
//			<< setw(20) << "IsFinished"
//			<< setw(20) << "IsCompleted"
//			<< setw(20) << "IsDroped"
//			<< setw(20) << "BornTime"
//			<< setw(20) << "FinishTime"
//			<< setw(20) << "Delay"
//			<< setw(20) << "XR_IorPFrame"
//			<< setw(20) << "UPT" << endl;
	}

	//RedCap FTPPacket
	{
//		Observer::Print("FTPPacket_eMBB") << "ID"
//			<< setw(20) << "LogTime"
//			<< setw(20) << "MSID"
//			<< setw(20) << "BTSID"
//			<< setw(20) << "OriginSize"
//			<< setw(20) << "RemainderSize"
//			<< setw(20) << "TxSize"
//			<< setw(20) << "RightRxSize"
//			<< setw(20) << "IsFinished"
//			<< setw(20) << "IsCompleted"
//			<< setw(20) << "IsDroped"
//			<< setw(20) << "BornTime"
//			<< setw(20) << "FinishTime"
//			<< setw(20) << "Delay"
//			<< setw(20) << "XR_IorPFrame"
//			<< setw(20) << "UPT" << endl;
//
//		Observer::Print("FTPPacket_RedCap") << "ID"
//			<< setw(20) << "LogTime"
//			<< setw(20) << "MSID"
//			<< setw(20) << "BTSID"
//			<< setw(20) << "OriginSize"
//			<< setw(20) << "RemainderSize"
//			<< setw(20) << "TxSize"
//			<< setw(20) << "RightRxSize"
//			<< setw(20) << "IsFinished"
//			<< setw(20) << "IsCompleted"
//			<< setw(20) << "IsDroped"
//			<< setw(20) << "BornTime"
//			<< setw(20) << "FinishTime"
//			<< setw(20) << "Delay"
//			<< setw(20) << "XR_IorPFrame"
//			<< setw(20) << "UPT" << endl;

	}

	///////////////////////输出校正文件///////////////////////
	Observer::Print("ITUCalibration") << "btsid"
		<< setw(20) << "btsIndex"
		<< setw(20) << "IsServBTS"
		<< setw(20) << "UEID"
		<< setw(20) << "UEPosX"
		<< setw(20) << "UEPosY"
		<< setw(20) << "UEWrapPosX"
		<< setw(20) << "UEWrapPosY"
		<< setw(20) << "MSOrient"
		<< setw(20) << "BTSPosX"
		<< setw(20) << "BTSPosY"
		<< setw(20) << "BTSWrapPosX"
		<< setw(20) << "BTSWrapPosY"
		<< setw(20) << "2DDistance"
		<< setw(20) << "3DDistance"
		<< setw(20) << "TotalFloorNum"
		<< setw(20) << "FloorIndex"
		<< setw(20) << "UEHeight"
		<< setw(20) << "IsStrongLink"
		<< setw(20) << "IsLOS"
		<< setw(20) << "IsInH"
		<< setw(20) << "isLowLoss"
		<< setw(20) << "LoSAoD"
		<< setw(20) << "LoSEoD"
		<< setw(20) << "LoSAoA"
		<< setw(20) << "LoSEoA"
		<< setw(20) << "LinkLossDB"
		<< setw(20) << "LoSLinkLossDB"
		<< setw(20) << "PathLossDB"
		<< setw(20) << "ShadowFadeDB"
		<< setw(20) << "BTSBeamIndex"
		<< setw(20) << "MSBeamIndex"
		<< setw(20) << "PanelIndex"
		<< setw(20) << "GeometryDB_SNR"
		<< setw(20) << "GeometryDB_SINR"
		<< setw(20) << "Geometrydb100MHz"
		<< setw(20) << "RSRP_36873"
		<< setw(20) << "CouplingLoss_36873"
		<< setw(20) << "SNR_36873"
		<< setw(20) << "SINR_36873_Radom"
		<< setw(20) << "BS_V_beamDEG"
		<< setw(20) << "BS_H_beamDEG"
		<< setw(20) << "UE_V_beamDEG"
		<< setw(20) << "UE_H_beamDEG"
		<< setw(20) << "InfP_Mean_36873"
		<< setw(20) << "UEOrientRAD"
		<< endl;
//hyl 冗余
//	{
//		Observer::Print("ITUCalibration_eMBB") << "btsid"
//			<< setw(20) << "btsIndex"
//			<< setw(20) << "IsServBTS"
//			<< setw(20) << "UEID"
//			<< setw(20) << "UEPosX"
//			<< setw(20) << "UEPosY"
//			<< setw(20) << "UEWrapPosX"
//			<< setw(20) << "UEWrapPosY"
//			<< setw(20) << "MSOrient"
//			<< setw(20) << "BTSPosX"
//			<< setw(20) << "BTSPosY"
//			<< setw(20) << "BTSWrapPosX"
//			<< setw(20) << "BTSWrapPosY"
//			<< setw(20) << "2DDistance"
//			<< setw(20) << "3DDistance"
//			<< setw(20) << "TotalFloorNum"
//			<< setw(20) << "FloorIndex"
//			<< setw(20) << "UEHeight"
//			<< setw(20) << "IsStrongLink"
//			<< setw(20) << "IsLOS"
//			<< setw(20) << "IsInH"
//			<< setw(20) << "isLowLoss"
//			<< setw(20) << "LoSAoD"
//			<< setw(20) << "LoSEoD"
//			<< setw(20) << "LoSAoA"
//			<< setw(20) << "LoSEoA"
//			<< setw(20) << "LinkLossDB"
//			<< setw(20) << "LoSLinkLossDB"
//			<< setw(20) << "PathLossDB"
//			<< setw(20) << "ShadowFadeDB"
//			<< setw(20) << "BTSBeamIndex"
//			<< setw(20) << "MSBeamIndex"
//			<< setw(20) << "PanelIndex"
//			<< setw(20) << "GeometryDB_SNR"
//			<< setw(20) << "GeometryDB_SINR"
//			<< setw(20) << "Geometrydb100MHz"
//			<< setw(20) << "RSRP_36873"
//			<< setw(20) << "CouplingLoss_36873"
//			<< setw(20) << "SNR_36873"
//			<< setw(20) << "SINR_36873_Radom"
//			<< setw(20) << "BS_V_beamDEG"
//			<< setw(20) << "BS_H_beamDEG"
//			<< setw(20) << "UE_V_beamDEG"
//			<< setw(20) << "UE_H_beamDEG"
//			<< setw(20) << "InfP_Mean_36873"
//			<< setw(20) << "UEOrientRAD"
//			<< endl;
//	}
//
//	{
//		Observer::Print("ITUCalibration_RedCap") << "btsid"
//			<< setw(20) << "btsIndex"
//			<< setw(20) << "IsServBTS"
//			<< setw(20) << "UEID"
//			<< setw(20) << "UEPosX"
//			<< setw(20) << "UEPosY"
//			<< setw(20) << "UEWrapPosX"
//			<< setw(20) << "UEWrapPosY"
//			<< setw(20) << "MSOrient"
//			<< setw(20) << "BTSPosX"
//			<< setw(20) << "BTSPosY"
//			<< setw(20) << "BTSWrapPosX"
//			<< setw(20) << "BTSWrapPosY"
//			<< setw(20) << "2DDistance"
//			<< setw(20) << "3DDistance"
//			<< setw(20) << "TotalFloorNum"
//			<< setw(20) << "FloorIndex"
//			<< setw(20) << "UEHeight"
//			<< setw(20) << "IsStrongLink"
//			<< setw(20) << "IsLOS"
//			<< setw(20) << "IsInH"
//			<< setw(20) << "isLowLoss"
//			<< setw(20) << "LoSAoD"
//			<< setw(20) << "LoSEoD"
//			<< setw(20) << "LoSAoA"
//			<< setw(20) << "LoSEoA"
//			<< setw(20) << "LinkLossDB"
//			<< setw(20) << "LoSLinkLossDB"
//			<< setw(20) << "PathLossDB"
//			<< setw(20) << "ShadowFadeDB"
//			<< setw(20) << "BTSBeamIndex"
//			<< setw(20) << "MSBeamIndex"
//			<< setw(20) << "PanelIndex"
//			<< setw(20) << "GeometryDB_SNR"
//			<< setw(20) << "GeometryDB_SINR"
//			<< setw(20) << "Geometrydb100MHz"
//			<< setw(20) << "RSRP_36873"
//			<< setw(20) << "CouplingLoss_36873"
//			<< setw(20) << "SNR_36873"
//			<< setw(20) << "SINR_36873_Radom"
//			<< setw(20) << "BS_V_beamDEG"
//			<< setw(20) << "BS_H_beamDEG"
//			<< setw(20) << "UE_V_beamDEG"
//			<< setw(20) << "UE_H_beamDEG"
//			<< setw(20) << "InfP_Mean_36873"
//			<< setw(20) << "UEOrientRAD"
//			<< endl;
//	}

//    {
//        Observer::Print("MUschedularUENum") << "Time"
//                                            << setw(20) << "btsIndex"
//                                            << setw(20) << "SB1"
//                                            << setw(20) << "SB2"
//                                            << setw(20) << "SB3"
//                                            << setw(20) << "SB4"
//                                            << setw(20) << "SB5"
//                                            << setw(20) << "SB6"
//                                            << setw(20) << "SB7"
//                                            << setw(20) << "SB8"
//                                            << setw(20) << "SB9"
//                                            << setw(20) << "SB10"
//                                            << endl;
//    }
//
//    {
//        Observer::Print("MUschedularLayerNum") << "Time"
//                                               << setw(20) << "btsIndex"
//                                               << setw(20) << "SB1"
//                                               << setw(20) << "SB2"
//                                               << setw(20) << "SB3"
//                                               << setw(20) << "SB4"
//                                               << setw(20) << "SB5"
//                                               << setw(20) << "SB6"
//                                               << setw(20) << "SB7"
//                                               << setw(20) << "SB8"
//                                               << setw(20) << "SB9"
//                                               << setw(20) << "SB10"
//                                               << endl;
//    }
//    if(Parameters::Instance().TRAFFIC.ITrafficModel==Parameters::ITrafficModel_XRmulti){
//        Observer::Print("XR_multi_stream_Packet") << setw(20) << "ID"
//                                                  << setw(20) << "LogTime"
//                                                  << setw(20) << "MSID"
//                                                  << setw(20) << "BTSID"
//                                                  << setw(20) << "OriginSize"
//                                                  << setw(20) << "RemainderSize"
//                                                  << setw(20) << "TxSize"
//                                                  << setw(20) << "RightRxSize"
//                                                  << setw(20) << "IsFinished"
//                                                  << setw(20) << "IsCompleted"
//                                                  << setw(20) << "IsDroped"
//                                                  << setw(20) << "BornTime"
//                                                  << setw(20) << "FinishTime"
//                                                  << setw(20) << "Delay" <<setw(20)<<"OriginalSlot"<<setw(20)<<"IfIFrame"<< endl;
//    }
//    Observer::Print("NAKQueueInfo")<<"Slot BTS before/after SchedulingMessageDLID MSID Born LastTrans Sendnum"<<endl;
//    Observer::Print("SchedulerMessageDLinfo")<<setw(20)<<"Slot"<<setw(20)<<"Function"<<setw(20)<<"(BTS,BTSIndex)"<<setw(20)<<"MessageID"<<setw(20)<<"MSID"<<setw(20)<<"Born"<<setw(20)<<"LastTrans"<<setw(20)<<"Sendnum"<<endl;


        //hyl 冗余
//        Observer::Print("TrafficRecord") << setw(20) << "ID"
//                                         << setw(20) << "MSID"
//                                         << setw(20) << "BTSID"
//                                         << setw(20) << "OriginSize"
//                                         << setw(20) << "RemainderSize"
//                                         << setw(20) << "TxSize"
//                                         << setw(20) << "RightRxSize"
//                                         << setw(20) << "IsFinished"
//                                         << setw(20) << "IsCompleted"
//                                         << setw(20) << "BornTime"
//                                         << setw(20) << "FinishTime"
//                                         << setw(20) << "Delay"
//                                         << setw(20) << "TrafficType"
//                                         << setw(20) << "Pirority"
//                                         << setw(20) << "DL/UL"
//                                         << endl;

        Observer::Print("PacketRecord") << setw(20) << "ID"
                                         << setw(20) << "MSID"
                                         << setw(20) << "BTSID"
                                         << setw(20) << "OriginSize"
                                         << setw(20) << "RemainderSize"
                                         << setw(20) << "RightRxSize"
                                         << setw(20) << "IsFinished"
                                         << setw(20) << "BornTime"
                                         << setw(20) << "FinishTime"
                                         << setw(20) << "Delay"
                                         << setw(20) << "TrafficType"
                                         << setw(20) << "Pirority"
                << setw(20) << "DL/UL"
                << endl;
}

void NetWorkDrive::simthread(){
    { //start
        ///添加基站
        BSManager::Instance().AddBSs();
        ///配置基站的位置，以及BTS的位置和天线方向
        BSManager::Instance().DistributeBSs();
        ///
        BSManager::Instance().ConstructPicoGroups();
        ///打印输出表的表头，即将统计数据的文件输出相应表头。统计类在生成类对象的时候，建立相应的输出文件流

    }
    for (int drop = 1; drop <= Parameters::Instance().BASIC.INumSnapShot; ++drop)
    {
        G_ICurDrop = drop;
        DropInitialize();
        cout << "Drop " << drop << " start ..." << endl;
        //@threads
        long int _timebegin = 0, _timeend = 0;
        time(&_timebegin);
//        if (Parameters::Instance().BASIC.BISMultiThread) {
            Thread_control::Instance().run();

//        }
//        else{
//            // 下面进行时隙循环
//            for (int slot = 1; slot <= Parameters::Instance().BASIC.ISlotPerDrop; ++slot) {
//                // 推动时钟前进
//                Clock::Instance().Forward();
//                // 输出当前的slot号
//                cout << "slot: " << slot << "/" << Parameters::Instance().BASIC.ISlotPerDrop
//                     << ",  drop: " << drop << "/" << Parameters::Instance().BASIC.INumSnapShot << endl;
//                SlotRun();
//            }
//        }
        time(&_timeend);
        long int elapsed =_timeend- _timebegin;
        cout << "threads" << elapsed << endl;
        DropEnd();
    }
    BSManager::Instance().ReMove();
}

///20260115
// void NetWorkDrive::ClearFinished() {
//     vector<MSID> ToBeClearedMSIDs = MSManager::Instance().GetFinishedMSIDs();
//     ClearLinkMatrix(ToBeClearedMSIDs);
//     DisconnectMS(ToBeClearedMSIDs);
//     ClearMSs();
// }