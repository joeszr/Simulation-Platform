///@file parameters.cpp
///@brief   实现parameters.h中定义的多个函数
///@author dushaofeng
#include "../Statistician/Directory.h"
#include "Parameters.h"
#include <vector>

// 构造函数，在本函数中读取输入参数，并且把参数再输出到文件中

Parameters::Parameters() {
    Build_DL();
    ReadOverWriteParameters_DL();

    Build_UL();
    ReadOverWriteParameters_UL();

    //20260115新增：读取RIS参数
    Build_Scene();
    ReadSceneData();
    cout << "RIS Parameters initialized completed" << endl;

    ConfigureArgs();
    MapArgsToParameters();

    //设置 场景和频率 确定的参数
    SetEnvironmentTypeParameters();
    //计算导出参数
    CalculateDerivedParameters_DL();
    CalculateDerivedParameters_UL();

    //处理上行_begin
//    Build_UL(); //建立参数名字符串到参数表的映射,用m_Str2ParaMap_UL来记录每个参数名到参数地址的映射
//    ReadInputData_UL(); //读取输入参数，即使得创建的对象的参数被赋予输入文件的值
//    cout << "UL Parameters initialized completed" << endl;
//    //处理上行_end
//
//    //处理下行_begin
//    Build_DL(); //建立参数名字符串到参数表的映射,用m_Str2ParaMap_DL来记录每个参数名到参数地址的映射
//    ReadInputData_DL(); //读取输入参数，即使得创建的对象的参数被赋予输入文件的值
    if (BASIC.IDLORUL == Parameters::UL||BASIC.IDLORUL == Parameters::ULandDL)
    {
        MSS_UL.DL.DNoisePowerMw = DB2L(LINK_UL.DNoisePowerSpectrumDensityDbmHz + MSS_UL.DL.DNoiseFigureDb) * BASIC.DCarrierWidthHz * BASIC.ISCNum;
        BTS_UL.UL.DNoisePowerMw = DB2L(LINK_UL.DNoisePowerSpectrumDensityDbmHz + BTS_UL.UL.DNoiseFigureDb) * BASIC.DCarrierWidthHz * BASIC.ISCNum;

        MSS_UL.DL.DSCNoisePowerMw = MSS_UL.DL.DNoisePowerMw / BASIC.ISCNum;
        BTS_UL.UL.DSCNoisePowerMw = BTS_UL.UL.DNoisePowerMw / BASIC.ISCNum + DB2L(BASIC.Noise);
    }
    string filename_New = "ParametersRecord_New"; //使用该文件来记录所有读取到的参数
    RecordParameters_New(filename_New); //把参数输出到文件中
    
    // 输出OverWriteDL、OverWriteUL和Scene.txt中的参数
    string inputFilesParamsFilename = "InputFilesParametersRecord.txt";
    OutputInputFilesParameters(inputFilesParamsFilename.c_str());
    
    cout << "DL Parameters initialized completed" << endl;
    //处理下行_end
}

//调用一系列函数从文件中读取输入参数

void Parameters::ReadInputData_DL() {

    cout << "Read DL input data..." << endl;
    //读取参数
    ReadOverWriteParameters_DL();

    //设置 场景和频率 确定的参数
    SetEnvironmentTypeParameters();
    //计算导出参数
    CalculateDerivedParameters_DL();
    cout << "ok!" << endl;
}
//读取系统的基本参数
//没用_begin

void Parameters::ReadBasicData() {
    ReadData_DL(const_cast<char*> ("./inputfiles/Simulate_Parameters.txt"));
    //    ReorderData(const_cast<char*> ("./inputfiles/Simulate_Parameters.txt"));
}

//读取基站的参数

void Parameters::ReadBaseStationData() {
    ReadData_DL(const_cast<char*> ("./inputfiles/BTS_Parameters.txt"));
    //    ReorderData(const_cast<char*> ("./inputfiles/BTS_Parameters.txt"));
}

//读取移动台的参数

void Parameters::ReadMobileStationData() {
    ReadData_DL(const_cast<char*> ("./inputfiles/MSS_Parameters.txt"));
    //    ReorderData(const_cast<char*> ("./inputfiles/MSS_Parameters.txt"));
}

//读取和链路有关的参数

void Parameters::ReadLinkData() {
    ReadData_DL(const_cast<char*> ("./inputfiles/Link_Parameters.txt"));
    //    ReorderData(const_cast<char*> ("./inputfiles/Link_Parameters.txt"));
}

//读取和无线资源有关的参数

void Parameters::ReadSimData() {
    ReadData_DL(const_cast<char*> ("./inputfiles/SIM_Parameters.txt"));
    //    ReorderData(const_cast<char*> ("./inputfiles/SIM_Parameters.txt"));
}
//没用_end

//读取重写参数

void Parameters::ReadOverWriteParameters_DL() {
    ReadData_DL(const_cast<char*> ("./inputfiles/OverWriteDL.txt"));
    //    ReorderData(const_cast<char*> ("./inputfiles/OverWrite.txt"));
}

void Parameters::SetEnvironmentTypeParameters() {
    ///工作模式
    //    static const int WorkingMode_Normal = 1;
    //    static const int WorkingMode_ITU_Calibration_Phase1 = 2;

    BASIC.DWorkingMode = WorkingMode_Normal;

    // 室外用户速度扩展因子
    MSS.DCarSpeedSacleFactor = -1;
    MSS.DProbLowloss = -1;

    Macro.DOTA_dB = -1.0 * 4.0;
    Macro.DMSBodyLoss_dB = -1.0 * 3.0;
    if (Macro.DL.IschedulerthreadNum <= 0) {
        Macro.DL.IschedulerthreadNum = 1 * ceil((double) sysconf(_SC_NPROCESSORS_CONF) /
                                                (BASIC.INumBSs *
                                                 BASIC.IBTSPerBS));
    }
    switch (BASIC.IScenarioModel) {
        case SCENARIO_HighFreq_INDOOR_FACTORY:
            BASIC.IChannelModel_for_Scenario = cm::P::InF;
            Set_HighFreq_InF();
            break;
        case SCENARIO_LowFreq_INDOOR:
            BASIC.IChannelModel_for_Scenario = cm::P::InH;
            Set_LowFreq_InH();
            break;
        case SCENARIO_LowFreq_URBAN_MICRO:
            BASIC.IChannelModel_for_Scenario = cm::P::UMI;
            Set_LowFreq_UMI();
            break;
        case SCENARIO_LowFreq_URBAN_MACRO:
            BASIC.IChannelModel_for_Scenario = cm::P::UMA;
            Set_LowFreq_UMA();
            break;
        case SCENARIO_LowFreq_RURAL_MACRO:
            BASIC.IChannelModel_for_Scenario = cm::P::RMA;
            Set_LowFreq_RMA();
            break;
        case SCENARIO_HighFreq_INDOOR:
            BASIC.IChannelModel_for_Scenario = cm::P::InH;
            Set_HighFreq_InH();
            break;
        case SCENARIO_HighFreq_URBAN_MICRO:
            BASIC.IChannelModel_for_Scenario = cm::P::UMI;
            Set_HighFreq_UMI();
            break;
        case SCENARIO_HighFreq_URBAN_MACRO:
            BASIC.IChannelModel_for_Scenario = cm::P::UMA;
            Set_HighFreq_UMA();
            break;
        case SCENARIO_HighFreq_RURAL_MACRO:
            BASIC.IChannelModel_for_Scenario = cm::P::RMA;
            Set_HighFreq_RMA();
            break;
        case SCENARIO_5G_DENSE_URBAN_1LAYER:
            Set_5G_1Layer();
            break;
        case SCENARIO_5G_DENSE_URBAN_2LAYER:
            BASIC.IChannelModel_for_Scenario = cm::P::_5GCM_TWO_LAYER;
            Set_5G_2Layer();
            break;
        default:
            assert(false);
            break;

    }
}

void Parameters::Set_LowFreq_InH() {

    switch (BASIC.ISubScenarioModel) {
        case Parameters::SCENARIO_Phase1_InH_LF:
        {
            BASIC.IBTSPerBS = 1; // 3 for 36 TRP ,1for 12 TRP
            BASIC.INumerologyIndicator = 1;
            BASIC.INumBSs = 12;
            BASIC.IMacroTopology = 3;
            BASIC.IMsDistributeMode = 0;
            BASIC.ISCSConfig = 1;

            MIMO_CTRL.IMUMIMONum = 12;
            MIMO_CTRL.IMaxRankNum = 2;
            MIMO_CTRL.IMaxLayerNum_BS = 32;

        }
        {
            
            Macro.DL.DMaxTxPowerDbm = 24; //23 for 30GNRMIMO;20 for ITU
            Macro.DAntennaHeightM = 3;
            Macro.DSiteDistanceM = 20;
            Macro.DMechanicalTiltDeg = 90; //20 for 36TRP ,90 for 12TRP 
            Macro.DElectricalTiltDeg = 0;
            
            Macro.DVAntSpace = 0.5;
            Macro.DHAntSpace = 0.5;
            Macro.IAntennaPatternMode = 1;
            Macro.DV3DBBeamWidthDeg = 90;
            Macro.DH3DBBeamWidthDeg = 90;
            Macro.DAntennaGainDb = 5;
            
            Macro.UL.DNoiseFigureDb = 5;

            Macro.IVAntNumPerPanel = 4; //4;
            Macro.IHAntNumPerPanel = 4; //8;
            Macro.IVPanelNum = 1; //1;
            Macro.IHPanelNum = 1; //1;
            Macro.Polarize_Num = 2;
            Macro.V_TXRU_DIV_NUM_PerPanel = 4;
            Macro.H_TXRU_DIV_NUM_PerPanel = 4;

            Macro.DVBackLossDB = -25;
            Macro.DHBackLossDB = -25;
            Macro.LINK.DMinDistanceM = 0; //原1.5;
            Macro.LINK.DRadioFrequencyMHz = 4000;
            Macro.DL.bEnable_BSAnalogBF = false;

        }
        {
            MSS.DProbOutdoor = 0;
            // 1: Outdoor is In Car; 0 : Outdoor is not In Car
            MSS.DAntennaHeightM = 1.5;
            MSS.FirstBand.bEnable_UEAnalogBF = false;

            MSS.FirstBand.IAntennaPatternMode = 0;
            MSS.FirstBand.DV3DBBeamWidthDeg = 90; //ADD
            MSS.FirstBand.DH3DBBeamWidthDeg = 90; //ADD
            MSS.FirstBand.DAntennaGainDb = 0;
            MSS.FirstBand.DVAntSpace = 0.5;
            MSS.FirstBand.DHAntSpace = 0.5;
            
            MSS.FirstBand.UL.DMaxTxPowerDbm = 23;
            MSS.FirstBand.DL.DNoiseFigureDb = 7;

            MSS.FirstBand.IVAntNumPerPanel = 1; //2;
            MSS.FirstBand.IHAntNumPerPanel = 2; //4;
            MSS.FirstBand.IVPanelNum = 1;
            MSS.FirstBand.IHPanelNum = 1;
            MSS.FirstBand.Polarize_Num = 2;
            MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel = 1;
            MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel = 2;

            MSS.FirstBand.DHBackLossDB = -25;
            MSS.FirstBand.DVBackLossDB = -25;

        }
            break;

        default:
            assert(false);
            break;
    }
}

void Parameters::Set_LowFreq_UMI() {

}

void Parameters::Set_LowFreq_UMA() {
    {
        //BASIC.IBTSPerBS = 3; //
        BASIC.INumerologyIndicator = 1; //
        //BASIC.INumBSs = 7; //7对应7小区，19对应19小区 ,小区数目
        BASIC.IMacroTopology = 0; // 0对应7小区，1对应19小区 ，wrap方式
        BASIC.IMsDistributeMode = 0; //

    }
    switch (BASIC.ISubScenarioModel) {

        case SCENARIO_NR_UMA_700M_350m_25m_MeTilt9:
        {
            MIMO_CTRL.IMUMIMONum = 2;
            MIMO_CTRL.IMaxRankNum = 2;
            MIMO_CTRL.IMaxLayerNum_BS = 4;
            {
                BASIC.IBTSPerBS = 3;
                BASIC.INumerologyIndicator = 2;
                BASIC.INumBSs = 7; //7对应7小区，19对应19小区 ,小区数目
                BASIC.IMacroTopology = 0; // 0对应7小区，1对应19小区 ，wrap方式
                BASIC.IMsDistributeMode = 0;
                BASIC.ISCSConfig = 1;
            }
            {
                Macro.DPenetrationLossdB = 10;
                Macro.DTransmissionLineLoss_dB = -1.0 * 0;

                Macro.LINK.DRadioFrequencyMHz = 700;
                
                Macro.IAntennaPatternMode = 1;
                Macro.DV3DBBeamWidthDeg = 14;
                Macro.DH3DBBeamWidthDeg = 65;
                Macro.DAntennaGainDb = 14.5;
                Macro.DVBackLossDB = -25;
                Macro.DHBackLossDB = -25;
                Macro.DVAntSpace = 0.9;
                Macro.DHAntSpace = 0.5;
                
                Macro.DL.DMaxTxPowerDbm = 53;
                Macro.DAntennaHeightM = 25;
                Macro.DSiteDistanceM = 350;
                Macro.DMechanicalTiltDeg = 9.0;
                Macro.DElectricalTiltDeg = 0;
                Macro.UL.DNoiseFigureDb = 4.0;

                Macro.IVAntNumPerPanel = 1;
                Macro.IHAntNumPerPanel = 2;
                Macro.IVPanelNum = 1;
                Macro.IHPanelNum = 1;
                Macro.Polarize_Num = 2;
                Macro.V_TXRU_DIV_NUM_PerPanel = 1;
                Macro.H_TXRU_DIV_NUM_PerPanel = 2;

                Macro.LINK.DMinDistanceM = 10;
                Macro.DL.bEnable_BSAnalogBF = false;
            }
            {
                MSS.FirstBand.bEnable_UEAnalogBF = false;

                MSS.IMinBuildingFloor = 4;
                MSS.IMaxBuildingFloor = 8;

                MSS.DProbOutdoor = 0.2;
                // 1: Outdoor is In Car; 0 : Outdoor is not In Car
                MSS.bIsOutdoorInCar_notOnCar = true;
                // 室外用户速度扩展因子
                MSS.DCarSpeedSacleFactor = 10.0;

                MSS.DProbLowloss = 0.8;

                MSS.FirstBand.IAntennaPatternMode = 0;
                MSS.FirstBand.DV3DBBeamWidthDeg = 90;
                MSS.FirstBand.DH3DBBeamWidthDeg = 90;
                MSS.FirstBand.DAntennaGainDb = 0;
                MSS.DAntennaHeightM = 1.5;
                MSS.FirstBand.DVAntSpace = 0.5;
                MSS.FirstBand.DHAntSpace = 0.5;
                MSS.FirstBand.UL.DMaxTxPowerDbm = 26;
                MSS.FirstBand.DL.DNoiseFigureDb = 7;

                
                MSS.FirstBand.DVBackLossDB = -25;
                MSS.FirstBand.DHBackLossDB = -25;
                
                
                

                MSS.FirstBand.IVAntNumPerPanel = 1;
                MSS.FirstBand.IHAntNumPerPanel = 1; //1上行，2下行
                MSS.FirstBand.IVPanelNum = 1;
                MSS.FirstBand.IHPanelNum = 1;
                MSS.FirstBand.Polarize_Num = 2;
                MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel = 1;
                MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel = 1; //1上行，2下行
            }
        }
            break;

    case chty211130:
//        {
//            BASIC.IBTSPerBS = 3;
//            BASIC.INumBSs = 7; //7对应7小区，19对应19小区 ,小区数目
//            BASIC.IMacroTopology = 0; // 0对应7小区，1对应19小区 ，wrap方式
//            BASIC.IMsDistributeMode = 0;
//        }
//        {
//            Macro.DPenetrationLossdB = 10;
//            Macro.DTransmissionLineLoss_dB = -1.0 * 0.2;
//
//            Macro.LINK.DRadioFrequencyMHz = 2130;
//            Macro.IAntennaPatternMode = 1;
//            Macro.DV3DBBeamWidthDeg = 14;
//            Macro.DH3DBBeamWidthDeg = 65;
//            Macro.DAntennaGainDb = 17.03;
//            Macro.DHBackLossDB = -25;
//            Macro.DVBackLossDB = -25;
//            Macro.DL.DMaxTxPowerDbm = 49;
//            Macro.DAntennaHeightM = 25;
//            Macro.DSiteDistanceM = 1000;
//            Macro.DMechanicalTiltDeg = 9.0;
//            Macro.DElectricalTiltDeg = 0;
//            Macro.DVAntSpace = 4;
//            Macro.DHAntSpace = 0.5;
//            Macro.UL.DNoiseFigureDb = 5.0;
//
//            Macro.IVAntNumPerPanel = 8;
//            Macro.IHAntNumPerPanel = 2;
//            Macro.Polarize_Num = 2;
//            Macro.IVPanelNum = 1;
//            Macro.IHPanelNum = 1;
//            Macro.V_TXRU_DIV_NUM_PerPanel = 8;
//            Macro.H_TXRU_DIV_NUM_PerPanel =2;
//
//            Macro.LINK.DMinDistanceM = 10;
//            Macro.DL.bEnable_BSAnalogBF = false;
//        }
//        {
//            MSS.FirstBand.bEnable_UEAnalogBF = false;
//
////            MSS.IMinBuildingFloor = 4;
////            MSS.IMaxBuildingFloor = 8;
//
//            MSS.DProbOutdoor = 0.2;
//            // 1: Outdoor is In Car; 0 : Outdoor is not In Car
//            MSS.bIsOutdoorInCar_notOnCar = 1;
//            // 室外用户速度扩展因子
//            MSS.DCarSpeedSacleFactor = 10.0;
//
//            MSS.DProbLowloss = 0.8;
//
            MSS.FirstBand.IAntennaPatternMode = 0;
////            MSS.FirstBand.DV3DBBeamWidthDeg = 90;
////            MSS.FirstBand.DH3DBBeamWidthDeg = 90;
////            MSS.FirstBand.DAntennaGainDb = 0;
////            MSS.DAntennaHeightM = 1.5;
////            MSS.FirstBand.DHAntSpace = 0.5;
////            MSS.FirstBand.DVAntSpace = 0.5;
            MSS.FirstBand.UL.DMaxTxPowerDbm = 23;
            MSS.FirstBand.DL.DNoiseFigureDb = 7;
//        }
//            
            break;
        default:
        {
            Macro.IAntennaPatternMode = 1;

            {
                // 室外用户速度扩展因子
                MSS.DCarSpeedSacleFactor = 10.0;
                MSS.DProbLowloss = 0.8;
            }
        }
            break;
    }

}

