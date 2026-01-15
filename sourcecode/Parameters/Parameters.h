///@file Parameters.h 	 	
///@brief 本类采用Singleton设计模式. 	 	
///@author dushaofeng 	 	

#pragma once

//#define RR

#include "../Utility/Include.h"   
#include "../Statistician/Observer.h"
/// @brief 记录仿真基本参数的结构体 	

struct POVERHEAD {

    // 每个PRB中DMRE占据的RE数
    int DMRS_PRB;
    // 每个PRB中信道开销，可配置为 0、6、12、18
    int OH_PRB;
    //DMRS 采用 Type 1/2
    int DMRS_Type;
    //DMRS采用双符号还是单符号
    int DMRS_Symbol;

    struct {
    //PDCCH开销      
        int IPDCCHNum;
    //CSI-RS开销
        int ICSIRSNum;
    //下行DMRS开销
        int IDMRSNum;
    //TRS开销
        int ITRSNum;
    //SSB开销
        int ISSBNum;
	}DL;

    struct {
    //PUCCH开销       
        int  IPUCCHNum;
    //SRS开销
        int ISRSNum;
    //上行DMRS开销
        int IDMRSNum;
    //PRACH开销
        int IPRACHNum;
	}UL;
};


struct PBASIC {

    //一个无线帧中有多少个时隙 10ms * slotperms
    int iFrameNum;

    int IMaxModulLevelUL;
    int IUnkCarrierBand;

    
    int ISINR2BLERTable;
    int IMaxModulLevel;
    
    double Noise;
    int NRMCSNumDL;

    int INRMCSNumUL;
    //使用LTE链路级接口的绑定开销 	
    double DOverHead_LTE;

    int DWorkingMode;

    double DInterfProb;
    ///上行或者下行标记量
    bool BDL;
    /// 仿真下行或上行的标记 	
    int IDLORUL; //1---DL, 2---UL, 3---DL and UL 	
    /// 帧结构从长度 	
    int ITotalFrameNum;
    /// 收集统计数据的时间点（预热的时间长度
    int IWarmUpSlot;
    /// 是否使用多线程的标记 	 	
    bool BISMultiThread;
    /// 选择调度算法0--PF;1--RR 	 	
    int IScheduleMode;
    /// 	 	
    int IScenarioModel;
    /// RIS多线程
    bool BRISMultiThread;
    ///20171205  用于判断UMA的子信道 	 	
    int ISubScenarioModel;
    /// 	 	
    int IChannelModel_for_Scenario;
    /// ModelA ModelB 	 	
    int IChannelModel_VariantMode;
    /// 仿真的Drop数目 	 	
    int INumSnapShot;
    /// 每个Drop仿真的时隙数 	 	
    int ISlotPerDrop;
    /// 随机数种子
    int ISeed;
    /// 移到BASIC
    ///接入基站选择RSRP波动门限
    double DHandOffMarginDb;
    ///最差RSRP接入门限
    double DRSRPConnectedThreshold;
    ///噪声功率谱密度
    double DNoisePowerSpectrumDensityDbmHz;
    /// 平均每BTS服务的用户数 	 	
    double DNumMSPerBTS;
    /// 每小区最大服务移动台个数
    int IMaxServeMSNum;
    /// 系统带宽指示 	 	
    int ISCSConfig;
    /// 小区数
    int INumBSs; //in 	 	
    /// 每个小区中的扇区数
    int IBTSPerBS; //in
    ///20260115
    ///每个小区中RIS数
    int IRISPerBTS;//in
    /// 是否开启RIS
    bool BISRIS;
    /// 每个小区中的RIS数
    int IRISCASE; //in

    /// 选择移动台撒点算法
    int IMsDistributeMode; //in 	 	
    /// 服务区形状/仿真拓扑
    int IMacroTopology; //in 	 	
    /// 系统带宽指示 	 	
    int INumerologyIndicator; //in 	 	

    /// 系统带宽 	 	
    double DBandWidthKHz;
    /// 时隙的时间长度
    double DSlotDuration_ms;
    //每个无线帧的时隙数
    int ISlotNumPerSFN;
    /// 子载波宽度
    double DCarrierWidthHz;
    /// 采样速率 	 	
    double DSampleRateHz;
    /// 子载波数 	 	
    int ISCNum;
    /// RB的个数
    int IRBNum;
    /// 每个RB的子载波数
    int IRBSize;
    /// SB的个数
    int ISBNum;
    /// 在一个Subband中RB的数目
    int ISBSize;
    /// 移动台总数
    int ITotalMSNum;
    ///每个BS cell内（BTS+Pico）的总数 	 	
    int ITotalBTSNumPerBS;
    /// 	 	
    int ITotalMacroNum;
    //系统总带宽 	
    double DSystemBandWidthKHz;
    //大带宽增益Bandwidth efficience Gain  	
    double DGuardBand;
    //总开销 	
    double DTotalFixedOH;

    //SINR or RSRP 	
    int IUTattachment;
    //NR开销折算结构体 	

    bool ifcoexist;
    double type1MSRate;
    double type1BandRate;
    struct {
        int CSI_RS_PeriodInSlot; //10个slot 	
        int CSI_RS_PortNum; //8*4port 	
        int TRS_PeriodInMS; //20ms 	
        int TRS_BWInPRB; //50 	
        double TRSDensity; // 1/3 	
        int SSB_PeriodInMS; //20ms 	
        int SSB_Num; //8 	
        int SSB_BWInPRB; //20 	
        int PDCCH_BWInPRB; //51PRB for 30KHz SCS 	
        int PDCCH_OSNum; //2
    } NR_OverHeadConfig;

    double DDLRENumPerRBforData;
    double DDLRENumPerRBforDataWithoutPDCCH;
    double DULRENumPerRBforData;
    int RRC_Config_On;
};

struct FrameStructure_singlePeriod {
    int IDLSlot; //下行完整时隙数目（从前向后数） 	
    int IULSlot; //上行完整时隙数目（从后向前数） 	
    int ISpecialSlot;
    int IDLOS; //下行符号数目（从下行完整时隙之后向后数，从前向后数） 	
    int IULOS; //上行符号数目（从上行完整时隙之前向前数，从后向前数） 	
    int IPeriodSlot; //桢结构周期，以时隙为单位

    double Calc_DL_Rate() const {
        const int static OSNumberInSlot = 14;
        double result = 1.0 * (IDLSlot * OSNumberInSlot + IDLOS) / (IPeriodSlot * OSNumberInSlot);
        return result;
    }

