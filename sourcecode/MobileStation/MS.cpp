///@file MS.cpp
///@brief MS类的实现
///@author wangfei

#include "../Utility/IDType.h"
#include "../Utility/RBID.h"
#include "../Statistician/Observer.h"
#include "../ChannelModel/AntennaOrientGain.h"
#include "../ChannelModel/AOGOmni.h"
#include "../ChannelModel/AOGSector.h"
#include "../ChannelModel/Rx.h"
#include "../ChannelModel/WrapAround.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../MobileStation/MSID.h"
#include "../BaseStation/BTSID.h"
#include "ACKNAKMessageDL.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../BaseStation/BS.h"
#include "../BaseStation/BTS.h"
#include "../NetworkDrive/BSManager.h"
#include "HARQRxStateMS.h"
#include "../DetectAlgorithm/Detector_DL.h"
#include "../DetectAlgorithm/MMSE_DL.h"
#include "../Statistician/Statistician.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "MSRxer.h"
#include "MSTxer.h"
#include "MSMRCTxer.h"
#include "MSEBBRank1Rxer.h"
#include "../NetworkDrive/Clock.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../Utility/SCID.h"
#include "MSMUEBBRankARxer.h"
#include "SRSTxer.h"
#include "MS.h"
#include "../ChannelModel/LinkMatrix.h"
//@thread
std::mutex initialize_lock;
std::mutex MacroMS_lock;
#include"../NetworkDrive/Thread_control.h"
#include <condition_variable>
//@thread

using namespace std;
using namespace itpp;

void MsTaskFunc(void* arg)
{
    MS* ms = (MS*)arg;
    ms->runThreadTask();
}

