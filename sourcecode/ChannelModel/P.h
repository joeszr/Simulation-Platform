///@file P.h
///@brief 信道模型的参数类头文件
///@author wangfei
#pragma once
#include "libfiles.h"

namespace cm {
    /// @brief 存储发射器参数的结构体

    struct P_MacroTX {
        double DSiteDistanceM;
        double DAntennaHeightM;
        double DDownTiltDeg;
        int IPolarizedType;
        double DHAntSpace;
        double DVAntSpace;
        int IHPanelNum;
        int IVPanelNum;
        int IHAntNumPerPanel;
        int IVAntNumPerPanel;
        int IHTotalAntNum;
        int ITotalAntNumPerPanel;
        int ITotalAntNum;
    };

    struct P_PicoTX {
        double DAntennaHeightM;
        double DDownTiltDeg;
        int IPolarizedType;
        double DHAntSpace;
        double DVAntSpace;
        int IHPanelNum;
        int IVPanelNum;
        int IHAntNumPerPanel;
        int IVAntNumPerPanel;
        int IHTotalAntNum;
        int ITotalAntNumPerPanel;
        int ITotalAntNum;
    };
    /// @brief 存储接收器参数的结构体

    struct P_RX {
        double DAntennaHeightM;
        int IPolarizedType;
        double DHAntSpace;
        double DVAntSpace;
        int IHPanelNum;
        int IVPanelNum;
        int IHAntNumPerPanel;
        int IVAntNumPerPanel;
        int IHTotalAntNumPerPanel;
        int ITotalAntNumPerPanel;
        int ITotalAntNum;
    };

    /// @brief 存储链路参数的结构体

    struct P_FX {
        double DRadioFrequencyMHz_Macro;
        double DRadioFrequencyMHz_Pico;
        double DWaveLength_Macro;
        double DWaveLength_Pico;
        int ICarrierNum;
        int ICarrierSampleSpace;
        double DCarrierWidthHz;
        double DSampleRateHz;
        int IStrongLinkNum;
        bool IISShadowFadingUsed;
        int IISFastFadingUsed;
        int I2DOr3DChannel;
        double DDownTiltDeg;
    };

    /// @brief 存储LOS径参数的结构体

    struct P_Macro2UE_LOS {
        double DelaySpreadAVE;
        double DelaySpreadSTD;
        double AODSpreadAVE;
        double AODSpreadSTD;
        double AOASpreadAVE;
        double AOASpreadSTD;
        double ShadowFadingSTD;
        double KFactorDBAVE;
        double KFactorDBSTD;
        double DelayScaling;
        double XPR;
        double XPR_u;
        double XPR_sigma;
        int NumOfCluster;
        double ClusterASD;
        double ClusterASA;
        double ClusterDS;
        double PerClusterShadowingSTDDB;
        double DSCorrDistM;
        double ASDCorrDistM;
        double ASACorrDistM;
        double SFCorrDistM;
        double KCorrDistM;
        double EODSpreadAVE;
        double EODSpreadSTD;
        double EODoffsetAVE;
        double EOASpreadAVE;
        double EOASpreadSTD;
        double ClusterESD;
        double ClusterESA;
        double ESDCorrDistM;
        double ESACorrDistM;
        itpp::mat R;
    };

    /// @brief 存储NLOS径参数的结构体

    struct P_Macro2UE_NLOS {
        double DelaySpreadAVE;
        double DelaySpreadSTD;
        double AODSpreadAVE;
        double AODSpreadSTD;
        double AOASpreadAVE;
        double AOASpreadSTD;
        double ShadowFadingSTD;
        double DelayScaling;
        double XPR;
        double XPR_u;
        double XPR_sigma;
        int NumOfCluster;
        double ClusterASD;
        double ClusterASA;
        double ClusterDS;
        double PerClusterShadowingSTDDB;
        double DSCorrDistM;
        double ASDCorrDistM;
        double ASACorrDistM;
        double SFCorrDistM;
        double EODSpreadAVE;
        double EODSpreadSTD;
        double EODoffsetAVE;
        double EOASpreadAVE;
        double EOASpreadSTD;
        double ClusterESD;
        double ClusterESA;
        double ESDCorrDistM;
        double ESACorrDistM;
        itpp::mat R;
    };