    int Get_DLandSpecialSlotNum() {
        return IDLSlot + (IDLOS > 0);
    }

    int Get_DLOSNum() {
        const int static OSNumberInSlot = 14;
        return OSNumberInSlot * IDLSlot + IDLOS;
    }
};

struct FrameStructure {
    
    double DCalDLPacketsize() const{
        double DCalDL_Packetsize = 0;
        if (IIsSinglePeriod == 1) {
            DCalDL_Packetsize = (double)(P1.IDLSlot * 14 + P1.IDLOS) / (P1.IDLSlot  * 14 + P1.ISpecialSlot  * 14);
            return DCalDL_Packetsize;
        } else if (IIsSinglePeriod == 0) {
            DCalDL_Packetsize = (double)((P1.IDLSlot + P2.IDLSlot) * 14 + P1.IDLOS + P2.IDLOS) / ((P1.IDLSlot + P2.IDLSlot + P1.ISpecialSlot + P2.ISpecialSlot) *14);
            return DCalDL_Packetsize;
        } else {
            assert(false);
        }
        assert(false);
        return -1;
    }

    double DCalDLRERate() const{
        double DCalDL_RERate = 0;
        if (IIsSinglePeriod == 1) {
            DCalDL_RERate = (double)(P1.IDLSlot * 14 + P1.IDLOS) / P1.IPeriodSlot *14;
            return DCalDL_RERate;
        } else if (IIsSinglePeriod == 0) {
            DCalDL_RERate = (double)((P1.IDLSlot + P2.IDLSlot) * 14 + P1.IDLOS + P2.IDLOS) / ((P1.IPeriodSlot + P2.IPeriodSlot) * 14);
            return DCalDL_RERate;
        } else {
            assert(false);
        }
        assert(false);
        return -1;
    }
    
    
    int IIsSinglePeriod; //1表示单周期，0表示双周期 	
    FrameStructure_singlePeriod P1;
    FrameStructure_singlePeriod P2;

    double Calc_DL_Rate() const {
        if (IIsSinglePeriod == 1) {
            return P1.Calc_DL_Rate();
        } else if (IIsSinglePeriod == 0) {
            return (P1.Calc_DL_Rate() + P2.Calc_DL_Rate()) / 2.0;
        } else {
            assert(false);
        }
        assert(false);
        return  -1;
    }

    //计算下行周期 	

    int Get_PeriodInSlot() {
        if (IIsSinglePeriod == 1) {
            return P1.IPeriodSlot;
        } else if (IIsSinglePeriod == 0) {
            return (P1.IPeriodSlot + P2.IPeriodSlot);
        } else {
            assert(false);
        }
        assert(false);
        return -1;
    }

    int Get_DLandSpecialSlotNum() {
        if (IIsSinglePeriod == 1) {
            return P1.Get_DLandSpecialSlotNum();
        } else if (IIsSinglePeriod == 0) {
            return (P1.Get_DLandSpecialSlotNum() + P2.Get_DLandSpecialSlotNum());
        } else {
            assert(false);
        }
        assert(false);
        return  -1;
    }

    int Get_DLOSNum() {
        if (IIsSinglePeriod == 1) {
            return P1.Get_DLOSNum();
        } else if (IIsSinglePeriod == 0) {
            return (P1.Get_DLOSNum() + P2.Get_DLOSNum());
        } else {
            assert(false);
        }
        assert(false);
        return -1;
    }
};

struct PSIM {
    
    int IChannelInterpolationSpace;
    //RANK自适应开关
    int IsRankAdaptive;

    struct {
        /// 下行链路的子帧数 	 	
        //        int ISubframeNum;
        /// HARQ最大发送次数
        int IHARQMaxTransNum;
        /// OLLA外环调整的目标BLER 	 	
        double DBlerTarget;
        /// 正比公平调度器的时间窗口长度 	 	
        int ISchedulerWindowLength;
        /// 正比公平调度器的公平性因数
        double DProportionFairFactor;
        /// 下行HARQ进程数
        int IHARQProcessNum;

        int Ik0_slot;

        int Imink1_slot;

        int IN1_OS;

        int Imink3_slot;
    } DL;

    struct {
        int IHARQProcessNum;
    } UL;

    FrameStructure FrameStructure4Sim; //仿真使用桢结构 	
    FrameStructure FrameStructure4Statis; //统计使用桢结构 	
    /// 帧长度
    int IFrameLength; //cal
};

struct PTRAFFIC {
    /// 	 	
    int ITrafficModel;
    double DPacketSize;
    double DPacketSize_2;
    double DMacroLamda;
    double DMacroLamda_2;
    int IQoSLevel;
};

struct PXR {
    double dPeriod_ms;
    double dS; //13625; 	
    double dPacketSizeDefault_Kbit; //15000; 	
    double dPmax; //150000; 	
    double dPave; //104167 Bytes; 	
    double SumRate;
    int FPS;
    double alpha;
    double dDelayBuget_ms;
    int iDelayBudget_slot;

    int iLogPeriod_slot;
};

struct PLINK_CONTROL {
    /// 移动台到基站之间的信道强链路的个数
    int IStrongLinkNum;
    /// 	 	
    int IFrequencySpace;
    /// 是否使用快衰落的标记 	 	
    int IISFastFadingUsed;
    /// 	 	
    int I2DOr3DChannel;
    /// 是否使用阴影衰落的标记
    int IISShadowFadingUsed;

    int Islot4Hupdate;
};

struct PMIMO_CONTROL {
    int IMaxRankNum;
    /// 	 	
    int I2Dor3DMIMO;
    ///下行算信道矩阵用的方法选择(0----H;1----CovR)
    int IHorCovR;
    /// 2D-MIMO映射矩阵 	 	
    int I2DMatrixmode;
    ///Subarr还是Fullconnection接入 	 	
    int SubarrORFullconnec;
    /// 	 	
    int IEnable_Portbased_BF_CSIRS;
    /// 	 	
    int BF_CSIRS_PortNum;
    /// the MIMO mode 	 	
    int IMIMOMode;
    /// 	 	
    int IMUMIMOScheduleMethod;
    /// MIMO方案(0----Spatial-Based;1----Panel-Based)
    int ISpatialBasedOrPanelBased;
    /// 	 	
    int IMUMIMONum;
    // 基站策最大支持的总流数 	
    int IMaxLayerNum_BS;
    /// 选择 Detector Mode 	 	
    int IDetectorMode;
    /// 选择 MMSE的Option 	 	
    int IMMSEOption;
    int IApSounding;