MS::MS(int _id):ms_SCPrecodeMat(Parameters::Instance().BASIC.ISCNum) {
    //@threads
//    msmutex = std::shared_ptr<std::mutex>(new std::mutex);
    //初始化基类Rx里面的属性
    bReInitialize = false;
    bHasTraffic = false;
    switch (Parameters::Instance().BASIC.IScenarioModel) {
        case Parameters::SCENARIO_LowFreq_INDOOR:
        case Parameters::SCENARIO_HighFreq_INDOOR:
            m_iSpecial = 0;
            m_dInCarLossDB = 0;
            m_SpeedSacleFactor = 1.0;
            break;
        case Parameters::SCENARIO_LowFreq_URBAN_MACRO:
        case Parameters::SCENARIO_HighFreq_URBAN_MACRO:
            // 0 for outdoor; 1 for indoor
            m_iSpecial = (random.xUniform_msconstruct(0, 1) <
                    Parameters::Instance().MSS.DProbOutdoor) ? 0 : 1;

            if (m_iSpecial == 0) {
                m_dInCarLossDB
                        = Parameters::Instance().MSS.bIsOutdoorInCar_notOnCar ?
                        -1 * (random.xNormal_msconstruct(0, 5) + 9) : 0.0;
                m_SpeedSacleFactor = Parameters::Instance().MSS.DCarSpeedSacleFactor;
            } else {
                m_dInCarLossDB = 0.0;
                m_SpeedSacleFactor = 1.0;
            }
            break;
        case Parameters::SCENARIO_5G_DENSE_URBAN_1LAYER:
            m_iSpecial = (random.xUniform_msconstruct(0, 1) < 0.2) ? 0 : 1;
            if (m_iSpecial == 0) {
                m_dInCarLossDB = -1 * (random.xNormal_msconstruct(0, 5) + 9);
                m_SpeedSacleFactor = Parameters::Instance().MSS.DCarSpeedSacleFactor;
            } else {
                m_dInCarLossDB = 0;
                m_SpeedSacleFactor = 1.0;
            }
            break;
        case Parameters::SCENARIO_LowFreq_URBAN_MICRO:
        case Parameters::SCENARIO_HighFreq_URBAN_MICRO:
            m_iSpecial = (random.xUniform_msconstruct(0, 1) <
                    Parameters::Instance().MSS.DProbOutdoor) ? 0 : 1;

            if (m_iSpecial == 0) {
                m_dInCarLossDB
                        = Parameters::Instance().MSS.bIsOutdoorInCar_notOnCar ?
                        -1 * (random.xNormal_msconstruct(0, 5) + 9) : 0.0;
                m_SpeedSacleFactor = Parameters::Instance().MSS.DCarSpeedSacleFactor;
            } else {
                m_dInCarLossDB = 0.0;
                m_SpeedSacleFactor = 1.0;
            }
            break;
        case Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER:
            m_iSpecial = (random.xUniform_msconstruct(0, 1) < 0.2) ? 0 : 1;
            m_dInCarLossDB = 0;
            m_SpeedSacleFactor = 1.0;
            break;
        case Parameters::SCENARIO_LowFreq_RURAL_MACRO:
            //if (Parameters::Instance().BASIC.ISubScenarioModel == Parameters::SCENARIO_Phase1_RMA_LMLC) {
            //            if (false) {
            //                m_iSpecial = (xUniform_msconstruct(0, 1) <
            //                        Parameters::Instance().MSS.DProbOutdoor) ? 0 : 1;
            //                //室外用户，有1/3是在车内，有2/3在车外
            //                bool bInCar = xUniform_msconstruct(0, 1) < 1.0 / 3.0;
            //                if (m_iSpecial == 0 && bInCar) {
            //                    m_dInCarLossDB = -1 * (xNormal_msconstruct(0, 5) + 9);
            //                    m_SpeedSacleFactor = Parameters::Instance().MSS.DCarSpeedSacleFactor;
            //                } else {
            //                    m_dInCarLossDB = 0.0;
            //                    m_SpeedSacleFactor = 1.0;
            //                }
            //            } else 
            //            {
            m_iSpecial = (random.xUniform_msconstruct(0, 1) <
                    Parameters::Instance().MSS.DProbOutdoor) ? 0 : 1;

            if (m_iSpecial == 0) {
                m_dInCarLossDB
                        = Parameters::Instance().MSS.bIsOutdoorInCar_notOnCar ?
                        -1 * (random.xNormal_msconstruct(0, 5) + 9) : 0.0;
                m_SpeedSacleFactor = Parameters::Instance().MSS.DCarSpeedSacleFactor;
            } else {
                m_dInCarLossDB = 0.0;
                m_SpeedSacleFactor = 1.0;
            }
            //            }
            break;
        case Parameters::SCENARIO_HighFreq_RURAL_MACRO:
            assert(false);
            break;
        default:
            assert(false);
            break;
    }
    m_iIsLowloss =
            (random.xUniform_channel(0, 1) < Parameters::Instance().MSS.DProbLowloss) ? 1 : 0;

    //    m_dRxOrientRAD = xUniform_msconstruct(0, 2 * M_PI);

    //20180625
    //    this->SetRxOrientRAD(xUniform_msconstruct(0, 2 * M_PI));
    m_iAntNum = Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel;
    m_dRxAntGainDB = Parameters::Instance().MSS.FirstBand.DAntennaGainDb;
    if (Parameters::Instance().MSS.FirstBand.IAntennaPatternMode == 0)
        m_pRxAOG = std::shared_ptr<cm::AntennaOrientGain > (new cm::AOGOmni());
    else if (Parameters::Instance().MSS.FirstBand.IAntennaPatternMode == 1)
        m_pRxAOG = std::shared_ptr<cm::AntennaOrientGain > (new cm::AOGSector(Parameters::Instance().MSS.FirstBand.DH3DBBeamWidthDeg, Parameters::Instance().MSS.FirstBand.DV3DBBeamWidthDeg, Parameters::Instance().MSS.FirstBand.DHBackLossDB, Parameters::Instance().MSS.FirstBand.DVBackLossDB, Parameters::Instance().MSS.FirstBand.DHBackLossDB, 0));
    else {
        cout<<"Parameters::Instance().MSS.FirstBand.IAntennaPatternMode ="<<Parameters::Instance().MSS.FirstBand.IAntennaPatternMode<<endl;
        cout << "Wrong Rx AntennaPattern" << endl;
        assert(false);
    }
    ///初始化基类Point里面的属性
    double dMoveDirection = random.xUniform_msconstruct(0, 2 * M_PI);
    complex<double> cSpeedMPS = polar(
            m_SpeedSacleFactor * Parameters::Instance().MSS.DVelocityMPS,
            dMoveDirection);
    SetSpeed(cSpeedMPS);

    ///初始化MSS新定义的属性
    m_ID = MSID(_id);
    m_vSINR.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, zeros(1, 1));

    if (Parameters::Instance().LINK_CTRL.I2DOr3DChannel == Parameters::IS3DCHANNEL
            && m_iSpecial == 1) {
        int iBuildingFloorNum =
                random.xUniformInt(Parameters::Instance().MSS.IMinBuildingFloor, Parameters::Instance().MSS.IMaxBuildingFloor);
        SetRxTotalFloorNum(iBuildingFloorNum);
        int iBuildingFloorIndex = random.xUniformInt(1, iBuildingFloorNum);
        SetRxFloorNum(iBuildingFloorIndex);
        double dUEHeight = 3 * (iBuildingFloorIndex - 1) + 1.5;
        SetRxHeightM(dUEHeight);
    }

    ///初始化MSS新定义的属性
    m_ID = MSID(_id);
    m_vSINR.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, zeros(1, 1));

    //初始化上行模块中MSS新定义的属性
    UL.Construct(m_ID);
    double lamda=Parameters::Instance().TRAFFIC.DMacroLamda;
    double interval = random.xExponent(lamda / 1000 / Parameters::Instance().BASIC.DSlotDuration_ms);
    while (interval < 1) {
        interval += random.xExponent(lamda / 1000 / Parameters::Instance().BASIC.DSlotDuration_ms);
    }
    packettime = interval;
    UL.packettime = interval;
}
MS::MS(int _id,int _type):Rx(_type),ms_SCPrecodeMat(Parameters::Instance().BASIC.ISCNum){
    //@threads
    //初始化基类Rx里面的属性
    bReInitialize = false;
    bHasTraffic = false;
    switch (Parameters::Instance().BASIC.IScenarioModel) {
        case Parameters::SCENARIO_LowFreq_INDOOR:
        case Parameters::SCENARIO_HighFreq_INDOOR:
            m_iSpecial = 1;
            m_dInCarLossDB = 0;
            m_SpeedSacleFactor = 1.0;
            break;
        case Parameters::SCENARIO_LowFreq_URBAN_MACRO:
        case Parameters::SCENARIO_HighFreq_URBAN_MACRO:
            // 0 for outdoor; 1 for indoor
            m_iSpecial = (random.xUniform_msconstruct(0, 1) <
                    Parameters::Instance().MSS.DProbOutdoor) ? 0 : 1;

            if (m_iSpecial == 0) {
                m_dInCarLossDB
                        = Parameters::Instance().MSS.bIsOutdoorInCar_notOnCar ?
                        -1 * (random.xNormal_msconstruct(0, 5) + 9) : 0.0;
                m_SpeedSacleFactor = Parameters::Instance().MSS.DCarSpeedSacleFactor;
            } else {
                m_dInCarLossDB = 0.0;
                m_SpeedSacleFactor = 1.0;
            }
            break;
        case Parameters::SCENARIO_5G_DENSE_URBAN_1LAYER:
            m_iSpecial = (random.xUniform_msconstruct(0, 1) < 0.2) ? 0 : 1;
            if (m_iSpecial == 0) {
                m_dInCarLossDB = -1 * (random.xNormal_msconstruct(0, 5) + 9);
                m_SpeedSacleFactor = Parameters::Instance().MSS.DCarSpeedSacleFactor;
            } else {
                m_dInCarLossDB = 0;
                m_SpeedSacleFactor = 1.0;
            }
            break;
        case Parameters::SCENARIO_LowFreq_URBAN_MICRO:
        case Parameters::SCENARIO_HighFreq_URBAN_MICRO:
            m_iSpecial = (random.xUniform_msconstruct(0, 1) <
                    Parameters::Instance().MSS.DProbOutdoor) ? 0 : 1;

            if (m_iSpecial == 0) {
                m_dInCarLossDB
                        = Parameters::Instance().MSS.bIsOutdoorInCar_notOnCar ?
                        -1 * (random.xNormal_msconstruct(0, 5) + 9) : 0.0;
                m_SpeedSacleFactor = Parameters::Instance().MSS.DCarSpeedSacleFactor;
            } else {
                m_dInCarLossDB = 0.0;
                m_SpeedSacleFactor = 1.0;
            }
            break;
        case Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER:
            m_iSpecial = (random.xUniform_msconstruct(0, 1) < 0.2) ? 0 : 1;
            m_dInCarLossDB = 0;
            m_SpeedSacleFactor = 1.0;
            break;
        case Parameters::SCENARIO_LowFreq_RURAL_MACRO:
            m_iSpecial = (random.xUniform_msconstruct(0, 1) <
                          Parameters::Instance().MSS.DProbOutdoor) ? 0 : 1;

            if (m_iSpecial == 0) {
                m_dInCarLossDB
                        = Parameters::Instance().MSS.bIsOutdoorInCar_notOnCar ?
                          -1 * (random.xNormal_msconstruct(0, 5) + 9) : 0.0;
                m_SpeedSacleFactor = Parameters::Instance().MSS.DCarSpeedSacleFactor;
            } else {
                m_dInCarLossDB = 0.0;
                m_SpeedSacleFactor = 1.0;
            }
            break;
        case Parameters::SCENARIO_HighFreq_RURAL_MACRO:
            assert(false);
            break;
        default:
            assert(false);
            break;
    }
    m_iIsLowloss =
            (random.xUniform_channel(0, 1) < Parameters::Instance().MSS.DProbLowloss) ? 1 : 0;

    //    m_dRxOrientRAD = xUniform_msconstruct(0, 2 * M_PI);

    //20180625
    //    this->SetRxOrientRAD(xUniform_msconstruct(0, 2 * M_PI));
    m_iAntNum = Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel;
    if(_type==1){
        m_dRxAntGainDB = Parameters::Instance().MSS.FirstBand.DAntennaGainDb;
    }
    else if(_type==2){
        m_dRxAntGainDB = Parameters::Instance().MSS.FirstBand.DAntennaGainDb_2;
    }
    if (Parameters::Instance().MSS.FirstBand.IAntennaPatternMode == 0)
        m_pRxAOG = std::shared_ptr<cm::AntennaOrientGain > (new cm::AOGOmni());
    else if (Parameters::Instance().MSS.FirstBand.IAntennaPatternMode == 1)
        m_pRxAOG = std::shared_ptr<cm::AntennaOrientGain > (new cm::AOGSector(Parameters::Instance().MSS.FirstBand.DH3DBBeamWidthDeg, Parameters::Instance().MSS.FirstBand.DV3DBBeamWidthDeg, Parameters::Instance().MSS.FirstBand.DHBackLossDB, Parameters::Instance().MSS.FirstBand.DVBackLossDB, Parameters::Instance().MSS.FirstBand.DHBackLossDB, 0));
    else {
        cout << "Wrong Rx AntennaPattern" << endl;
        assert(false);
    }
    ///初始化基类Point里面的属性
    double dMoveDirection = random.xUniform_msconstruct(0, 2 * M_PI);
    complex<double> cSpeedMPS = polar(
            m_SpeedSacleFactor * Parameters::Instance().MSS.DVelocityMPS,
            dMoveDirection);
    SetSpeed(cSpeedMPS);


    if (Parameters::Instance().LINK_CTRL.I2DOr3DChannel == Parameters::IS3DCHANNEL
            && m_iSpecial == 1) {
        int iBuildingFloorNum =
                random.xUniformInt(Parameters::Instance().MSS.IMinBuildingFloor, Parameters::Instance().MSS.IMaxBuildingFloor);
        SetRxTotalFloorNum(iBuildingFloorNum);
        int iBuildingFloorIndex = random.xUniformInt(1, iBuildingFloorNum);
        SetRxFloorNum(iBuildingFloorIndex);
        double dUEHeight = 3 * (iBuildingFloorIndex - 1) + 1.5;
        SetRxHeightM(dUEHeight);
    }
    m_ID = MSID(_id);
    m_vSINR.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, zeros(1, 1));
    iBestRank_UL = -1;
    //初始化上行模块中MSS新定义的属性
    UL.Construct(m_ID);
    double lamda=Parameters::Instance().TRAFFIC.DMacroLamda;
    double interval = random.xExponent(lamda / 1000 / Parameters::Instance().BASIC.DSlotDuration_ms);
    while (interval < 1) {
        interval += random.xExponent(lamda / 1000 / Parameters::Instance().BASIC.DSlotDuration_ms);
    }
    packettime = interval;
    UL.packettime = interval;
}