    /// @brief 存储O2I（室外到室内）参数的结构体

    struct P_Macro2UE_O2I {
        double DelaySpreadAVE;
        double DelaySpreadSTD;
        double AODSpreadAVE;
        double AODSpreadSTD;
        double AOASpreadAVE;
        double AOASpreadSTD;
        double ShadowFadingSTD;
        double DelayScaling;
        double XPR;
        double XPR_u;
        double XPR_sigma;
        int NumOfCluster;
        double ClusterASD;
        double ClusterASA;
        double ClusterDS;
        double PerClusterShadowingSTDDB;
        double DSCorrDistM;
        double ASDCorrDistM;
        double ASACorrDistM;
        double SFCorrDistM;
        double EODSpreadAVE;
        double EODSpreadSTD;
        double EOASpreadAVE;
        double EOASpreadSTD;
        double ClusterESD;
        double ClusterESA;
        double ESDCorrDistM;
        double ESACorrDistM;
        itpp::mat R;
    };

    /// @brief 存储Pico-to-UE LOS径参数的结构体

    struct P_Pico2UE_LOS {
        double DelaySpreadAVE;
        double DelaySpreadSTD;
        double AODSpreadAVE;
        double AODSpreadSTD;
        double AOASpreadAVE;
        double AOASpreadSTD;
        double ShadowFadingSTD;
        double KFactorDBAVE;
        double KFactorDBSTD;
        double DelayScaling;
        double XPR;
        double XPR_u;
        double XPR_sigma;
        int NumOfCluster;
        double ClusterASD;
        double ClusterASA;
        double ClusterDS;
        double PerClusterShadowingSTDDB;
        double DSCorrDistM;
        double ASDCorrDistM;
        double ASACorrDistM;
        double SFCorrDistM;
        double KCorrDistM;
        double EODSpreadAVE;
        double EODSpreadSTD;
        double EODoffsetAVE;
        double EOASpreadAVE;
        double EOASpreadSTD;
        double ClusterESD;
        double ClusterESA;
        double ESDCorrDistM;
        double ESACorrDistM;
        itpp::mat R;
    };

    /// @brief 存储Pico-to-UE NLOS径参数的结构体

    struct P_Pico2UE_NLOS {
        double DelaySpreadAVE;
        double DelaySpreadSTD;
        double AODSpreadAVE;
        double AODSpreadSTD;
        double AOASpreadAVE;
        double AOASpreadSTD;
        double ShadowFadingSTD;
        double DelayScaling;
        double XPR;
        double XPR_u;
        double XPR_sigma;
        int NumOfCluster;
        double ClusterASD;
        double ClusterASA;
        double ClusterDS;
        double PerClusterShadowingSTDDB;
        double DSCorrDistM;
        double ASDCorrDistM;
        double ASACorrDistM;
        double SFCorrDistM;
        double EODSpreadAVE;
        double EODSpreadSTD;
        double EODoffsetAVE;
        double EOASpreadAVE;
        double EOASpreadSTD;
        double ClusterESD;
        double ClusterESA;
        double ESDCorrDistM;
        double ESACorrDistM;
        itpp::mat R;
    };

    /// @brief 存储Pico-to-UE O2I（室外到室内）参数的结构体