    int PMI_based_Precoding;
    int Total_TXRU_Num;
};

struct PERROR_MODEL {
    /// 基站端的天线校准误差 	 	
    int IAntennaCalibrationError;
    /// CQI消息的反馈周期
    int ICQI_PERIOD;
    /// CQI消息的时延
    int ICQI_DELAY;
    /// CQI的计算方法
    int IRI_Meathod;

    /// Sounding消息的周期
    int ISRS_PERIOD;
    /// 	 	
    int ISRS_COMB;
    /// Sounding误差是否建模 	 	
    int ISRS_Error;

    /// Sounding消息在每个slot中占用的符号数目 	 	
    double DSRS_OSNumPerSlot;
    double DSRS_SystemBW_MHz;
    double DSRS_SRS_UnitBW_MHz;

    /// Sounding消息的带宽
    int ISRS_WIDTH;

    /// Sounding子载波发送间隔
    int ISRS_TxSpace;
    /// Sounding消息的时延
    int ISRS_DELAY;
    ///Sounding的发送模式
    int ISRS_TxMode;

    /// DMRS误差是否建模 	 	
    int IDMRS_Error;

    ///是否开启信道误差建模
    int IChannelMatrixError;
    struct{
        //上行信道误差建模
        int IChannelMatrixError;
    }UL;

    /// EVM误差 	 	
    double DEVMGateDb;
};

struct PMacro {
    
    int ILOSType;
    double DPenetrationLossdB;
    double DOTA_dB;
    double DMSBodyLoss_dB;
    double DTransmissionLineLoss_dB;
    double DNLOS_FreFadingFactor;
    double DElectricalTiltDeg;
    //    int IPolarize;//out 	 	
    /// 基站之间的距离
    double DSiteDistanceM;
    /// 基站天线高度 	 	
    double DAntennaHeightM;
    /// 选择天线pattern模式（0.全向；1.水平垂直都定向；2.水平全向垂直定向）
    int IAntennaPatternMode;
    /// 基站天线的下倾角 	 	
    double DMechanicalTiltDeg;

    int IHPanelNum; // Ng 	 	
    int IVPanelNum; // Mg 	 	
    int IHAntNumPerPanel; // N 	 	
    int IVAntNumPerPanel; // M 	 	

    int Polarize_Num; // P 	 	

    int H_TXRU_DIV_NUM_PerPanel; // Np, 将面板内水平阵子分成 Np 份，每份包含一个TXRU通道
    int V_TXRU_DIV_NUM_PerPanel; // Mp 	 	

    /// 	 	
    double DCellRadiusM; //cal 	 	
    /// 水平方向总的天线振子数（双极化） 	 	
    int IHTotalAntNum; //cal 	 	
    /// 基站天线数
    int ITotalAntNum; //cal 	 	
    /// 天线垂直方向间距 	 	
    double DVAntSpace; // dV 	 	
    /// 天线间距 	 	
    double DHAntSpace; // dH 	 	
    /// 天线水平3DB宽度
    double DH3DBBeamWidthDeg;
    /// 天线垂直3DB宽度
    double DV3DBBeamWidthDeg;
    /// 基站天线增益 	 	
    double DAntennaGainDb;
    /// 基站天线水平背向损耗
    double DHBackLossDB;
    /// 基站天线垂直背向损耗
    double DVBackLossDB;

    struct PBTS_DL {
        /// 基站的最大发射功率
        double DMaxTxPowerDbm;
        /// 基站的最大发射功率
        double DMaxTxPowerMw; //cal 	 	
        /// 基站子载波上的发射功率
        double DSCTxPowerMw; //cal 	 	
        bool bEnable_BSAnalogBF;
        int IschedulerthreadNum;
    } DL;

    struct PBTS_UL {
        /// 噪声指数 	 	
        double DNoiseFigureDb;
        /// 每个子载波上的噪声功率，mW 	 	
        double DSCNoisePowerMw; //cal 	 	
        /// 在整个带宽上的噪声功率，mW 	 	
        double DNoisePowerMw; //cal
    } UL;

    struct {
        double DRadioFrequencyMHz;
        /// 系统载波频率对应的波长
        double DWaveLength; //cal 	 	
        /// 移动台到基站的最小距离
        double DMinDistanceM;
    } LINK;

    struct {

        int iVBSBeamNum, iHBSBeamNum;
        double dStartVBSBeamDeg, dStartHBSBeamDeg;
        double dSpaceVBSBeamDeg, dSpaceHBSBeamDeg;
        std::vector<double> vBSetiltRAD, vBSescanRAD;

        int iVUEBeamNum, iHUEBeamNum;
        double dStartVUEBeamDeg, dStartHUEBeamDeg;
        double dSpaceVUEBeamDeg, dSpaceHUEBeamDeg;
        std::vector<double> vUEetiltRAD, vUEescanRAD;
    } ANALOGBEAM_CONFIG;
};

struct PSmallCell {
    /// 基站之间的距离
    double DSiteDistanceM;
    /// 小区半径 	 	
    double DCellRadiusM;
    /// 基站天线高度 	 	
    double DAntennaHeightM;
    /// 选择天线pattern模式（0.全向；1.水平垂直都定向；2.水平全向垂直定向）
    int IAntennaPatternMode;
    /// 基站天线的下倾角 	 	
    double DDownTiltDeg;
    int IPolarize;
    int IHPanelNum;
    int IVPanelNum;
    int IHAntNumPerPanel;
    int IVAntNumPerPanel;
    /// 水平方向总的天线振子数（双极化） 	 	
    int IHTotalAntNum;
    /// 基站天线数
    int ITotalAntNum;
    /// 天线垂直方向间距 	 	
    double DVAntSpace;
    /// 天线间距 	 	
    double DHAntSpace;
    /// 天线水平3DB宽度
    double DH3DBBeamWidthDeg;
    /// 天线垂直3DB宽度
    double DV3DBBeamWidthDeg;
    /// 基站天线增益 	 	
    double DAntennaGainDb;
    /// 基站天线水平背向损耗
    double DHBackLossDB;
    /// 基站天线垂直背向损耗
    double DVBackLossDB;