void MS::Initialize_for_smallcell() {
    std::vector<std::pair<double, MacroID> > vReceiveMacroPowerDB;
    std::vector<std::pair<double, PicoID> > vReceivePicoPowerDB;
    //20200522 Di 对geometry进行初始化
    m_dGeometryDB_SNR = 0;
    m_dGeometryDB_SINR = 0;
    m_dGeometrydb100MHz = 0;
    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) { ///注意此处修改了BTSID的++重载函数
        double dLinkLossDB = cm::LinkMatrix::Instance().GetCouplingLossDB(btsid.GetBTS(), *this);
        double dReceivePowerDB = 0;
        if (BSManager::IsMacro(btsid)) {
            dReceivePowerDB = L2DB(Parameters::Instance().Macro.DL.DMaxTxPowerMw * DB2L(dLinkLossDB));
            vReceiveMacroPowerDB.push_back(std::make_pair(dReceivePowerDB, btsid));
        } else {
            dReceivePowerDB = L2DB(Parameters::Instance().SmallCell.LINK.DPicoOutMaxTxPowerMw * DB2L(dLinkLossDB)) + Parameters::Instance().SmallCell.LINK.DPicoOutBiasDb;
            vReceivePicoPowerDB.push_back(std::make_pair(dReceivePowerDB, btsid));
        }
    }
    std::sort(vReceiveMacroPowerDB.begin(), vReceiveMacroPowerDB.end(), std::greater<std::pair<double, BTSID> >());
    std::sort(vReceivePicoPowerDB.begin(), vReceivePicoPowerDB.end(), std::greater<std::pair<double, BTSID> >());

    int iCandidateMacroSize = 1;
    for (; iCandidateMacroSize<static_cast<int> (vReceiveMacroPowerDB.size()); ++iCandidateMacroSize) {
        if (vReceiveMacroPowerDB[0].first - vReceiveMacroPowerDB[iCandidateMacroSize].first > Parameters::Instance().BASIC.DHandOffMarginDb) {
            break;
        }
    }
    int iBestMacroIndex = random.xUniformInt(0, iCandidateMacroSize - 1);

    int iCandidatePicoSize = 1;
    for (; iCandidatePicoSize<static_cast<int> (vReceivePicoPowerDB.size()); ++iCandidatePicoSize) {
        if (vReceivePicoPowerDB[0].first - vReceivePicoPowerDB[iCandidatePicoSize].first > Parameters::Instance().BASIC.DHandOffMarginDb) {
            break;
        }
    }
    int iBestPicoIndex = random.xUniformInt(0, iCandidatePicoSize - 1);

    MacroID bestMacroID = vReceiveMacroPowerDB[iBestMacroIndex].second;
    PicoID bestPicoID = vReceivePicoPowerDB[iBestPicoIndex].second;

    double dMacroRSRPDB = vReceiveMacroPowerDB[iBestMacroIndex].first;
    double dPicoRSRPDB = vReceivePicoPowerDB[iBestPicoIndex].first;

    double dMacroRSRQDB = 0;
    double dPicoRSRQDB = 0;
    double dbestMacroLinkLoss = DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(bestMacroID.GetBTS(), *this));
    double dbestMacroRevPower = dbestMacroLinkLoss * Parameters::Instance().Macro.DL.DMaxTxPowerMw;
    double dbestPicoLinkLoss = DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(bestPicoID.GetBTS(), *this));
    double dbestPicoRevPower = dbestPicoLinkLoss * Parameters::Instance().SmallCell.LINK.DPicoOutMaxTxPowerMw;

    double dInterfLinkLossMacroSum = 0;
    double dInterfLinkLossPicoSum = 0;
    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        if (btsid == bestMacroID || btsid == bestPicoID)
            continue;
        if (BSManager::IsMacro(btsid)) {
            dInterfLinkLossMacroSum += DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(btsid.GetBTS(), *this));
        } else {
            dInterfLinkLossPicoSum += DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(btsid.GetBTS(), *this));
        }
    }

    double dInterfMacro = Parameters::Instance().Macro.DL.DMaxTxPowerMw * dInterfLinkLossMacroSum;
    double dInterfPico = Parameters::Instance().SmallCell.LINK.DPicoOutMaxTxPowerMw * dInterfLinkLossPicoSum;
    double dNoise = Parameters::Instance().MSS.FirstBand.DL.DNoisePowerMw;
    dMacroRSRQDB = L2DB(dbestMacroRevPower / (dInterfMacro + dNoise));
    dPicoRSRQDB = L2DB(dbestPicoRevPower / (dInterfPico + dNoise));

    if (Parameters::Instance().Macro.LINK.DRadioFrequencyMHz == Parameters::Instance().SmallCell.LINK.DRadioFrequencyMHz) {
        m_MainServBTS = dMacroRSRPDB > dPicoRSRPDB ? bestMacroID : bestPicoID;
        double dbestRevPower = dMacroRSRPDB > dPicoRSRPDB ? dbestMacroRevPower : dbestPicoRevPower;
        m_dGeometryDB_SNR = L2DB(dbestRevPower / (dInterfMacro + dInterfPico + dNoise));
    } else {
        //按照RSRQ
        m_MainServBTS = dMacroRSRQDB > dPicoRSRQDB ? bestMacroID : bestPicoID;
        m_dGeometryDB_SNR = dMacroRSRQDB > dPicoRSRQDB ? dMacroRSRQDB : dPicoRSRQDB;
        //按照RSRP
        //            m_MainServBTS = dMacroRSRPDB>dPicoRSRPDB?bestMacroID:bestPicoID;
        //            m_dGeometryDB = dMacroRSRPDB>dPicoRSRPDB?dMacroRSRQDB:dPicoRSRQDB;
    }
}

void MS::UE_attach(){
    std::vector<std::pair<double, BTSID> > vReceivePowerDBrBTSID; ///@xlong
    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) { ///@xlong 注意此处修改了BTSID的++重载函数
        double dLinkLossDB = cm::LinkMatrix::Instance().GetCouplingLossDB(btsid.GetBTS(), *this);
        //double dLinkLossDB = cm::LinkMatrix::Instance().GetLoSLinkLossDB(btsid.GetBTS(), *this);
        double dReceivePowerDB = 0;
        if (BSManager::IsMacro(btsid)) {///@xlong 如果是真正的BTS
            dReceivePowerDB = L2DB(Parameters::Instance().Macro.DL.DMaxTxPowerMw * DB2L(dLinkLossDB));
        } else {///@zxy 如果是被当作BTS的pico（两者分开处理是因为发射功率不同）
            dReceivePowerDB = L2DB(Parameters::Instance().SmallCell.LINK.DPicoOutMaxTxPowerMw * DB2L(dLinkLossDB)) + Parameters::Instance().SmallCell.LINK.DPicoOutBiasDb;
        }
        vReceivePowerDBrBTSID.push_back(std::make_pair(dReceivePowerDB, btsid));
    }
    
    std::sort(vReceivePowerDBrBTSID.begin(), vReceivePowerDBrBTSID.end(), std::greater<std::pair<double, BTSID> >());
    
    int iCandidateSize = 1;
    for (; iCandidateSize<static_cast<int> (vReceivePowerDBrBTSID.size()); ++iCandidateSize) {
        if (vReceivePowerDBrBTSID[0].first - vReceivePowerDBrBTSID[iCandidateSize].first > Parameters::Instance().BASIC.DHandOffMarginDb) {
            break;
        }
    }
    int iIndex = random.xUniformInt(0, iCandidateSize - 1);
//    m_MainServBTS = vReceivePowerDBrBTSID[iIndex].second;
    m_MainServBTS = vReceivePowerDBrBTSID[0].second;
}