void Parameters::Set_LowFreq_RMA() {

    switch (BASIC.ISubScenarioModel) {
        case Parameters::SCENARIO_Phase1_RMA_NORMAL:
        {
            {
                BASIC.IBTSPerBS = 3; //
                BASIC.INumerologyIndicator = 1; //
                BASIC.INumBSs = 19; //7对应7小区，19对应19小区 ,小区数目
                BASIC.IMacroTopology = 1; // 0对应7小区，1对应19小区 ，wrap方式
                BASIC.IMsDistributeMode = 0; //
            }
            {
                Macro.LINK.DRadioFrequencyMHz = 700;
                Macro.IAntennaPatternMode = 1;
                Macro.DV3DBBeamWidthDeg = 65;
                Macro.DH3DBBeamWidthDeg = 65;
                Macro.DAntennaGainDb = 8;
                //            Macro.DL.DMaxTxPowerDbm = 46;
                Macro.DAntennaHeightM = 35;
                Macro.DSiteDistanceM = 1732;
                Macro.DMechanicalTiltDeg = 0;
                Macro.DElectricalTiltDeg = 10;
                Macro.DVAntSpace = 0.8;
                Macro.DHAntSpace = 0.5;
                Macro.UL.DNoiseFigureDb = 5;
                Macro.IVAntNumPerPanel = 8;
                Macro.IHAntNumPerPanel = 4;
                Macro.IVPanelNum = 1; //
                Macro.IHPanelNum = 1; //

                // 20180621
                Macro.Polarize_Num = 2;
                Macro.V_TXRU_DIV_NUM_PerPanel = 1;
                Macro.H_TXRU_DIV_NUM_PerPanel = 4;

                Macro.DHBackLossDB = -30;
                Macro.DVBackLossDB = -30;
                Macro.LINK.DMinDistanceM = 10; //35;
                Macro.DL.bEnable_BSAnalogBF = false;

            }

            {
                MSS.FirstBand.bEnable_UEAnalogBF = false;

                MSS.DProbOutdoor = 0.5;
                // 1: Outdoor is In Car; 0 : Outdoor is not In Car
                MSS.bIsOutdoorInCar_notOnCar = true;
                // 室外用户速度扩展因子
                MSS.DCarSpeedSacleFactor = 40.0;

                MSS.DProbLowloss = 1.0;

                MSS.FirstBand.IAntennaPatternMode = 0;
                MSS.FirstBand.DV3DBBeamWidthDeg = 90; //ADD
                MSS.FirstBand.DH3DBBeamWidthDeg = 90; //ADD
                MSS.FirstBand.DAntennaGainDb = 0;
                MSS.DAntennaHeightM = 1.5;
                MSS.FirstBand.DHAntSpace = 0.5;
                MSS.FirstBand.DVAntSpace = 0.5; //ADD
                MSS.FirstBand.UL.DMaxTxPowerDbm = 23;
                MSS.FirstBand.DL.DNoiseFigureDb = 7;
                MSS.FirstBand.IVAntNumPerPanel = 1;
                MSS.FirstBand.IHAntNumPerPanel = 1;
                MSS.FirstBand.IVPanelNum = 1;
                MSS.FirstBand.IHPanelNum = MSS.IInitialize1or2Panel;
                MSS.FirstBand.DHBackLossDB = -25;
                MSS.FirstBand.DVBackLossDB = -25;

                MSS.FirstBand.Polarize_Num = 2;
                MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel = 1;
                MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel = 1;

            }
        }
            break;
        default:
            assert(false);
            break;
    }

}

void Parameters::Set_HighFreq_InH() {
    {
        BASIC.IBTSPerBS = 1; // 3 for 36 TRP ,1for 12 TRP
        BASIC.INumerologyIndicator = 4;
        BASIC.INumBSs = 12;
        BASIC.IMacroTopology = 3;
        BASIC.IMsDistributeMode = 0;
    }
    {
        Macro.IAntennaPatternMode = 1;
        Macro.DV3DBBeamWidthDeg = 90;
        Macro.DH3DBBeamWidthDeg = 90;
        Macro.DAntennaGainDb = 5;
        //        Macro.DL.DMaxTxPowerDbm = 20;//23 for 30GNRMIMO;20 for ITU
        Macro.DAntennaHeightM = 3;
        Macro.DSiteDistanceM = 20;
        Macro.DMechanicalTiltDeg = 90; //20 for 36TRP ,90 for 12TRP
        Macro.DElectricalTiltDeg = 0;
        Macro.DVAntSpace = 0.5;
        Macro.DHAntSpace = 0.5;
        Macro.UL.DNoiseFigureDb = 7;
        Macro.IVAntNumPerPanel = 2; //4;
        Macro.IHAntNumPerPanel = 4; //8;
        Macro.IVPanelNum = 2; //1;
        Macro.IHPanelNum = 2; //1;
        Macro.DHBackLossDB = -25;
        Macro.DVBackLossDB = -25;
        Macro.LINK.DMinDistanceM = 0; //原1.5;
        Macro.LINK.DRadioFrequencyMHz = 30000;
        Macro.DL.bEnable_BSAnalogBF = true;
    }
    {
        MSS.DProbOutdoor = 0;
        // 1: Outdoor is In Car; 0 : Outdoor is not In Car
        MSS.DAntennaHeightM = 1.5;
        MSS.FirstBand.bEnable_UEAnalogBF = true;
        MSS.FirstBand.IAntennaPatternMode = 1;
        MSS.FirstBand.DV3DBBeamWidthDeg = 90; //ADD
        MSS.FirstBand.DH3DBBeamWidthDeg = 90; //ADD
        MSS.FirstBand.DAntennaGainDb = 5;
        MSS.FirstBand.DHAntSpace = 0.5;
        MSS.FirstBand.DVAntSpace = 0.5; //ADD
        MSS.FirstBand.UL.DMaxTxPowerDbm = 23;
        MSS.FirstBand.DL.DNoiseFigureDb = 10;
        MSS.FirstBand.IVAntNumPerPanel = 2;
        MSS.FirstBand.IHAntNumPerPanel = 4;
        MSS.FirstBand.IVPanelNum = 1;
        MSS.FirstBand.IHPanelNum = MSS.IInitialize1or2Panel;
        MSS.FirstBand.DHBackLossDB = -25;
        MSS.FirstBand.DVBackLossDB = -25;
    }



}

void Parameters::Set_HighFreq_UMI() {
    {
        BASIC.IBTSPerBS = 3;
        BASIC.INumerologyIndicator = 4;
        BASIC.INumBSs = 7; //7对应7小区，19对应19小区 ,小区数目
        BASIC.IMacroTopology = 0; //0对应7小区，1对应19小区 ，wrap方式
        BASIC.IMsDistributeMode = 0;
    }
    {


        Macro.IAntennaPatternMode = 1;
        Macro.DV3DBBeamWidthDeg = 90; //65 for 5GUMA
        Macro.DH3DBBeamWidthDeg = 90; //65 for 5GUMA
        Macro.DAntennaGainDb = 5; //8 for 5GUMA
        //        Macro.DL.DMaxTxPowerDbm = 21;//37 for 5GUMA
        Macro.DAntennaHeightM = 10;
        Macro.DSiteDistanceM = 200;
        Macro.DMechanicalTiltDeg = 0;
        Macro.DElectricalTiltDeg = 0;
        Macro.DVAntSpace = 0.5;
        Macro.DHAntSpace = 0.5;
        Macro.UL.DNoiseFigureDb = 7;
        Macro.IVAntNumPerPanel = 8;
        Macro.IHAntNumPerPanel = 8;
        Macro.IVPanelNum = 1; //这里设置1是只考虑1个面板，方便查看结果，实际值为2;
        Macro.IHPanelNum = 1; //这里设置1是只考虑1个面板，方便查看结果，实际值为2;
        Macro.DHBackLossDB = -30;
        Macro.DVBackLossDB = -30;
        Macro.LINK.DMinDistanceM = 10; //35;
        Macro.LINK.DRadioFrequencyMHz = 30000;
        Macro.DL.bEnable_BSAnalogBF = true;
    }
    {

        MSS.DProbOutdoor = 0.2;
        // 1: Outdoor is In Car; 0 : Outdoor is not In Car
        MSS.bIsOutdoorInCar_notOnCar = true;
        // 室外用户速度扩展因子
        MSS.DCarSpeedSacleFactor = 10.0;

        MSS.DProbLowloss = 0.8;

        MSS.FirstBand.bEnable_UEAnalogBF = true;
        MSS.FirstBand.IAntennaPatternMode = 1;
        MSS.FirstBand.DV3DBBeamWidthDeg = 90; //ADD
        MSS.FirstBand.DH3DBBeamWidthDeg = 90; //ADD
        MSS.FirstBand.DAntennaGainDb = 3; //5 for 5GUMA
        MSS.DAntennaHeightM = 1.5;
        MSS.FirstBand.DHAntSpace = 0.5;
        MSS.FirstBand.DVAntSpace = 0.5; //ADD
        MSS.FirstBand.UL.DMaxTxPowerDbm = 23;
        MSS.FirstBand.DL.DNoiseFigureDb = 13; //10 for 5GUMA
        MSS.FirstBand.IVAntNumPerPanel = 2;
        MSS.FirstBand.IHAntNumPerPanel = 4;
        MSS.FirstBand.IVPanelNum = 1;
        MSS.FirstBand.IHPanelNum = MSS.IInitialize1or2Panel;
        MSS.FirstBand.DHBackLossDB = -25;
        MSS.FirstBand.DVBackLossDB = -25;
    }
}

void Parameters::Set_HighFreq_UMA() {     
            {
                BASIC.IBTSPerBS = 1;
                BASIC.INumerologyIndicator = 9;
                BASIC.INumBSs = 1; //7对应7小区，19对应19小区 ,小区数目
                BASIC.IMacroTopology = 0; // 0对应7小区，1对应19小区 ，wrap方式
                BASIC.IMsDistributeMode = 0;
            }
            {
                Macro.DPenetrationLossdB = 16;
                Macro.DTransmissionLineLoss_dB = -1.0 * 0.0;

                Macro.LINK.DRadioFrequencyMHz = 26000;
                Macro.IAntennaPatternMode = 1;
                Macro.DV3DBBeamWidthDeg = 14;//14
                Macro.DH3DBBeamWidthDeg = 14;//14
                Macro.DAntennaGainDb = 23.06; // 23.06
                Macro.DHBackLossDB = -30;
                Macro.DVBackLossDB = -30;
                Macro.DL.DMaxTxPowerDbm = 53;  //52
                Macro.DAntennaHeightM = 25;
                Macro.DSiteDistanceM = 200;
                //                Macro.DMechanicalTiltDeg = 9.1;
                Macro.DElectricalTiltDeg = 12;
                Macro.DVAntSpace = 4;//4
                Macro.DHAntSpace = 4;//4
                Macro.UL.DNoiseFigureDb = 5.0; // 4.0

                Macro.IVAntNumPerPanel = 2;   //  1       16    2
                Macro.IHAntNumPerPanel = 1;   //  2       8    1
                Macro.IVPanelNum = 1;
                Macro.IHPanelNum = 1;  //   1
                Macro.Polarize_Num = 2;
                Macro.V_TXRU_DIV_NUM_PerPanel = 2;  //    2
                Macro.H_TXRU_DIV_NUM_PerPanel = 1;  //    1

                Macro.LINK.DMinDistanceM = 35;
                Macro.DL.bEnable_BSAnalogBF = false;
            }
            {
                MSS.FirstBand.bEnable_UEAnalogBF = false;

                MSS.DProbOutdoor = 1.0;
                // 1: Outdoor is In Car; 0 : Outdoor is not In Car
                MSS.bIsOutdoorInCar_notOnCar = true;
                // 室外用户速度扩展因子
                MSS.DCarSpeedSacleFactor = 1.0;

                MSS.DProbLowloss = 0.8;

                MSS.FirstBand.IAntennaPatternMode = 0;
                MSS.FirstBand.DV3DBBeamWidthDeg = 65;
                MSS.FirstBand.DH3DBBeamWidthDeg = 65;
                MSS.FirstBand.DAntennaGainDb = 9; // 9
                MSS.DAntennaHeightM = 1.5;
                MSS.FirstBand.DHAntSpace = 0.5;
                MSS.FirstBand.DVAntSpace = 0.5;
                MSS.FirstBand.UL.DMaxTxPowerDbm = 26;
                MSS.FirstBand.DL.DNoiseFigureDb = 9;

                MSS.FirstBand.DHBackLossDB = -25;
                MSS.FirstBand.DVBackLossDB = -25;

                MSS.FirstBand.IVAntNumPerPanel = 2; // 1   2
                MSS.FirstBand.IHAntNumPerPanel = 4; // 1   4
                MSS.FirstBand.IVPanelNum = 1; // 1
                MSS.FirstBand.IHPanelNum = 1;  // 1
                MSS.FirstBand.Polarize_Num = 2; // 2
                MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel = 1; // 1
                MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel = 2; // 1
            }
            
}

void Parameters::Set_HighFreq_RMA() {

}

void Parameters::Set_5G_1Layer() {
    {
        BASIC.IBTSPerBS = 3;
        BASIC.INumerologyIndicator = 1;
        BASIC.INumBSs = 7;
        BASIC.IMacroTopology = 0;
        BASIC.IMsDistributeMode = 0;
    }
    {
        Macro.IAntennaPatternMode = 1;
        Macro.DV3DBBeamWidthDeg = 65;
        Macro.DH3DBBeamWidthDeg = 65;
        Macro.DAntennaGainDb = 8;
        //        Macro.DL.DMaxTxPowerDbm = 41;
        Macro.DAntennaHeightM = 25;
        Macro.DSiteDistanceM = 500;
        Macro.DMechanicalTiltDeg = 0;
        Macro.DElectricalTiltDeg = 0;
        Macro.DVAntSpace = 0.8;
        Macro.DHAntSpace = 0.5;
        Macro.UL.DNoiseFigureDb = 5;
        Macro.IVAntNumPerPanel = 8;
        Macro.IHAntNumPerPanel = 1;
        Macro.IVPanelNum = 1;
        Macro.IHPanelNum = 8;
        Macro.DHBackLossDB = -30;
        Macro.DVBackLossDB = -30;
        Macro.LINK.DMinDistanceM = 35;
        Macro.LINK.DRadioFrequencyMHz = 4000;
        Macro.DL.bEnable_BSAnalogBF = true;
    }
    {
        MSS.FirstBand.bEnable_UEAnalogBF = false;
        MSS.FirstBand.IAntennaPatternMode = 0;
        MSS.FirstBand.DV3DBBeamWidthDeg = 90; //ADD
        MSS.FirstBand.DH3DBBeamWidthDeg = 90; //ADD
        MSS.FirstBand.DAntennaGainDb = 0;
        MSS.DAntennaHeightM = 1.5;
        MSS.FirstBand.DHAntSpace = 0.5;
        MSS.FirstBand.DVAntSpace = 0.5; //ADD
        MSS.FirstBand.UL.DMaxTxPowerDbm = 23;
        MSS.FirstBand.DL.DNoiseFigureDb = 9;
        MSS.FirstBand.IVAntNumPerPanel = 1;
        MSS.FirstBand.IHAntNumPerPanel = 1;
        MSS.FirstBand.IVPanelNum = 1;
        MSS.FirstBand.IHPanelNum = 1;
        MSS.FirstBand.DHBackLossDB = -25;
        MSS.FirstBand.DVBackLossDB = -25;
    }


}

void Parameters::Set_5G_2Layer() {
    {
        BASIC.IBTSPerBS = 3;
        BASIC.INumerologyIndicator = 1;
        BASIC.INumBSs = 7;
        BASIC.IMacroTopology = 0;
        BASIC.IMsDistributeMode = 0;
    }
    {
        Macro.IAntennaPatternMode = 1;
        Macro.DV3DBBeamWidthDeg = 65;
        Macro.DH3DBBeamWidthDeg = 65;
        Macro.DAntennaGainDb = 5;
        //        Macro.DL.DMaxTxPowerDbm = 41;
        Macro.DAntennaHeightM = 25;
        Macro.DSiteDistanceM = 200;
        Macro.DMechanicalTiltDeg = 0;
        Macro.DElectricalTiltDeg = 0;
        Macro.DVAntSpace = 0.8;
        Macro.DHAntSpace = 0.5;
        Macro.UL.DNoiseFigureDb = 5;
        Macro.IVAntNumPerPanel = 8;
        Macro.IHAntNumPerPanel = 1;
        Macro.IVPanelNum = 1;
        Macro.IHPanelNum = 8;
        Macro.DHBackLossDB = -30;
        Macro.DVBackLossDB = -30;
        Macro.LINK.DMinDistanceM = 35;
        Macro.LINK.DRadioFrequencyMHz = 4000;
    }
    {
        SmallCell.LINK.DRadioFrequencyMHz = 3500;
    }
    {
        MSS.FirstBand.bEnable_UEAnalogBF = false;
        MSS.FirstBand.IAntennaPatternMode = 0;
        MSS.FirstBand.DV3DBBeamWidthDeg = 90; //ADD
        MSS.FirstBand.DH3DBBeamWidthDeg = 90; //ADD
        MSS.FirstBand.DAntennaGainDb = 0;
        MSS.DAntennaHeightM = 1.5;
        MSS.FirstBand.DHAntSpace = 0.5;
        MSS.FirstBand.DVAntSpace = 0.5; //ADD
        MSS.FirstBand.UL.DMaxTxPowerDbm = 23;
        MSS.FirstBand.DL.DNoiseFigureDb = 9;
        MSS.FirstBand.IVAntNumPerPanel = 1;
        MSS.FirstBand.IHAntNumPerPanel = 1;
        MSS.FirstBand.IVPanelNum = 1;
        MSS.FirstBand.IHPanelNum = 1;
        MSS.FirstBand.DHBackLossDB = -25;
        MSS.FirstBand.DVBackLossDB = -25;
    }
}

//范型的读取参数的函数