    struct PBTS_DL {
        /// 基站的最大发射功率
        double DMaxTxPowerDbm;
        /// 基站的最大发射功率
        double DMaxTxPowerMw;
        /// 基站子载波上的发射功率
        double DSCTxPowerMw;
    } DL;

    struct PBTS_UL {
        /// 噪声指数 	 	
        double DNoiseFigureDb;
        /// 每个子载波上的噪声功率，mW 	 	
        double DSCNoisePowerMw;
        /// 在整个带宽上的噪声功率，mW 	 	
        double DNoisePowerMw;
    } UL;

    struct {
        double DRadioFrequencyMHz;
        /// 	 	
        double DWaveLength;
        /// 移动台到基站的最小距离
        double DMinDistanceM;
        double DPicoOutBiasDb;

        double DUEProbInPicoZone4b;
        int IPicoNumPerMacro;
        int IPicoNumPerMacro4b;
        int IPicoNumPerMacroNormal;
        int IPicoUENumPerCell4b;
        int IPicoUENumPerCellNormal;
        double DPicoMaxTxPowerDbm;
        double DPicoRadiusM;
        double DMinDistPico2MacroM;
        double DMinDistPico2PicoM;
        double DMinDistPico2UEM;
        double DMinDistCluster2MacroM;
        double DMinDistClusterRadiusM_PicoDrop;
        double DMinDistClusterRadiusM_UEDrop;
        int IHetnetEnvironmentType;
        double DPicoAntennaGainDbi;
        double DPicoAntennaHeightM;
        int IPicoAntennaNum;
        ///@xlong 	 	
        double DPicoOutMaxTxPowerMw;
        ///@xlong 	 	
        double DPicoOutSCTxPowerMw;
        /// 	 	
        int IPicoNumPerCluster_Cluster;
        int IPicoClusterNumPerMacro_Cluster;
        int IUENumPerCell_Cluster;
        double DUEProbInCluster_Cluster;
    } LINK;

    struct {
        int iVBSBeamNum, iHBSBeamNum;
        double dStartVBSBeamDeg, dStartHBSBeamDeg;
        double dSpaceVBSBeamDeg, dSpaceHBSBeamDeg;
        std::vector<double> vBSetiltRAD, vBSescanRAD;

        int iVUEBeamNum, iHUEBeamNum;
        double dStartVUEBeamDeg, dStartHUEBeamDeg;
        double dSpaceVUEBeamDeg, dSpaceHUEBeamDeg;
        std::vector<double> vUEetiltRAD, vUEescanRAD;
    } ANALOGBEAM_CONFIG;
};

struct PMSS {

    
    //移动台所在高楼的楼层区间[IMinBuildingFloor,IMaxBuildingFloor] 	
    int IMinBuildingFloor;
    int IMaxBuildingFloor;

    /// 移动台天线高度
    double DAntennaHeightM;
    /// 移动台的移动速度 	 	
    double DVelocityMPS; //out 	 	
    /// 30G场景下，小尺度几个UEPanel 	 	
    int IInitialize1or2Panel;
    int IHPanelNum; //Ng
    int IVPanelNum; //Mg
    int Polarize_Num; // P
    int H_TXRU_DIV_NUM_PerPanel; // Np, 将面板内水平阵子分成 Np 份，每份包含在1个 TXRU 中
    int V_TXRU_DIV_NUM_PerPanel; // Mp
    double DProbLowloss;
    //outdoor 概率 	 	
    double DProbOutdoor;

    // 1: Outdoor is In Car; 0 : Outdoor is not In Car 	 	
    bool bIsOutdoorInCar_notOnCar;

    // 室外用户速度扩展因子 	 	
    double DCarSpeedSacleFactor;

    struct {
        bool bEnable_UEAnalogBF; ///add to set 	 	
        int IAntennaPatternMode; ///add to set,0-Omni 1-Sector 2-Sector-InH 	 	
        //        int IPolarize; //out 	 	
        int IHPanelNum; //Ng 	 	
        int IVPanelNum; //Mg 	 	
        int IHAntNumPerPanel;
        int IVAntNumPerPanel;
        /// 移动台天线数 	 	
        int ITotalAntNumPerPanel; //cal 	 	
        int ITotalAntNum; ////////cal 	 	
        ///天线水平间距 	 	
        double DVAntSpace; //dV 	 	
        /// 天线间距 	 	
        double DHAntSpace; //dH 	 	


        int Polarize_Num; // P 	 	

        int H_TXRU_DIV_NUM_PerPanel; // Np, 将面板内水平阵子分成 Np 份，每份包含一个TXRU 通道
        int V_TXRU_DIV_NUM_PerPanel; // Mp
        int Total_TXRU_Num;



        ///天线垂直3Db带宽 	 	
        double DV3DBBeamWidthDeg;
        ///天线水平3Db带宽 	 	
        double DH3DBBeamWidthDeg;
        /// 移动台天线增益
        double DAntennaGainDb;
        /// 基站天线水平背向损耗
        double DHBackLossDB;
        /// 基站天线垂直背向损耗
        double DVBackLossDB;

        //add for redcap embb coexist
        int IHPanelNum_2; //Ng 	
        int IVPanelNum_2; //Mg 	
        int IHAntNumPerPanel_2;
        int IVAntNumPerPanel_2;
        /// 移动台天线数 	
        // int ITotalAntNumPerPanel_2; //cal 	
        // int ITotalAntNum_2 ////////cal 	
        ///天线水平间距 	
        double DVAntSpace_2; //dV 	
        /// 天线间距 	
        double DHAntSpace_2; //dH 	
        int Polarize_Num_2; // P 	
        int H_TXRU_DIV_NUM_PerPanel_2; // Np, 将面板内水平阵子分成 Np 份，每份包含一个TXRU通道
        int V_TXRU_DIV_NUM_PerPanel_2; // Mp 
        /// 移动台天线增益
        double DAntennaGainDb_2;	
        ///天线垂直3Db带宽 	
        double DV3DBBeamWidthDeg_2;
        ///天线水平3Db带宽 	
        double DH3DBBeamWidthDeg_2;
        /// 基站天线水平背向损耗
        double DHBackLossDB_2;
        /// 基站天线垂直背向损耗
        double DVBackLossDB_2;  
        
        
        struct {
            /// 每个子载波上的噪声功率，mW 	 	
            double DSCNoisePowerMw; //cal 	 	
            /// 噪声指数 	 	
            double DNoiseFigureDb;
            /// 在整个带宽上的噪声功率，mW 	 	
            double DNoisePowerMw; //cal
        } DL;