void MS::UE_attach_new() {
    std::vector<std::pair<double, BTSID> > vReceivePowerDBrBTSID; ///@xlong
    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) { ///@xlong 注意此处修改了BTSID的++重载函数
//        //double dLinkLossDB = cm::LinkMatrix::Instance().GetCouplingLossDB(btsid.GetBTS(), *this);
//        double dLinkLossDB = cm::LinkMatrix::Instance().GetPurePathLossDB(btsid.GetBTS(), *this);//未计算天线增益的路损tsm@20221102
//        double max_antenna_gain(0);
        std::shared_ptr<cm::Antenna> pBSAntenna = btsid.GetBTS().GetAntennaPointer();
        double BTS_x(0),BTS_y(0),BTS_z(0),MS_x(0),MS_y(0),MS_z(0);
        BTS_x=btsid.GetBTS().GetX();
        BTS_y=btsid.GetBTS().GetY();
        BTS_z=btsid.GetBTS().GetTxHeightM();
        MS_x=this->GetX();
        MS_y=this->GetY();//此处还要添加wraproundtsm@20221102
        MS_z=this->GetRxHeightM();
//        double temp_GSC_AOD=atan((MS_y-BTS_y)/(MS_x-BTS_x));
//        double temp_GSC_EOD=atan(sqrt(pow(BTS_x-MS_x,2)+pow(BTS_y-MS_y,2))/(BTS_z-MS_z));
//        BOOST_FOREACH(std::shared_ptr<cm::AntennaPanel> pBSAntennaPanel , pBSAntenna->GetvAntennaPanels()) {
//                        for (int i = 0; i < Parameters::Instance().Real_map_info.antenna_gain.size(); i++)
//                            max_antenna_gain = max(Parameters::Instance().Real_map_info.antenna_gain[i][
//                                                           ALoSRAD_GCS2LCS(temp_GSC_AOD, temp_GSC_EOD,
//                                                                           pBSAntennaPanel->GetTxRxOrientRAD(),
//                                                                           pBSAntennaPanel->GetMechanicalTiltRAD(),
//                                                                           0)+180][
//                                                           ELoSRAD_GCS2LCS(temp_GSC_AOD, temp_GSC_EOD,
//                                                                           pBSAntennaPanel->GetTxRxOrientRAD(),
//                                                                           pBSAntennaPanel->GetMechanicalTiltRAD(),
//                                                                           0)], max_antenna_gain);
//                    }
//        dLinkLossDB+=max_antenna_gain;
//        double dReceivePowerDB = 0;
//        if (BSManager::IsMacro(btsid)) {///@xlong 如果是真正的BTS
//            dReceivePowerDB = L2DB(Parameters::Instance().Macro.DL.DMaxTxPowerMw * DB2L(dLinkLossDB));
//        } else {///@zxy 如果是被当作BTS的pico（两者分开处理是因为发射功率不同）
//            dReceivePowerDB = L2DB(Parameters::Instance().SmallCell.LINK.DPicoOutMaxTxPowerMw * DB2L(dLinkLossDB)) + Parameters::Instance().SmallCell.LINK.DPicoOutBiasDb;
//        }
//        cout<<"BTS = "<<btsid<<"  ("<<BTS_x<<','<<BTS_y<<','<<BTS_z<<")  orient = "<<pBSAntenna->GetTxRxOrientRAD()/M_PI*180<<"    MS = "<<m_ID<<"  ("<<MS_x<<','<<MS_y<<','<<MS_z<<")   dxdy= ("<<MS_x-BTS_x<<','<<MS_y-BTS_y<<")   RSRP = "<<dReceivePowerDB<<"  LinkLoss = "<<dLinkLossDB<<"  Gain = "<<max_antenna_gain<<endl;
//        vReceivePowerDBrBTSID.push_back(std::make_pair(dReceivePowerDB, btsid));
        double dLinkLossDB = cm::LinkMatrix::Instance().GetCouplingLossDB(btsid.GetBTS(), *this);
        double dpurePathLoss = cm::LinkMatrix::Instance().GetPathLossDB(btsid.GetBTS(), *this);
        //double dLinkLossDB = cm::LinkMatrix::Instance().GetLoSLinkLossDB(btsid.GetBTS(), *this);
        double dReceivePowerDB = 0;
        if (BSManager::IsMacro(btsid)) {///@xlong 如果是真正的BTS
            dReceivePowerDB = L2DB(Parameters::Instance().Macro.DL.DMaxTxPowerMw * DB2L(dLinkLossDB));
        } else {///@zxy 如果是被当作BTS的pico（两者分开处理是因为发射功率不同）
            dReceivePowerDB = L2DB(Parameters::Instance().SmallCell.LINK.DPicoOutMaxTxPowerMw * DB2L(dLinkLossDB)) + Parameters::Instance().SmallCell.LINK.DPicoOutBiasDb;
        }
        vReceivePowerDBrBTSID.push_back(std::make_pair(dReceivePowerDB, btsid));
    }

    std::sort(vReceivePowerDBrBTSID.begin(), vReceivePowerDBrBTSID.end(), std::greater<std::pair<double, BTSID> >());

    int iCandidateSize = 0;
    for (; iCandidateSize<static_cast<int> (vReceivePowerDBrBTSID.size()); ++iCandidateSize) {
        if (vReceivePowerDBrBTSID[0].first - vReceivePowerDBrBTSID[iCandidateSize].first > Parameters::Instance().BASIC.DHandOffMarginDb) {
            break;
        }
    }
    if(iCandidateSize==0)
        m_MainServBTS=BTSID(-1);
    else {
        int iIndex = random.xUniformInt(0, iCandidateSize - 1);
        m_MainServBTS = vReceivePowerDBrBTSID[iIndex].second;
    }
}
std::mutex outlock;
bool MS::Initialize() {
    m_clsMacMs.setMs(this);
    uci.initilize(m_ID);
    // 选择主服务基站
    if (Parameters::Instance().BASIC.IScenarioModel == Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER && Parameters::Instance().SmallCell.LINK.IHetnetEnvironmentType == Parameters::IHetnet_Cluster) {
        Initialize_for_smallcell();

    } else {
        // 选择主服务基站
        UE_attach();
    }

    //接入失败或者MS位置不对，需重新撒点，重新接入
    if (m_MainServBTS.ToInt()==-1||!m_MainServBTS.GetBTS().ConnectMS(m_ID)) {
        return false;
    }
    BasicOutput_Statistican();


    //
    //    Statistician::Instance().m_MSData[m_ID].m_x = this->GetX();
    //    Statistician::Instance().m_MSData[m_ID].m_y = this->GetY();
    //    Statistician::Instance().m_BTSData[m_MainServBTS].m_iNumServMS += 1; //zy: move this to bts::ConnnectMS();

    if (Parameters::Instance().BASIC.DWorkingMode
            == Parameters::WorkingMode_Normal) {
        //初始化接收机
        switch (Parameters::Instance().MIMO_CTRL.IMIMOMode) {
            case Parameters::IMIMOMode_MUBFRankA:
                m_pMSRxer = std::shared_ptr<MSRxer > (new MSMUEBBRankARxer(m_MainServBTS, m_ID, &m_vSINR, &m_HARQRxState));
                break;
            default:
                cout << "The value of Parameters::Instance().SIM.DL.IMIMOMode is uncorrect, please check it." << std::endl;
                assert(false);
                break;
        }
        ///初始化SRS发射机
        m_pSRSTxer = std::shared_ptr<SRSTxer > (new SRSTxer(m_MainServBTS, m_ID));

    }


    ITU_Phase1_Stat_after_UE_attach_new();


    //用于上行模块的初始化，上行发射机初始化+信息统计
    UL.Initialize();

    return true;
}
std::mutex data_lock;
void MS::WorkSlot()
{
    UL.m_pMSTxer->GetPHR().timerRun();
    m_clsMacMs.TimerRun();
    if (Parameters::Instance().BASIC.DWorkingMode == Parameters::WorkingMode_Normal) {
        int iTime = Clock::Instance().GetTimeSlot();
        if (DownOrUpLink(iTime) == 0 || DownOrUpLink(iTime) == 2) {
            WorkSlotDL();
        } else if (DownOrUpLink(iTime) == 1) {
            WorkSlotUL();
        }
    }
}