void Parameters::ReadData_DL(char const * _pFilename) {
    ifstream f(_pFilename); //建立一个输入文件流
    string buffer, temp;
    while (!f.eof()) {
        f >> buffer;
        if (buffer == "&") //找到标记符号&
        {
            f >> buffer; //将参数的名称读入buffer中
            //确定该参数是否是合法参数
            if (m_Str2ParaMap_DL.find(buffer) == m_Str2ParaMap_DL.end()) {
                cout << "illegal parameters " << buffer << " !" << endl;
                //                assert(false);
                continue;
            }
            //确定该参数的数据类型，并且读入这个参数
            if (m_Str2ParaMap_DL[buffer].type() == typeid (int *)) {
                int para;
                f >> para;
                *any_cast<int*>(m_Str2ParaMap_DL[buffer]) = para;
            } else if (m_Str2ParaMap_DL[buffer].type() == typeid (double *)) {
                double para;
                f >> para;
                *any_cast<double*>(m_Str2ParaMap_DL[buffer]) = para;
            } else if (m_Str2ParaMap_DL[buffer].type() == typeid (float *)) {
                float para;
                f >> para;
                *any_cast<float*>(m_Str2ParaMap_DL[buffer]) = para;
            } else if (m_Str2ParaMap_DL[buffer].type() == typeid (bool *)) {
                bool para;
                f >> para;
                *any_cast<bool*>(m_Str2ParaMap_DL[buffer]) = para;
            } else if (m_Str2ParaMap_DL[buffer].type() == typeid (string *)) {
                string para;
                f >> para;
                *any_cast<string*>(m_Str2ParaMap_DL[buffer]) = para;
            } else if (m_Str2ParaMap_DL[buffer].type() == typeid (long *)){
                long para;
                f >> para;
                *any_cast<long*>(m_Str2ParaMap_DL[buffer]) = para;
            } else {
                assert(false);
            }
        }
    }
}

void Parameters::ReadData_UL(char const * _pFilename) {
    ifstream f(_pFilename); //建立一个输入文件流
    string buffer, temp;
    while (!f.eof()) {
        f >> buffer;
        if (buffer == "&") //找到标记符号&
        {
            f >> buffer; //将参数的名称读入buffer中
            //确定该参数是否是合法参数
            if (m_Str2ParaMap_UL.find(buffer) == m_Str2ParaMap_UL.end()) {
                cout << "illegal parameters " << buffer << " !" << endl;
                //                assert(false);
                continue;
            }
            //确定该参数的数据类型，并且读入这个参数
            if (m_Str2ParaMap_UL[buffer].type() == typeid (int *)) {
                int para;
                f >> para;
                *any_cast<int*>(m_Str2ParaMap_UL[buffer]) = para;
            } else if (m_Str2ParaMap_UL[buffer].type() == typeid (double *)) {
                double para;
                f >> para;
                *any_cast<double*>(m_Str2ParaMap_UL[buffer]) = para;
            } else if (m_Str2ParaMap_UL[buffer].type() == typeid (float *)) {
                float para;
                f >> para;
                *any_cast<float*>(m_Str2ParaMap_UL[buffer]) = para;
            } else if (m_Str2ParaMap_UL[buffer].type() == typeid (bool *)) {
                bool para;
                f >> para;
                *any_cast<bool*>(m_Str2ParaMap_UL[buffer]) = para;
            } else if (m_Str2ParaMap_UL[buffer].type() == typeid (string *)) {
                string para;
                f >> para;
                *any_cast<string*>(m_Str2ParaMap_UL[buffer]) = para;
            } else {
                assert(false);
            }
        }
    }
}

///ReorderData
///范型的刷新输入参数格式函数
///@param _pFilename 指向输入文件流

void Parameters::ReorderData(char const * _pFilename) {
    std::ifstream f(_pFilename); ////建立一个输入文件流
    std::ifstream ftitle(_pFilename);
    std::vector <std::string> VParameter;

    std::map<std::string, std::string> mStr2ValueMap;
    std::vector<std::string> vNotes;
    std::vector<std::string> vTitles_2;
    std::map<std::string, std::vector< std::string > > mStr2NotesMap;

    std::string buffer, sTitle;
    std::string temp("none");

    getline(ftitle, sTitle); ///处理表头和题目
    while (sTitle.find("&") > sTitle.length() && (!ftitle.eof())) {
        vTitles_2.push_back(sTitle);
        getline(ftitle, sTitle);
    }


    while (!f.eof()) { ///处理parameter,value,Notes
        if (temp == "none") {
            f >> buffer;
        }
        if (buffer == "&" || temp == "&") ///找到标记符号&
        {
            f >> buffer; ///将参数的名称读入buffer中
            std::string sParameter = buffer;
            VParameter.push_back(buffer); ///将参数的名称保存中

            f >> buffer;
            std::string sValue = buffer;
            mStr2ValueMap[sParameter] = sValue;

            f >> buffer;
            if (buffer == "&") {
                temp = buffer;
                continue;
            }
            std::string tmp;
            std::ifstream fnotes(_pFilename); ////建立一个输入文件流,用来读取Notes
            do {
                getline(fnotes, tmp); /// 找到当前f所指的位置
            } while (tmp.find(sParameter) > tmp.length());
            std::string::size_type tmpsize_2 = tmp.find(sValue);
            tmp.erase(0, sValue.length() + tmpsize_2);
            tmp = boost::trim_left_copy(tmp);
            vNotes.push_back(tmp);
            getline(fnotes, tmp);
            while (tmp.find("&") > tmp.length() && (!fnotes.eof())) {
                tmp = boost::trim_left_copy(tmp);
                if (tmp.length() != 0) {
                    vNotes.push_back(tmp);
                }
                getline(fnotes, tmp);
            }
            mStr2NotesMap[sParameter] = vNotes;
            vNotes.clear();
            temp = "none";

        }
    }

    std::ofstream fout(_pFilename); ////建立一个输出文件流
    std::sort(VParameter.begin(), VParameter.end());
    for (std::vector <std::string>::iterator it = vTitles_2.begin();
            it != vTitles_2.end();
            ++it) {
        fout << (*it) << std::endl;
    }
    for (std::vector <std::string>::iterator it = VParameter.begin(); it != VParameter.end(); ++it) {

        fout << "& " << std::left << std::setw(36) << (*it) << "  "
                << std::right << std::setw(10) << mStr2ValueMap[(*it)]
                << std::setw(10) << " ";
        std::vector<std::string> tmp_2 = mStr2NotesMap[(*it)];
        int len = tmp_2.size();
        if (len == 0) {
            fout << std::endl;
            continue;
        }
        for (int i = 0; i < len; ++i) {
            if (i > 0) {
                std::string s2(60, ' ');
                s2.append(tmp_2[i]);
                fout << s2 << std::endl;
            } else {
                fout << std::left << tmp_2[i] << std::endl;
            }
        }
    }

}

//ReorderCaseParameter
///范型的刷新（重排序）inputfiles中的CaseParameter中的输入

void Parameters::ReorderCaseParameter() {
    boost::filesystem::path dir_path("./inputfiles/CaseParameters");
    if (!exists(dir_path)) {
        return;
    }

    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(dir_path);
            itr != end_itr;
            ++itr) {
        std::string tmp; // itr->path().native_directory_string();
        if (tmp.find(".txt") < tmp.length()) {
            //            ReorderData(const_cast<char*> (tmp.data()));

        }
    }
}

// 建立参数关联映射表