        struct {
            /// 功率控制参数P0 	 	
            double DP0Dbm4SRS; //out 	 	
            /// 功率控制参数alfa 	 	
            double DAlfa4SRS; //out 	 	
            /// 移动台最大发射功率
            double DMaxTxPowerDbm;
            /// 移动台最大发射功率
            double DMaxTxPowerMw; //cal
        } UL;
    } FirstBand;

    struct {
        bool bEnable_UEAnalogBF; ///add to set 	 	
        int IAntennaPatternMode; ///add to set,0-Omni 1-Sector 2-Sector-InH 	 	
        int IPolarize;
        int IHPanelNum;
        int IVPanelNum;
        int IHAntNumPerPanel;
        int IVAntNumPerPanel;
        /// 移动台天线数 	 	
        int ITotalAntNumPerPanel; //名称有误？实际上是使用的UE的天线数目？ 	 	
        int ITotalAntNum; ////////NEED TO ADD 	 	
        ///天线水平间距 	 	
        double DVAntSpace;
        /// 天线间距 	 	
        double DHAntSpace;
        ///天线垂直3Db带宽 	 	
        double DV3DBBeamWidthDeg;
        ///天线水平3Db带宽 	 	
        double DH3DBBeamWidthDeg;
        /// 移动台天线增益
        double DAntennaGainDb;
        /// 基站天线水平背向损耗
        double DHBackLossDB;
        /// 基站天线垂直背向损耗
        double DVBackLossDB;

        struct {
            /// 每个子载波上的噪声功率，mW 	 	
            double DSCNoisePowerMw;
            /// 噪声指数 	 	
            double DNoiseFigureDb;
            /// 在整个带宽上的噪声功率，mW 	 	
            double DNoisePowerMw;
        } DL;

        struct {
            /// 功率控制参数P0 	 	
            double DP0Dbm4SRS;
            /// 功率控制参数alfa 	 	
            double DAlfa4SRS;
            /// 移动台最大发射功率
            double DMaxTxPowerDbm;
            /// 移动台最大发射功率
            double DMaxTxPowerMw;
        } UL;
    } SecondBand;


};

struct PRIS {
   double DPenetrationLossdB;
    double DOTA_dB;
    double DMSBodyLoss_dB;
    double DTransmissionLineLoss_dB;

    double DElectricalTiltDeg;
    //    int IPolarize;//out
    /// 基站之间的距离

    /// 基站天线高度
    double DAntennaHeightM;
    /// 选择天线pattern模式（0.全向；1.水平垂直都定向；2.水平全向垂直定向）
    int IAntennaPatternMode;
    /// 基站天线的下倾角
    double DMechanicalTiltDeg;


    int IHPanelNum; // Ng
    int IVPanelNum; // Mg
    int IHAntNumPerPanel; // N
    int IVAntNumPerPanel; // M

    int Polarize_Num; // P

    int H_TXRU_DIV_NUM_PerPanel; // Np, 将面板内水平阵子分成 Np 份，每份包含在1个 TXRU 中
    int V_TXRU_DIV_NUM_PerPanel; // Mp

    ///
    double DCellRadiusM; //cal
    /// 水平方向总的天线振子数（双极化）
    int IHTotalAntNum; //cal
    /// 基站天线数
    int ITotalAntNum; //cal
    /// 天线垂直方向间距
    double DVAntSpace; // dV
    /// 天线间距
    double DHAntSpace; // dH
    /// 天线水平面3DB宽度
    double DH3DBBeamWidthDeg;
    /// 天线垂直面3DB宽度
    double DV3DBBeamWidthDeg;
    /// 基站天线增益
    double DAntennaGainDb;
    /// 基站天线水平背向损耗
    double DHBackLossDB;
    /// 基站天线垂直背向损耗
    double DVBackLossDB;
    ///RIS是否进行小尺度仿真
    double IS_SmallScale;
    ///MMSE等效RIS影响
    bool dMMSE_RIS;
    ///RIS选择以及调相模式
    bool is_BestBTS2MSBeam;
    bool is_DoubleBest;
    bool is_BestBeam;
    bool is_BestCase;
    bool is_Max_UE_RIS;
    bool is_BestMiddle;
    bool is_New;
    bool is_baseline;
    //调度RIS增益来自哪种模式 0-is_New 1-is_BestCase
    bool com_case;
    int _case;
    double DistributeHexagon_Min_Radiu;    //六边形撒点内径
    double DistributeHexagon_Max_Radiu;    //六边形撒点外径

    struct {
        /// 基站的最大发射功率
        double DMaxTxPowerDbm;
        /// 基站的最大发射功率
        double DMaxTxPowerMw; //cal
        /// 基站子载波上的发射功率
        double DSCTxPowerMw; //cal
        bool bEnable_BSAnalogBF;
    } DL;

    struct {
        /// 噪声指数
        double DNoiseFigureDb;
        /// 每个子载波上的噪声功率，mW
        double DSCNoisePowerMw; //cal
        /// 在整个带宽上的噪声功率，mW
        double DNoisePowerMw; //cal
    } UL;

    struct {
        double DRadioFrequencyMHz;
        /// 系统载波频率对应的波长
        double DWaveLength; //cal
        /// 移动台到基站的最小距离
        double DMinDistanceM;

    } LINK;

    struct {
        int iVRISBeamNum, iHRISBeamNum;
        double dStartVRISBeamDeg, dStartHRISBeamDeg;
        double dSpaceVRISBeamDeg, dSpaceHRISBeamDeg;
        std::vector<double> vRISetiltRAD, vRISescanRAD;
    } ANALOGBEAM_CONFIG;
};

struct PBTS_UL {
    struct {
        /// 选择调度算法0--PF;1--RR 	
        int IScheduleMode;
        /// 正比公平调度器的时间窗口长度
        int ISchedulerWindowLength;
        /// 正比公平因子 	
        double dPFfactor;
        /// 噪声指数 	
        double DNoiseFigureDb;
        /// 每个子载波上的噪声功率，mW 	
        double DSCNoisePowerMw;
        /// 在整个带宽上的噪声功率，mW 	
        double DNoisePowerMw;
        /// 选择 Detector Mode 	
        int IDetectorMode;
    } UL;
};

/// @brief 记录移动台参数的结构体 	