    struct P_Pico2UE_O2I {
        double DelaySpreadAVE;
        double DelaySpreadSTD;
        double AODSpreadAVE;
        double AODSpreadSTD_LogDeg;
        double AOASpreadAVE;
        double AOASpreadSTD_LogDeg;
        double ShadowFadingSTD;
        double DelayScaling;
        double XPR;
        double XPR_u;
        double XPR_sigma;
        int NumOfCluster;
        double ClusterASD;
        double ClusterASA;
        double ClusterDS;
        double PerClusterShadowingSTDDB;
        double DSCorrDistM;
        double ASDCorrDistM;
        double ASACorrDistM;
        double SFCorrDistM;
        double EODSpreadAVE;
        double EODSpreadSTD;
        double EOASpreadAVE;
        double EOASpreadSTD;
        double ClusterESD;
        double ClusterESA;
        double ESDCorrDistM;
        double ESACorrDistM;
        itpp::mat R;
    };

    

    /// @brief 存储信道模型参数的类
    class P {
    public:
        /// @brief 设置信道环境并产生相应环境对应的参数
        void SetChannelModel(int _iChannelModel,int _iChannelModel_VariantMode);

        void Set_itu_indoor_channel_parameters_ModeA();
        void Set_itu_indoor_channel_parameters_above_ModeB() ;

        void Set_itu_umi_channel_parameters_ModeA();
        void Set_itu_umi_channel_parameters_ModeB();

        void Set_itu_uma_channel_parameters_ModeA();
        void Set_itu_uma_channel_parameters_ModeB() ;

        void Set_itu_rma_channel_parameters_ModelA();
        void Set_itu_rma_channel_parameters_ModelB();
        
        //20200115
        void Set_itu_indoor_factory_channel_parameters() ;

    public:
        P_MacroTX MacroTX;
        P_PicoTX PicoTX;
        P_RX RX;
        P_FX FX;
        P_Macro2UE_LOS Macro2UE_LOS;
        P_Macro2UE_NLOS Macro2UE_NLOS;
        P_Macro2UE_O2I Macro2UE_O2I;
        P_Pico2UE_LOS Pico2UE_LOS;
        P_Pico2UE_NLOS Pico2UE_NLOS;
        P_Pico2UE_O2I Pico2UE_O2I;
        int NumOfRayPerCluster;
        int IChannelModel_for_Scenario;
        int IMacroTopology;
        
        static const int InF = 99;
        static const int InH = 100;
        static const int UMI = 101;
        static const int UMA = 102;
        static const int RMA = 103;

//        static const int ITU_INDOOR = 0;
//        static const int ITU_UMI = 1;
//        static const int ITU_UMA = 2;
//        static const int ITU_RMA = 3;
//        static const int _5GCM_INDOOR = 4;
//        static const int _5GCM_UMI = 5;
//        static const int _5GCM_UMA = 6;
//        static const int _5GCM_RMA = 7;
        static const int _5GCM_TWO_LAYER = 8;
        
    

        
        //20171211
        int IChannelModel_VariantMode;
        static const int ITU_ChannelModel_ModeA = 10;       // only valid for sub 6G 
        static const int ITU_ChannelModel_ModeB = 11;

        
        
        static const int HEXAGONAL7CELL = 0;
        static const int HEXAGONAL19CELL = 1;

        static const int FASTFADING_USED = 1;
        static const int IS2DCHANNEL = 0;
        static const int IS3DCHANNEL = 1;

    private:
        static P* m_pParameters;
    public:
        /// This "s" function can be invoked to return a uniqe object of this class.

        /** The "Singleton Pattern" is applied to implement this class, The construction and
         * destruction function is declaired as private members, so there is no other way to
         * generate any object of this class. Users have to invoke this function to get the
         * uniqe object of this class. Notice that this function return a const reference, so
         * users can't change any member with the returned const object reference.*/
        static P& s() {
            if (!m_pParameters) {
                m_pParameters = new P;
            }
            return *m_pParameters;
        };
    private:
        /// @brief 构造函数
        P() = default;
        /// @brief 拷贝构造函数
        P(const P&);
        /// @brief 赋值构造函数
        P & operator=(const P&);
        /// @brief 析构函数
        ~P() = default;
    };
}