void Parameters::Build_DL() {
    // 关联基本参数
    m_Str2ParaMap_DL.clear();
    //BASIC
    m_Str2ParaMap_DL["BASIC.Noise"] = &(BASIC.Noise);
    m_Str2ParaMap_DL["BASIC.DOverHead_LTE"] = &(BASIC.DOverHead_LTE);
    m_Str2ParaMap_DL["BASIC.ISINR2BLERTable"] = &(BASIC.ISINR2BLERTable);
    m_Str2ParaMap_DL["BASIC.IMaxModulLevel"] = &(BASIC.IMaxModulLevel);
    m_Str2ParaMap_DL["BASIC.INRMCSNumUL"] = &(BASIC.INRMCSNumUL);
    m_Str2ParaMap_DL["BASIC.ISeed"] = &(BASIC.ISeed);
    m_Str2ParaMap_DL["BASIC.ISlotPerDrop"] = &(BASIC.ISlotPerDrop);
    m_Str2ParaMap_DL["BASIC.INumSnapShot"] = &(BASIC.INumSnapShot);
    m_Str2ParaMap_DL["BASIC.DHandOffMarginDb"] = &(BASIC.DHandOffMarginDb);
    m_Str2ParaMap_DL["BASIC.DRSRPConnectedThreshold"] = &(BASIC.DRSRPConnectedThreshold);
    m_Str2ParaMap_DL["BASIC.DNoisePowerSpectrumDensityDbmHz"] = &(BASIC.DNoisePowerSpectrumDensityDbmHz);
    m_Str2ParaMap_DL["BASIC.DNumMSPerBTS"] = &(BASIC.DNumMSPerBTS);
    m_Str2ParaMap_DL["BASIC.IMaxServeMSNum"] = &(BASIC.IMaxServeMSNum);
    m_Str2ParaMap_DL["BASIC.IDLORUL"] = &(BASIC.IDLORUL);
    m_Str2ParaMap_DL["BASIC.ITotalFrameNum"] = &(BASIC.ITotalFrameNum);
    m_Str2ParaMap_DL["BASIC.IWarmUpSlot"] = &(BASIC.IWarmUpSlot);
    m_Str2ParaMap_DL["BASIC.BISMultiThread"] = &(BASIC.BISMultiThread);
    m_Str2ParaMap_DL["BASIC.DInterfProb"] = &(BASIC.DInterfProb);
    //m_Str2ParaMap_DL["BASIC.DDLRENumPerRBforData"] = &(BASIC.DDLRENumPerRBforData);
    m_Str2ParaMap_DL["BASIC.RRC_Config_On"] = &(BASIC.RRC_Config_On);
//    cout << BASIC.RRC_Config_On << "RRC_Config_On" << endl;

    //LINK_CTRL
    m_Str2ParaMap_DL["LINK_CTRL.IStrongLinkNum"] = &(LINK_CTRL.IStrongLinkNum);
    m_Str2ParaMap_DL["LINK_CTRL.IFrequencySpace"] = &(LINK_CTRL.IFrequencySpace);
    m_Str2ParaMap_DL["LINK_CTRL.IISFastFadingUsed"] = &(LINK_CTRL.IISFastFadingUsed);
    m_Str2ParaMap_DL["LINK_CTRL.I2DOr3DChannel"] = &(LINK_CTRL.I2DOr3DChannel);
    m_Str2ParaMap_DL["LINK_CTRL.IISShadowFadingUsed"] = &(LINK_CTRL.IISShadowFadingUsed);
    m_Str2ParaMap_DL["LINK_CTRL.Islot4Hupdate"] = &(LINK_CTRL.Islot4Hupdate);
    //20200506BYLYC
    //    m_Str2ParaMap_DL["BASIC.DDLFrameRate"] = &(BASIC.DDLFrameRate);
    m_Str2ParaMap_DL["BASIC.DSystemBandWidthKHz"] = &(BASIC.DSystemBandWidthKHz);
    m_Str2ParaMap_DL["BASIC.IUTattachment"] = &(BASIC.IUTattachment);
    //MIMO_CTRL
    m_Str2ParaMap_DL["MIMO_CTRL.I2Dor3DMIMO"] = &(MIMO_CTRL.I2Dor3DMIMO);
    m_Str2ParaMap_DL["MIMO_CTRL.IHorCovR"] = &(MIMO_CTRL.IHorCovR);
    m_Str2ParaMap_DL["MIMO_CTRL.I2DMatrixmode"] = &(MIMO_CTRL.I2DMatrixmode);
    m_Str2ParaMap_DL["MIMO_CTRL.SubarrORFullconnec"] = &(MIMO_CTRL.SubarrORFullconnec);
    m_Str2ParaMap_DL["MIMO_CTRL.IEnable_Portbased_BF_CSIRS"] = &(MIMO_CTRL.IEnable_Portbased_BF_CSIRS);
    m_Str2ParaMap_DL["MIMO_CTRL.CSIRS_PortNum"] = &(MIMO_CTRL.BF_CSIRS_PortNum);

    //ERROR
    m_Str2ParaMap_DL["ERROR.IAntennaCalibrationError"] = &(ERROR.IAntennaCalibrationError);
    m_Str2ParaMap_DL["ERROR.ICQI_DELAY"] = &(ERROR.ICQI_DELAY);
    m_Str2ParaMap_DL["ERROR.ICQI_PERIOD"] = &(ERROR.ICQI_PERIOD);
    m_Str2ParaMap_DL["ERROR.IRI_Meathod"] = &(ERROR.IRI_Meathod);
    m_Str2ParaMap_DL["ERROR.ISRS_PERIOD"] = &(ERROR.ISRS_PERIOD);
    m_Str2ParaMap_DL["ERROR.ISRS_WIDTH"] = &(ERROR.ISRS_WIDTH);
    m_Str2ParaMap_DL["ERROR.ISRS_COMB"] = &(ERROR.ISRS_COMB);
    m_Str2ParaMap_DL["ERROR.ISRS_TxSpace"] = &(ERROR.ISRS_TxSpace);
    m_Str2ParaMap_DL["ERROR.ISRS_DELAY"] = &(ERROR.ISRS_DELAY);
    m_Str2ParaMap_DL["ERROR.ISRS_TxMode"] = &(ERROR.ISRS_TxMode);
    m_Str2ParaMap_DL["ERROR.ISRS_Error"] = &(ERROR.ISRS_Error);
    m_Str2ParaMap_DL["ERROR.IDMRS_Error"] = &(ERROR.IDMRS_Error);
    m_Str2ParaMap_DL["ERROR.DEVMGateDb"] = &(ERROR.DEVMGateDb);
    m_Str2ParaMap_DL["ERROR.DSRS_OSNumPerSlot"] = &(ERROR.DSRS_OSNumPerSlot);
    m_Str2ParaMap_DL["ERROR.DSRS_SystemBW_MHz"] = &(ERROR.DSRS_SystemBW_MHz);
    m_Str2ParaMap_DL["ERROR.DSRS_SRS_UnitBW_MHz"] = &(ERROR.DSRS_SRS_UnitBW_MHz);

    //BTS
    //    m_Str2ParaMap_DL["Macro.IAntennaPatternMode"] = &(Macro.IAntennaPatternMode);
    m_Str2ParaMap_DL["BASIC.IScheduleMode"] = &(BASIC.IScheduleMode);
    m_Str2ParaMap_DL["Macro.DOTA_dB"] = &(Macro.DOTA_dB);
    m_Str2ParaMap_DL["Macro.DMSBodyLoss_dB"] = &(Macro.DMSBodyLoss_dB);
    m_Str2ParaMap_DL["BASIC.ISCSConfig"] = &(BASIC.ISCSConfig);
    m_Str2ParaMap_DL["BASIC.IRBSize"] = &(BASIC.IRBSize);
    //MSS
    m_Str2ParaMap_DL["MSS.DVelocityMPS"] = &(MSS.DVelocityMPS);
    m_Str2ParaMap_DL["MSS.IInitialize1or2Panel"] = &(MSS.IInitialize1or2Panel);
    m_Str2ParaMap_DL["MIMO_CTRL.IDetectorMode"] = &(MIMO_CTRL.IDetectorMode);
    m_Str2ParaMap_DL["MIMO_CTRL.IMMSEOption"] = &(MIMO_CTRL.IMMSEOption);
    m_Str2ParaMap_DL["MIMO_CTRL.IApSounding"] = &(MIMO_CTRL.IApSounding);
    m_Str2ParaMap_DL["MSS.FirstBand.UL.DP0Dbm4SRS"] = &(MSS.FirstBand.UL.DP0Dbm4SRS);
    m_Str2ParaMap_DL["MSS.FirstBand.UL.DAlfa4SRS"] = &(MSS.FirstBand.UL.DAlfa4SRS);
    m_Str2ParaMap_DL["Macro.DNLOS_FreFadingFactor"] = &(Macro.DNLOS_FreFadingFactor);
    //LINK
    m_Str2ParaMap_DL["BASIC.IScenarioModel"] = &(BASIC.IScenarioModel);
    m_Str2ParaMap_DL["BASIC.ISubScenarioModel"] = &(BASIC.ISubScenarioModel);

    //    m_Str2ParaMap_DL["BASIC.IChannelModel_for_Scenario"] = &(BASIC.IChannelModel_for_Scenario);
    m_Str2ParaMap_DL["BASIC.IChannelModel_VariantMode"] = &(BASIC.IChannelModel_VariantMode);

    //    m_Str2ParaMap_DL["Macro.IPolarize"] = &(Macro.IPolarize);
    m_Str2ParaMap_DL["Macro.DL.DMaxTxPowerDbm"] = &(Macro.DL.DMaxTxPowerDbm);
    m_Str2ParaMap_DL["Macro.DMechanicalTiltDeg"] = &(Macro.DMechanicalTiltDeg);

    //    m_Str2ParaMap_DL["MSS.FirstBand.IPolarize"] = &(MSS.FirstBand.IPolarize);
    //XR
    m_Str2ParaMap_DL["XR.dPeriod_ms"] = &(XR.dPeriod_ms);
    m_Str2ParaMap_DL["XR.FPS"] = &(XR.FPS);
    m_Str2ParaMap_DL["XR.alpha"] = &(XR.alpha);
    m_Str2ParaMap_DL["XR.SumRate"] = &(XR.SumRate);
    m_Str2ParaMap_DL["XR.dS"] = &(XR.dS);
    m_Str2ParaMap_DL["XR.dPacketSizeDefault_Kbit"] = &(XR.dPacketSizeDefault_Kbit);
    m_Str2ParaMap_DL["XR.dPmax"] = &(XR.dPmax);
    m_Str2ParaMap_DL["XR.dPave"] = &(XR.dPave);
    m_Str2ParaMap_DL["XR.dDelayBuget_ms"] = &(XR.dDelayBuget_ms);
    m_Str2ParaMap_DL["XR.iLogperiod_slot"] = &(XR.iLogPeriod_slot);

    //20200601Di
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.IIsSinglePeriod"] = &(SIM.FrameStructure4Sim.IIsSinglePeriod);
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.P1.IPeriodSlot"] = &(SIM.FrameStructure4Sim.P1.IPeriodSlot);
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.P1.IDLSlot"] = &(SIM.FrameStructure4Sim.P1.IDLSlot);
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.P1.IULSlot"] = &(SIM.FrameStructure4Sim.P1.IULSlot);
    //    m_Str2ParaMap["SIM.FrameStructure4Sim.P1.ISpecialSlot"] = &(SIM.FrameStructure4Sim.P1.ISpecialSlot);
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.P1.IDLOS"] = &(SIM.FrameStructure4Sim.P1.IDLOS);
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.P1.IULOS"] = &(SIM.FrameStructure4Sim.P1.IULOS);
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.P2.IPeriodSlot"] = &(SIM.FrameStructure4Sim.P2.IPeriodSlot);
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.P2.IDLSlot"] = &(SIM.FrameStructure4Sim.P2.IDLSlot);
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.P2.IULSlot"] = &(SIM.FrameStructure4Sim.P2.IULSlot);
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.P2.IDLOS"] = &(SIM.FrameStructure4Sim.P2.IDLOS);
    m_Str2ParaMap_DL["SIM.FrameStructure4Sim.P2.IULOS"] = &(SIM.FrameStructure4Sim.P2.IULOS);

    m_Str2ParaMap_DL["SIM.FrameStructure4Stat.IIsSinglePeriod"] = &(SIM.FrameStructure4Statis.IIsSinglePeriod);
    m_Str2ParaMap_DL["SIM.FrameStructure4Stat.P1.IPeriodSlot"] = &(SIM.FrameStructure4Statis.P1.IPeriodSlot);
    m_Str2ParaMap_DL["SIM.FrameStructure4Stat.P1.IDLSlot"] = &(SIM.FrameStructure4Statis.P1.IDLSlot);
    m_Str2ParaMap_DL["SIM.FrameStructure4Stat.P1.IULSlot"] = &(SIM.FrameStructure4Statis.P1.IULSlot);
    m_Str2ParaMap_DL["SIM.FrameStructure4Stat.P1.IDLOS"] = &(SIM.FrameStructure4Statis.P1.IDLOS);
    m_Str2ParaMap_DL["SIM.FrameStructure4Stat.P1.IULOS"] = &(SIM.FrameStructure4Statis.P1.IULOS);

    m_Str2ParaMap_DL["SIM.DL.Ik0_slot"] = &(SIM.DL.Ik0_slot);
    m_Str2ParaMap_DL["SIM.DL.Ik1_slot"] = &(SIM.DL.Imink1_slot);
    m_Str2ParaMap_DL["SIM.DL.Ik3_slot"] = &(SIM.DL.Imink3_slot);
    // 关联HetNet参数
    m_Str2ParaMap_DL["SmallCell.LINK.DPicoOutBiasDb"] = &(SmallCell.LINK.DPicoOutBiasDb);
    m_Str2ParaMap_DL["SmallCell.LINK.DUEProbInPicoZone4b"] = &(SmallCell.LINK.DUEProbInPicoZone4b);
    m_Str2ParaMap_DL["SmallCell.LINK.IPicoNumPerMacro4b"] = &(SmallCell.LINK.IPicoNumPerMacro4b);
    m_Str2ParaMap_DL["SmallCell.LINK.IPicoNumPerMacroNormal"] = &(SmallCell.LINK.IPicoNumPerMacroNormal);
    m_Str2ParaMap_DL["SmallCell.LINK.IPicoUENumPerCell4b"] = &(SmallCell.LINK.IPicoUENumPerCell4b);
    m_Str2ParaMap_DL["SmallCell.LINK.IPicoUENumPerCellNormal"] = &(SmallCell.LINK.IPicoUENumPerCellNormal);
    m_Str2ParaMap_DL["SmallCell.LINK.DPicoMaxTxPowerDbm"] = &(SmallCell.LINK.DPicoMaxTxPowerDbm);
    m_Str2ParaMap_DL["SmallCell.LINK.DPicoRadiusM"] = &(SmallCell.LINK.DPicoRadiusM);
    m_Str2ParaMap_DL["SmallCell.LINK.DMinDistPico2MacroM"] = &(SmallCell.LINK.DMinDistPico2MacroM);
    m_Str2ParaMap_DL["SmallCell.LINK.DMinDistPico2PicoM"] = &(SmallCell.LINK.DMinDistPico2PicoM);
    m_Str2ParaMap_DL["SmallCell.LINK.DMinDistPico2UEM"] = &(SmallCell.LINK.DMinDistPico2UEM);
    m_Str2ParaMap_DL["SmallCell.LINK.DMinDistCluster2MacroM"] = &(SmallCell.LINK.DMinDistCluster2MacroM);
    m_Str2ParaMap_DL["SmallCell.LINK.DMinDistClusterRadiusM_PicoDrop"] = &(SmallCell.LINK.DMinDistClusterRadiusM_PicoDrop);
    m_Str2ParaMap_DL["SmallCell.LINK.DMinDistClusterRadiusM_UEDrop"] = &(SmallCell.LINK.DMinDistClusterRadiusM_UEDrop);
    m_Str2ParaMap_DL["SmallCell.LINK.IHetnetEnvironmentType"] = &(SmallCell.LINK.IHetnetEnvironmentType);
    m_Str2ParaMap_DL["SmallCell.LINK.DPicoAntennaGainDbi"] = &(SmallCell.LINK.DPicoAntennaGainDbi);
    m_Str2ParaMap_DL["SmallCell.LINK.DPicoAntennaHeightM"] = &(SmallCell.LINK.DPicoAntennaHeightM);
    m_Str2ParaMap_DL["SmallCell.LINK.IPicoAntennaNum"] = &(SmallCell.LINK.IPicoAntennaNum);
    m_Str2ParaMap_DL["SmallCell.LINK.IPicoNumPerCluster_Cluster"] = &(SmallCell.LINK.IPicoNumPerCluster_Cluster);
    m_Str2ParaMap_DL["SmallCell.LINK.IPicoClusterNumPerMacro_Cluster"] = &(SmallCell.LINK.IPicoClusterNumPerMacro_Cluster);
    m_Str2ParaMap_DL["SmallCell.LINK.IUENumPerCell_Cluster"] = &(SmallCell.LINK.IUENumPerCell_Cluster);
    m_Str2ParaMap_DL["SmallCell.LINK.DUEProbInCluster_Cluster"] = &(SmallCell.LINK.DUEProbInCluster_Cluster);
    ///
    m_Str2ParaMap_DL["TRAFFIC.ITrafficModel"] = &(TRAFFIC.ITrafficModel);
    m_Str2ParaMap_DL["TRAFFIC.DPacketSize"] = &(TRAFFIC.DPacketSize);
    m_Str2ParaMap_DL["TRAFFIC.DMacroLamda"] = &(TRAFFIC.DMacroLamda);
    m_Str2ParaMap_DL["TRAFFIC.IQoSLevel"] =&(TRAFFIC.IQoSLevel);

    //SIM
    //    m_Str2ParaMap_DL["SIM.DL.ISubframeNum"] = &(SIM.DL.ISubframeNum);
    m_Str2ParaMap_DL["SIM.DL.IHARQMaxTransNum"] = &(SIM.DL.IHARQMaxTransNum);
    m_Str2ParaMap_DL["SIM.DL.DBlerTarget"] = &(SIM.DL.DBlerTarget);
    m_Str2ParaMap_DL["SIM.DL.ISchedulerWindowLength"] = &(SIM.DL.ISchedulerWindowLength);
    m_Str2ParaMap_DL["SIM.DL.DProportionFairFactor"] = &(SIM.DL.DProportionFairFactor);
    m_Str2ParaMap_DL["SIM.DL.IHARQProcessNum"] = &(SIM.DL.IHARQProcessNum);
    m_Str2ParaMap_DL["MIMO_CTRL.IMIMOMode"] = &(MIMO_CTRL.IMIMOMode);
    m_Str2ParaMap_DL["MIMO_CTRL.IMUMIMOScheduleMethod"] = &(MIMO_CTRL.IMUMIMOScheduleMethod);
    m_Str2ParaMap_DL["MIMO_CTRL.ISpatialBasedOrPanelBased"] = &(MIMO_CTRL.ISpatialBasedOrPanelBased);
    m_Str2ParaMap_DL["SIM.DL.N1_OS"] = &(SIM.DL.IN1_OS);
    m_Str2ParaMap_DL["MIMO_CTRL.IMUMIMONum"] = &(MIMO_CTRL.IMUMIMONum);
    m_Str2ParaMap_DL["MIMO_CTRL.IMaxRankNum"] = &(MIMO_CTRL.IMaxRankNum);
    //    m_Str2ParaMap_DL["SIM.UL.ISubframeNum"] = &(SIM.UL.ISubframeNum);
    m_Str2ParaMap_DL["SIM.UL.IHARQProcessNum"] = &(SIM.UL.IHARQProcessNum);
    m_Str2ParaMap_DL["MIMO_CTRL.IMaxLayerNum_BS"] = &(MIMO_CTRL.IMaxLayerNum_BS);
    m_Str2ParaMap_DL["MIMO_CTRL.PMI_based_Precoding"] = &(MIMO_CTRL.PMI_based_Precoding);
    
    //chty add
    m_Str2ParaMap_DL["BASIC.ifcoexist"] = &(BASIC.ifcoexist);
    m_Str2ParaMap_DL["BASIC.type1MSRate"] = &(BASIC.type1MSRate);
    m_Str2ParaMap_DL["BASIC.type1BandRate"] = &(BASIC.type1BandRate);
    m_Str2ParaMap_DL["TRAFFIC.DPacketSize_2"] = &(TRAFFIC.DPacketSize_2);
    m_Str2ParaMap_DL["TRAFFIC.DMacroLamda_2"] = &(TRAFFIC.DMacroLamda_2);
    
    m_Str2ParaMap_DL["MSS.FirstBand.DV3DBBeamWidthDeg_2"] = &(MSS.FirstBand.DV3DBBeamWidthDeg_2);
    m_Str2ParaMap_DL["MSS.FirstBand.DH3DBBeamWidthDeg_2"] = &(MSS.FirstBand.DH3DBBeamWidthDeg_2);
    m_Str2ParaMap_DL["MSS.FirstBand.DHAntSpace_2"] = &(MSS.FirstBand.DHAntSpace_2);
    m_Str2ParaMap_DL["MSS.FirstBand.DVAntSpace_2"] = &(MSS.FirstBand.DVAntSpace_2);
    m_Str2ParaMap_DL["MSS.FirstBand.IVAntNumPerPanel_2"] = &(MSS.FirstBand.IVAntNumPerPanel_2);
    m_Str2ParaMap_DL["MSS.FirstBand.IHAntNumPerPanel_2"] = &(MSS.FirstBand.IHAntNumPerPanel_2);
    m_Str2ParaMap_DL["MSS.FirstBand.IVPanelNum_2"] = &(MSS.FirstBand.IVPanelNum_2);
    m_Str2ParaMap_DL["MSS.FirstBand.IHPanelNum_2"] = &(MSS.FirstBand.IHPanelNum_2);
    m_Str2ParaMap_DL["MSS.FirstBand.Polarize_Num_2"] = &(MSS.FirstBand.Polarize_Num_2);
    m_Str2ParaMap_DL["MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel_2"] = &(MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel_2);
    m_Str2ParaMap_DL["MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel_2"] = &(MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel_2);
    m_Str2ParaMap_DL["MSS.FirstBand.DAntennaGainDb_2"] = &(MSS.FirstBand.DAntennaGainDb_2);

    
    m_Str2ParaMap_DL["MSS.FirstBand.DV3DBBeamWidthDeg"] = &(MSS.FirstBand.DV3DBBeamWidthDeg);
    m_Str2ParaMap_DL["MSS.FirstBand.DH3DBBeamWidthDeg"] = &(MSS.FirstBand.DH3DBBeamWidthDeg);
    m_Str2ParaMap_DL["MSS.FirstBand.DHAntSpace"] = &(MSS.FirstBand.DHAntSpace);
    m_Str2ParaMap_DL["MSS.FirstBand.DVAntSpace"] = &(MSS.FirstBand.DVAntSpace);
    m_Str2ParaMap_DL["MSS.FirstBand.IVAntNumPerPanel"] = &(MSS.FirstBand.IVAntNumPerPanel);
    m_Str2ParaMap_DL["MSS.FirstBand.IHAntNumPerPanel"] = &(MSS.FirstBand.IHAntNumPerPanel);
    m_Str2ParaMap_DL["MSS.FirstBand.IVPanelNum"] = &(MSS.FirstBand.IVPanelNum);
    m_Str2ParaMap_DL["MSS.FirstBand.IHPanelNum"] = &(MSS.FirstBand.IHPanelNum);
    m_Str2ParaMap_DL["MSS.FirstBand.Polarize_Num"] = &(MSS.FirstBand.Polarize_Num);
    m_Str2ParaMap_DL["MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel"] = &(MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel);
    m_Str2ParaMap_DL["MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel"] = &(MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel);
    m_Str2ParaMap_DL["MSS.FirstBand.DAntennaGainDb"] = &(MSS.FirstBand.DAntennaGainDb);
    
    m_Str2ParaMap_DL["MSS.FirstBand.DHBackLossDB"] = &(MSS.FirstBand.DHBackLossDB);
    m_Str2ParaMap_DL["MSS.FirstBand.DVBackLossDB"] = &(MSS.FirstBand.DVBackLossDB);
    m_Str2ParaMap_DL["MSS.DAntennaHeightM"] = &(MSS.DAntennaHeightM);
    m_Str2ParaMap_DL["MSS.IMinBuildingFloor"] = &(MSS.IMinBuildingFloor);
    m_Str2ParaMap_DL["MSS.IMaxBuildingFloor"] = &(MSS.IMaxBuildingFloor);
    
    m_Str2ParaMap_DL["BASIC.IBTSPerBS"] = &(BASIC.IBTSPerBS);
    m_Str2ParaMap_DL["BASIC.INumBSs"] = &(BASIC.INumBSs);
    m_Str2ParaMap_DL["BASIC.IMacroTopology"] = &(BASIC.IMacroTopology);
    m_Str2ParaMap_DL["BASIC.IMsDistributeMode"] = &(BASIC.IMsDistributeMode);
    m_Str2ParaMap_DL["Macro.DPenetrationLossdB"] = &(Macro.DPenetrationLossdB);
    m_Str2ParaMap_DL["Macro.DTransmissionLineLoss_dB"] = &(Macro.DTransmissionLineLoss_dB);
    m_Str2ParaMap_DL["Macro.LINK.DRadioFrequencyMHz"] = &(Macro.LINK.DRadioFrequencyMHz);
    m_Str2ParaMap_DL["Macro.IAntennaPatternMode"] = &(Macro.IAntennaPatternMode);
    m_Str2ParaMap_DL["Macro.DV3DBBeamWidthDeg"] = &(Macro.DV3DBBeamWidthDeg);
    m_Str2ParaMap_DL["Macro.DH3DBBeamWidthDeg"] = &(Macro.DH3DBBeamWidthDeg);
    m_Str2ParaMap_DL["Macro.DAntennaGainDb"] = &(Macro.DAntennaGainDb);
    m_Str2ParaMap_DL["Macro.DHBackLossDB"] = &(Macro.DHBackLossDB);
    m_Str2ParaMap_DL["Macro.DVBackLossDB"] = &(Macro.DVBackLossDB);
    m_Str2ParaMap_DL["Macro.DL.DMaxTxPowerDbm"] = &(Macro.DL.DMaxTxPowerDbm);
    m_Str2ParaMap_DL["Macro.DAntennaHeightM"] = &(Macro.DAntennaHeightM);
    m_Str2ParaMap_DL["Macro.DSiteDistanceM"] = &(Macro.DSiteDistanceM);
    m_Str2ParaMap_DL["Macro.DMechanicalTiltDeg"] = &(Macro.DMechanicalTiltDeg);
    m_Str2ParaMap_DL["Macro.DElectricalTiltDeg"] = &(Macro.DElectricalTiltDeg);
    m_Str2ParaMap_DL["Macro.DVAntSpace"] = &(Macro.DVAntSpace);
    m_Str2ParaMap_DL["Macro.DHAntSpace"] = &(Macro.DHAntSpace);
    m_Str2ParaMap_DL["Macro.UL.DNoiseFigureDb"] = &(Macro.UL.DNoiseFigureDb);
    m_Str2ParaMap_DL["Macro.IVAntNumPerPanel"] = &(Macro.IVAntNumPerPanel);
    m_Str2ParaMap_DL["Macro.IHAntNumPerPanel"] = &(Macro.IHAntNumPerPanel);
    m_Str2ParaMap_DL["Macro.Polarize_Num"] = &(Macro.Polarize_Num);
    m_Str2ParaMap_DL["Macro.IVPanelNum"] = &(Macro.IVPanelNum);
    m_Str2ParaMap_DL["Macro.IHPanelNum"] = &(Macro.IHPanelNum);
    m_Str2ParaMap_DL["Macro.V_TXRU_DIV_NUM_PerPanel"] = &(Macro.V_TXRU_DIV_NUM_PerPanel);
    m_Str2ParaMap_DL["Macro.H_TXRU_DIV_NUM_PerPanel"] = &(Macro.H_TXRU_DIV_NUM_PerPanel);
    m_Str2ParaMap_DL["Macro.LINK.DMinDistanceM"] = &(Macro.LINK.DMinDistanceM);
    m_Str2ParaMap_DL["Macro.DL.bEnable_BSAnalogBF"] = &(Macro.DL.bEnable_BSAnalogBF);
    m_Str2ParaMap_DL["Macro.DL.IschedulerthreadNum"] = &(Macro.DL.IschedulerthreadNum);
    m_Str2ParaMap_DL["MSS.FirstBand.bEnable_UEAnalogBF"] = &(MSS.FirstBand.bEnable_UEAnalogBF);
    m_Str2ParaMap_DL["MSS.DProbOutdoor"] = &(MSS.DProbOutdoor);
    m_Str2ParaMap_DL["MSS.bIsOutdoorInCar_notOnCar"] = &(MSS.bIsOutdoorInCar_notOnCar);
    m_Str2ParaMap_DL["MSS.DCarSpeedSacleFactor"] = &(MSS.DCarSpeedSacleFactor);
    m_Str2ParaMap_DL["MSS.DProbLowloss"] = &(MSS.DProbLowloss);
    m_Str2ParaMap_DL["MSS.FirstBand.IAntennaPatternMode"] = &(MSS.FirstBand.IAntennaPatternMode);
    m_Str2ParaMap_DL["MSS.FirstBand.DL.DNoiseFigureDb"] = &(MSS.FirstBand.DL.DNoiseFigureDb);
    m_Str2ParaMap_DL["ERROR.IChannelMatrixError"] = &(ERROR.IChannelMatrixError);
    m_Str2ParaMap_DL["ERROR.UL.IChannelMatrixError"] = &(ERROR.UL.IChannelMatrixError);
    m_Str2ParaMap_DL["OVERHEAD.DMRS_PRB"] = &(OVERHEAD.DMRS_PRB);
    m_Str2ParaMap_DL["OVERHEAD.OH_PRB"] = &(OVERHEAD.OH_PRB);
    m_Str2ParaMap_DL["OVERHEAD.DMRS_Type"] = &(OVERHEAD.DMRS_Type);
    m_Str2ParaMap_DL["OVERHEAD.DMRS_Symbol"] = &(OVERHEAD.DMRS_Symbol);
    m_Str2ParaMap_DL["OVERHEAD.DL.IPDCCHNum"] = &(OVERHEAD.DL.IPDCCHNum);
    m_Str2ParaMap_DL["OVERHEAD.DL.ICSIRSNum"] = &(OVERHEAD.DL.ICSIRSNum);
    m_Str2ParaMap_DL["OVERHEAD.DL.IDMRSNum"] = &(OVERHEAD.DL.IDMRSNum);
    m_Str2ParaMap_DL["OVERHEAD.DL.ITRSNum"] = &(OVERHEAD.DL.ITRSNum);
    m_Str2ParaMap_DL["OVERHEAD.DL.ISSBNum"] = &(OVERHEAD.DL.ISSBNum);
    m_Str2ParaMap_DL["OVERHEAD.UL.IPUCCHNum"] = &(OVERHEAD.UL.IPUCCHNum);
    m_Str2ParaMap_DL["OVERHEAD.UL.ISRSNum"] = &(OVERHEAD.UL.ISRSNum);
    m_Str2ParaMap_DL["OVERHEAD.UL.IDMRSNum"] = &(OVERHEAD.UL.IDMRSNum);
    m_Str2ParaMap_DL["OVERHEAD.UL.IPRACHNum"] = &(OVERHEAD.UL.IPRACHNum);
    m_Str2ParaMap_DL["BASIC.IMaxModulLevelUL"] = &(BASIC.IMaxModulLevelUL);
    m_Str2ParaMap_DL["SIM.IChannelInterpolationSpace"] = &(SIM.IChannelInterpolationSpace);
    m_Str2ParaMap_DL["Macro.ILOSType"] = &(Macro.ILOSType);
    m_Str2ParaMap_DL["SIM.BIsRankAdaptive"] = &(SIM.IsRankAdaptive);
    //信道估计插值间隔
    if(SIM.IChannelInterpolationSpace != 0){
        LINK_CTRL.IFrequencySpace = SIM.IChannelInterpolationSpace;
    }

    //AMC参数
    m_Str2ParaMap_DL["AMC.SelfConfig"] = &(AMC.SelfConfigOlla);
    m_Str2ParaMap_DL["AMC.ACKSchedWindow"] = &(AMC.ACKSchedWindow);
    m_Str2ParaMap_DL["AMC.NACKSchedWindow"] = &(AMC.NACKSchedWindow);
    m_Str2ParaMap_DL["AMC.ACKThred"] = &(AMC.ACKThred);
    m_Str2ParaMap_DL["AMC.NACKThred"] = &(AMC.NACKThred);
    m_Str2ParaMap_DL["AMC.IinitialSINR"] = &(AMC.IinitialSINR);
    m_Str2ParaMap_DL["AMC.SINRUpStep"] = &(AMC.SINRUpStep);
    m_Str2ParaMap_DL["AMC.SINRDownStep"] = &(AMC.SINRDownStep);
    m_Str2ParaMap_DL["AMC.CQIFilterFactor"] = &(AMC.CQIFilterFactor);

    //DRX参数
    m_Str2ParaMap_DL["DRX.Switch"] = &(DRX.Switch);
    m_Str2ParaMap_DL["DRX.LongCycle"] = &(DRX.LongCycle);
    m_Str2ParaMap_DL["DRX.OnDurationTimer"] = &(DRX.OnDurationTimer);
    m_Str2ParaMap_DL["DRX.InactivityTimer"] = &(DRX.InactivityTimer);
    m_Str2ParaMap_DL["DRX.ShortSwitch"] = &(DRX.ShortSwitch);
    m_Str2ParaMap_DL["DRX.ShortCycle"] = &(DRX.ShortCycle);
    m_Str2ParaMap_DL["DRX.ShortCycleTimer"] = &(DRX.ShortCycleTimer);

    m_Str2ParaMap_DL["CSI_data_set_on"] = &(CSI_data_set_on);
    m_Str2ParaMap_DL["AI_model_on"] = &(AI_model_on);
    m_Str2ParaMap_DL["SubBand_Size"] = &(SubBand_Size);
    
}