struct PMSS_UL {
    /// Sounding消息的时延 	
    int ISRS_DELAY;
    /// UE类型 	
    int iMSCategory;

    struct {
        /// 下行时移动台天线数 	
        //        int IAntennaNum;
        /// 选择 Detector Mode 	
        int IDetectorMode;
        /// 每个子载波上的噪声功率，mW 	
        double DSCNoisePowerMw;
        /// 噪声指数 	
        double DNoiseFigureDb;
        /// 在整个带宽上的噪声功率，mW 	
        double DNoisePowerMw;
    } DL;

    struct {
        //////////////20220828 cwq 闭环功控参数
        bool CLPCofPUSCH;
        bool UserStrategy;
        bool TPC_Accumulation;
        bool ResetKey;
        int ResetPeriod;
        double HighTargetPL;
        double LowTargetPL;
        double HighTargetSINR;
        double MidTargetSINR;
        double LowTargetSINR;
        double TargetSINRMargin;
        ///////////////////
        //PHR参数
        bool IsPhrOn;
        int PeriodicPeriod;
        int ProhibitPeriod;
        double PowerFactorChange_dB;

        //SR参数
        bool IsSROn;
        double SR_ProhibitPeriod;
        int SR_TransMax;
        int sr_Periodicity_slot;

        int SR_PRBNum;
        int SR_PUCCHFormat;
        int SR_InitCyc;
        int SR_Format1_OCC;

        bool SR_PeriodSwitch;
        bool SR_RBNumSwitch;

        bool SR_SmartSchSwitch;         //SR智能调度开关，{off，on}

        /////BSR参数///
        int BSR_RetransTimer; //BSR重传计数器，单位：子帧
        int BSR_PeriodicTimer; //BSR周期计时器，单位：子帧

        ///////////////
        /// 功率控制参数P0 	
        double DP0Dbm;
        /// 功率控制参数alfa 	
        double DAlfa;
        /// 功率控制参数P0 	
        double DP0Dbm4SRS;
        /// 功率控制参数alfa 	
        double DAlfa4SRS;
        /// 移动台最小发射功率 	
        double DMinTxPowerDbm;
        /// 移动台最大发射功率 	
        double DMaxTxPowerDbm;
        /// 移动台最大发射功率 	
        double DMaxTxPowerMw;
        /// Sounding消息的周期 	
        int ISRS_PERIOD;
        ///SRS带宽---候选值1，2，4分别表示全带宽，半带宽，1/4带宽 	
        int ISRSBandWidthIndex;
    } UL;
};

/// @brief 记录链路参数的结构体 	

struct PLINK_UL {
    /// 选择EESM或MIESM的标记 	
    int ISEESMorMIESM;
    /// 噪声功率谱密度 	
    double DNoisePowerSpectrumDensityDbmHz;
};

/// @brief 记录仿真参数的结构体 	

struct PSIM_UL {
    struct {
        /// 下行仿真时的子载波数 	
        int ISCNum;
        /// 下行仿真时的RB的个数 	
        int IRBNum;
        /// SB的个数 	
        int ISBNum;
        /// 在一个Subband中RB的数目 	
        int ISBSize;
        /// OLLA外环调整的目标BLER 	
        double DBlerTarget;
    } DL;

    struct {
        /// 上行链路的子帧数 	
        //        int ISubframeNum;
        /// HARQ最大发送次数 	
        int IHARQMaxTransNum;
        /// 上行HARQ进程数 	
        int IHARQProcessNum;
        /// the MIMO mode 	
        int IMIMOMode;
        /// OLLA外环调整的目标BLER 	
        double DBlerTarget;
        ///MU MIMO用户数 	
        int IMSnumOfMUMIMO;
        int RatePairMaxMode;
        /// 调度过程中边缘用户与中心用户的判决门限，高于门限的用户认为是中心用户，参与配对，低于门限的认为是边缘用户，不参与配对
        double RBSupportedNumDoor;
        /// EVM参数 	
        double DEVMdb;
        ///开启MMSE option 1 算法 	
        bool BIsMMSEOptionOneOn;
        ///多用户配对第一轮SU资源分配方式选择 	
        int FirstTimeMode;
        ///VMIMO 配对模式选择 	
        int IvmimoAdaptiveMode;
        ///多用户配对第二轮配对方法选择 	
        int RateGainMode;
        ///different interference load 	
        double dLoad;
        /// rb segments num 	
        int iRBsegmentNum;
        int imaxRBsegNum;
        ///Maxium time of number finding higher riority MS among the RBSet 	
        int iMaxRBFindNum;
        //        double dO2Iratio;

        //        int iRBSchLevel; //nml 	

        //zhengyi 	
        bool BisMUollaOn;

        struct {
            ///DMRS 信道估计误差 	
            bool BIsDMRSErrorOn;
            ///SRS信道误差 	
            bool BIsSRSErrorOn;
            ///SRS信道误差参数 	
            double DDeltaMSEDb;
            ///DMRS error options 	
            ///0 for DMRS old error 	
            ///1 for SRS error apply to DMRS 	
            int IDmrsErrorOptions;

            // zhengyi  	
            bool BIsErrorEstimationOn;
        } ERROR;
    } UL;
};
//上行添加_end 	


//AMC参数 20221101 cwq
struct AMC_Para
{
    bool SelfConfigOlla;
    int ACKSchedWindow;
    int NACKSchedWindow;
    int ACKThred;
    int NACKThred;
    double IinitialSINR;
    double SINRUpStep;
    double SINRDownStep;
    double CQIFilterFactor;
};
struct DRX_Para
{
    bool Switch;
    int LongCycle;
    int OnDurationTimer;
    int InactivityTimer;
    bool ShortSwitch;
    int ShortCycle;
    int ShortCycleTimer;
};

struct DaHuaWu_Para
{
    int iBSNum;
    int iMsPerBTS;
    int iSimulationDropNum;
    int iSlotPerDrop;
    int iSystemBandWidth;
    int iSubCarrierSpace;
    int iTxAntennaPortNum;
    int iRxAntennaPortNum;