void MS::WorkSlotDL() {
    if(Parameters::Instance().BASIC.IDLORUL != Parameters::UL){
        m_pMSRxer->WorkSlot();
    }
}

void MS::WorkSlotUL() {

    if(Parameters::Instance().BASIC.IDLORUL != Parameters::DL){

        //TODO
        UL.WorkSlotUL();
//        if(Parameters::Instance().MSS_UL.UL.IsPhrOn){
//            uci.SendPHRMes();
//        }
//        if(Parameters::Instance().MSS_UL.UL.IsSROn){
//            uci.SendSRMes();
//        }
    }
    uci.WorkSlot();
}

void MS::Reset() {
    m_vSINR.clear();
    UL.Reset();
    uci.Reset();
}

void MS::ReceiveSchedulingMessage(std::shared_ptr<SchedulingMessageDL> _pSchedulingMes) {
    m_pMSRxer->ReceiveScheduleMessage(_pSchedulingMes);
}

BTSID MS::GetMainServBTS() {
    return m_MainServBTS;
}



std::vector<BTSID> MS::GetAllServBTSs() {
    std::vector<BTSID> v;
    // 根据你的初始化逻辑，m_MainServBTS 有可能是 -1 表示无服务基站
    if (m_MainServBTS.ToInt() != -1) {
        v.push_back(m_MainServBTS);
    }
    return v;
}



MSID MS::GetID() {
    return m_ID;
}

double MS::GetGeometry() {
    return m_dGeometryDB_SNR;
}

//返回数据符号

cmat MS::GetTxDataSymbol(SCID _scid) {
    //add code here
    return itpp::cmat(0, 0);
}

//返回参考符号

cmat MS::GetTxSRS(SCID _scid) {
    //add code here
    return itpp::cmat(0, 0);
}

double MS::GetOllA(int _iRank, int _iRankID) {
    //输入：第一个是用户是几流，第二个是当前是第几流
    /*
    if (_iRankID == 0) {
        return m_pMSRxer->m_dOLLAOffsetDB;
    } else if (_iRankID == 1) {
        return m_pMSRxer->m_dOLLAOffset2DB;
    } else {
        cout << "MS::GetOllA Error!" << endl;
        assert(false);
    }
     */
    return m_pMSRxer->m_dOLLAOffsetDB(_iRank, _iRankID);

}
double MS::GetOllA(int _iRank) {
    //输入：第一个是用户是几流，第二个是当前是第几流
    /*
    if (_iRankID == 0) {
        return m_pMSRxer->m_dOLLAOffsetDB;
    } else if (_iRankID == 1) {
        return m_pMSRxer->m_dOLLAOffset2DB;
    } else {
        cout << "MS::GetOllA Error!" << endl;
        assert(false);
    }
     */
    return m_pMSRxer->m_dOLLAOffsetDB(_iRank, 0);

}

cmat MS::GetCovR(SCID _scid) {
    const static int iSpace = Parameters::Instance().LINK_CTRL.IFrequencySpace; //子载波插值粒度
    return m_pMSRxer->m_vCovR[_scid.ToInt() / iSpace];
}

void MS::BeginApSounding() {
    m_pMSRxer->ApSounding();
}

void MS::CalcSINR_for_Calibration_new(
        double& _RSRP_dB_out, double& _CouplingLoss_dB_out,
        double& _SNR_dB_out, double& _SINR_dB_out, double& _Interf_dB_out) {

    double dServpower_forAllRBS = 0;
    double dInterfPower_forAllRBS = 0;
    double dCouplingLoss = 0;

    ///////////////////////////
    ///计算Geometry
    cm::LinkMatrix&lm = cm::LinkMatrix::Instance();

    for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
        if (btsid == m_MainServBTS) {
            dCouplingLoss =
                    lm.GetCouplingLoss_linear_u36873_w_BestBeamPair_4_BestPanelPair(
                    btsid.GetBTS(), *this);

            dServpower_forAllRBS = dCouplingLoss *
                    Parameters::Instance().Macro.DL.DMaxTxPowerMw;

        } else { // calc interf BTS case
            double dInterfCouplingLoss =
                    lm.GetCouplingLoss_linear_u36873_w_RandomBSBeam_4_BestPanelPair(
                    btsid.GetBTS(), *this);

            dInterfPower_forAllRBS += dInterfCouplingLoss *
                    Parameters::Instance().Macro.DL.DMaxTxPowerMw;
        }
    }

    double dNoise_forAllRBS =
            Parameters::Instance().MSS.FirstBand.DL.DNoisePowerMw;

    _RSRP_dB_out = L2DB(dServpower_forAllRBS);
    _CouplingLoss_dB_out = L2DB(dCouplingLoss);

    //20180325
    _Interf_dB_out = L2DB(dInterfPower_forAllRBS
            / Parameters::Instance().BASIC.IRBNum);

    _SNR_dB_out = L2DB(dServpower_forAllRBS / dNoise_forAllRBS);
    _SINR_dB_out = L2DB(
            dServpower_forAllRBS / (dNoise_forAllRBS + dInterfPower_forAllRBS));
//    cout<<"dInterfPower_forAllRBS="<<dInterfPower_forAllRBS<<endl;
//    cout<<"dNoise_forAllRBS="<<dNoise_forAllRBS<<endl;
//    cout<<"(dNoise_forAllRBS + dInterfPower_forAllRBS)="<<(dNoise_forAllRBS + dInterfPower_forAllRBS)<<endl;
//    cout<<"dServpower_forAllRBS="<<dServpower_forAllRBS<<endl;
//    std::cout<<"_SINR_dB_out="<<_SINR_dB_out<<endl;

}