///将所有的参数及其值输出到文件中
///＠para _pFilename 指向当前路径的文件

//没用

void Parameters::RecordParameters(char const* _pFilename) {
    boost::filesystem::path filename;
    filename = Directory::Instance().GetPath(_pFilename);
    boost::filesystem::ofstream f(filename);
    //assert(f);
    int width = 30;
    f << setw(width) << "ParameterName" << setw(width) << "Value" << setw(width) << endl;
    for (auto& it : m_Str2ParaMap_DL) {
        f << setw(width) << it.first; ///记录的参数名///(*it).second是该参数的地址*any_cast<int*>((*it).second)将地址中记录的数值为转化成相应类型
        if (it.second.type() == typeid (int *)) {
            f << setw(width) << *any_cast<int*>(it.second) << endl;
            continue;
        } else if (it.second.type() == typeid (double *)) {
            f << setw(width) << *any_cast<double*>(it.second) << endl;
            continue;
        } else if (it.second.type() == typeid (float *)) {
            f << setw(width) << *any_cast<float*>(it.second) << endl;
            continue;
        } else if (it.second.type() == typeid (bool *)) {
            f << setw(width) << *any_cast<bool*>(it.second) << endl;
            continue;
        } else if (it.second.type() == typeid (string *)) {
            f << setw(width) << *any_cast<string*>(it.second) << endl;
            continue;
        } else {
            assert(false);
        }
    }
}

void Parameters::ParametersPassToDLFromUL()
{

}

//计算一些导出参数

void Parameters::CalculateDerivedParameters_DL() {
//    if (BASIC.IMacroTopology == 3) {
//        BASIC.INumBSs = 12; //INH
//    } else if (BASIC.IMacroTopology == 0) {
//        //BASIC.INumBSs = 7; //outdoor 7cell
//    } else if (BASIC.IMacroTopology == 1) {
//        BASIC.INumBSs = 19; //outdoor 19cell
//    }
    BASIC.ISBSize = 1;
    Parameters::CalNRE();

    switch (BASIC.IMaxModulLevel) {
        case 2:
            BASIC.NRMCSNumDL = 5;
            break;
        case 4:
            BASIC.NRMCSNumDL = 11;
            break;
        case 6:
            BASIC.NRMCSNumDL = 20;
            break;
        case 8:
            BASIC.NRMCSNumDL = 28;
            break;
        default:
            assert(false);
            break;
    }

    /*BTS.DCellRadius = BTS.DSiteDistance /M_SQRT3;*/
    Macro.IHTotalAntNum = Macro.IHAntNumPerPanel * Macro.IHPanelNum * Macro.Polarize_Num;
    Macro.ITotalAntNum = Macro.IHAntNumPerPanel * Macro.IHPanelNum * Macro.IVAntNumPerPanel * Macro.IVPanelNum * Macro.Polarize_Num;
    //    MSS.FirstBand.ITotalAntNumPerPanel = MSS.FirstBand.IHAntNumPerPanel * MSS.FirstBand.IVAntNumPerPanel * (Macro.IPolarize == 1 ? 2 : 1);

    //20171129
    MSS.FirstBand.ITotalAntNumPerPanel = MSS.FirstBand.IHAntNumPerPanel * MSS.FirstBand.IVAntNumPerPanel * MSS.FirstBand.Polarize_Num;

    MSS.FirstBand.ITotalAntNum = MSS.FirstBand.ITotalAntNumPerPanel * MSS.FirstBand.IHPanelNum * MSS.FirstBand.IVPanelNum;

    Macro.DCellRadiusM = Macro.DSiteDistanceM / M_SQRT3;
    Macro.LINK.DWaveLength = (3e8) / (Macro.LINK.DRadioFrequencyMHz * 1e6);
    SmallCell.LINK.DWaveLength = (3e8) / (SmallCell.LINK.DRadioFrequencyMHz * 1e6);
    Macro.DL.DMaxTxPowerMw = DB2L(Macro.DL.DMaxTxPowerDbm);
    BASIC.ITotalMSNum = floor(BASIC.DNumMSPerBTS * BASIC.IBTSPerBS * BASIC.INumBSs);


    int SCSFactor = pow(2, BASIC.ISCSConfig); //取值范围0，1，2
    BASIC.DCarrierWidthHz = 15000 * SCSFactor;
    BASIC.DBandWidthKHz = 10000 * SCSFactor;
    BASIC.DSampleRateHz = 15360000 * SCSFactor;
    BASIC.DSlotDuration_ms = 1.0 / SCSFactor;
    BASIC.ISlotNumPerSFN = SCSFactor*10;
    MSS_UL.UL.ProhibitPeriod *= SCSFactor;
    MSS_UL.UL.PeriodicPeriod *= SCSFactor;

    //一个无线帧中有多少个时隙 10ms * slotperms
    BASIC.iFrameNum = 10 / BASIC.DSlotDuration_ms;

    double dTotal_RB_20MHz_LTE = 100 * (BASIC.DBandWidthKHz / 20000) * (15000 / BASIC.DCarrierWidthHz);
    double dTotal_RB4NRStatBW = CalcPRBNum(BASIC.DSystemBandWidthKHz, BASIC.DCarrierWidthHz);
    //20MHz
    double dScalingfactor = BASIC.DSystemBandWidthKHz / 20000.0;

    BASIC.DGuardBand = dTotal_RB4NRStatBW / (dTotal_RB_20MHz_LTE * dScalingfactor);

    int SCS_Config = 0;
    switch (static_cast<int> (BASIC.DCarrierWidthHz)) {
        case 15000:
            SCS_Config = 0;
            break;
        case 30000:
            SCS_Config = 1;
            break;
        case 60000:
            SCS_Config = 2;
            break;
        default:
            assert(false);
            break;
    }


    BASIC.IRBSize = 12;
    BASIC.IRBNum = CalcPRBNum(BASIC.DSystemBandWidthKHz, BASIC.DCarrierWidthHz);

    BASIC.ISBNum = BASIC.IRBNum / BASIC.ISBSize;
    BASIC.ISCNum = BASIC.ISBNum *BASIC.ISBSize* BASIC.IRBSize;

    MSS.FirstBand.DL.DNoisePowerMw
            = DB2L(
            BASIC.DNoisePowerSpectrumDensityDbmHz
            + MSS.FirstBand.DL.DNoiseFigureDb)
            * BASIC.DCarrierWidthHz * BASIC.ISCNum;

    int CSI_RS_PeriodInSlot = 10; //10个slot
    int CSI_RS_PortNum = 32; //8*4port
    int TRS_PeriodInMS = 20; //20ms
    int TRS_BWInPRB = 50; //50
    double TRSDensity = 1.0 / 3.0; // 1/3
    int SSB_PeriodInMS = 20; //20ms
    int SSB_Num = 8; //8
    int SSB_BWInPRB = 20; //20
    int PDCCH_BWInPRB = 51; //51PRB for 30KHz SCS
    int PDCCH_OSNum = 2; //2

    //先计算10ms之内的下行开销情况   
    int FSNumIn10ms = 10 * pow(2, SCS_Config) / SIM.FrameStructure4Statis.Get_PeriodInSlot();
    int DLOSNumIn10ms = FSNumIn10ms * SIM.FrameStructure4Statis.Get_DLOSNum();
    int DLRENumIn10ms = DLOSNumIn10ms * BASIC.IRBSize * dTotal_RB4NRStatBW;

    int CSI_RSNumIn10ms = pow(2, SCS_Config) * CSI_RS_PortNum * dTotal_RB4NRStatBW;
    int TRS_NumIn10ms = 1.0 * 10 / TRS_PeriodInMS * 2 * 2 / TRSDensity * TRS_BWInPRB;
    int SSB_NumIn10ms = 1.0 * 10 / SSB_PeriodInMS * SSB_Num * SSB_BWInPRB * BASIC.IRBSize;
    PDCCH_BWInPRB = CalcPRBNum(20000, BASIC.DCarrierWidthHz);
    int PDCCH_NumIn10ms = SIM.FrameStructure4Statis.Get_DLandSpecialSlotNum() * PDCCH_OSNum * BASIC.IRBSize * PDCCH_BWInPRB; //problem

    BASIC.DTotalFixedOH = 1.0 * (CSI_RSNumIn10ms + TRS_NumIn10ms + SSB_NumIn10ms + PDCCH_NumIn10ms) / DLRENumIn10ms;

    Macro.UL.DNoisePowerMw = DB2L(BASIC.DNoisePowerSpectrumDensityDbmHz + Macro.UL.DNoiseFigureDb) * BASIC.DCarrierWidthHz * BASIC.ISCNum;

    Macro.DL.DSCTxPowerMw = Macro.DL.DMaxTxPowerMw / BASIC.ISCNum;
    MSS.FirstBand.DL.DSCNoisePowerMw = MSS.FirstBand.DL.DNoisePowerMw / BASIC.ISCNum;
    Macro.UL.DSCNoisePowerMw = Macro.UL.DNoisePowerMw / BASIC.ISCNum;

    //    SIM.IFrameLength = SIM.DL.ISubframeNum + SIM.UL.ISubframeNum;
    //    SIM.BFDD = (SIM.DL.ISubframeNum * SIM.UL.ISubframeNum == 0);
    //    SIM.BTDD = !SIM.BFDD;
    ///@xlong   根据部署场景确定仿真的总用户数
    BASIC.ITotalMacroNum = BASIC.IBTSPerBS * BASIC.INumBSs;
    if (BASIC.IScenarioModel == Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER) {
        switch (SmallCell.LINK.IHetnetEnvironmentType) {
            case IHetnet_Normal:
                SmallCell.LINK.IPicoNumPerMacro = SmallCell.LINK.IPicoNumPerMacroNormal;
                BASIC.ITotalMSNum = floor(SmallCell.LINK.IPicoUENumPerCellNormal * BASIC.IBTSPerBS * BASIC.INumBSs);
                BASIC.ITotalBTSNumPerBS = floor((1 + SmallCell.LINK.IPicoNumPerMacroNormal) * BASIC.IBTSPerBS);
                break;
            case IHetnet_4b:
                SmallCell.LINK.IPicoNumPerMacro = SmallCell.LINK.IPicoNumPerMacro4b;
                BASIC.ITotalMSNum = floor(SmallCell.LINK.IPicoUENumPerCell4b * BASIC.IBTSPerBS * BASIC.INumBSs);
                BASIC.ITotalBTSNumPerBS = floor((1 + SmallCell.LINK.IPicoNumPerMacro4b) * BASIC.IBTSPerBS);
                break;
            case IHetnet_Cluster:
                SmallCell.LINK.IPicoNumPerMacro = SmallCell.LINK.IPicoNumPerCluster_Cluster * SmallCell.LINK.IPicoClusterNumPerMacro_Cluster;
                BASIC.ITotalMSNum = floor(SmallCell.LINK.IUENumPerCell_Cluster * BASIC.IBTSPerBS * BASIC.INumBSs);
                BASIC.ITotalBTSNumPerBS = floor((1 + SmallCell.LINK.IPicoNumPerMacro) * BASIC.IBTSPerBS);
                break;
            default:
                assert(false);
                break;
        }
    } else {
        SmallCell.LINK.IPicoNumPerMacro = 0;
        BASIC.ITotalMSNum = floor(BASIC.DNumMSPerBTS * BASIC.IBTSPerBS * BASIC.INumBSs);
        BASIC.ITotalBTSNumPerBS = BASIC.IBTSPerBS;
    }
//    BASIC.ITotalMSNum = 793;
    ///@xlong
    SmallCell.LINK.DPicoOutMaxTxPowerMw = DB2L(SmallCell.LINK.DPicoMaxTxPowerDbm);
    ///@xlong
    SmallCell.LINK.DPicoOutSCTxPowerMw = SmallCell.LINK.DPicoOutMaxTxPowerMw / BASIC.ISCNum;

    bool bFrequency_Above6GHz = (Macro.LINK.DRadioFrequencyMHz / 6000 > 1);
    if (bFrequency_Above6GHz == 1) {
        if (BASIC.IScenarioModel == SCENARIO_HighFreq_URBAN_MICRO) {
            Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 4;
            Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 16;
            Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = -22.5; //-pi/8
            Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = -56.25; //-5*pi/16
            Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 15.0; //pi/12
            Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 7.5; //pi/24

            Macro.ANALOGBEAM_CONFIG.iVUEBeamNum = 2;
            Macro.ANALOGBEAM_CONFIG.iHUEBeamNum = 4;
            Macro.ANALOGBEAM_CONFIG.dStartVUEBeamDeg = -45; //45;//error -45;
            Macro.ANALOGBEAM_CONFIG.dStartHUEBeamDeg = -67.5;
            Macro.ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg = 90;
            Macro.ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg = 45;
        } else if (BASIC.IScenarioModel == SCENARIO_HighFreq_URBAN_MACRO) {
            //            
            //            Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 4;
            //            Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 16;
            //            Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = 22.5;//pi/8
            //            Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = -56.25;//-5*pi/16
            //            Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 15.0;//pi/12
            //            Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 7.5;//pi/24
            //for 5GUMA
            Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 2;
            Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 6;
            Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = 22.5; //112.5;
            Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = -56.25;
            Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 45;
            Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 22.5;

            Macro.ANALOGBEAM_CONFIG.iVUEBeamNum = 2;
            Macro.ANALOGBEAM_CONFIG.iHUEBeamNum = 4;
            Macro.ANALOGBEAM_CONFIG.dStartVUEBeamDeg = -45; //45;//error -45;
            Macro.ANALOGBEAM_CONFIG.dStartHUEBeamDeg = -67.5;
            Macro.ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg = 90;
            Macro.ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg = 45;
        } else if (BASIC.IScenarioModel == SCENARIO_HighFreq_INDOOR) {
            Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 2;
            Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 4;
            Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = -45; //-67.5;
            Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = -67.5; //-78.75;
            Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 90;
            Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 45;

            Macro.ANALOGBEAM_CONFIG.iVUEBeamNum = 2;
            Macro.ANALOGBEAM_CONFIG.iHUEBeamNum = 4;
            Macro.ANALOGBEAM_CONFIG.dStartVUEBeamDeg = -45; //45;//error -45;
            Macro.ANALOGBEAM_CONFIG.dStartHUEBeamDeg = -67.5;
            Macro.ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg = 90;
            Macro.ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg = 45;
        }
    } else if (bFrequency_Above6GHz == 0) {
        if (BASIC.IScenarioModel == SCENARIO_LowFreq_URBAN_MICRO) {
            //            ANALOGBEAM_CONFIG.iVBSBeamNum = 4;
            //            ANALOGBEAM_CONFIG.iHBSBeamNum = 6;
            //            ANALOGBEAM_CONFIG.dStartVBSBeamDeg = -18;
            //            ANALOGBEAM_CONFIG.dStartHBSBeamDeg = -56.25;
            //            ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 22.5;
            //            ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 22.5;
            //            ANALOGBEAM_CONFIG.iVUEBeamNum = 1;
            //            ANALOGBEAM_CONFIG.iHUEBeamNum = 1;
            //            ANALOGBEAM_CONFIG.dStartVUEBeamDeg = -45;
            //            ANALOGBEAM_CONFIG.dStartHUEBeamDeg = -67.5;
            //            ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg = 90;
            //            ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg = 45;
        } else if (BASIC.IScenarioModel == SCENARIO_5G_DENSE_URBAN_1LAYER) {
            if (Macro.IHPanelNum == 1) {//8811
                Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 4;
                Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 6;
                Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = 11.25;
                Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = -56.25;
                Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 22.5;
                Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 22.5;
            } else if (Macro.IHPanelNum == 8) {//8118仅作校准
                Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 3;
                Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = 11.25;
                Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 22.5;
                Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 22.5;
            }
            Macro.ANALOGBEAM_CONFIG.iVUEBeamNum = 1;
            Macro.ANALOGBEAM_CONFIG.iHUEBeamNum = 1;
            Macro.ANALOGBEAM_CONFIG.dStartVUEBeamDeg = -45;
            Macro.ANALOGBEAM_CONFIG.dStartHUEBeamDeg = -67.5;
            Macro.ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg = 90;
            Macro.ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg = 45;

        } else if (BASIC.IScenarioModel == SCENARIO_LowFreq_INDOOR) {
            Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 1;
            Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 1;
            Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = Macro.DElectricalTiltDeg;
            Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = 0;
            Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 0;
            Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 0;

            Macro.ANALOGBEAM_CONFIG.iVUEBeamNum = 1;
            Macro.ANALOGBEAM_CONFIG.iHUEBeamNum = 1;
            Macro.ANALOGBEAM_CONFIG.dStartVUEBeamDeg = 0;
            Macro.ANALOGBEAM_CONFIG.dStartHUEBeamDeg = 0;
            Macro.ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg = 0;
            Macro.ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg = 0;

        } else if (BASIC.IScenarioModel == SCENARIO_LowFreq_URBAN_MACRO) {
            if (BASIC.ISubScenarioModel ==
                    Parameters::SCENARIO_Phase1_DenseUrban_4G) {
                Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 2;
                Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 6;
                Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = -22.5; //112.5;
                Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = -56.25;
                Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 45;
                Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 22.5;

                Macro.ANALOGBEAM_CONFIG.iVUEBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.iHUEBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.dStartVUEBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dStartHUEBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg = 0;
            } else if (BASIC.ISubScenarioModel == Parameters::SCENARIO_Phase1_UMA_URLLC
                    || BASIC.ISubScenarioModel == Parameters::SCENARIO_Phase1_UMA_mMTC
                    || BASIC.ISubScenarioModel == Parameters::SCENARIO_Phase2_UMA_2dot6G
                    || BASIC.ISubScenarioModel == Parameters::SCENARIO_Phase2_UMA_3dot5G
                    || BASIC.ISubScenarioModel == Parameters::SCENARIO_Phase2_DenseUrban) {
                Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = Macro.DElectricalTiltDeg;
                Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 0;

                Macro.ANALOGBEAM_CONFIG.iVUEBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.iHUEBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.dStartVUEBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dStartHUEBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg = 0;
            } else {
                Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = Macro.DElectricalTiltDeg;
                Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 0;

                Macro.ANALOGBEAM_CONFIG.iVUEBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.iHUEBeamNum = 1;
                Macro.ANALOGBEAM_CONFIG.dStartVUEBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dStartHUEBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg = 0;
                Macro.ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg = 0;
            }
        } else if (BASIC.IScenarioModel == Parameters::SCENARIO_LowFreq_RURAL_MACRO) {
            Macro.ANALOGBEAM_CONFIG.iVBSBeamNum = 1;
            Macro.ANALOGBEAM_CONFIG.iHBSBeamNum = 1;
            Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg = Macro.DElectricalTiltDeg;
            Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg = 0;
            Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg = 0;
            Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg = 0;

            Macro.ANALOGBEAM_CONFIG.iVUEBeamNum = 1;
            Macro.ANALOGBEAM_CONFIG.iHUEBeamNum = 1;
            Macro.ANALOGBEAM_CONFIG.dStartVUEBeamDeg = 0;
            Macro.ANALOGBEAM_CONFIG.dStartHUEBeamDeg = 0;
            Macro.ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg = 0;
            Macro.ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg = 0;
        }
    }
    for (int i = 0; i < Macro.ANALOGBEAM_CONFIG.iVBSBeamNum; i++) {
        double dBSetiltRAD = cm::DEG2RAD(Macro.ANALOGBEAM_CONFIG.dStartVBSBeamDeg + i * Macro.ANALOGBEAM_CONFIG.dSpaceVBSBeamDeg);
        Macro.ANALOGBEAM_CONFIG.vBSetiltRAD.push_back(dBSetiltRAD);
    }
    for (int j = 0; j < Macro.ANALOGBEAM_CONFIG.iHBSBeamNum; j++) {
        double dBSescanRAD = cm::DEG2RAD(Macro.ANALOGBEAM_CONFIG.dStartHBSBeamDeg + j * Macro.ANALOGBEAM_CONFIG.dSpaceHBSBeamDeg);
        Macro.ANALOGBEAM_CONFIG.vBSescanRAD.push_back(dBSescanRAD);
    }

    for (int i = 0; i < Macro.ANALOGBEAM_CONFIG.iVUEBeamNum; i++) {
        double dUEetiltRAD = cm::DEG2RAD(Macro.ANALOGBEAM_CONFIG.dStartVUEBeamDeg + i * Macro.ANALOGBEAM_CONFIG.dSpaceVUEBeamDeg);
        Macro.ANALOGBEAM_CONFIG.vUEetiltRAD.push_back(dUEetiltRAD);
    }
    for (int j = 0; j < Macro.ANALOGBEAM_CONFIG.iHUEBeamNum; j++) {
        double dUEescanRAD = cm::DEG2RAD(Macro.ANALOGBEAM_CONFIG.dStartHUEBeamDeg + j * Macro.ANALOGBEAM_CONFIG.dSpaceHUEBeamDeg);
        Macro.ANALOGBEAM_CONFIG.vUEescanRAD.push_back(dUEescanRAD);
    }

    XR.iDelayBudget_slot = XR.dDelayBuget_ms / BASIC.DSlotDuration_ms;

    MIMO_CTRL.Total_TXRU_Num = Macro.H_TXRU_DIV_NUM_PerPanel * Macro.V_TXRU_DIV_NUM_PerPanel * Macro.Polarize_Num;
    MSS.FirstBand.Total_TXRU_Num = MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel * MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel * MSS.FirstBand.Polarize_Num;

    ///目前平台BASIC.DSystemBandWidthKHz是系统带宽，也是实际使用带宽;没有BASIC.DBandWidthKHz的说法；
    ///因此先将二者等效
    BASIC.DBandWidthKHz = BASIC.DSystemBandWidthKHz;

    Macro.DL.DMaxTxPowerDbm = Macro.DL.DMaxTxPowerDbm - (10 * log10(BASIC.DSystemBandWidthKHz / BASIC.DBandWidthKHz));

    assert(SIM.FrameStructure4Sim.P1.IDLOS > 2 || SIM.FrameStructure4Sim.P1.IDLOS == 0);

    SIM.FrameStructure4Sim.P1.ISpecialSlot = SIM.FrameStructure4Sim.P1.IPeriodSlot - SIM.FrameStructure4Sim.P1.IDLSlot - SIM.FrameStructure4Sim.P1.IULSlot;
    SIM.FrameStructure4Sim.P2.ISpecialSlot = SIM.FrameStructure4Sim.P2.IPeriodSlot - SIM.FrameStructure4Sim.P2.IDLSlot - SIM.FrameStructure4Sim.P2.IULSlot;

    SIM.FrameStructure4Statis.P1.ISpecialSlot = SIM.FrameStructure4Statis.P1.IPeriodSlot - SIM.FrameStructure4Statis.P1.IDLSlot - SIM.FrameStructure4Statis.P1.IULSlot;
    SIM.FrameStructure4Statis.P2.ISpecialSlot = SIM.FrameStructure4Statis.P2.IPeriodSlot - SIM.FrameStructure4Statis.P2.IDLSlot - SIM.FrameStructure4Statis.P2.IULSlot;
}