    int iOccupiedSymbolNum;         //PDCCH占用的符号数, {1、2、3}
    int iOccupiedRbNum;             //PDCCH所在符号上占用的RB数, [0~272]
    bool bPdcchAlgoEnhSwitch;       //聚合级别初始选择优化开关, {off，on}
    bool bPdcchAggLvlAdaptPol;      //PDCCH聚合级别自适应策略, {JOINT_ADAPT，SEPARATE_ADAPT}
    bool bRateMatchSwitch;          //PDCCH RateMatch开关, {off，on}
    bool bDl256QamSwitch;           //下行256QAM开关, {off，on}
    bool bSuMimoMultipleLayerSw_DL; //下行SU-MIMO多流开关, {off，on}
    bool bSuMimoMultipleLayerSw_UL; //上行SU-MIMO多流开关, {off，on}
    bool bMuMimoSwitch_DL;          //下行MU-MIMO开关, {off，on}
    bool bMuMimoSwitch_UL;          //上行MU-MIMO开关, {off，on}
    bool bNrDuCellDrxAlgoSwitch;    //DRX开关, {off，on}
    enum
    {
        RATIO_FIXED,
        RATIO_OPT,
        RATIO_30 = 30,              //百分比
        RATIO_35 = 35,
        RATIO_40 = 40,
        RATIO_45 = 45,
        RATIO_50 = 50,
        RATIO_55 = 55,
        RATIO_60 = 60,
        RATIO_65 = 65,
        RATIO_70 = 70,
        RATIO_75 = 75,
        RATIO_80 = 80,
        RATIO_85 = 85,
        RATIO_90 = 90
    }HeavyLoadUlCceAdjPolicy;       //重载场景上行CCE调整策略

};






/// @brief 系统参数配置
/// 	 	
/// 从外部文件中读取仿真参数，并产生导出参数，供程序的其它部分读取使用

class Parameters {
private:
    /// 从OverWrite参数的字符串到参数地址用的映射表，实现范型读参数函数的重要部件
    std::map<std::string, boost::any> m_Str2ParaMap_DL;

    std::map<std::string, boost::any> m_Str2ParaMap_UL;


private:
    ///20260115
    void Build_Scene();
    /// @brief 读取Scene参数
    void ReadSceneData();
    ///计算开销的函数(弃用）
    void CalculateOH();
    ///计算每个PRB中可用的RE数
    /// 38.214 5.1.3.2 Transport block size determination
    void CalNRE();
    ///将上行的参数赋值给下行
    void ParametersPassToDLFromUL();
    /// @brief 建立字符串和参数之间的关系
    void Build_DL();
    /// @brief 读取输入参数 	 	
    void ReadInputData_DL();
    /// @brief 读取Case覆盖参数 	 	
    void ReadOverWriteParameters_DL();
    /// @brief 计算推导参数 	 	
    void CalculateDerivedParameters_DL();

    //没有用 	
    /// @brief 读取基本参数 	 	
    void ReadBasicData();
    /// @brief 读取基站参数 	 	
    void ReadBaseStationData();
    /// @brief 读取移动台参数
    void ReadMobileStationData();
    /// @brief 读取链路参数 	 	
    void ReadLinkData();
    /// @brief 读取无线资源参数 	 	
    void ReadSimData();
    //没有用 	


    /// @brief 设置场景参数 	 	
    void SetEnvironmentTypeParameters();

    void Set_LowFreq_InH();
    void Set_LowFreq_UMI();
    void Set_LowFreq_UMA();
    void Set_LowFreq_RMA();
    //20200115 	
    void Set_HighFreq_InF();

    void Set_HighFreq_InH();
    void Set_HighFreq_UMI();
    void Set_HighFreq_UMA();
    void Set_HighFreq_RMA();

    void Set_5G_1Layer();
    void Set_5G_2Layer();

    /// @brief 泛型的读参函数
    void ReadData_DL(char const* _pFilename);
    void ReadData_UL(char const* _pFilename);
    /// @brief 将仿真参数输出到系统指定的文件
    void RecordParameters(char const* _pFilename);

    /// @brief 重新排序输入参数函数 	 	
    void ReorderData(char const* _pFilename);
    void RecordParameters_New(const string& _pFilename);
    /// @brief 重新排序输入参数函数 	 	
    void ReorderCaseParameter();

    //上行添加_begin 	
    /// @brief 建立字符串和参数之间的关系 	
    void Build_UL();
    /// @brief 读取输入参数 	
    void ReadInputData_UL();
    /// @brief 计算推导参数 	
    void CalculateDerivedParameters_UL();
    /// @brief 读取Case覆盖参数 	
    void ReadOverWriteParameters_UL();
    //上行添加_end 	

    int CalcPRBNum(int _DSystemBandWidthKHz, int _DCarrierWidthHz);

public:
    POVERHEAD OVERHEAD;
    PBASIC BASIC;
    PMSS MSS;
    PLINK_CONTROL LINK_CTRL;
    PMIMO_CONTROL MIMO_CTRL;
    PSIM SIM;
    PTRAFFIC TRAFFIC;
    PXR XR;
    PERROR_MODEL ERROR;

    PSmallCell SmallCell;
    PMacro Macro;
    PRIS RIS;
    PBTS_UL BTS_UL;
    PMSS_UL MSS_UL;
    PLINK_UL LINK_UL;
    PSIM_UL SIM_UL;
    //上行添加_end

    AMC_Para AMC;
    DRX_Para DRX;
    bool CSI_data_set_on;
    bool AI_model_on;
    int SubBand_Size;

    DaHuaWu_Para DaHuaWu;


    static const int SymbolNumPerSlot = 14;
    int CoresetDuration;
    static const int DMRS_SingleSymbol = 1;
    static const int DMRS_DoubleSymbol = 2;

    //上行添加_begin 	
    static const int LTE = 0;
    static const int WIMAX = 1;
    //上行添加_end 	

    static const int GAP = 0;
    static const int DL = 1;
    static const int UL = 2;
    static const int ULandDL=3; 

    //上行添加_begin 	
    static const int EESM = 0;
    static const int MIESM = 1;
    //上行添加_end 	


    static const int LTEMCSNumDL = 28;
    static const int LTEMCSNumUL = 29;
    //上行添加_begin 	
    static const int WIMAXMCSNum = 16;
    //上行添加_end 	

    static const int ProportionalFairScheduler = 0;
    static const int RoundRobinScheduler = 1;
    //上行添加_begin 	
    static const int VmimoSchedulerUL = 2;
    //上行添加_end 	

    static const int SpatialBased = 0;
    static const int PanelBased = 1;