void MS::ITU_Phase1_Stat_after_UE_attach_new() {
    cm::Point tx_wrap;
    cm::Point rx_wrap;
    tx_wrap = cm::WrapAround::Instance().WrapTx(*this, m_MainServBTS.GetBTS());
    rx_wrap = cm::WrapAround::Instance().WrapRx(*this, m_MainServBTS.GetBTS());
    bool IsServBTS = true;
    double d2DDistanceM = cm::LinkMatrix::Instance().Get2DDistanceM(m_MainServBTS.GetBTS(), *this);
    double d3DDistanceM = cm::LinkMatrix::Instance().Get3DDistanceM(m_MainServBTS.GetBTS(), *this);
    bool bIsStrong = cm::LinkMatrix::Instance().IsStrong(m_MainServBTS.GetBTS(), *this);
    if (!bIsStrong) {
//        cout << "not Strong Link" << endl;
    }

    double dServePathLossDB = cm::LinkMatrix::Instance().GetPathLossDB(m_MainServBTS.GetBTS(), *this);
    double dServeLinkLossDB = cm::LinkMatrix::Instance().GetCouplingLossDB(m_MainServBTS.GetBTS(), *this);
    double dServeShadowFadeDB = cm::LinkMatrix::Instance().GetShadowFadeDB(m_MainServBTS.GetBTS(), *this);

    bool bIsLoS = cm::LinkMatrix::Instance().IsLOS(m_MainServBTS.GetBTS(), *this);
    m_dLOSEODDeg = (cm::LinkMatrix::Instance().GetLoSEoDRAD(m_MainServBTS.GetBTS(), *this)) / M_PI * 180;
    m_dLOSEOADeg = (cm::LinkMatrix::Instance().GetLoSEoARAD(m_MainServBTS.GetBTS(), *this)) / M_PI * 180;
    m_dLOSAODDeg = (cm::LinkMatrix::Instance().GetLoSAoDRAD(m_MainServBTS.GetBTS(), *this)) / M_PI * 180;
    m_dLOSAOADeg = (cm::LinkMatrix::Instance().GetLoSAoARAD(m_MainServBTS.GetBTS(), *this)) / M_PI * 180;
    double dServeLoSLinkLossDB = cm::LinkMatrix::Instance().GetLoSLinkLossDB(m_MainServBTS.GetBTS(), *this);
    int iBTSBeamIndex = cm::LinkMatrix::Instance().GetStrongestBSBeamIndex(m_MainServBTS.GetBTS(), *this);
    int iUEBeamIndex = cm::LinkMatrix::Instance().GetStrongestUEBeamIndex(m_MainServBTS.GetBTS(), *this);
    int iPanelIndex = cm::LinkMatrix::Instance().GetStrongestUEPanelIndex(m_MainServBTS.GetBTS(), *this);


    double RSRP_36873, CouplingLoss_36873, SNR_36873;
    double SINR_36873_Radom;
    double Interfpower_MeanPerRB_36873;

    //@thread
    initialize_lock.lock();
    CalcSINR_for_Calibration_new(
            RSRP_36873, CouplingLoss_36873, SNR_36873,
            SINR_36873_Radom, Interfpower_MeanPerRB_36873
            );


    double BS_V_beamDEG = cm::RAD2DEG(BSBeamIndex2EtiltRAD(iBTSBeamIndex));
    double BS_H_beamDEG = cm::RAD2DEG(BSBeamIndex2EscanRAD(iBTSBeamIndex));

    double UE_V_beamDEG = cm::RAD2DEG(UEBeamIndex2EtiltRAD(iUEBeamIndex));
    double UE_H_beamDEG = cm::RAD2DEG(UEBeamIndex2EscanRAD(iUEBeamIndex));


    // output
    //    string t = "abd";
    //    Parameters::Instance().RecordParameters_New(t);
    Observer::Print("ITUCalibration") << m_MainServBTS
            << setw(20) << (m_MainServBTS.GetIndex() + 3 * m_MainServBTS.ToInt())
            << setw(20) << IsServBTS
            << setw(20) << m_ID.ToInt()
            << setw(20) << (this->GetX())
            << setw(20) << (this->GetY())
            << setw(20) << (rx_wrap.GetX())
            << setw(20) << (rx_wrap.GetY())
            << setw(20) << cm::RAD2DEG(m_ID.GetMS().GetRxOrientRAD())
            << setw(20) << (m_MainServBTS.GetBTS().GetX())
            << setw(20) << (m_MainServBTS.GetBTS().GetY())
            << setw(20) << (tx_wrap.GetX())
            << setw(20) << (tx_wrap.GetY())
            << setw(20) << d2DDistanceM
            << setw(20) << d3DDistanceM
            << setw(20) << m_iTotalFloorNum
            << setw(20) << m_iFloorNum
            << setw(20) << m_dRxHeight
            << setw(20) << bIsStrong
            << setw(20) << bIsLoS
            << setw(20) << m_iSpecial
            << setw(20) << m_iIsLowloss
            << setw(20) << m_dLOSAODDeg
            << setw(20) << m_dLOSEODDeg
            << setw(20) << m_dLOSAOADeg
            << setw(20) << m_dLOSEOADeg
            << setw(20) << dServeLinkLossDB
            << setw(20) << dServeLoSLinkLossDB
            << setw(20) << dServePathLossDB
            << setw(20) << dServeShadowFadeDB
            << setw(20) << iBTSBeamIndex
            << setw(20) << iUEBeamIndex
            << setw(20) << iPanelIndex

            << setw(20) << m_dGeometryDB_SNR
            << setw(20) << m_dGeometryDB_SINR
            << setw(20) << m_dGeometrydb100MHz //13

            // caoyuhua
            << setw(20) << RSRP_36873
            //<< setw(20) << RSRP_freqH

            << setw(20) << CouplingLoss_36873
            //<< setw(20) << CouplingLoss_freqH

            << setw(20) << SNR_36873
            //<< setw(20) << SNR_freqH

            //<< setw(20) << SINR_36873_StrongInter            
            << setw(20) << SINR_36873_Radom
            //<< setw(20) << SINR_freqH_Radom

            << setw(20) << BS_V_beamDEG
            << setw(20) << BS_H_beamDEG
            << setw(20) << UE_V_beamDEG
            << setw(20) << UE_H_beamDEG

            << setw(20) << Interfpower_MeanPerRB_36873
            //<< setw(20) << Interfpower_MeanPerRB_freqH

            << setw(20) << this->m_pAntenna->GetTxRxOrientRAD()

            << endl;

	//RedCap :
	{

		if (m_ID.ToInt() < floor(Parameters::Instance().BASIC.ITotalMSNum * Parameters::Instance().BASIC.type1MSRate + 0.5)) {
            //hyl 冗余
//			Observer::Print("ITUCalibration_eMBB") << m_MainServBTS
//				<< setw(20) << (m_MainServBTS.GetIndex() + 3 * m_MainServBTS.ToInt())
//				<< setw(20) << IsServBTS
//				<< setw(20) << m_ID.ToInt()
//				<< setw(20) << (this->GetX())
//				<< setw(20) << (this->GetY())
//				<< setw(20) << (rx_wrap.GetX())
//				<< setw(20) << (rx_wrap.GetY())
//				<< setw(20) << cm::RAD2DEG(m_ID.GetMS().GetRxOrientRAD())
//				<< setw(20) << (m_MainServBTS.GetBTS().GetX())
//				<< setw(20) << (m_MainServBTS.GetBTS().GetY())
//				<< setw(20) << (tx_wrap.GetX())
//				<< setw(20) << (tx_wrap.GetY())
//				<< setw(20) << d2DDistanceM
//				<< setw(20) << d3DDistanceM
//				<< setw(20) << m_iTotalFloorNum
//				<< setw(20) << m_iFloorNum
//				<< setw(20) << m_dRxHeight
//				<< setw(20) << bIsStrong
//				<< setw(20) << bIsLoS
//				<< setw(20) << m_iSpecial
//				<< setw(20) << m_iIsLowloss
//				<< setw(20) << m_dLOSAODDeg
//				<< setw(20) << m_dLOSEODDeg
//				<< setw(20) << m_dLOSAOADeg
//				<< setw(20) << m_dLOSEOADeg
//				<< setw(20) << dServeLinkLossDB
//				<< setw(20) << dServeLoSLinkLossDB
//				<< setw(20) << dServePathLossDB
//				<< setw(20) << dServeShadowFadeDB
//				<< setw(20) << iBTSBeamIndex
//				<< setw(20) << iUEBeamIndex
//				<< setw(20) << iPanelIndex
//
//				<< setw(20) << m_dGeometryDB_SNR
//				<< setw(20) << m_dGeometryDB_SINR
//				<< setw(20) << m_dGeometrydb100MHz //13
//
//				// caoyuhua
//				<< setw(20) << RSRP_36873
//				//<< setw(20) << RSRP_freqH
//
//				<< setw(20) << CouplingLoss_36873
//				//<< setw(20) << CouplingLoss_freqH
//
//
//				//下行SINR？
//				<< setw(20) << SNR_36873
//				//<< setw(20) << SNR_freqH
//
//				//<< setw(20) << SINR_36873_StrongInter
//				<< setw(20) << SINR_36873_Radom
//				//<< setw(20) << SINR_freqH_Radom
//
//				<< setw(20) << BS_V_beamDEG
//				<< setw(20) << BS_H_beamDEG
//				<< setw(20) << UE_V_beamDEG
//				<< setw(20) << UE_H_beamDEG
//
//				<< setw(20) << Interfpower_MeanPerRB_36873
//				//<< setw(20) << Interfpower_MeanPerRB_freqH
//
//				<< setw(20) << this->m_pAntenna->GetTxRxOrientRAD()
//
//				<< endl;
		}
		else {
            //hyl 冗余
//			Observer::Print("ITUCalibration_RedCap") << m_MainServBTS
//				<< setw(20) << (m_MainServBTS.GetIndex() + 3 * m_MainServBTS.ToInt())
//				<< setw(20) << IsServBTS
//				<< setw(20) << m_ID.ToInt()
//				<< setw(20) << (this->GetX())
//				<< setw(20) << (this->GetY())
//				<< setw(20) << (rx_wrap.GetX())
//				<< setw(20) << (rx_wrap.GetY())
//				<< setw(20) << cm::RAD2DEG(m_ID.GetMS().GetRxOrientRAD())
//				<< setw(20) << (m_MainServBTS.GetBTS().GetX())
//				<< setw(20) << (m_MainServBTS.GetBTS().GetY())
//				<< setw(20) << (tx_wrap.GetX())
//				<< setw(20) << (tx_wrap.GetY())
//				<< setw(20) << d2DDistanceM
//				<< setw(20) << d3DDistanceM
//				<< setw(20) << m_iTotalFloorNum
//				<< setw(20) << m_iFloorNum
//				<< setw(20) << m_dRxHeight
//				<< setw(20) << bIsStrong
//				<< setw(20) << bIsLoS
//				<< setw(20) << m_iSpecial
//				<< setw(20) << m_iIsLowloss
//				<< setw(20) << m_dLOSAODDeg
//				<< setw(20) << m_dLOSEODDeg
//				<< setw(20) << m_dLOSAOADeg
//				<< setw(20) << m_dLOSEOADeg
//				<< setw(20) << dServeLinkLossDB
//				<< setw(20) << dServeLoSLinkLossDB
//				<< setw(20) << dServePathLossDB
//				<< setw(20) << dServeShadowFadeDB
//				<< setw(20) << iBTSBeamIndex
//				<< setw(20) << iUEBeamIndex
//				<< setw(20) << iPanelIndex
//
//				<< setw(20) << m_dGeometryDB_SNR
//				<< setw(20) << m_dGeometryDB_SINR
//				<< setw(20) << m_dGeometrydb100MHz //13
//
//				// caoyuhua
//				<< setw(20) << RSRP_36873
//				//<< setw(20) << RSRP_freqH
//
//				<< setw(20) << CouplingLoss_36873
//				//<< setw(20) << CouplingLoss_freqH
//
//
//				//下行SINR？
//				<< setw(20) << SNR_36873
//				//<< setw(20) << SNR_freqH
//
//				//<< setw(20) << SINR_36873_StrongInter
//				<< setw(20) << SINR_36873_Radom
//				//<< setw(20) << SINR_freqH_Radom
//
//				<< setw(20) << BS_V_beamDEG
//				<< setw(20) << BS_H_beamDEG
//				<< setw(20) << UE_V_beamDEG
//				<< setw(20) << UE_H_beamDEG
//
//				<< setw(20) << Interfpower_MeanPerRB_36873
//				//<< setw(20) << Interfpower_MeanPerRB_freqH
//
//				<< setw(20) << this->m_pAntenna->GetTxRxOrientRAD()
//
//				<< endl;
		}

	}

    ///3D-MIMO校准
    Statistician::Instance().RSRP.push_back(RSRP_36873);
//    cout<<"SINR_36873_Radom="<<SINR_36873_Radom<<endl;
    initialize_lock.unlock();
    //@thread
}