void Parameters::Set_HighFreq_InF() {
    //下面这些参数还没有改
    {
        BASIC.IBTSPerBS = 1; // 3 for 36 TRP ,1for 12 TRP
        BASIC.INumerologyIndicator = 4;
        BASIC.INumBSs = 12;
        BASIC.IMacroTopology = 3;
        BASIC.IMsDistributeMode = 0;
    }
    {
        Macro.IAntennaPatternMode = 1;
        Macro.DV3DBBeamWidthDeg = 90;
        Macro.DH3DBBeamWidthDeg = 90;
        Macro.DAntennaGainDb = 5;
        //        Macro.DL.DMaxTxPowerDbm = 20;//23 for 30GNRMIMO;20 for ITU
        Macro.DAntennaHeightM = 3;
        Macro.DSiteDistanceM = 20;
        Macro.DMechanicalTiltDeg = 90; //20 for 36TRP ,90 for 12TRP
        Macro.DElectricalTiltDeg = 0;
        Macro.DVAntSpace = 0.5;
        Macro.DHAntSpace = 0.5;
        Macro.UL.DNoiseFigureDb = 7;
        Macro.IVAntNumPerPanel = 2; //4;
        Macro.IHAntNumPerPanel = 4; //8;
        Macro.IVPanelNum = 2; //1;
        Macro.IHPanelNum = 2; //1;
        Macro.DHBackLossDB = -25;
        Macro.DVBackLossDB = -25;
        Macro.LINK.DMinDistanceM = 0; //原1.5;
        Macro.LINK.DRadioFrequencyMHz = 30000;
        Macro.DL.bEnable_BSAnalogBF = true;
    }
    {
        MSS.DProbOutdoor = 0;
        // 1: Outdoor is In Car; 0 : Outdoor is not In Car
        MSS.DAntennaHeightM = 1.5;
        MSS.FirstBand.bEnable_UEAnalogBF = true;
        MSS.FirstBand.IAntennaPatternMode = 1;
        MSS.FirstBand.DV3DBBeamWidthDeg = 90; //ADD
        MSS.FirstBand.DH3DBBeamWidthDeg = 90; //ADD
        MSS.FirstBand.DAntennaGainDb = 5;
        MSS.FirstBand.DHAntSpace = 0.5;
        MSS.FirstBand.DVAntSpace = 0.5; //ADD
        MSS.FirstBand.UL.DMaxTxPowerDbm = 23;
        MSS.FirstBand.DL.DNoiseFigureDb = 10;
        MSS.FirstBand.IVAntNumPerPanel = 2;
        MSS.FirstBand.IHAntNumPerPanel = 4;
        MSS.FirstBand.IVPanelNum = 1;
        MSS.FirstBand.IHPanelNum = MSS.IInitialize1or2Panel;
        MSS.FirstBand.DHBackLossDB = -25;
        MSS.FirstBand.DVBackLossDB = -25;
    }
}

const Parameters& Parameters::Instance() {
    static Parameters& m_Parameters = *(new Parameters);
    return m_Parameters;
}

void Parameters::RecordParameters_New(const string& _pFilename) {
    LINK_CTRL.Islot4Hupdate = max(1,LINK_CTRL.Islot4Hupdate);
    Observer::SetIsEnable(true);
}

//上行添加_begin