    static const int IDetectorMode_MRC = 0;
    static const int IDetectorMode_IRC = 1;
    static const int IDetectorMode_SimpleMMSE = 2;
    static const int IDetectorMode_MMSE = 3;
    //上行添加_begin 	
    static const int IDetectorMode_MMSE_Precoding_Debug = 4;
    static const int IDetectorMode_MRC_Debug = 5;
    //上行添加_end 	

    //没有用_begin 	
    static const int IMIMOMode_MRC = 0;
    //    static const int IMIMOMode_PrecodeRank1 = 1; 	
    //    static const int IMIMOMode_PrecodeRankA = 2; 	
    //    static const int IMIMOMode_TransPrecodeRank1 = 3; 	
    //    static const int IMIMOMode_TransPrecodeRankA = 4; 	
    //    static const int IMIMOMode_EBBRank1 = 5; 	
    //    static const int IMIMOMode_EBBRankA = 6; 	
    //    static const int IMIMOMode_LTBF = 7; 	
    //    static const int IMIMOMode_MUTransPrecode = 8; 	
    //    static const int IMIMOMode_MULTBF = 9; 	
    //没有用_begin 	

    static const int IMIMOMode_MUBF = 10;
    static const int IMIMOMode_MUBFRankA = 11;
    static const int IMIMOMode_JPCoMP = 12;
    //上行添加_begin 	
    static const int IMIMOMode_EBB = 1; //use for DL 	
    static const int IMIMOMode_Precoding = 2; //use for DL 	
    static const int IMIMOMode_Precoding_Debug = 3; //use for DL 	
    static const int IMIMOMode_TransformedPrecode = 4; //use for DL 	
    static const int IMIMOMode_VirtualMIMO = 6; //use for UL 	
    static const int IMIMOMode_COMP = 7; //use for UL 	
    //上行添加_end 	

    static const int DistributeMS_Hexagonal = 0;
    static const int DistributeMS_Diamond = 1;
    static const int DistributeMS_CenterRetangle = 2;
    static const int DistributeMS_File = 3;
    static const int DistributeMS_fix = 4;

    static const int IAntennaPattern_OmniAntenna = 0;
    // static const int IAntennaPattern_Sector_UEAntenna = 0; 	 	
    static const int IAntennaPattern_SectorAntenna = 1;
    //上行添加_begin 	
    static const int IAntennaPattern_SectorAntennaReal = 2;
    //上行添加_end 
    static const int IAntennaPattern_importAntennafile = 3;

    static const int ITrafficModel_FullBuffer = 0;
    static const int ITrafficModel_FTP = 1;
    static const int ITrafficModel_VoIP = 2;
    static const int ITrafficModel_FTP2 = 3;
    static const int ITrafficModel_FTP3 = 4;
    static const int ITrafficModel_XR = 5;
    static const int ITrafficModel_XRmulti = 6;
    static const int DataBaseTraffic = 7;

    static const int MixedTraffic = 100;

    static const int FTPPacket = 0;
    static const int XRSinglePacket = 1;
    static const int XRMultiP = 9;
    static const int XRMultiI = 13;




    static const int IHetnet_Normal = 1;
    static const int IHetnet_4b = 2;
    static const int IHetnet_Cluster = 3;

    //比较Parameters::Instance().BASIC.IScenarioModel 	 	
    static const int SCENARIO_LowFreq_INDOOR = 0;
    static const int SCENARIO_LowFreq_URBAN_MICRO = 1;
    static const int SCENARIO_LowFreq_URBAN_MACRO = 2;
    static const int SCENARIO_LowFreq_RURAL_MACRO = 3;
    static const int SCENARIO_HighFreq_INDOOR = 4;
    static const int SCENARIO_HighFreq_URBAN_MICRO = 5;
    static const int SCENARIO_HighFreq_URBAN_MACRO = 6;
    static const int SCENARIO_HighFreq_RURAL_MACRO = 7;
    static const int SCENARIO_5G_DENSE_URBAN_1LAYER = 8;
    static const int SCENARIO_5G_DENSE_URBAN_2LAYER = 9;
    //20200115 	
    static const int SCENARIO_HighFreq_INDOOR_FACTORY = 10;

    ///20171205 下面用于子信道判断for UMA 信道
    static const int SCENARIO_Phase1_UMA_mMTC = 10;
    static const int SCENARIO_Phase1_UMA_URLLC = 11;
    static const int SCENARIO_Phase1_DenseUrban_4G = 12;
    static const int SCENARIO_Phase2_UMA_2dot6G = 13;
    static const int SCENARIO_Phase2_UMA_3dot5G = 14;
    static const int SCENARIO_Phase2_DenseUrban = 15;

    static const int SCENARIO_NR_UMA_700M_350m_25m_MeTilt9 = 301;
    static const int chty211130=0;

    ///20171220 下面用于子信道判断for RMA 信道
    static const int SCENARIO_Phase1_RMA_NORMAL = 16;
    static const int SCENARIO_Phase1_RMA_LMLC = 17;
    static const int SCENARIO_Phase2_RMA = 18;
    static const int SCENARIO_Phase1_InH_LF = 19;
    static const int SCENARIO_Phase2_InH_LF = 20;

    static const int SERVICEAREA_HEXAGONAL7CELL = 0;
    static const int SERVICEAREA_HEXAGONAL19CELL = 1;
    static const int SERVICEAREA_MANHATTAN = 2;
    static const int SERVICEAREA_INDOORHOTSPOT = 3;

    static const int IS2DCHANNEL = 0;
    static const int IS3DCHANNEL = 1;

    static const int ISLOWFREQ = 0;
    static const int ISHIGHFREQ = 1;

    ////// 	 	
    //是否Apsouding 	 	
    static const int IApSounding = 1;
    static const int INoApSounding = 0;


    ///工作模式 	 	
    static const int WorkingMode_Normal = 1;
    static const int WorkingMode_ITU_Calibration_Phase1 = 2;

    /////// 	 	
    ///Singleton设计模式 	 	
public:
    /// @brief 返回唯一的参数类对象实例 	 	
    /// 	 	
    /// 因为本函数返回的是一个常引用,平台其它模块不能改变本类中的参数   
    /// @return a constant reference
    static const Parameters& Instance();

    void ConfigureArgs();
    void MapArgsToParameters();
private:


    /// @brief 构造函数
    Parameters();
    /// @brief 拷贝构造函数
    Parameters(const Parameters&);
    /// @brief 赋值构造函数
    Parameters & operator=(const Parameters&);
    /// @brief 析构函数 	 	
    ~Parameters() = default;
};