void MS::BasicOutput_Statistican() {


    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].m_x = this->GetX();
    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].m_y = this->GetY();
    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].m_z = this->GetRxHeightM();
    Statistician::Instance().m_BTSData[m_MainServBTS.GetTxID()].m_iNumServMS += 1;

    {
        //hyl 冗余
//        Observer::Print("SmallScale") << m_dGeometryDB_SNR
//                << setw(20) << m_dGeometryDB_SINR << endl;
    }


    //    {
    //        for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) { ///@xlong 注意此处修改了BTSID的++重载函数
    //            //double dLinkLossDB = cm::LinkMatrix::Instance().GetLinkLossDB(btsid.GetBTS(), *this);
    //            double dLinkLossDB = cm::LinkMatrix::Instance().GetCouplingLossDB(btsid.GetBTS(), *this);
    //            Observer::Print("LinkLoSS_Test") << dLinkLossDB << setw(20);
    //        }
    //        double dServeLinkLossDB = cm::LinkMatrix::Instance().GetCouplingLossDB(m_MainServBTS.GetBTS(), *this);
    //        Observer::Print("LinkLoSS_Test") << dServeLinkLossDB << endl;
    //    }

    //    {
    //        for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) { ///@xlong 注意此处修改了BTSID的++重载函数
    //            //double dLinkLossDB = cm::LinkMatrix::Instance().GetLinkLossDB(btsid.GetBTS(), *this);
    //            double dLoSLinkLossDB = cm::LinkMatrix::Instance().GetLoSLinkLossDB(btsid.GetBTS(), *this);
    //            Observer::Print("LoSLinkLoSS_Test") << dLoSLinkLossDB << setw(20);
    //        }
    //        double dServeLoSLinkLossDB = cm::LinkMatrix::Instance().GetLoSLinkLossDB(m_MainServBTS.GetBTS(), *this);
    //        Observer::Print("LoSLinkLoSS_Test") << dServeLoSLinkLossDB << endl;
    //    }

    //记录统计信息
    double dServePathLossDB = cm::LinkMatrix::Instance().GetPathLossDB(m_MainServBTS.GetBTS(), *this);
    double dServeLinkLossDB = cm::LinkMatrix::Instance().GetCouplingLossDB(m_MainServBTS.GetBTS(), *this);
    double dServePurePathLossDB = cm::LinkMatrix::Instance().GetPurePathLossDB(m_MainServBTS.GetBTS(), *this);
    double dServeShadowFadeDB = cm::LinkMatrix::Instance().GetShadowFadeDB(m_MainServBTS.GetBTS(), *this);
    double dServeDistanceM = cm::LinkMatrix::Instance().Get2DDistanceM(m_MainServBTS.GetBTS(), *this);
    double dServeBroadcastAntGainDB = -1;
    double dESD = cm::LinkMatrix::Instance().GetESD(m_MainServBTS.GetBTS(), *this);
    double dESA = cm::LinkMatrix::Instance().GetESA(m_MainServBTS.GetBTS(), *this);
    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].m_dESD = dESD;
    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].m_dESA = dESA;
    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].m_ActiveBTS = m_MainServBTS;
    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].m_x = this->GetX();
    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].m_y = this->GetY();
    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].m_dLinkLossDB = dServeLinkLossDB;
    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].m_dAveAntGainDB = dServeBroadcastAntGainDB;
    Statistician::Instance().m_MSData_DL[m_ID.ToInt()].DL.m_dGeometryDB = m_dGeometryDB_SNR;
    std::lock_guard<std::mutex> l(MacroMS_lock);
    //hyl 冗余
//    if (BSManager::IsMacro(m_MainServBTS)) {
//        Observer::Print("MacroMS") << m_ID.ToInt() << setw(20) << m_MainServBTS << setw(20) << m_dGeometryDB_SNR << setw(20) << dServePathLossDB << setw(20) << dServeLinkLossDB << setw(20) << dServePurePathLossDB << setw(20) << dServeShadowFadeDB << setw(20) << dServeBroadcastAntGainDB << setw(20) << dServeDistanceM << endl;
//        Observer::Print("TotalMS") << m_ID.ToInt() << setw(20) << m_MainServBTS << setw(20) << m_dGeometryDB_SNR << setw(20) << dServePathLossDB << setw(20) << dServeLinkLossDB << setw(20) << dServePurePathLossDB << setw(20) << dServeShadowFadeDB << setw(20) << dServeBroadcastAntGainDB << setw(20) << dServeDistanceM << endl;
//    } else {
//        Observer::Print("PicoMS") << m_ID.ToInt() << setw(20) << m_MainServBTS << setw(20) << m_dGeometryDB_SNR << setw(20) << dServePathLossDB << setw(20) << dServeLinkLossDB << setw(20) << dServePurePathLossDB << setw(20) << dServeShadowFadeDB << setw(20) << dServeBroadcastAntGainDB << setw(20) << dServeDistanceM << endl;
//        Observer::Print("TotalMS") << m_ID.ToInt() << setw(20) << m_MainServBTS << setw(20) << m_dGeometryDB_SNR << setw(20) << dServePathLossDB << setw(20) << dServeLinkLossDB << setw(20) << dServePurePathLossDB << setw(20) << dServeShadowFadeDB << setw(20) << dServeBroadcastAntGainDB << setw(20) << dServeDistanceM << endl;
//    }
}

//MS_UL_begin

void MS_UL::WorkSlotUL() {
    //    m_HARQTxState_MS_UL.WorkSlot();
    m_pMSTxer->WorkSlot();
}