void Parameters::Build_UL() {
    cout << "Build_UL begin" << endl;
    // 关联基本参数
    m_Str2ParaMap_UL.clear();

    m_Str2ParaMap_UL["SIM.DL.DBlerTarget"] = &(SIM_UL.DL.DBlerTarget);
    m_Str2ParaMap_UL["SIM.UL.DBlerTarget"] = &(SIM_UL.UL.DBlerTarget); //IMSnumOfMUMIMO
    //    m_Str2ParaMap_UL["SIM.DL.DEVMdb"] = &(SIM_UL.DL.DEVMdb);
    m_Str2ParaMap_UL["SIM.UL.DEVMdb"] = &(SIM_UL.UL.DEVMdb);
    m_Str2ParaMap_UL["SIM.UL.IMSnumOfMUMIMO"] = &(SIM_UL.UL.IMSnumOfMUMIMO);
    m_Str2ParaMap_UL["SIM.UL.iMaxRBFindNum"] = &(SIM_UL.UL.iMaxRBFindNum);
    m_Str2ParaMap_UL["SIM.UL.RatePairMaxMode"] = &(SIM_UL.UL.RatePairMaxMode);
    //zhengyi 20150125
    //DMuGThresholdDB
    //    m_Str2ParaMap_UL["SIM.UL.DMuGThresholdDB"] = &(SIM_UL.UL.DMuGThresholdDB);
    m_Str2ParaMap_UL["SIM.UL.DRBSupportedNumDoor"] = &(SIM_UL.UL.RBSupportedNumDoor);

    m_Str2ParaMap_UL["MSS.DL.IDetectorMode"] = &(MSS_UL.DL.IDetectorMode);
    //    m_Str2ParaMap_UL["SIM.DL.IMIMOMode"] = &(SIM_UL.DL.IMIMOMode);
    m_Str2ParaMap_UL["SIM.UL.IMIMOMode"] = &(SIM_UL.UL.IMIMOMode);
    //    m_Str2ParaMap_UL["Precode.IPrecodingMode"] = &(Precode_UL.IPrecodingMode);
    //    m_Str2ParaMap_UL["Precode.IReTransMode"] = &(Precode_UL.IReTransMode);
    //    m_Str2ParaMap_UL["Precode.ITransformedPeriod"] = &(Precode_UL.ITransformedPeriod);
    //    m_Str2ParaMap_UL["Precode.IIsQuantize"] = &(Precode_UL.IIsQuantize);
    //    m_Str2ParaMap_UL["BF.IBeamformingMode"] = &(BF_UL.IBeamformingMode);
    //    m_Str2ParaMap_UL["SIM.DL.ISchedulerWindowLength"] = &(SIM_UL.DL.ISchedulerWindowLength);
    //    m_Str2ParaMap_UL["SIM.DL.IHARQProcessNum"] = &(SIM_UL.DL.IHARQProcessNum);
    m_Str2ParaMap_UL["SIM.UL.IHARQProcessNum"] = &(SIM_UL.UL.IHARQProcessNum);

    //errors
    m_Str2ParaMap_UL["SIM.UL.ERROR.BIsDMRSErrorOn"] = &(SIM_UL.UL.ERROR.BIsDMRSErrorOn);
    m_Str2ParaMap_UL["SIM.UL.ERROR.BIsSRSErrorOn"] = &(SIM_UL.UL.ERROR.BIsSRSErrorOn);
    m_Str2ParaMap_UL["SIM.UL.ERROR.DDeltaMSEDb"] = &(SIM_UL.UL.ERROR.DDeltaMSEDb);
    m_Str2ParaMap_UL["SIM.UL.ERROR.IDmrsErrorOptions"] = &(SIM_UL.UL.ERROR.IDmrsErrorOptions);
    //    m_Str2ParaMap_UL["SIM.UL.ERROR.BIsErrorEstimationOn"] = &(SIM_UL.UL.ERROR.BIsErrorEstimationOn);
    // double dLoad;
    m_Str2ParaMap_UL["SIM.UL.dLoad"] = &(SIM_UL.UL.dLoad);
    //    m_Str2ParaMap_UL["SIM.UL.dO2Iratio"] = &(SIM_UL.UL.dO2Iratio);
    //iRBsegmentNum
    m_Str2ParaMap_UL["SIM.UL.iRBsegmentNum"] = &(SIM_UL.UL.iRBsegmentNum);
    //imaxRBsegNum
    m_Str2ParaMap_UL["SIM.UL.imaxRBsegNum"] = &(SIM_UL.UL.imaxRBsegNum);
    //iRBSchLevel
    //    m_Str2ParaMap_UL["SIM.UL.iRBSchLevel"] = &(SIM_UL.UL.iRBSchLevel);
    //Maxium time of number finding higher riority MS among the RBSet
    m_Str2ParaMap_UL["SIM.UL.iMaxRBFindNum"] = &(SIM_UL.UL.iMaxRBFindNum);

    m_Str2ParaMap_UL["SIM.UL.BIsMMSEOptionOneOn"] = &(SIM_UL.UL.BIsMMSEOptionOneOn);
    //    m_Str2ParaMap_UL["SIM.UL.BIsFrequencyReuseOn"] = &(SIM_UL.UL.BIsFrequencyReuseOn);
    m_Str2ParaMap_UL["MSS.iMSCategory"] = &(MSS_UL.iMSCategory); // DRsrpThresholdDB
    //m_Str2ParaMap_UL["SIM.UL.DCompRsrpThresholdDB"] = &(SIM.UL.DCompRsrpThresholdDB);
    //    m_Str2ParaMap_UL["SIM.UL.COMP.DCompRsrpThresholdDB"] = &(SIM_UL.UL.COMP.DCompRsrpThresholdDB);
    //    m_Str2ParaMap_UL["SIM.UL.BIsCompSelectedOn"] = &(SIM.UL.BIsCompSelectedOn);


    //zhengyi 3d mimo
    //    m_Str2ParaMap_UL["BASIC.I3DElevationUsed"] = &(BASIC_UL.I3DElevationUsed);
    //    m_Str2ParaMap_UL["BTS.DVerticalAntennaSpace"] = &(BTS_UL.DVerticalAntennaSpace);
    //    m_Str2ParaMap_UL["BTS.IHorizontalElemNum"] = &(BTS_UL.IHorizontalElemNum);
    //    m_Str2ParaMap_UL["BASIC.I2DOr3DChannel"] = &(BASIC_UL.I2DOr3DChannel);
    //    m_Str2ParaMap_UL["LINK.DDownTiltDeg"] = &(LINK_UL.DDownTiltDeg);

    //    m_Str2ParaMap_UL["SIM.UL.COMP.BIsCompSelectedOn"] = &(SIM.UL.COMP.BIsCompSelectedOn); //0503
    //    m_Str2ParaMap_UL["SIM.UL.BIs2btsInComp"] = &(SIM.UL.BIs2btsInComp); //

    //    m_Str2ParaMap_UL["SIM.UL.COMP.BIs2btsInComp"] = &(SIM.UL.COMP.BIs2btsInComp); //ICompClusterSize
    //    m_Str2ParaMap_UL["SIM.UL.COMP.ICompClusterSize"] = &(SIM_UL.UL.COMP.ICompClusterSize); //ICompClusterSize
    //    m_Str2ParaMap_UL["SIM.UL.BIsMUCoMPOn"] = &(SIM_UL.UL.BIsMUCoMPOn);
    //BIsMUCoMPOn
    //    m_Str2ParaMap_UL["SIM.UL.COMP.BIsMUCoMPOn"] = &(SIM_UL.UL.COMP.BIsMUCoMPOn);
    //BisMUollaOn
    m_Str2ParaMap_UL["SIM.UL.BisMUollaOn"] = &(SIM_UL.UL.BisMUollaOn);
    m_Str2ParaMap_UL["SIM.UL.FirstTimeMode"] = &(SIM_UL.UL.FirstTimeMode);
    m_Str2ParaMap_UL["SIM.UL.IvmimoAdaptiveMode"] = &(SIM_UL.UL.IvmimoAdaptiveMode);
    m_Str2ParaMap_UL["SIM.UL.RateGainMode"] = &(SIM_UL.UL.RateGainMode);
    //zhengyi 3d mimo
    //    m_Str2ParaMap_UL["BASIC.IWrappingMode"] = &(BASIC_UL.IWrappingMode);

    m_Str2ParaMap_UL["BTS.UL.DNoiseFigureDb"] = &(BTS_UL.UL.DNoiseFigureDb);
    //    m_Str2ParaMap_UL["BTS.Dlamda"] = &(BTS_UL.Dlamda);
    //    m_Str2ParaMap_UL["BTS.DL.IScheduleMode"] = &(BTS_UL.DL.IScheduleMode);
    m_Str2ParaMap_UL["BTS.UL.IScheduleMode"] = &(BTS_UL.UL.IScheduleMode);
    m_Str2ParaMap_UL["BTS.UL.ISchedulerWindowLength"] = &(BTS_UL.UL.ISchedulerWindowLength);
    m_Str2ParaMap_UL["BTS.UL.dPFfactor"] = &(BTS_UL.UL.dPFfactor);
    m_Str2ParaMap_UL["BTS.UL.IDetectorMode"] = &(BTS_UL.UL.IDetectorMode);
    // 关联移动台参数
    //20190702新添加的楼层选项的变量
    //    m_Str2ParaMap_UL["MSS.iBuildingFloorSelection"] = &(MSS_UL.iBuildingFloorSelection);
    //    m_Str2ParaMap_UL["MSS.DAntennaHeightM"] = &(MSS_UL.DAntennaHeightM);
    //    m_Str2ParaMap_UL["MSS.DAntennaGainDb"] = &(MSS_UL.DAntennaGainDb);
    m_Str2ParaMap_UL["MSS.UL.DMaxTxPowerDbm"] = &(MSS_UL.UL.DMaxTxPowerDbm);
    m_Str2ParaMap_UL["MSS.UL.DMinTxPowerDbm"] = &(MSS_UL.UL.DMinTxPowerDbm);
    m_Str2ParaMap_UL["MSS.UL.DMaxTxPowerMw"] = &(MSS_UL.UL.DMaxTxPowerMw);
    //    m_Str2ParaMap_UL["MSS.DHandOffMarginDb"] = &(MSS_UL.DHandOffMarginDb);
    //    m_Str2ParaMap_UL["MSS.DAntennaPosition"] = &(MSS_UL.DAntennaPosition);
    //    m_Str2ParaMap_UL["MSS.DL.IAntennaNum"] = &(MSS_UL.DL.IAntennaNum);
    //    m_Str2ParaMap_UL["MSS.UL.IAntennaNum"] = &(MSS_UL.UL.IAntennaNum);
    //    m_Str2ParaMap_UL["MSS.VelocityMPS"] = &(MSS_UL.DVelocityMPS);
    //    m_Str2ParaMap_UL["MSS.ICQI_DELAY"] = &(MSS_UL.ICQI_DELAY);
    //    m_Str2ParaMap_UL["MSS.ICQI_PERIOD"] = &(MSS_UL.ICQI_PERIOD);
    m_Str2ParaMap_UL["MSS.ISRS_DELAY"] = &(MSS_UL.ISRS_DELAY);
    m_Str2ParaMap_UL["MSS.UL.ISRS_PERIOD"] = &(MSS_UL.UL.ISRS_PERIOD);
    m_Str2ParaMap_UL["MSS.UL.ISRSBandWidthIndex"] = &(MSS_UL.UL.ISRSBandWidthIndex); //IsSRSfixed
    //    m_Str2ParaMap_UL["MSS.UL.BIsSRSfixed"] = &(MSS_UL.UL.BIsSRSfixed);
    m_Str2ParaMap_UL["MSS.DL.DNoiseFigureDb"] = &(MSS_UL.DL.DNoiseFigureDb);
    m_Str2ParaMap_UL["MSS.UL.DP0dBm"] = &(MSS_UL.UL.DP0Dbm);
    m_Str2ParaMap_UL["MSS.UL.DAlfa"] = &(MSS_UL.UL.DAlfa);
    m_Str2ParaMap_UL["MSS.UL.DP0dBm4SRS"] = &(MSS_UL.UL.DP0Dbm4SRS);
    m_Str2ParaMap_UL["MSS.UL.DAlfa4SRS"] = &(MSS_UL.UL.DAlfa4SRS);
    m_Str2ParaMap_UL["MSS.UL.CLPCofPUSCH"] = &(MSS_UL.UL.CLPCofPUSCH);
    m_Str2ParaMap_UL["MSS.UL.UserStrategy"] = &(MSS_UL.UL.UserStrategy);
    m_Str2ParaMap_UL["MSS.UL.TPC_Accumulation"] = &(MSS_UL.UL.TPC_Accumulation);
    m_Str2ParaMap_UL["MSS.UL.ResetKey"] = &(MSS_UL.UL.ResetKey);
    m_Str2ParaMap_UL["MSS.UL.ResetPeriod"] = &(MSS_UL.UL.ResetPeriod);
    m_Str2ParaMap_UL["MSS.UL.HighTargetPL"] = &(MSS_UL.UL.HighTargetPL);
    m_Str2ParaMap_UL["MSS.UL.LowTargetPL"] = &(MSS_UL.UL.LowTargetPL);
    m_Str2ParaMap_UL["MSS.UL.HighTargetSINR"] = &(MSS_UL.UL.HighTargetSINR);
    m_Str2ParaMap_UL["MSS.UL.MidTargetSINR"] = &(MSS_UL.UL.MidTargetSINR);
    m_Str2ParaMap_UL["MSS.UL.LowTargetSINR"] = &(MSS_UL.UL.LowTargetSINR);
    m_Str2ParaMap_UL["MSS.UL.TargetSINRMargin"] = &(MSS_UL.UL.TargetSINRMargin);
    m_Str2ParaMap_UL["MSS.UL.IsPhrOn"] = &(MSS_UL.UL.IsPhrOn);
    m_Str2ParaMap_UL["MSS.UL.PeriodicPeriod"] = &(MSS_UL.UL.PeriodicPeriod);
    m_Str2ParaMap_UL["MSS.UL.ProhibitPeriod"] = &(MSS_UL.UL.ProhibitPeriod);
    m_Str2ParaMap_UL["MSS.UL.PowerFactorChange_dB"] = &(MSS_UL.UL.PowerFactorChange_dB);
    m_Str2ParaMap_UL["MSS.UL.IsSROn"] = &(MSS_UL.UL.IsSROn);
    m_Str2ParaMap_UL["MSS.UL.SR_ProhibitPeriod"] = &(MSS_UL.UL.SR_ProhibitPeriod);
    m_Str2ParaMap_UL["MSS.UL.SR_TransMax"] = &(MSS_UL.UL.SR_TransMax);
    m_Str2ParaMap_UL["MSS.UL.sr_Periodicity_slot"] = &(MSS_UL.UL.sr_Periodicity_slot);
    m_Str2ParaMap_UL["MSS.UL.SR_PRBNum"] = &(MSS_UL.UL.SR_PRBNum);
    m_Str2ParaMap_UL["MSS.UL.SR_PUCCHFormat"] = &(MSS_UL.UL.SR_PUCCHFormat);
    m_Str2ParaMap_UL["MSS.UL.SR_InitCyc"] = &(MSS_UL.UL.SR_InitCyc);
    m_Str2ParaMap_UL["MSS.UL.SR_Format1_OCC"] = &(MSS_UL.UL.SR_Format1_OCC);
    m_Str2ParaMap_UL["MSS.UL.SR_PeriodSwitch"] = &(MSS_UL.UL.SR_PeriodSwitch);
    m_Str2ParaMap_UL["MSS.UL.SR_RBNumSwitch"] = &(MSS_UL.UL.SR_RBNumSwitch);
    m_Str2ParaMap_UL["MSS.UL.SR_SmartSchSwitch"] = &(MSS_UL.UL.SR_SmartSchSwitch);
    m_Str2ParaMap_UL["MSS.UL.BSR_RetransTimer"] = &(MSS_UL.UL.BSR_RetransTimer);
    m_Str2ParaMap_UL["MSS.UL.BSR_PeriodicTimer"] = &(MSS_UL.UL.BSR_PeriodicTimer);

    // 关联链路参数
    //    m_Str2ParaMap_UL["LINK.DRadioFrequencyMHz"] = &(LINK_UL.DRadioFrequencyMHz);
    //    m_Str2ParaMap_UL["LINK.IEnvironmentType"] = &(LINK_UL.IEnvironmentType);
    //    m_Str2ParaMap_UL["LINK.DPenetrationLossDB"] = &(LINK_UL.DPenetrationLossDB);
    //    m_Str2ParaMap_UL["LINK.BPolarizeNB"] = &(LINK_UL.BPolarizeNB);
    //    m_Str2ParaMap_UL["LINK.BPolarizeUE"] = &(LINK_UL.BPolarizeUE);
    //    m_Str2ParaMap_UL["LINK.DHBackLossDB"] = &(LINK_UL.DHBackLossDB);
    m_Str2ParaMap_UL["LINK.ISEESMorMIESM"] = &(LINK_UL.ISEESMorMIESM);
    //    m_Str2ParaMap_UL["LINK.DVBackLossDB"] = &(LINK_UL.DVBackLossDB);
    //    m_Str2ParaMap_UL["LINK.DMinDistanceM"] = &(LINK_UL.DMinDistanceM);
    //    m_Str2ParaMap_UL["LINK.BIS3DAntUsed"] = &(LINK_UL.BIS3DAntUsed);
    //    m_Str2ParaMap_UL["LINK.DDownTiltDeg"] = &(LINK_UL.DDownTiltDeg);
    m_Str2ParaMap_UL["LINK.DNoisePowerSpectrumDensityDbmHz"] = &(LINK_UL.DNoisePowerSpectrumDensityDbmHz);

    // 关联无线资源参数
    //    m_Str2ParaMap_UL["SIM.DL.ISubframeNum"] = &(SIM_UL.DL.ISubframeNum);
    //    m_Str2ParaMap_UL["SIM.UL.ISubframeNum"] = &(SIM_UL.UL.ISubframeNum);
    //    m_Str2ParaMap_UL["SIM.DL.IHARQMaxTransNum"] = &(SIM_UL.DL.IHARQMaxTransNum);
    m_Str2ParaMap_UL["SIM.UL.IHARQMaxTransNum"] = &(SIM_UL.UL.IHARQMaxTransNum);
    
    

    //m_Str2ParaMap_UL["SIM.UL.IHARQProcessNum"] = &(SIM.UL.IHARQProcessNum);
    cout << "Build_UL end" << endl;
}

void Parameters::ReadInputData_UL() {
    cout << "Read UL input data..." << endl;

    //读取重写参数
    ReadOverWriteParameters_UL();

    CalculateDerivedParameters_UL();
    cout << "ok!" << endl;
}

void Parameters::CalculateDerivedParameters_UL()
{
    MSS_UL.UL.DMaxTxPowerMw = DB2L(MSS_UL.UL.DMaxTxPowerDbm);
}

void Parameters::ReadOverWriteParameters_UL()
{
    ReadData_UL(const_cast<char*> ("./inputfiles/OverWriteUL.txt"));
    // ReorderData(const_cast<char*> ("./inputfiles/OverWrite.txt"));
}