void MS_UL::ReceiveSchedulingMessage(std::shared_ptr<SchedulingMessageUL> _pSchedulingMes) {
    m_pMSTxer->PushSchedulingMessage(_pSchedulingMes);
}

cmat MS_UL::GetTxDataSymbol(SCID _scid) {
    return m_pMSTxer->GetTxDataSymbol(_scid);
}
cmat MS_UL::GetTxDataSymbol(SCID _scid,int iPortNum) {
    return m_pMSTxer->GetTxDataSymbol(_scid,iPortNum);
}

cmat MS_UL::GetTxSRS(SCID _scid) {
    return m_pMSTxer->GetTxSRS(_scid);
}
cmat MS_UL::GetTxSRS(SCID _scid,int _iPortNum) {
    return m_pMSTxer-> GetTxSRS(_scid,_iPortNum);
}

cmat MS_UL::GetLastTxP() {
    return m_pMSTxer->GetLastTxP();
}

void MS_UL::ConfigTxSRS(const int SRSTxCyclicSCID, const int iSRSTxSpace) {
    m_pMSTxer->ConfigTxSRS(SRSTxCyclicSCID, iSRSTxSpace);
}

void MS_UL::ReceiveACK(std::shared_ptr<ACKNAKMessageUL> _pACKNAKMessageUL) {
    m_pMSTxer->ReceiveACK(_pACKNAKMessageUL);
}

int MS_UL::GetSRSTxCyclicSCID() {
    return m_pMSTxer->GetSRSTxCyclicSCID();
}

double MS_UL::GetGeometryDB() {
    return m_dGeometryDB;
}

int MS_UL::GetRBNumSupport() {
    return m_pMSTxer->GetRBNumSupport();
}

void MS_UL::ConfigSRSBWIndex(int _iSRSIndex) {
    m_pMSTxer->ConfigSRSBWIndex(_iSRSIndex);
}

int MS_UL::GetSRSBWIndex() {
    return m_pMSTxer->GetSRSBWIndex();
}

void MS_UL::SetSRSBWspan(int _ifirst, int _isecond) {
    m_pMSTxer->SetSRSBWspan(_ifirst, _isecond);
}

pair<int, int> MS_UL::GetSRSBWspan() {
    return m_pMSTxer->GetSRSBWspan();
}

void MS_UL::Initialize() {

    MSTxBuffer.m_MSID = msId;
    //Statistician::Instance().m_MSData_UL[msId].m_x = msId.GetMS().GetX();//下面已经统计过了
    //Statistician::Instance().m_MSData_UL[msId].m_y = msId.GetMS().GetY();//下面已经统计过了
    //Statistician::Instance().m_BTSData[msId.GetMS().m_MainServBTS].m_iNumServMS += 1; //交给下行统计即可，上行不需要再次统计

    //统计LOS概率和Shadowading
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_iIsLOS = cm::LinkMatrix::Instance().IsLOS(msId.GetMS().m_MainServBTS.GetBTS(), msId.GetMS());
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_dShadowfading = cm::LinkMatrix::Instance().GetShadowFadeDB(msId.GetMS().m_MainServBTS.GetBTS(), msId.GetMS());


    double dInterfLinkLossSum = 0;
    for (BTSID btsid = BTSID::Begin(); btsid != BTSID::End(); ++btsid) {
        if (btsid == msId.GetMS().m_MainServBTS)
            continue;
        dInterfLinkLossSum += DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(btsid.GetBTS(), msId.GetMS()));
    }

    double dServeLinkLoss = DB2L(cm::LinkMatrix::Instance().GetCouplingLossDB(msId.GetMS().m_MainServBTS.GetBTS(), msId.GetMS()));
    m_dGeometryDB = L2DB(Parameters::Instance().Macro.DL.DMaxTxPowerMw * dServeLinkLoss / (Parameters::Instance().Macro.DL.DMaxTxPowerMw * dInterfLinkLossSum + Parameters::Instance().MSS_UL.DL.DNoisePowerMw));

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    //初始化上行的发射机
    switch (Parameters::Instance().SIM_UL.UL.IMIMOMode) {
        case Parameters::IMIMOMode_MRC:
        case Parameters::IMIMOMode_VirtualMIMO:
            m_pMSTxer = std::shared_ptr<MSTxer > (new MSMRCTxer(msId.GetMS().m_MainServBTS, msId, std::make_shared<HARQTxStateMS>(m_HARQTxState_MS_UL)));
            break;
        default:
            assert(false);
            break;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///记录统计信息
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_ActiveBTS = msId.GetMS().m_MainServBTS;
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_x = msId.GetMS().GetX();
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_y = msId.GetMS().GetY();
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_z = msId.GetMS().GetRxHeightM();
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_dLinkLossDB = L2DB(dServeLinkLoss);
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_dGeometryDB = m_dGeometryDB;
    // szx  get AODDDEG
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_AODLOSDEG = 0.0;
    //cm::RAD2DEG(cm::LinkMatrix::Instance().GetAngleDeg(m_MainServBTS.GetBTS(), msId.GetMS()));
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_dUE2BSTiltDEG = 0.0; //szx
    //cm::LinkMatrix::Instance().GetUE2BSTiltDEG(m_MainServBTS.GetBTS(), msId.GetMS());
    ///szx get m_dPlusOffset
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_dPlusOffsetDEG = 0.0;
    //cm::RAD2DEG(cm::LinkMatrix::Instance().GetPlusOffsetRAD(m_MainServBTS.GetBTS(), msId.GetMS()));
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_dAveTxAntennaPatternDB = 0.0;
    //cm::LinkMatrix::Instance().GetAntGainDB(m_MainServBTS.GetBTS(), msId.GetMS());
    //szx
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_dRxHeight = msId.GetMS().GetRxHeightM();
    //m_iIsO2I
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_iIsO2I = msId.GetMS().GetSpecial();

    double dServePathLossDB = cm::LinkMatrix::Instance().GetPathLossDB(msId.GetMS().m_MainServBTS.GetBTS(), msId.GetMS());
    double dServeLinkLossDB = cm::LinkMatrix::Instance().GetCouplingLossDB(msId.GetMS().m_MainServBTS.GetBTS(), msId.GetMS());
    double dServePurePathLossDB = cm::LinkMatrix::Instance().GetPurePathLossDB(msId.GetMS().m_MainServBTS.GetBTS(), msId.GetMS());
    double dServeShadowFadeDB = cm::LinkMatrix::Instance().GetShadowFadeDB(msId.GetMS().m_MainServBTS.GetBTS(), msId.GetMS());
    double dServeDistanceM = cm::LinkMatrix::Instance().Get2DDistanceM(msId.GetMS().m_MainServBTS.GetBTS(), msId.GetMS());
    double dServeBroadcastAntGainDB = 0.0; //cm::LinkMatrix::Instance().GetBroadcastAntGainDB(m_MainServBTS.GetBTS(), msId.GetMS());
    //    double dESD = cm::LinkMatrix::Instance().GetESD(m_MainServBTS.GetBTS(), msId.GetMS());
    //    double dESA = cm::LinkMatrix::Instance().GetESA(m_MainServBTS.GetBTS(), msId.GetMS());
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_d2D_Distance = dServeDistanceM;


    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_dAvePathlossDB =
            cm::LinkMatrix::Instance().GetCouplingLossDB(msId.GetMS().m_MainServBTS.GetBTS(), msId.GetMS());
    Statistician::Instance().m_MSData_UL[msId.ToInt()].m_dPathlossDB =
            cm::LinkMatrix::Instance().GetPathLossDB(msId.GetMS().m_MainServBTS.GetBTS(), msId.GetMS());
}

void MS_UL::Reset() {
    m_vSINR.clear();
    //    m_HARQTxState.Clear();
    m_HARQTxState_BTS_DL.ClearReTxMessage();
}

void MS_UL::Construct(MSID& msid) {
    msId = msid;
    m_vSINR.resize(Parameters::Instance().BASIC.ISCNum, zeros(1, 1));
}
//MS_UL_end


void MS::RefreshHARQSINR(int harqID, int iRank){
    m_pMSRxer->RefreshHARQSINR(harqID, iRank);
}
void MS::runThreadTask()
{
    tc=&Thread_control::Instance();

    WorkSlot();

    return;
}