int Parameters::CalcPRBNum(int _DSystemBandWidthKHz, int _DCarrierWidthHz)
{
    int ITotal_RB4NRStatBW;
    switch (_DSystemBandWidthKHz) {
        case 5000:
            switch(_DCarrierWidthHz){
                case 15000:
                    ITotal_RB4NRStatBW=25;
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 11;
                    break;        
                default:
                    assert(false);
                    break;
            }
            break;
        case 10000:
            switch (_DCarrierWidthHz) {
                case 15000:
                    ITotal_RB4NRStatBW = 52;
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 24;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 11;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case 15000:
            switch(_DCarrierWidthHz){
                case 15000:
                    ITotal_RB4NRStatBW=79;
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 38;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 18;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;        
        case 20000:
            switch (_DCarrierWidthHz) {
                case 15000:
                    ITotal_RB4NRStatBW = 106;
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 51;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 24;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case 25000:
            switch (_DCarrierWidthHz) {
                case 15000:
                    ITotal_RB4NRStatBW = 133;
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 65;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 31;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case 30000:
            switch (_DCarrierWidthHz) {
                case 15000:
                    ITotal_RB4NRStatBW = 160;
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 78;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 38;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case 40000:
            switch (_DCarrierWidthHz) {
                case 15000:
                    ITotal_RB4NRStatBW = 216;
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 106;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 51;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case 50000:
            switch (_DCarrierWidthHz) {
                case 15000:
                    ITotal_RB4NRStatBW = 270;
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 133;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 65;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case 60000:
            switch (_DCarrierWidthHz) {
                case 15000:
                    assert(false);
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 162;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 79;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case 80000:
            switch (_DCarrierWidthHz) {
                case 15000:
                    assert(false);
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 217;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 107;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case 90000:
            switch (_DCarrierWidthHz) {
                case 15000:
                    assert(false);
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 245;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 121;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case 100000:
            switch (_DCarrierWidthHz) {
                case 15000:
                    assert(false);
                    break;
                case 30000:
                    ITotal_RB4NRStatBW = 273;
                    break;
                case 60000:
                    ITotal_RB4NRStatBW = 135;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        default:
            assert(false);
            break;
    }
    return ITotal_RB4NRStatBW;
}

//上行添加_end

//将下行的参数赋值给上行


void Parameters::CalculateOH() {

    //下行开销计算
    //以20ms为例,用于开销的RE；
    double DOHREs = 0;
    //以20ms为例,总共的RE；
    double DTotalREs = 0;
    //通过子载波间隔，计算20ms内有多少个时隙（slot）
    double Dslotper20ms = 20 / BASIC.DSlotDuration_ms;
    //通过带宽计算NPRB
    int IPRBNum = CalcPRBNum(BASIC.DSystemBandWidthKHz, BASIC.DCarrierWidthHz);

    //PDCCH: CORESET of 24 PRBs (4 CCE) in every slot；	12 RE/PRB/slot
    //默认值 : 288 RE/slot     界面：RE/slot
    double DOH_PDCCH = OVERHEAD.DL.IPDCCHNum * Dslotper20ms;
    //TRS burst of 2 slots with periodicity of 20ms and occupies 52 PRBs  ;12 RE/PRB/20 ms
    //默认值 : 624RE/20ms     界面：RE/10ms
    double DOH_TRS = OVERHEAD.DL.ITRSNum * 2;
    //DMRS: Type 2, 16 RE/PRB/slot for 8 layers
    //默认值 : 16 RE/PRB/slot    界面：RE/PRB/slot
    double DOH_DMRS = OVERHEAD.DL.IDMRSNum  * IPRBNum * Dslotper20ms;
    //CSI-RS: 8 CSI-RS ports with periodicity of 20ms;    8RE/PRB/20 ms
    //默认值 : 64 RE/PRB/20ms    界面：RE/PRB/20ms
    double DOH_CSIRS = OVERHEAD.DL.ICSIRSNum  * IPRBNum;
    //1 SS/PBCH blocks (SSB) per 20ms; one SSB occupies 960REs = 4 OFDM symbols × 20 PRB × 12 REs/PRB
    //默认值 : 960 RE/20ms    界面：RE/20ms
    double DOH_SSB = OVERHEAD.DL.ISSBNum;

    if(MIMO_CTRL.PMI_based_Precoding == 1){
        //FDD

        //计算20ms总共的RE数，一个时隙有12个RE * 14个正交的OFDM符号 * PRB * 20ms内的时隙数；
        DTotalREs = 12 * 14 * IPRBNum * Dslotper20ms;
        //计算总共的下行开销
        DOHREs = DOH_PDCCH + DOH_TRS + DOH_CSIRS + DOH_DMRS + DOH_SSB;
    }else
    {
        //计算20ms总共的RE数，一个时隙有12个RE * 14个正交的OFDM符号 * PRB * 20ms内的时隙数 * 下行RE占比
        DTotalREs = 12 * 14 * IPRBNum * Dslotper20ms * SIM.FrameStructure4Sim.DCalDLRERate();
        //计算总共的下行开销
        DOHREs = DOH_PDCCH + DOH_TRS + DOH_CSIRS + DOH_DMRS + DOH_SSB;
    }


    //信道开销占比
    double DOHRate = DOHREs / DTotalREs;
    //平台下行每个RB能够利用的RE数，以12*14为基数；
    BASIC.DDLRENumPerRBforData = 168 * (1 - DOHREs / DTotalREs);

    //上行开销计算
    //以20ms为例,用于开销的RE；
    double DOHREsUL;
    //以20ms为例,总共的RE；
    double DTotalREsUL;
    //通过带宽计算NPRB
    int IPRBNumUL = CalcPRBNum(BASIC.DSystemBandWidthKHz, BASIC.DCarrierWidthHz);

    //PUCCH: short PUCCH with 1 PRB and 1 symbol in every UL slot;  12 RE/slot
    //默认值 : 12 RE/slot     界面：RE/slot
    double DOH_PUCCH = OVERHEAD.UL.IPUCCHNum * Dslotper20ms;
    //DMRS: Type I, one complete symbol; 12 RE/PRB/slot
    //默认值 : 12 RE/PRB/slot    界面：RE/PRB/slot
    double DOH_DMRSUL = OVERHEAD.UL.IDMRSNum  * IPRBNumUL * Dslotper20ms;
    //SRS: 1 symbol with periodicity of 10ms for FDD; 1 symbol with periodicity of 20ms for TDD
    //默认值 : 12 RE/10ms    界面：RE/10ms
    double DOH_SRS = OVERHEAD.UL.ISRSNum * 2;
    //PRACH开销，37910中上行OH1中没有计算PRACH开销
    double DOH_PRACH = OVERHEAD.UL.IPRACHNum;

    if(MIMO_CTRL.PMI_based_Precoding == 1){
        //FDD

        //计算20ms总共的RE数，一个时隙有12个RE * 14个正交的OFDM符号 * PRB * 20ms内的时隙数；
        DTotalREsUL = 12 * 14 * IPRBNumUL * Dslotper20ms;
        //计算总共的下行开销
        DOHREsUL = DOH_PUCCH + DOH_DMRSUL + DOH_SRS;
    }else
    {
        //计算20ms总共的RE数，一个时隙有12个RE * 14个正交的OFDM符号 * PRB * 20ms内的时隙数 * 上行RE占比
        DTotalREsUL = 12 * 14 * IPRBNumUL * Dslotper20ms * (1 - SIM.FrameStructure4Sim.DCalDLRERate());
        //计算总共的下行开销
        DOHREsUL = DOH_PUCCH + DOH_DMRSUL + DOH_SRS;
    }




    //信道开销占比
    double DOHRateUL = DOHREsUL / DTotalREsUL;
    //平台上行每个RB能够利用的RE数，以12*14为基数；
    BASIC.DULRENumPerRBforData = 168 * (1 - DOHREsUL / DTotalREsUL);

//    double a = SIM.FrameStructure4Sim.DCalTDDDLRE();;
//    int b = BASIC.DDLRENumPerRBforData;
//    int c= MIMO_CTRL.PMI_based_Precoding;
}

void Parameters::CalNRE() {
    /*
    N_RE = N_SCperRB * N_SymbolperSH - N_DMRSperRB - N_OHperPRB
    N_SCperRB is the number of subcarriers in a physical resource block
    N_SymbolperSH is the number of symbols of the PDSCH allocation within the slot
    N_DMRSperRB is the number of REs for DM-RS per PRB in the scheduled duration including the overhead of the DM-RS CDM groups without data
    N_OHperPRB is the overhead configured by higher layer parameter xOverhead in PDSCH-ServingCellConfig.
    If the xOverhead in PDSCH-ServingCellconfig is not configured (a value from 6, 12, or 18), the   is set to 0.
    */
    int N_SCperRB = 12;
    int N_SymbolperSH = 14;     ///未考虑PUCCH和PDCCH占据的RE数？
    //int N_DMRSperRB = OVERHEAD.DMRS_PRB;
    int N_DMRSperRB;

    switch(OVERHEAD.DMRS_Symbol){
        case DMRS_SingleSymbol:
            if(OVERHEAD.DMRS_Type == 1){
//                N_DMRSperRB = 12 / 2;
                N_DMRSperRB = 12;
            }else if(OVERHEAD.DMRS_Type == 2){
//                N_DMRSperRB = 12 / 3;
                N_DMRSperRB = 12;
            }else{
                assert(false);
            }
            break;
        case DMRS_DoubleSymbol:
            if(OVERHEAD.DMRS_Type == 1){
//                N_DMRSperRB = 24 / 2;
                N_DMRSperRB = 24;
            }else if(OVERHEAD.DMRS_Type == 2){
//                N_DMRSperRB = 24 / 3;
                N_DMRSperRB = 24;
            }else{
                assert(false);
            }
            break;
        default:
            assert(false);
            break;
    }
    int N_OHperPRB = OVERHEAD.OH_PRB;
    assert(OVERHEAD.OH_PRB == 0 || OVERHEAD.OH_PRB == 6 || OVERHEAD.OH_PRB == 12 ||  OVERHEAD.OH_PRB == 18 );

//    BASIC.DDLRENumPerRBforData = N_SCperRB * N_SymbolperSH - N_DMRSperRB - N_OHperPRB;
    BASIC.DDLRENumPerRBforData = N_SCperRB * (SymbolNumPerSlot-CoresetDuration) - N_DMRSperRB - N_OHperPRB;
    //根据38.214中TBSize确定时，一个PRB最多有156个RE进行传输
//    BASIC.DDLRENumPerRBforData = std::min(156,BASIC.DDLRENumPerRBforData);
    BASIC.DDLRENumPerRBforData = 156 > BASIC.DDLRENumPerRBforData ? BASIC.DDLRENumPerRBforData : 156;

    BASIC.DDLRENumPerRBforDataWithoutPDCCH = BASIC.DDLRENumPerRBforData;

//    BASIC.DDLRENumPerRBforData = BASIC.DDLRENumPerRBforData*(SymbolNumPerSlot-CoresetDuration)/SymbolNumPerSlot;
    assert(BASIC.DDLRENumPerRBforData > 0 && BASIC.DDLRENumPerRBforData <= 156);

//    int b = BASIC.DDLRENumPerRBforData;
//    cout << b;

}

void Parameters::ConfigureArgs()
{
    DaHuaWu.iOccupiedSymbolNum          = 1;
    DaHuaWu.iOccupiedRbNum              = 200;
    DaHuaWu.bPdcchAlgoEnhSwitch         = false;
    DaHuaWu.bPdcchAggLvlAdaptPol        = false;
    DaHuaWu.bRateMatchSwitch            = false;
    DaHuaWu.bDl256QamSwitch             = true;
    DaHuaWu.bNrDuCellDrxAlgoSwitch      = false;

    if(DaHuaWu.bRateMatchSwitch == false)
    {
        DaHuaWu.iOccupiedRbNum = 51;
    }
    MSS_UL.UL.SR_RBNumSwitch            = false; //默认true
    MSS_UL.UL.SR_PeriodSwitch           = false;  //默认true

    MSS_UL.UL.SR_SmartSchSwitch         = false;
    MSS_UL.UL.BSR_RetransTimer          = 10;
    DaHuaWu.HeavyLoadUlCceAdjPolicy     = DaHuaWu_Para::RATIO_OPT;

    Observer::Print("DaHuaWuParas") <<"int : iOccupiedSymbolNum"<<setw(30)<<DaHuaWu.iOccupiedSymbolNum<<endl
                                    <<"int : iOccupiedRbNum"<<setw(30)<<DaHuaWu.iOccupiedRbNum<<endl
                                    <<"bool : bPdcchAlgoEnhSwitch"<<setw(30)<<DaHuaWu.bPdcchAlgoEnhSwitch<<endl
                                    <<"bool : bPdcchAggLvlAdaptPol"<<setw(30)<<DaHuaWu.bPdcchAggLvlAdaptPol<<endl
                                    <<"bool : bRateMatchSwitch"<<setw(30)<<DaHuaWu.bRateMatchSwitch<<endl
                                    <<"bool : bDl256QamSwitch"<<setw(30)<<DaHuaWu.bDl256QamSwitch<<endl
                                    <<"bool : bSuMimoMultipleLayerSw_DL"<<setw(30)<<DaHuaWu.bSuMimoMultipleLayerSw_DL<<endl
                                    <<"bool : bSuMimoMultipleLayerSw_UL"<<setw(30)<<DaHuaWu.bSuMimoMultipleLayerSw_UL<<endl
                                    <<"bool : bMuMimoSwitch_DL"<<setw(30)<<DaHuaWu.bMuMimoSwitch_DL<<endl
                                    <<"bool : bMuMimoSwitch_UL"<<setw(30)<<DaHuaWu.bMuMimoSwitch_UL<<endl
                                    <<"bool : bNrDuCellDrxAlgoSwitch"<<setw(30)<<DaHuaWu.bNrDuCellDrxAlgoSwitch<<endl
                                    <<"bool : SR_RBNumSwitch"<<setw(30)<<MSS_UL.UL.SR_RBNumSwitch<<endl
                                    <<"bool : SR_PeriodSwitch"<<setw(30)<<MSS_UL.UL.SR_PeriodSwitch<<endl
                                    <<"bool : SR_SmartSchSwitch"<<setw(30)<<MSS_UL.UL.SR_SmartSchSwitch<<endl
                                    <<"int : BSR_RetransTimer"<<setw(30)<<MSS_UL.UL.BSR_RetransTimer<<endl
                                    <<"enum : HeavyLoadUlCceAdjPolicy"<<setw(30)<<DaHuaWu.HeavyLoadUlCceAdjPolicy<<endl;
}
void Parameters::MapArgsToParameters()
{
    CoresetDuration             = DaHuaWu.iOccupiedSymbolNum;
}

//20260115
void Parameters::ReadSceneData() {
    ReadData_DL(const_cast<char*> ("./inputfiles/Scene.txt"));
    // 如果需要格式化输出，可添加下面这行
    // ReorderData(const_cast<char*> ("./inputfiles/Scene.txt")
}
void Parameters::Build_Scene() {
    // 关联基本参数
    m_Str2ParaMap_DL.clear();
    m_Str2ParaMap_DL["BASIC.BISRIS"] = &(BASIC.BISRIS);
    m_Str2ParaMap_DL["BASIC.BRISCASE"] = &(BASIC.IRISCASE);
    m_Str2ParaMap_DL["BASIC.IRISPerBTS"] = &(BASIC.IRISPerBTS);
    m_Str2ParaMap_DL["RIS.LINK.DRadioFrequencyMHz"] = &(RIS.LINK.DRadioFrequencyMHz);
    m_Str2ParaMap_DL["RIS.IAntennaPatternMode"] = &(RIS.IAntennaPatternMode);
    m_Str2ParaMap_DL["RIS.DV3DBBeamWidthDeg"] = &(RIS.DV3DBBeamWidthDeg);
    m_Str2ParaMap_DL["RIS.DH3DBBeamWidthDeg"] = &(RIS.DH3DBBeamWidthDeg);
    m_Str2ParaMap_DL["RIS.DAntennaGainDb"] = &(RIS.DAntennaGainDb);
    m_Str2ParaMap_DL["RIS.DAntennaHeightM"] = &(RIS.DAntennaHeightM);
    m_Str2ParaMap_DL["RIS.DMechanicalTiltDeg"] = &(RIS.DMechanicalTiltDeg);
    m_Str2ParaMap_DL["RIS.DElectricalTiltDeg"] = &(RIS.DElectricalTiltDeg);
    m_Str2ParaMap_DL["RIS.DVAntSpace"] = &(RIS.DVAntSpace);
    m_Str2ParaMap_DL["RIS.DHAntSpace"] = &(RIS.DHAntSpace);
    m_Str2ParaMap_DL["RIS.UL.DNoiseFigureDb"] = &(RIS.UL.DNoiseFigureDb);
    m_Str2ParaMap_DL["RIS.IVAntNumPerPanel"] = &(RIS.IVAntNumPerPanel);
    m_Str2ParaMap_DL["RIS.IHAntNumPerPanel"] = &(RIS.IHAntNumPerPanel);
    m_Str2ParaMap_DL["RIS.IVPanelNum"] = &(RIS.IVPanelNum);
    m_Str2ParaMap_DL["RIS.IHPanelNum"] = &(RIS.IHPanelNum);
    m_Str2ParaMap_DL["RIS.Polarize_Num"] = &(RIS.Polarize_Num);
    m_Str2ParaMap_DL["RIS.V_TXRU_DIV_NUM_PerPanel"] = &(RIS.V_TXRU_DIV_NUM_PerPanel);
    m_Str2ParaMap_DL["RIS.H_TXRU_DIV_NUM_PerPanel"] = &(RIS.H_TXRU_DIV_NUM_PerPanel);
    m_Str2ParaMap_DL["RIS.ANALOGBEAM_CONFIG.iHRISBeamNum"] = &(RIS.ANALOGBEAM_CONFIG.iHRISBeamNum);
    m_Str2ParaMap_DL["RIS.ANALOGBEAM_CONFIG.iVRISBeamNum"] = &(RIS.ANALOGBEAM_CONFIG.iVRISBeamNum);
    m_Str2ParaMap_DL["RIS.ANALOGBEAM_CONFIG.dStartHRISBeamDeg"] = &(RIS.ANALOGBEAM_CONFIG.dStartHRISBeamDeg);
    m_Str2ParaMap_DL["RIS.ANALOGBEAM_CONFIG.dStartVRISBeamDeg"] = &(RIS.ANALOGBEAM_CONFIG.dStartVRISBeamDeg);
    m_Str2ParaMap_DL["RIS.ANALOGBEAM_CONFIG.dSpaceHRISBeamDeg"] = &(RIS.ANALOGBEAM_CONFIG.dSpaceHRISBeamDeg);
    m_Str2ParaMap_DL["RIS.ANALOGBEAM_CONFIG.dSpaceVRISBeamDeg"] = &(RIS.ANALOGBEAM_CONFIG.dSpaceVRISBeamDeg);
    m_Str2ParaMap_DL["RIS.DHBackLossDB"] = &(RIS.DHBackLossDB);
    m_Str2ParaMap_DL["RIS.DVBackLossDB"] = &(RIS.DVBackLossDB);
    m_Str2ParaMap_DL["RIS.LINK.DMinDistanceM"] = &(RIS.LINK.DMinDistanceM);
    m_Str2ParaMap_DL["RIS.DL.bEnable_BSAnalogBF"] = &(RIS.DL.bEnable_BSAnalogBF);
    m_Str2ParaMap_DL["RIS.IS_SmallScale"] = &(RIS.IS_SmallScale);
    m_Str2ParaMap_DL["RIS.dMMSE_RIS"] = &(RIS.dMMSE_RIS);
    m_Str2ParaMap_DL["RIS.is_BestBTS2MSBeam"] = &(RIS.is_BestBTS2MSBeam);
    m_Str2ParaMap_DL["RIS.is_DoubleBest"] = &(RIS.is_DoubleBest);
    m_Str2ParaMap_DL["RIS.is_BestBeam"] = &(RIS.is_BestBeam);
    m_Str2ParaMap_DL["RIS.is_BestCase"] = &(RIS.is_BestCase);
    m_Str2ParaMap_DL["RIS.is_Max_UE_RIS"] = &(RIS.is_Max_UE_RIS);
    m_Str2ParaMap_DL["RIS.is_BestMiddle"] = &(RIS.is_BestMiddle);
    m_Str2ParaMap_DL["RIS.is_New"] = &(RIS.is_New);
    m_Str2ParaMap_DL["RIS.is_baseline"] = &(RIS.is_baseline);
    m_Str2ParaMap_DL["RIS.com_case"] = &(RIS.com_case);
    m_Str2ParaMap_DL["RIS._case"]=&(RIS._case);
    m_Str2ParaMap_DL["RIS.DistributeHexagon_Min_Radiu"] = &(RIS.DistributeHexagon_Min_Radiu);
    m_Str2ParaMap_DL["RIS.DistributeHexagon_Max_Radiu"] = &(RIS.DistributeHexagon_Max_Radiu);

}

/// @brief 输出OverWriteDL、OverWriteUL和Scene.txt中的参数，按首字母排序
/// @param _pOutputFilename 输出文件名
void Parameters::OutputInputFilesParameters(const char* _pOutputFilename) {
    // 存储参数信息：参数名 -> (值, 注释列表)
    struct ParameterInfo {
        string value;
        vector<string> comments;
    };
    map<string, ParameterInfo> paramMap;
    
    // 要读取的文件列表
    vector<string> inputFiles = {
        "./inputfiles/OverWriteDL.txt",
        "./inputfiles/OverWriteUL.txt",
        "./inputfiles/Scene.txt"
    };
    
    // 读取每个文件
    for (const auto& filename : inputFiles) {
        ifstream f(filename);
        if (!f.is_open()) {
            cout << "Warning: Cannot open file " << filename << endl;
            continue;
        }
        
        string line;
        string currentParam;
        string currentValue;
        vector<string> currentComments;
        bool hasCurrentParam = false;
        
        while (getline(f, line)) {
            // 去除行尾的\r（Windows换行符）
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            
            // 去除行首空白
            size_t start = line.find_first_not_of(" \t");
            string trimmedLine = (start == string::npos) ? "" : line.substr(start);
            
            // 检查是否是参数行（以&开头）
            if (!trimmedLine.empty() && trimmedLine[0] == '&') {
                // 如果有之前的参数，先保存
                if (hasCurrentParam && !currentParam.empty()) {
                    if (paramMap.find(currentParam) == paramMap.end()) {
                        paramMap[currentParam] = ParameterInfo();
                        paramMap[currentParam].value = currentValue;
                    }
                    // 合并注释
                    if (!currentComments.empty()) {
                        for (const auto& comment : currentComments) {
                            paramMap[currentParam].comments.push_back(comment);
                        }
                    }
                }
                
                // 解析新参数行：&	参数名	值 [可能的注释]
                // 使用制表符或空格分割
                istringstream iss(trimmedLine);
                string marker;
                iss >> marker; // 读取 &
                
                string paramName;
                if (iss >> paramName) {
                    currentParam = paramName;
                    hasCurrentParam = true;
                    
                    // 读取值（可能用制表符分隔）
                    string value;
                    if (iss >> value) {
                        currentValue = value;
                    } else {
                        currentValue = "";
                    }
                    
                    // 读取同一行可能存在的注释（剩余部分）
                    string remaining;
                    getline(iss, remaining);
                    if (!remaining.empty()) {
                        // 去除前导空白
                        size_t remStart = remaining.find_first_not_of(" \t");
                        if (remStart != string::npos) {
                            currentComments.clear();
                            currentComments.push_back(remaining.substr(remStart));
                        } else {
                            currentComments.clear();
                        }
                    } else {
                        currentComments.clear();
                    }
                }
            } else if (!trimmedLine.empty()) {
                // 非参数行，可能是注释或其他内容
                if (hasCurrentParam && !currentParam.empty()) {
                    // 保存为注释
                    currentComments.push_back(trimmedLine);
                }
            } else {
                // 空行：如果当前有参数且有注释，保存参数（空行表示参数结束）
                if (hasCurrentParam && !currentParam.empty() && !currentComments.empty()) {
                    if (paramMap.find(currentParam) != paramMap.end()) {
                        for (const auto& comment : currentComments) {
                            paramMap[currentParam].comments.push_back(comment);
                        }
                    }
                    currentComments.clear();
                }
            }
        }
        
        // 保存最后一个参数
        if (hasCurrentParam && !currentParam.empty()) {
            if (paramMap.find(currentParam) == paramMap.end()) {
                paramMap[currentParam] = ParameterInfo();
                paramMap[currentParam].value = currentValue;
            }
            if (!currentComments.empty()) {
                for (const auto& comment : currentComments) {
                    paramMap[currentParam].comments.push_back(comment);
                }
            }
        }
        
        f.close();
    }
    
    // 输出到文件（map已经按参数名自动排序）
    ofstream fout(_pOutputFilename);
    if (!fout.is_open()) {
        cout << "Error: Cannot create output file " << _pOutputFilename << endl;
        return;
    }
    
    // 输出参数，按首字母顺序
    for (const auto& pair : paramMap) {
        const string& paramName = pair.first;
        const ParameterInfo& info = pair.second;
        
        // 输出参数行：&	参数名	值（格式与输入文件一致，使用制表符）
        fout << "&\t" << paramName << "\t\t\t" << info.value << endl;
        
        // 输出注释
        for (const auto& comment : info.comments) {
            fout << comment << endl;
        }
        
        // 参数之间空一行
        fout << endl;
    }
    
    fout.close();
    cout << "Parameters output completed to " << _pOutputFilename << ". Total parameters: " << paramMap.size() << endl;
}