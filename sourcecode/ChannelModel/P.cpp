#include "P.h"
using namespace cm;


P* P::m_pParameters = nullptr;

void P::SetChannelModel(int _iChannelModel, int _iChannelModel_VariantMode) {
    IChannelModel_for_Scenario = _iChannelModel;
    IChannelModel_VariantMode = _iChannelModel_VariantMode;
    NumOfRayPerCluster = 20;

    switch (IChannelModel_for_Scenario) {
        case InF:
            //ModeA和B还不确定
            Set_itu_indoor_factory_channel_parameters();
            break;
        case InH:
            if (IChannelModel_VariantMode == ITU_ChannelModel_ModeA) {
                Set_itu_indoor_channel_parameters_ModeA();
            } else if (IChannelModel_VariantMode == ITU_ChannelModel_ModeB) {
                Set_itu_indoor_channel_parameters_above_ModeB();
            } else {
                assert(false);
            }
            break;
        case UMI:
            if (IChannelModel_VariantMode == ITU_ChannelModel_ModeA) {
                Set_itu_umi_channel_parameters_ModeA();
            } else if (IChannelModel_VariantMode == ITU_ChannelModel_ModeB) {
                Set_itu_umi_channel_parameters_ModeB();
            } else {
                assert(false);
            }
            break;
        case UMA:
            if (IChannelModel_VariantMode == ITU_ChannelModel_ModeA) {
                Set_itu_uma_channel_parameters_ModeA();
            } else if (IChannelModel_VariantMode == ITU_ChannelModel_ModeB) {
                Set_itu_uma_channel_parameters_ModeB();
            } else {
                assert(false);
            }
            break;
        case RMA:
            if (IChannelModel_VariantMode == ITU_ChannelModel_ModeA) {
                Set_itu_rma_channel_parameters_ModelA();
            } else if (IChannelModel_VariantMode == ITU_ChannelModel_ModeB) {
                Set_itu_rma_channel_parameters_ModelB();
            } else {
                assert(false);
            }
            break;
        default:
            assert(false);
    }
}

void P::Set_itu_indoor_channel_parameters_ModeA() {

    Macro2UE_LOS.DelaySpreadAVE = -7.70;
    Macro2UE_LOS.DelaySpreadSTD = 0.18;
    Macro2UE_NLOS.DelaySpreadAVE = -7.41;
    Macro2UE_NLOS.DelaySpreadSTD = 0.14;
    Macro2UE_LOS.AODSpreadAVE = 1.60;
    Macro2UE_LOS.AODSpreadSTD = 0.18;
    Macro2UE_NLOS.AODSpreadAVE = 1.62;
    Macro2UE_NLOS.AODSpreadSTD = 0.25;
    Macro2UE_LOS.AOASpreadAVE = 1.62;
    Macro2UE_LOS.AOASpreadSTD = 0.22;
    Macro2UE_NLOS.AOASpreadAVE = 1.77;
    Macro2UE_NLOS.AOASpreadSTD = 0.16;
    Macro2UE_LOS.ShadowFadingSTD = 3;
    Macro2UE_NLOS.ShadowFadingSTD = 4;
    Macro2UE_LOS.KFactorDBAVE = 7;
    Macro2UE_LOS.KFactorDBSTD = 4;
    Macro2UE_LOS.DelayScaling = 3.6;
    Macro2UE_NLOS.DelayScaling = 3;
    Macro2UE_LOS.XPR = 11;
    Macro2UE_NLOS.XPR = 10;
    Macro2UE_LOS.NumOfCluster = 15;
    Macro2UE_NLOS.NumOfCluster = 19;
    Macro2UE_LOS.ClusterASD = 5;
    Macro2UE_NLOS.ClusterASD = 5;
    Macro2UE_LOS.ClusterASA = 8;
    Macro2UE_NLOS.ClusterASA = 11;
    Macro2UE_LOS.PerClusterShadowingSTDDB = 6;
    Macro2UE_NLOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_LOS.DSCorrDistM = 8;
    Macro2UE_NLOS.DSCorrDistM = 5;
    Macro2UE_LOS.ASDCorrDistM = 7;
    Macro2UE_NLOS.ASDCorrDistM = 3;
    Macro2UE_LOS.ASACorrDistM = 5;
    Macro2UE_NLOS.ASACorrDistM = 3;
    Macro2UE_LOS.SFCorrDistM = 10;
    Macro2UE_NLOS.SFCorrDistM = 6;
    Macro2UE_LOS.KCorrDistM = 4;
    ///elevation  20180329szx
    Macro2UE_LOS.EODSpreadAVE = 1.02; //0.88;
    Macro2UE_NLOS.EODSpreadAVE = 1.08; //1.06;
    Macro2UE_LOS.EODSpreadSTD = 0.41; //0.31;
    Macro2UE_NLOS.EODSpreadSTD = 0.36; //0.21;
    Macro2UE_LOS.EOASpreadAVE = 1.22; //0.94;
    Macro2UE_NLOS.EOASpreadAVE = 1.26; //1.10;
    Macro2UE_LOS.EOASpreadSTD = 0.23; //0.26;
    Macro2UE_NLOS.EOASpreadSTD = 0.67; //0.17;
    Macro2UE_LOS.ClusterESD = 3;
    Macro2UE_NLOS.ClusterESD = 3;
    Macro2UE_LOS.ClusterESA = 9; //3;
    Macro2UE_NLOS.ClusterESA = 9; //3;
    Macro2UE_LOS.ESDCorrDistM = 4; //6;
    Macro2UE_NLOS.ESDCorrDistM = 4; //5;
    Macro2UE_LOS.ESACorrDistM = 4; //2;
    Macro2UE_NLOS.ESACorrDistM = 4; //3;

    if (P::s().FX.I2DOr3DChannel == P::IS2DCHANNEL) {
        Macro2UE_LOS.R = itpp::mat(5, 5);
        Macro2UE_LOS.R(0, 0) = 0.6212;
        Macro2UE_LOS.R(0, 1) = 0.3270;
        Macro2UE_LOS.R(0, 2) = 0.4985;
        Macro2UE_LOS.R(0, 3) = -0.3974;
        Macro2UE_LOS.R(0, 4) = -0.3174;
        Macro2UE_LOS.R(1, 0) = 0.3270;
        Macro2UE_LOS.R(1, 1) = 0.9236;
        Macro2UE_LOS.R(1, 2) = 0.1152;
        Macro2UE_LOS.R(1, 3) = -0.1486;
        Macro2UE_LOS.R(1, 4) = 0.0682;
        Macro2UE_LOS.R(2, 0) = 0.4985;
        Macro2UE_LOS.R(2, 1) = 0.1152;
        Macro2UE_LOS.R(2, 2) = 0.8321;
        Macro2UE_LOS.R(2, 3) = -0.1836;
        Macro2UE_LOS.R(2, 4) = 0.1103;
        Macro2UE_LOS.R(3, 0) = -0.3974;
        Macro2UE_LOS.R(3, 1) = -0.1486;
        Macro2UE_LOS.R(3, 2) = -0.1836;
        Macro2UE_LOS.R(3, 3) = 0.8568;
        Macro2UE_LOS.R(3, 4) = 0.2287;
        Macro2UE_LOS.R(4, 0) = -0.3174;
        Macro2UE_LOS.R(4, 1) = 0.0682;
        Macro2UE_LOS.R(4, 2) = 0.1103;
        Macro2UE_LOS.R(4, 3) = 0.2287;
        Macro2UE_LOS.R(4, 4) = 0.9110;

        Macro2UE_NLOS.R = itpp::mat(5, 5);
        Macro2UE_NLOS.R(0, 0) = 0.9372;
        Macro2UE_NLOS.R(0, 1) = 0.2136;
        Macro2UE_NLOS.R(0, 2) = -0.0313;
        Macro2UE_NLOS.R(0, 3) = -0.2739;
        Macro2UE_NLOS.R(0, 4) = 0.0000;
        Macro2UE_NLOS.R(1, 0) = 0.2136;
        Macro2UE_NLOS.R(1, 1) = 0.9764;
        Macro2UE_NLOS.R(1, 2) = 0.0069;
        Macro2UE_NLOS.R(1, 3) = 0.0313;
        Macro2UE_NLOS.R(1, 4) = 0.0000;
        Macro2UE_NLOS.R(2, 0) = -0.0313;
        Macro2UE_NLOS.R(2, 1) = 0.0069;
        Macro2UE_NLOS.R(2, 2) = 0.9764;
        Macro2UE_NLOS.R(2, 3) = -0.2136;
        Macro2UE_NLOS.R(2, 4) = 0.0000;
        Macro2UE_NLOS.R(3, 0) = -0.2739;
        Macro2UE_NLOS.R(3, 1) = 0.0313;
        Macro2UE_NLOS.R(3, 2) = -0.2136;
        Macro2UE_NLOS.R(3, 3) = 0.9372;
        Macro2UE_NLOS.R(3, 4) = 0.0000;
        Macro2UE_NLOS.R(4, 0) = 0.0000;
        Macro2UE_NLOS.R(4, 1) = 0.0000;
        Macro2UE_NLOS.R(4, 2) = 0.0000;
        Macro2UE_NLOS.R(4, 3) = 0.0000;
        Macro2UE_NLOS.R(4, 4) = 1.0000;

    } else if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
        Macro2UE_LOS.R = itpp::mat(7, 7);
        Macro2UE_LOS.R(0, 0) = 0.6212;
        Macro2UE_LOS.R(0, 1) = 0.3270;
        Macro2UE_LOS.R(0, 2) = 0.4985;
        Macro2UE_LOS.R(0, 3) = -0.3974;
        Macro2UE_LOS.R(0, 4) = -0.3174;
        Macro2UE_LOS.R(0, 5) = 0;
        Macro2UE_LOS.R(0, 6) = 0;
        Macro2UE_LOS.R(1, 0) = 0.3270;
        Macro2UE_LOS.R(1, 1) = 0.9236;
        Macro2UE_LOS.R(1, 2) = 0.1152;
        Macro2UE_LOS.R(1, 3) = -0.1486;
        Macro2UE_LOS.R(1, 4) = 0.0682;
        Macro2UE_LOS.R(1, 5) = 0;
        Macro2UE_LOS.R(1, 6) = 0;
        Macro2UE_LOS.R(2, 0) = 0.4985;
        Macro2UE_LOS.R(2, 1) = 0.1152;
        Macro2UE_LOS.R(2, 2) = 0.8321;
        Macro2UE_LOS.R(2, 3) = -0.1836;
        Macro2UE_LOS.R(2, 4) = 0.1103;
        Macro2UE_LOS.R(2, 5) = 0;
        Macro2UE_LOS.R(2, 6) = 0;
        Macro2UE_LOS.R(3, 0) = -0.3974;
        Macro2UE_LOS.R(3, 1) = -0.1486;
        Macro2UE_LOS.R(3, 2) = -0.1836;
        Macro2UE_LOS.R(3, 3) = 0.8568;
        Macro2UE_LOS.R(3, 4) = 0.2287;
        Macro2UE_LOS.R(3, 5) = 0;
        Macro2UE_LOS.R(3, 6) = 0;
        Macro2UE_LOS.R(4, 0) = -0.3174;
        Macro2UE_LOS.R(4, 1) = 0.0682;
        Macro2UE_LOS.R(4, 2) = 0.1103;
        Macro2UE_LOS.R(4, 3) = 0.2287;
        Macro2UE_LOS.R(4, 4) = 0.9110;
        Macro2UE_LOS.R(4, 5) = 0;
        Macro2UE_LOS.R(4, 6) = 0;
        Macro2UE_LOS.R(5, 0) = 0;
        Macro2UE_LOS.R(5, 1) = 0;
        Macro2UE_LOS.R(5, 2) = 0;
        Macro2UE_LOS.R(5, 3) = 0;
        Macro2UE_LOS.R(5, 4) = 0;
        Macro2UE_LOS.R(5, 5) = 1;
        Macro2UE_LOS.R(5, 6) = 0;
        Macro2UE_LOS.R(6, 0) = 0;
        Macro2UE_LOS.R(6, 1) = 0;
        Macro2UE_LOS.R(6, 2) = 0;
        Macro2UE_LOS.R(6, 3) = 0;
        Macro2UE_LOS.R(6, 4) = 0;
        Macro2UE_LOS.R(6, 5) = 0;
        Macro2UE_LOS.R(6, 6) = 1;

        Macro2UE_NLOS.R = itpp::mat(7, 7);
        Macro2UE_NLOS.R(0, 0) = 0.9372;
        Macro2UE_NLOS.R(0, 1) = 0.2136;
        Macro2UE_NLOS.R(0, 2) = -0.0313;
        Macro2UE_NLOS.R(0, 3) = -0.2739;
        Macro2UE_NLOS.R(0, 4) = 0.0000;
        Macro2UE_NLOS.R(0, 5) = 0;
        Macro2UE_NLOS.R(0, 6) = 0;
        Macro2UE_NLOS.R(1, 0) = 0.2136;
        Macro2UE_NLOS.R(1, 1) = 0.9764;
        Macro2UE_NLOS.R(1, 2) = 0.0069;
        Macro2UE_NLOS.R(1, 3) = 0.0313;
        Macro2UE_NLOS.R(1, 4) = 0.0000;
        Macro2UE_NLOS.R(1, 5) = 0;
        Macro2UE_NLOS.R(1, 6) = 0;
        Macro2UE_NLOS.R(2, 0) = -0.0313;
        Macro2UE_NLOS.R(2, 1) = 0.0069;
        Macro2UE_NLOS.R(2, 2) = 0.9764;
        Macro2UE_NLOS.R(2, 3) = -0.2136;
        Macro2UE_NLOS.R(2, 4) = 0.0000;
        Macro2UE_NLOS.R(2, 5) = 0;
        Macro2UE_NLOS.R(2, 6) = 0;
        Macro2UE_NLOS.R(3, 0) = -0.2739;
        Macro2UE_NLOS.R(3, 1) = 0.0313;
        Macro2UE_NLOS.R(3, 2) = -0.2136;
        Macro2UE_NLOS.R(3, 3) = 0.9372;
        Macro2UE_NLOS.R(3, 4) = 0.0000;
        Macro2UE_NLOS.R(3, 5) = 0;
        Macro2UE_NLOS.R(3, 6) = 0;
        Macro2UE_NLOS.R(4, 0) = 0.0000;
        Macro2UE_NLOS.R(4, 1) = 0.0000;
        Macro2UE_NLOS.R(4, 2) = 0.0000;
        Macro2UE_NLOS.R(4, 3) = 0.0000;
        Macro2UE_NLOS.R(4, 4) = 1.0000;
        Macro2UE_NLOS.R(4, 5) = 0;
        Macro2UE_NLOS.R(4, 6) = 0;
        Macro2UE_NLOS.R(5, 0) = 0;
        Macro2UE_NLOS.R(5, 1) = 0;
        Macro2UE_NLOS.R(5, 2) = 0;
        Macro2UE_NLOS.R(5, 3) = 0;
        Macro2UE_NLOS.R(5, 4) = 0;
        Macro2UE_NLOS.R(5, 5) = 1;
        Macro2UE_NLOS.R(5, 6) = 0;
        Macro2UE_NLOS.R(6, 0) = 0;
        Macro2UE_NLOS.R(6, 1) = 0;
        Macro2UE_NLOS.R(6, 2) = 0;
        Macro2UE_NLOS.R(6, 3) = 0;
        Macro2UE_NLOS.R(6, 4) = 0;
        Macro2UE_NLOS.R(6, 5) = 0;
        Macro2UE_NLOS.R(6, 6) = 1;

    } else {
        assert(false);
    }

}

void P::Set_itu_indoor_channel_parameters_above_ModeB() {
    double dFrequencyGHz = FX.DRadioFrequencyMHz_Macro * 1e-3;
    //    std::cout << dFrequencyGHz << std::endl;

    //    LOS.DelaySpreadAVE = -7.70;
    //    LOS.DelaySpreadSTD = 0.18;
    //    Macro2UE_LOS.DelaySpreadAVE = -0.01 * log10(1 + dFrequencyGHz) - 7.79;
    //    Macro2UE_LOS.DelaySpreadSTD = -0.16 * log10(1 + dFrequencyGHz) + 0.5;
    //    Macro2UE_NLOS.DelaySpreadAVE = -0.28 * log10(1 + dFrequencyGHz) - 7.29;
    //    Macro2UE_NLOS.DelaySpreadSTD = 0.1 * log10(1 + dFrequencyGHz) + 0.11;


    //    Macro2UE_NLOS.AODSpreadAVE = 1.49;
    //    Macro2UE_NLOS.AODSpreadSTD = 0.17;

    //    Macro2UE_LOS.AOASpreadAVE = -0.19 * log10(1 + dFrequencyGHz) + 1.86;
    //    Macro2UE_LOS.AOASpreadSTD = 0.12 * log10(1 + dFrequencyGHz);
    //
    //    Macro2UE_NLOS.AOASpreadAVE = -0.11 * log10(1 + dFrequencyGHz) + 1.8;
    //    Macro2UE_NLOS.AOASpreadSTD = 0.12 * log10(1 + dFrequencyGHz);

    //171127修改
    Macro2UE_LOS.DelaySpreadAVE = -0.01 * log10(1 + dFrequencyGHz) - 7.692;
    Macro2UE_LOS.DelaySpreadSTD = 0.18;
    Macro2UE_NLOS.DelaySpreadAVE = -0.28 * log10(1 + dFrequencyGHz) - 7.173;
    Macro2UE_NLOS.DelaySpreadSTD = 0.1 * log10(1 + dFrequencyGHz) + 0.055;

    Macro2UE_LOS.AODSpreadAVE = 1.60;
    Macro2UE_LOS.AODSpreadSTD = 0.18;

    Macro2UE_NLOS.AODSpreadAVE = 1.62;
    Macro2UE_NLOS.AODSpreadSTD = 0.25;

    Macro2UE_LOS.AOASpreadAVE = -0.19 * log10(1 + dFrequencyGHz) + 1.781;
    Macro2UE_LOS.AOASpreadSTD = 0.12 * log10(1 + dFrequencyGHz) + 0.119;

    Macro2UE_NLOS.AOASpreadAVE = -0.11 * log10(1 + dFrequencyGHz) + 1.863;
    Macro2UE_NLOS.AOASpreadSTD = 0.12 * log10(1 + dFrequencyGHz) + 0.059;

    //    Macro2UE_LOS.EOASpreadAVE = -0.26 * log10(1 + dFrequencyGHz) + 1.21;
    //    Macro2UE_LOS.EOASpreadSTD = -0.04 * log10(1 + dFrequencyGHz) + 0.17;
    //    Macro2UE_NLOS.EOASpreadAVE = -0.15 * log10(1 + dFrequencyGHz) + 1.04;
    //    Macro2UE_NLOS.EOASpreadSTD = -0.09 * log10(1 + dFrequencyGHz) + 0.24;
    //    
    // EOA = ZOA
    // 20171127
    Macro2UE_LOS.EOASpreadAVE = -0.26 * log10(1 + dFrequencyGHz) + 1.44;
    Macro2UE_LOS.EOASpreadSTD = -0.04 * log10(1 + dFrequencyGHz) + 0.264;
    Macro2UE_NLOS.EOASpreadAVE = -0.15 * log10(1 + dFrequencyGHz) + 1.387;
    Macro2UE_NLOS.EOASpreadSTD = -0.09 * log10(1 + dFrequencyGHz) + 0.746;

    Macro2UE_LOS.ShadowFadingSTD = 3;
    Macro2UE_NLOS.ShadowFadingSTD = 8.03;

    //    Macro2UE_LOS.KFactorDBAVE = 0.84 * log10(1 + dFrequencyGHz) + 2.12;
    //    Macro2UE_LOS.KFactorDBSTD = -0.58 * log10(1 + dFrequencyGHz) + 6.19;
    //    
    // 20171127
    Macro2UE_LOS.KFactorDBAVE = 7;
    Macro2UE_LOS.KFactorDBSTD = 4;

    //    Macro2UE_LOS.DelayScaling = 2.15;
    //    Macro2UE_NLOS.DelayScaling = 1.84;
    // 20171127
    Macro2UE_LOS.DelayScaling = 3.6;
    Macro2UE_NLOS.DelayScaling = 3;

    //    Macro2UE_LOS.XPR_u = 15;
    //    Macro2UE_LOS.XPR_sigma = 3;
    //    Macro2UE_NLOS.XPR_u = 12;
    //    Macro2UE_NLOS.XPR_sigma = 7;
    // 20171127
    Macro2UE_LOS.XPR_u = 11;
    Macro2UE_LOS.XPR_sigma = 4;
    Macro2UE_NLOS.XPR_u = 10;
    Macro2UE_NLOS.XPR_sigma = 4;

    //    Macro2UE_LOS.NumOfCluster = 8;
    //    Macro2UE_NLOS.NumOfCluster = 10;
    // 20171127
    Macro2UE_LOS.NumOfCluster = 15;
    Macro2UE_NLOS.NumOfCluster = 19;

    //zhnegyi
    //    LOS.ClusterDS = -10000; // not available
    //    NLOS.ClusterDS = -10000; // not available

    //    Macro2UE_LOS.ClusterASD = 7;
    //    Macro2UE_NLOS.ClusterASD = 3;
    // 20171127
    Macro2UE_LOS.ClusterASD = 5;
    Macro2UE_NLOS.ClusterASD = 5;

    //    Macro2UE_LOS.ClusterASA = -6.2 * log10(1 + dFrequencyGHz) + 16.72;
    //    Macro2UE_NLOS.ClusterASA = -13.0 * log10(1 + dFrequencyGHz) + 30.53;
    // 20171127
    Macro2UE_LOS.ClusterASA = 8;
    Macro2UE_NLOS.ClusterASA = 11;

    //    Macro2UE_LOS.ClusterESA = -3.85 * log10(1 + dFrequencyGHz) + 10.28;
    //    Macro2UE_NLOS.ClusterESA = -3.72 * log10(1 + dFrequencyGHz) + 10.25;
    // 20171127
    Macro2UE_LOS.ClusterESD = 3;
    Macro2UE_NLOS.ClusterESD = 3;
    Macro2UE_LOS.ClusterESA = 9;
    Macro2UE_NLOS.ClusterESA = 9;

    Macro2UE_LOS.PerClusterShadowingSTDDB = 6;
    Macro2UE_NLOS.PerClusterShadowingSTDDB = 3;

    Macro2UE_LOS.DSCorrDistM = 8;
    Macro2UE_NLOS.DSCorrDistM = 5;

    Macro2UE_LOS.ASDCorrDistM = 7;
    Macro2UE_NLOS.ASDCorrDistM = 3;

    Macro2UE_LOS.ASACorrDistM = 5;
    Macro2UE_NLOS.ASACorrDistM = 3;

    Macro2UE_LOS.SFCorrDistM = 10;
    Macro2UE_NLOS.SFCorrDistM = 6;

    Macro2UE_LOS.KCorrDistM = 4;

    //    LOS.ESDCorrDistM = 6;
    //    NLOS.ESDCorrDistM = 5;
    //    LOS.ESACorrDistM = 2;
    //    NLOS.ESACorrDistM = 3;
    //
    //    Macro2UE_LOS.ESDCorrDistM = 3;
    //    Macro2UE_NLOS.ESDCorrDistM = 3;
    // 20171127
    Macro2UE_LOS.ESDCorrDistM = 4;
    Macro2UE_NLOS.ESDCorrDistM = 4;

    //    Macro2UE_LOS.ESACorrDistM = 3;
    //    Macro2UE_NLOS.ESACorrDistM = 3;
    // 20171127
    Macro2UE_LOS.ESACorrDistM = 4;
    Macro2UE_NLOS.ESACorrDistM = 4;

    ///elevation
    //    Macro2UE_LOS.EODSpreadAVE = -1.43 * log10(1 + dFrequencyGHz) + 2.25; //0.88;
    //    Macro2UE_LOS.EODSpreadSTD = 0.13 * log10(1 + dFrequencyGHz) + 0.15; //0.31;
    // 20171127
    Macro2UE_LOS.EODSpreadAVE = -1.43 * log10(1 + dFrequencyGHz) + 2.228;
    Macro2UE_LOS.EODSpreadSTD = 0.13 * log10(1 + dFrequencyGHz) + 0.30;

    //    Macro2UE_NLOS.EODSpreadAVE = 1.37; //1.06;
    //    Macro2UE_NLOS.EODSpreadSTD = 0.38; //0.21;
    // 20171127
    Macro2UE_NLOS.EODSpreadAVE = 1.08;
    Macro2UE_NLOS.EODSpreadSTD = 0.36;



    // 20171127
    //    //zhengyi
    //    //    assert(false);
    //    //感觉好像没用上？
    //    Macro2UE_LOS.ClusterESD = 3;
    //    Macro2UE_NLOS.ClusterESD = 3;





    //    LOS.R = itpp::mat(5, 5);
    //    LOS.R = "1,2,3,4,5;"
    //            "1,2,3,4,5;"
    //            "1,2,3,4,5;"
    //            "1,2,3,4,5;"
    //            "1,2,3,4,5;";
    //        LOS.R.set_row(0,"2,3,4,5,6");
    //    std::cout << LOS.R << std::endl;
    //    std::cout << LOS.R(0,0) << std::endl;
    //    std::cout << LOS.R(4,4) << std::endl;

    if (P::s().FX.I2DOr3DChannel == P::IS2DCHANNEL) {
        Macro2UE_LOS.R = itpp::mat(5, 5);
        Macro2UE_LOS.R = "0.6212,0.3270,0.4985,-0.3974,-0.3174;"
                "0.3270,0.9236,0.1152,-0.1486,0.0682;"
                "0.4985,0.1152,0.8321,-0.1836,0.1103;"
                "-0.3974,-0.1486,-0.1836,0.8568,0.2287;"
                "-0.3174,0.0682,0.1103,0.2287,0.9110;";


        Macro2UE_NLOS.R = itpp::mat(5, 5);
        Macro2UE_NLOS.R(0, 0) = 0.9372;
        Macro2UE_NLOS.R(0, 1) = 0.2136;
        Macro2UE_NLOS.R(0, 2) = -0.0313;
        Macro2UE_NLOS.R(0, 3) = -0.2739;
        Macro2UE_NLOS.R(0, 4) = 0.0000;
        Macro2UE_NLOS.R(1, 0) = 0.2136;
        Macro2UE_NLOS.R(1, 1) = 0.9764;
        Macro2UE_NLOS.R(1, 2) = 0.0069;
        Macro2UE_NLOS.R(1, 3) = 0.0313;
        Macro2UE_NLOS.R(1, 4) = 0.0000;
        Macro2UE_NLOS.R(2, 0) = -0.0313;
        Macro2UE_NLOS.R(2, 1) = 0.0069;
        Macro2UE_NLOS.R(2, 2) = 0.9764;
        Macro2UE_NLOS.R(2, 3) = -0.2136;
        Macro2UE_NLOS.R(2, 4) = 0.0000;
        Macro2UE_NLOS.R(3, 0) = -0.2739;
        Macro2UE_NLOS.R(3, 1) = 0.0313;
        Macro2UE_NLOS.R(3, 2) = -0.2136;
        Macro2UE_NLOS.R(3, 3) = 0.9372;
        Macro2UE_NLOS.R(3, 4) = 0.0000;
        Macro2UE_NLOS.R(4, 0) = 0.0000;
        Macro2UE_NLOS.R(4, 1) = 0.0000;
        Macro2UE_NLOS.R(4, 2) = 0.0000;
        Macro2UE_NLOS.R(4, 3) = 0.0000;
        Macro2UE_NLOS.R(4, 4) = 1.0000;

    } else if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {

        //        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        //        assert(false);
        itpp::mat Macro2UE_LOS_R_square = itpp::mat(7, 7);
        Macro2UE_LOS_R_square = itpp::zeros(7, 7);
        Macro2UE_LOS_R_square(0, 0) = 1;
        Macro2UE_LOS_R_square(1, 1) = 1;
        Macro2UE_LOS_R_square(2, 2) = 1;
        Macro2UE_LOS_R_square(3, 3) = 1;
        Macro2UE_LOS_R_square(4, 4) = 1;
        Macro2UE_LOS_R_square(5, 5) = 1;
        Macro2UE_LOS_R_square(6, 6) = 1;

        Macro2UE_LOS_R_square(0, 1) = 0.6; // DS vs. ASD
        Macro2UE_LOS_R_square(0, 2) = 0.8; // DS vs. ASA
        Macro2UE_LOS_R_square(0, 3) = -0.8; // DS vs. SF
        Macro2UE_LOS_R_square(0, 4) = -0.5; // DS vs. K
        Macro2UE_LOS_R_square(0, 5) = 0.1; // DS vs. ZSD
        Macro2UE_LOS_R_square(0, 6) = 0.2; // DS vs. ZSA

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_LOS_R_square(1, 2) = 0.4; // ASD vs. ASA
        Macro2UE_LOS_R_square(1, 3) = -0.4; // ASD vs. SF
        Macro2UE_LOS_R_square(1, 4) = 0; // ASD vs. K

        //        Macro2UE_LOS_R_square(1, 5) = 0.2;  // ASD vs. ZSD
        // 20171127
        Macro2UE_LOS_R_square(1, 5) = 0.5; // ASD vs. ZSD

        //        Macro2UE_LOS_R_square(1, 6) = 0.2;  // ASD vs. ZSA
        // 20171127
        Macro2UE_LOS_R_square(1, 6) = 0.0; // ASD vs. ZSA

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_LOS_R_square(2, 3) = -0.5; // ASA vs. SF
        Macro2UE_LOS_R_square(2, 4) = 0; // ASA vs. K

        //        Macro2UE_LOS_R_square(2, 5) = 0.1;  // ASA vs. ZSD
        // 20171127
        Macro2UE_LOS_R_square(2, 5) = 0.0; // ASA vs. ZSD

        //        Macro2UE_LOS_R_square(2, 6) = 0.3;  // ASA vs. ZSA
        // 20171127
        Macro2UE_LOS_R_square(2, 6) = 0.5; // ASA vs. ZSA

        Macro2UE_LOS_R_square(3, 4) = 0.5; // SF vs. K
        Macro2UE_LOS_R_square(3, 5) = 0.2; // SF vs. ZSD

        //        Macro2UE_LOS_R_square(3, 6) = -0.1; // SF vs. ZSA
        // 20171127
        Macro2UE_LOS_R_square(3, 6) = 0.3; // SF vs. ZSA

        Macro2UE_LOS_R_square(4, 5) = 0; // K vs. ZSD
        Macro2UE_LOS_R_square(4, 6) = 0.1; // K vs. ZSA

        //        Macro2UE_LOS_R_square(5, 6) = 0.2;  // ZSD vs. ZSA
        // 20171127
        Macro2UE_LOS_R_square(5, 6) = 0.0; // ZSD vs. ZSA

        //from up triangle matrix to symmetric matrix
        itpp::mat aa = Macro2UE_LOS_R_square + Macro2UE_LOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_LOS_R_square));
        itpp::cmat bb = itpp::sqrtm(aa);
        Macro2UE_LOS.R = itpp::real(bb);

        itpp::mat Macro2UE_NLOS_R_square = itpp::mat(7, 7);
        Macro2UE_NLOS_R_square = itpp::zeros(7, 7);

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        //        Macro2UE_NLOS_R_square(0, 0) = 1;
        //        Macro2UE_NLOS_R_square(1, 1) = 1;
        //        Macro2UE_NLOS_R_square(2, 2) = 1;
        //        Macro2UE_NLOS_R_square(3, 3) = 1;
        //        Macro2UE_NLOS_R_square(4, 4) = 1;
        //        Macro2UE_NLOS_R_square(5, 5) = 1;
        //        Macro2UE_NLOS_R_square(6, 6) = 1;
        //
        //        Macro2UE_NLOS_R_square(0, 1) = 0.4;
        //        Macro2UE_NLOS_R_square(0, 2) = 0;
        //        Macro2UE_NLOS_R_square(0, 3) = -0.5;
        //        Macro2UE_NLOS_R_square(0, 4) = 0;
        //        Macro2UE_NLOS_R_square(0, 5) = -0.1;
        //        Macro2UE_NLOS_R_square(0, 6) = -0.1;
        //
        //        Macro2UE_NLOS_R_square(1, 2) = 0;
        //        Macro2UE_NLOS_R_square(1, 3) = 0;
        //        Macro2UE_NLOS_R_square(1, 4) = 0;
        //        Macro2UE_NLOS_R_square(1, 5) = 0.3;
        //        Macro2UE_NLOS_R_square(1, 6) = 0.2;
        //
        //        Macro2UE_NLOS_R_square(2, 3) = -0.4;
        //        Macro2UE_NLOS_R_square(2, 4) = 0;
        //        Macro2UE_NLOS_R_square(2, 5) = 0.1;
        //        Macro2UE_NLOS_R_square(2, 6) = 0.0;
        //
        //        Macro2UE_NLOS_R_square(3, 4) = 0;
        //        Macro2UE_NLOS_R_square(3, 5) = 0;
        //        Macro2UE_NLOS_R_square(3, 6) = -0.1;
        //
        //        Macro2UE_NLOS_R_square(4, 5) = 0;
        //        Macro2UE_NLOS_R_square(4, 6) = 0;
        //
        //        Macro2UE_NLOS_R_square(5, 6) = 0.4;

        // 20171127
        Macro2UE_NLOS_R_square(0, 0) = 1;
        Macro2UE_NLOS_R_square(1, 1) = 1;
        Macro2UE_NLOS_R_square(2, 2) = 1;
        Macro2UE_NLOS_R_square(3, 3) = 1;
        Macro2UE_NLOS_R_square(4, 4) = 1;
        Macro2UE_NLOS_R_square(5, 5) = 1;
        Macro2UE_NLOS_R_square(6, 6) = 1;

        Macro2UE_NLOS_R_square(0, 1) = 0.4;
        Macro2UE_NLOS_R_square(0, 2) = 0;
        Macro2UE_NLOS_R_square(0, 3) = -0.5;
        Macro2UE_NLOS_R_square(0, 4) = 0; // DS vs. K, N/A
        Macro2UE_NLOS_R_square(0, 5) = -0.27;
        Macro2UE_NLOS_R_square(0, 6) = -0.06;

        Macro2UE_NLOS_R_square(1, 2) = 0;
        Macro2UE_NLOS_R_square(1, 3) = 0;
        Macro2UE_NLOS_R_square(1, 4) = 0; // ASD vs. K. N/A
        Macro2UE_NLOS_R_square(1, 5) = 0.35;
        Macro2UE_NLOS_R_square(1, 6) = 0.23;

        Macro2UE_NLOS_R_square(2, 3) = -0.4;
        Macro2UE_NLOS_R_square(2, 4) = 0; // ASA vs. K, N/A
        Macro2UE_NLOS_R_square(2, 5) = -0.08;
        Macro2UE_NLOS_R_square(2, 6) = 0.43;

        Macro2UE_NLOS_R_square(3, 4) = 0; // SF vs. K, N/A
        Macro2UE_NLOS_R_square(3, 5) = 0;
        Macro2UE_NLOS_R_square(3, 6) = 0;

        Macro2UE_NLOS_R_square(4, 5) = 0; // K vs. ZSD, N/A
        Macro2UE_NLOS_R_square(4, 6) = 0; // K vs. ZSA, N/A

        Macro2UE_NLOS_R_square(5, 6) = 0.42;

        itpp::mat aa_nlos = Macro2UE_NLOS_R_square + Macro2UE_NLOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_NLOS_R_square));
        itpp::cmat bb_nlos = itpp::sqrtm(aa_nlos);
        Macro2UE_NLOS.R = itpp::real(bb_nlos);
        //        std::cout << bb_nlos << std::endl;

        //        int deb?ug = 0;

    } else {
        assert(false);
    }
}

void P::Set_itu_umi_channel_parameters_ModeA() {
    //20171221
    Macro2UE_LOS.DelaySpreadAVE = -7.19;
    Macro2UE_LOS.DelaySpreadSTD = 0.40;
    Macro2UE_NLOS.DelaySpreadAVE = -6.89;
    Macro2UE_NLOS.DelaySpreadSTD = 0.54;
    Macro2UE_LOS.AODSpreadAVE = 1.20;
    Macro2UE_LOS.AODSpreadSTD = 0.43;
    Macro2UE_NLOS.AODSpreadAVE = 1.41;
    Macro2UE_NLOS.AODSpreadSTD = 0.17;
    Macro2UE_LOS.AOASpreadAVE = 1.75;
    Macro2UE_LOS.AOASpreadSTD = 0.19;
    Macro2UE_NLOS.AOASpreadAVE = 1.84;
    Macro2UE_NLOS.AOASpreadSTD = 0.15;
    Macro2UE_LOS.ShadowFadingSTD = 3;
    Macro2UE_NLOS.ShadowFadingSTD = 4;

    Macro2UE_LOS.KFactorDBAVE = 9;
    Macro2UE_LOS.KFactorDBSTD = 5;
    Macro2UE_LOS.DelayScaling = 3.2;
    Macro2UE_NLOS.DelayScaling = 3;
    Macro2UE_LOS.XPR = 9;
    Macro2UE_LOS.XPR_u = 9;
    Macro2UE_LOS.XPR_sigma = 3;
    Macro2UE_NLOS.XPR = 8;
    Macro2UE_NLOS.XPR_u = 8;
    Macro2UE_NLOS.XPR_sigma = 3;
    Macro2UE_LOS.NumOfCluster = 12;
    Macro2UE_NLOS.NumOfCluster = 19;
    Macro2UE_LOS.ClusterASD = 3;
    Macro2UE_NLOS.ClusterASD = 10;
    Macro2UE_LOS.ClusterASA = 17;
    Macro2UE_NLOS.ClusterASA = 22;
    Macro2UE_LOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_NLOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_LOS.DSCorrDistM = 7;
    Macro2UE_NLOS.DSCorrDistM = 10;
    Macro2UE_LOS.ASDCorrDistM = 8;
    Macro2UE_NLOS.ASDCorrDistM = 10;
    Macro2UE_LOS.ASACorrDistM = 8;
    Macro2UE_NLOS.ASACorrDistM = 9;
    Macro2UE_LOS.SFCorrDistM = 10;
    Macro2UE_NLOS.SFCorrDistM = 13;
    Macro2UE_LOS.KCorrDistM = 15;
    Macro2UE_O2I.DelaySpreadAVE = -6.62;
    Macro2UE_O2I.DelaySpreadSTD = 0.32;
    Macro2UE_O2I.AODSpreadAVE = 1.25;
    Macro2UE_O2I.AODSpreadSTD = 0.42;
    Macro2UE_O2I.AOASpreadAVE = 1.76;
    Macro2UE_O2I.AOASpreadSTD = 0.16;
    Macro2UE_O2I.ShadowFadingSTD = 7;
    Macro2UE_O2I.DelayScaling = 2.2;
    Macro2UE_O2I.XPR = 9;
    Macro2UE_O2I.XPR_u = 9;
    Macro2UE_O2I.XPR_sigma = 11;
    Macro2UE_O2I.NumOfCluster = 12;
    Macro2UE_O2I.ClusterASD = 5;
    Macro2UE_O2I.ClusterASA = 8;
    Macro2UE_O2I.PerClusterShadowingSTDDB = 4;
    Macro2UE_O2I.DSCorrDistM = 10;
    Macro2UE_O2I.ASDCorrDistM = 11;
    Macro2UE_O2I.ASACorrDistM = 17;
    Macro2UE_O2I.SFCorrDistM = 7;
    ///elevation
    Macro2UE_LOS.EODSpreadAVE = 1.14;
    Macro2UE_NLOS.EODSpreadAVE = 1.26;
    Macro2UE_O2I.EODSpreadAVE = 0.96;
    Macro2UE_LOS.EODSpreadSTD = 0.4;
    Macro2UE_NLOS.EODSpreadSTD = 0.6;
    Macro2UE_O2I.EODSpreadSTD = 0.4;
    Macro2UE_LOS.EOASpreadAVE = 0.60;
    Macro2UE_NLOS.EOASpreadAVE = 0.88;
    Macro2UE_O2I.EOASpreadAVE = 1.01;
    Macro2UE_LOS.EOASpreadSTD = 0.16;
    Macro2UE_NLOS.EOASpreadSTD = 0.16;
    Macro2UE_O2I.EOASpreadSTD = 0.43;
    Macro2UE_LOS.ClusterESD = 3;
    Macro2UE_NLOS.ClusterESD = 3;
    Macro2UE_O2I.ClusterESD = 3;
    Macro2UE_LOS.ClusterESA = 7;
    Macro2UE_NLOS.ClusterESA = 7;
    Macro2UE_O2I.ClusterESA = 3;
    Macro2UE_LOS.ESDCorrDistM = 12;
    Macro2UE_NLOS.ESDCorrDistM = 10;
    Macro2UE_O2I.ESDCorrDistM = 25;
    Macro2UE_LOS.ESACorrDistM = 12;
    Macro2UE_NLOS.ESACorrDistM = 10;
    Macro2UE_O2I.ESACorrDistM = 25;

    if (P::s().FX.I2DOr3DChannel == P::IS2DCHANNEL) {
        Macro2UE_LOS.R = itpp::mat(5, 5);
        Macro2UE_LOS.R(0, 0) = 0.7531;
        Macro2UE_LOS.R(0, 1) = 0.2410;
        Macro2UE_LOS.R(0, 2) = 0.4541;
        Macro2UE_LOS.R(0, 3) = -0.0972;
        Macro2UE_LOS.R(0, 4) = -0.3989;
        Macro2UE_LOS.R(1, 0) = 0.2410;
        Macro2UE_LOS.R(1, 1) = 0.9294;
        Macro2UE_LOS.R(1, 2) = 0.1380;
        Macro2UE_LOS.R(1, 3) = -0.2424;
        Macro2UE_LOS.R(1, 4) = -0.0208;
        Macro2UE_LOS.R(2, 0) = 0.4541;
        Macro2UE_LOS.R(2, 1) = 0.1380;
        Macro2UE_LOS.R(2, 2) = 0.8615;
        Macro2UE_LOS.R(2, 3) = -0.1756;
        Macro2UE_LOS.R(2, 4) = -0.0414;
        Macro2UE_LOS.R(3, 0) = -0.0972;
        Macro2UE_LOS.R(3, 1) = -0.2424;
        Macro2UE_LOS.R(3, 2) = -0.1756;
        Macro2UE_LOS.R(3, 3) = 0.9157;
        Macro2UE_LOS.R(3, 4) = 0.2499;
        Macro2UE_LOS.R(4, 0) = -0.3989;
        Macro2UE_LOS.R(4, 1) = -0.0208;
        Macro2UE_LOS.R(4, 2) = -0.0414;
        Macro2UE_LOS.R(4, 3) = 0.2499;
        Macro2UE_LOS.R(4, 4) = 0.8811;

        Macro2UE_NLOS.R = itpp::mat(5, 5);
        Macro2UE_NLOS.R(0, 0) = 0.9135;
        Macro2UE_NLOS.R(0, 1) = 0.0000;
        Macro2UE_NLOS.R(0, 2) = 0.1780;
        Macro2UE_NLOS.R(0, 3) = -0.3658;
        Macro2UE_NLOS.R(0, 4) = 0.0000;
        Macro2UE_NLOS.R(1, 0) = 0.0000;
        Macro2UE_NLOS.R(1, 1) = 1.0000;
        Macro2UE_NLOS.R(1, 2) = -0.0000;
        Macro2UE_NLOS.R(1, 3) = 0.0000;
        Macro2UE_NLOS.R(1, 4) = 0.0000;
        Macro2UE_NLOS.R(2, 0) = 0.1780;
        Macro2UE_NLOS.R(2, 1) = 0.0000;
        Macro2UE_NLOS.R(2, 2) = 0.9678;
        Macro2UE_NLOS.R(2, 3) = -0.1780;
        Macro2UE_NLOS.R(2, 4) = 0.0000;
        Macro2UE_NLOS.R(3, 0) = -0.3658;
        Macro2UE_NLOS.R(3, 1) = 0.0000;
        Macro2UE_NLOS.R(3, 2) = -0.1780;
        Macro2UE_NLOS.R(3, 3) = 0.9135;
        Macro2UE_NLOS.R(3, 4) = 0.0000;
        Macro2UE_NLOS.R(4, 0) = 0.0000;
        Macro2UE_NLOS.R(4, 1) = 0.0000;
        Macro2UE_NLOS.R(4, 2) = 0.0000;
        Macro2UE_NLOS.R(4, 3) = 0.0000;
        Macro2UE_NLOS.R(4, 4) = 1.0000;

        Macro2UE_O2I.R = itpp::mat(5, 5);
        Macro2UE_O2I.R(0, 0) = 0.8970;
        Macro2UE_O2I.R(0, 1) = 0.2419;
        Macro2UE_O2I.R(0, 2) = 0.2236;
        Macro2UE_O2I.R(0, 3) = -0.2949;
        Macro2UE_O2I.R(0, 4) = 0.0000;
        Macro2UE_O2I.R(1, 0) = 0.2419;
        Macro2UE_O2I.R(1, 1) = 0.9592;
        Macro2UE_O2I.R(1, 2) = -0.0307;
        Macro2UE_O2I.R(1, 3) = 0.1432;
        Macro2UE_O2I.R(1, 4) = 0.0000;
        Macro2UE_O2I.R(2, 0) = 0.2236;
        Macro2UE_O2I.R(2, 1) = -0.0307;
        Macro2UE_O2I.R(2, 2) = 0.9735;
        Macro2UE_O2I.R(2, 3) = 0.0367;
        Macro2UE_O2I.R(2, 4) = 0.0000;
        Macro2UE_O2I.R(3, 0) = -0.2949;
        Macro2UE_O2I.R(3, 1) = 0.1432;
        Macro2UE_O2I.R(3, 2) = 0.0367;
        Macro2UE_O2I.R(3, 3) = 0.9440;
        Macro2UE_O2I.R(3, 4) = 0.0000;
        Macro2UE_O2I.R(4, 0) = 0.0000;
        Macro2UE_O2I.R(4, 1) = 0.0000;
        Macro2UE_O2I.R(4, 2) = 0.0000;
        Macro2UE_O2I.R(4, 3) = 0.0000;
        Macro2UE_O2I.R(4, 4) = 1.0000;

    } else if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
        Macro2UE_LOS.R = itpp::mat(7, 7);
        Macro2UE_LOS.R(0, 0) = 0.7249;
        Macro2UE_LOS.R(0, 1) = 0.2510;
        Macro2UE_LOS.R(0, 2) = 0.4678;
        Macro2UE_LOS.R(0, 3) = -0.0903;
        Macro2UE_LOS.R(0, 4) = -0.4132;
        Macro2UE_LOS.R(0, 5) = -0.0401;
        Macro2UE_LOS.R(0, 6) = 0.1106;
        Macro2UE_LOS.R(1, 0) = 0.2510;
        Macro2UE_LOS.R(1, 1) = 0.8598;
        Macro2UE_LOS.R(1, 2) = 0.1428;
        Macro2UE_LOS.R(1, 3) = -0.2620;
        Macro2UE_LOS.R(1, 4) = -0.0143;
        Macro2UE_LOS.R(1, 5) = 0.2893;
        Macro2UE_LOS.R(1, 6) = 0.1575;
        Macro2UE_LOS.R(2, 0) = 0.4678;
        Macro2UE_LOS.R(2, 1) = 0.1428;
        Macro2UE_LOS.R(2, 2) = 0.8526;
        Macro2UE_LOS.R(2, 3) = -0.1763;
        Macro2UE_LOS.R(2, 4) = -0.0343;
        Macro2UE_LOS.R(2, 5) = -0.0093;
        Macro2UE_LOS.R(2, 6) = -0.0382;
        Macro2UE_LOS.R(3, 0) = -0.0903;
        Macro2UE_LOS.R(3, 1) = -0.2620;
        Macro2UE_LOS.R(3, 2) = -0.1763;
        Macro2UE_LOS.R(3, 3) = 0.9086;
        Macro2UE_LOS.R(3, 4) = 0.2542;
        Macro2UE_LOS.R(3, 5) = 0.0398;
        Macro2UE_LOS.R(3, 6) = 0.0211;
        Macro2UE_LOS.R(4, 0) = -0.4132;
        Macro2UE_LOS.R(4, 1) = -0.0143;
        Macro2UE_LOS.R(4, 2) = -0.0343;
        Macro2UE_LOS.R(4, 3) = 0.2542;
        Macro2UE_LOS.R(4, 4) = 0.8733;
        Macro2UE_LOS.R(4, 5) = -0.0122;
        Macro2UE_LOS.R(4, 6) = 0.0221;
        Macro2UE_LOS.R(5, 0) = -0.0401;
        Macro2UE_LOS.R(5, 1) = 0.2893;
        Macro2UE_LOS.R(5, 2) = -0.0093;
        Macro2UE_LOS.R(5, 3) = 0.0398;
        Macro2UE_LOS.R(5, 4) = -0.0122;
        Macro2UE_LOS.R(5, 5) = 0.9552;
        Macro2UE_LOS.R(5, 6) = -0.0217;
        Macro2UE_LOS.R(6, 0) = 0.1106;
        Macro2UE_LOS.R(6, 1) = 0.1575;
        Macro2UE_LOS.R(6, 2) = -0.0382;
        Macro2UE_LOS.R(6, 3) = 0.0211;
        Macro2UE_LOS.R(6, 4) = 0.0221;
        Macro2UE_LOS.R(6, 5) = -0.0217;
        Macro2UE_LOS.R(6, 6) = 0.9798;
        //            LOS.R(0, 0) = 0.6774; LOS.R(0, 1) = 0.2332; LOS.R(0, 2) = 0.5000; LOS.R(0, 3) = -0.0908; LOS.R(0, 4) = -0.4366; LOS.R(0, 5) = 0.1941;  LOS.R(0, 6) = -0.0135;
        //            LOS.R(1, 0) = 0.2332; LOS.R(1, 1) = 0.9117; LOS.R(1, 2) = 0.1422; LOS.R(1, 3) = -0.2475; LOS.R(1, 4) = -0.0219; LOS.R(1, 5) = 0.1434;  LOS.R(1, 6) = 0.1091;
        //            LOS.R(2, 0) = 0.5000; LOS.R(2, 1) = 0.1422; LOS.R(2, 2) = 0.8325; LOS.R(2, 3) = -0.1798; LOS.R(2, 4) = -0.0177; LOS.R(2, 5) = -0.0632; LOS.R(2, 6) = -0.0037;
        //            LOS.R(3, 0) = -0.0908;LOS.R(3, 1) = -0.2475;LOS.R(3, 2) = -0.1798;LOS.R(3, 3) = 0.9127;  LOS.R(3, 4) = 0.2543;  LOS.R(3, 5) = 0.0161;  LOS.R(3, 6) = 0.0137;
        //            LOS.R(4, 0) = -0.4366;LOS.R(4, 1) = -0.0219;LOS.R(4, 2) = -0.0177;LOS.R(4, 3) = 0.2543;  LOS.R(4, 4) = 0.8613;  LOS.R(4, 5) = 0.0452;  LOS.R(4, 6) = -0.0036;
        //            LOS.R(5, 0) = 0.1941; LOS.R(5, 1) = 0.1434; LOS.R(5, 2) = -0.0632;LOS.R(5, 3) = 0.0161;  LOS.R(5, 4) = 0.0452;  LOS.R(5, 5) = 0.9672;  LOS.R(5, 6) = -0.0068;
        //            LOS.R(6, 0) = -0.0135;LOS.R(6, 1) = 0.1091; LOS.R(6, 2) = -0.0037;LOS.R(6, 3) = 0.0137;  LOS.R(6, 4) = -0.0036; LOS.R(6, 5) = -0.0068; LOS.R(6, 6) = 0.9938;

        Macro2UE_NLOS.R = itpp::mat(7, 7);
        Macro2UE_NLOS.R(0, 0) = 0.8302;
        Macro2UE_NLOS.R(0, 1) = 0.0709;
        Macro2UE_NLOS.R(0, 2) = 0.1949;
        Macro2UE_NLOS.R(0, 3) = -0.4011;
        Macro2UE_NLOS.R(0, 4) = 0.0000;
        Macro2UE_NLOS.R(0, 5) = -0.3252;
        Macro2UE_NLOS.R(0, 6) = -0.0341;
        Macro2UE_NLOS.R(1, 0) = 0.0709;
        Macro2UE_NLOS.R(1, 1) = 0.9079;
        Macro2UE_NLOS.R(1, 2) = -0.0277;
        Macro2UE_NLOS.R(1, 3) = 0.0259;
        Macro2UE_NLOS.R(1, 4) = 0.0000;
        Macro2UE_NLOS.R(1, 5) = 0.3008;
        Macro2UE_NLOS.R(1, 6) = 0.2808;
        Macro2UE_NLOS.R(2, 0) = 0.1949;
        Macro2UE_NLOS.R(2, 1) = -0.0277;
        Macro2UE_NLOS.R(2, 2) = 0.9580;
        Macro2UE_NLOS.R(2, 3) = -0.1716;
        Macro2UE_NLOS.R(2, 4) = 0.0000;
        Macro2UE_NLOS.R(2, 5) = 0.0352;
        Macro2UE_NLOS.R(2, 6) = 0.1131;
        Macro2UE_NLOS.R(3, 0) = -0.4011;
        Macro2UE_NLOS.R(3, 1) = 0.0259;
        Macro2UE_NLOS.R(3, 2) = -0.1716;
        Macro2UE_NLOS.R(3, 3) = 0.8964;
        Macro2UE_NLOS.R(3, 4) = 0.0000;
        Macro2UE_NLOS.R(3, 5) = -0.0741;
        Macro2UE_NLOS.R(3, 6) = -0.0030;
        Macro2UE_NLOS.R(4, 0) = 0.0000;
        Macro2UE_NLOS.R(4, 1) = 0.0000;
        Macro2UE_NLOS.R(4, 2) = 0.0000;
        Macro2UE_NLOS.R(4, 3) = 0.0000;
        Macro2UE_NLOS.R(4, 4) = 1.0000;
        Macro2UE_NLOS.R(4, 5) = 0;
        Macro2UE_NLOS.R(4, 6) = 0;
        Macro2UE_NLOS.R(5, 0) = -0.3252;
        Macro2UE_NLOS.R(5, 1) = 0.3008;
        Macro2UE_NLOS.R(5, 2) = 0.0352;
        Macro2UE_NLOS.R(5, 3) = -0.0741;
        Macro2UE_NLOS.R(5, 4) = 0;
        Macro2UE_NLOS.R(5, 5) = 0.8911;
        Macro2UE_NLOS.R(5, 6) = -0.0542;
        Macro2UE_NLOS.R(6, 0) = -0.0341;
        Macro2UE_NLOS.R(6, 1) = 0.2808;
        Macro2UE_NLOS.R(6, 2) = 0.1131;
        Macro2UE_NLOS.R(6, 3) = -0.0030;
        Macro2UE_NLOS.R(6, 4) = 0;
        Macro2UE_NLOS.R(6, 5) = -0.0542;
        Macro2UE_NLOS.R(6, 6) = 0.9509;
        //            NLOS.R(0, 0) = 0.8343; NLOS.R(0, 1) = 0.0653; NLOS.R(0, 2) = 0.1889; NLOS.R(0, 3) = -0.4005; NLOS.R(0, 4) = 0.0000;  NLOS.R(0, 5) = -0.3212; NLOS.R(0, 6) = -0.0213;
        //            NLOS.R(1, 0) = 0.0653; NLOS.R(1, 1) = 0.9113; NLOS.R(1, 2) = -0.0088;NLOS.R(1, 3) = 0.0274;  NLOS.R(1, 4) = 0.0000;  NLOS.R(1, 5) = 0.2971;  NLOS.R(1, 6) = 0.2758;
        //            NLOS.R(2, 0) = 0.1889; NLOS.R(2, 1) = -0.0088;NLOS.R(2, 2) = 0.9662; NLOS.R(2, 3) = -0.1729; NLOS.R(2, 4) = 0.0000;  NLOS.R(2, 5) = 0.0272;  NLOS.R(2, 6) = 0.0031;
        //            NLOS.R(3, 0) = -0.4005;NLOS.R(3, 1) = 0.0274; NLOS.R(3, 2) = -0.1729;NLOS.R(3, 3) = 0.8963;  NLOS.R(3, 4) = 0.0000;  NLOS.R(3, 5) = -0.0740; NLOS.R(3, 6) = -0.0103;
        //            NLOS.R(4, 0) = 0.0000; NLOS.R(4, 1) = 0.0000; NLOS.R(4, 2) = 0.0000; NLOS.R(4, 3) = 0.0000;  NLOS.R(4, 4) = 1.0000;  NLOS.R(4, 5) = 0;       NLOS.R(4, 6) = 0;
        //            NLOS.R(5, 0) = -0.3212;NLOS.R(5, 1) = 0.2971; NLOS.R(5, 2) = 0.0272; NLOS.R(5, 3) = -0.0740; NLOS.R(5, 4) = 0;       NLOS.R(5, 5) = 0.8944;  NLOS.R(5, 6) = -0.0484;
        //            NLOS.R(6, 0) = -0.0213;NLOS.R(6, 1) = 0.2758; NLOS.R(6, 2) = 0.0031; NLOS.R(6, 3) = -0.0103; NLOS.R(6, 4) = 0;       NLOS.R(6, 5) = -0.0484; NLOS.R(6, 6) = 0.9597;

        Macro2UE_O2I.R = itpp::mat(7, 7);
        Macro2UE_O2I.R(0, 0) = 0.7731;
        Macro2UE_O2I.R(0, 1) = 0.2628;
        Macro2UE_O2I.R(0, 2) = 0.2841;
        Macro2UE_O2I.R(0, 3) = -0.3433;
        Macro2UE_O2I.R(0, 4) = 0.0000;
        Macro2UE_O2I.R(0, 5) = -0.3435;
        Macro2UE_O2I.R(0, 6) = -0.1294;
        Macro2UE_O2I.R(1, 0) = 0.2628;
        Macro2UE_O2I.R(1, 1) = 0.9483;
        Macro2UE_O2I.R(1, 2) = -0.0509;
        Macro2UE_O2I.R(1, 3) = 0.1557;
        Macro2UE_O2I.R(1, 4) = 0.0000;
        Macro2UE_O2I.R(1, 5) = -0.0592;
        Macro2UE_O2I.R(1, 6) = 0.0368;
        Macro2UE_O2I.R(2, 0) = 0.2841;
        Macro2UE_O2I.R(2, 1) = -0.0509;
        Macro2UE_O2I.R(2, 2) = 0.9086;
        Macro2UE_O2I.R(2, 3) = 0.0628;
        Macro2UE_O2I.R(2, 4) = 0.0000;
        Macro2UE_O2I.R(2, 5) = 0.0136;
        Macro2UE_O2I.R(2, 6) = 0.2950;
        Macro2UE_O2I.R(3, 0) = -0.3433;
        Macro2UE_O2I.R(3, 1) = 0.1557;
        Macro2UE_O2I.R(3, 2) = 0.0628;
        Macro2UE_O2I.R(3, 3) = 0.9219;
        Macro2UE_O2I.R(3, 4) = 0.0000;
        Macro2UE_O2I.R(3, 5) = -0.0560;
        Macro2UE_O2I.R(3, 6) = -0.0299;
        Macro2UE_O2I.R(4, 0) = 0.0000;
        Macro2UE_O2I.R(4, 1) = 0.0000;
        Macro2UE_O2I.R(4, 2) = 0.0000;
        Macro2UE_O2I.R(4, 3) = 0.0000;
        Macro2UE_O2I.R(4, 4) = 1.0000;
        Macro2UE_O2I.R(4, 5) = 0;
        Macro2UE_O2I.R(4, 6) = 0;
        Macro2UE_O2I.R(5, 0) = -0.3435;
        Macro2UE_O2I.R(5, 1) = -0.0592;
        Macro2UE_O2I.R(5, 2) = 0.0136;
        Macro2UE_O2I.R(5, 3) = -0.0560;
        Macro2UE_O2I.R(5, 4) = 0;
        Macro2UE_O2I.R(5, 5) = 0.9017;
        Macro2UE_O2I.R(5, 6) = 0.2492;
        Macro2UE_O2I.R(6, 0) = -0.1294;
        Macro2UE_O2I.R(6, 1) = 0.0368;
        Macro2UE_O2I.R(6, 2) = 0.2950;
        Macro2UE_O2I.R(6, 3) = -0.0299;
        Macro2UE_O2I.R(6, 4) = 0;
        Macro2UE_O2I.R(6, 5) = 0.2492;
        Macro2UE_O2I.R(6, 6) = 0.9121;
        //            O2I.R(0, 0) = 0.8439; O2I.R(0, 1) = 0.2869; O2I.R(0, 2) = 0.2377;  O2I.R(0, 3) = -0.3213; O2I.R(0, 4) = 0.0000; O2I.R(0, 5) = -0.2143; O2I.R(0, 6) = 0.0025;
        //            O2I.R(1, 0) = 0.2869; O2I.R(1, 1) = 0.9197; O2I.R(1, 2) = -0.0360; O2I.R(1, 3) = 0.1652;  O2I.R(1, 4) = 0.0000; O2I.R(1, 5) = 0.2063;  O2I.R(1, 6) = -0.0266;
        //            O2I.R(2, 0) = 0.2377; O2I.R(2, 1) = -0.0360;O2I.R(2, 2) = 0.9313;  O2I.R(2, 3) = 0.0433;  O2I.R(2, 4) = 0.0000; O2I.R(2, 5) = -0.0076; O2I.R(2, 6) = 0.2700;
        //            O2I.R(3, 0) = -0.3213;O2I.R(3, 1) = 0.1652; O2I.R(3, 2) = 0.0433;  O2I.R(3, 3) = 0.9297;  O2I.R(3, 4) = 0.0000; O2I.R(3, 5) = -0.0565; O2I.R(3, 6) = 0.0050;
        //            O2I.R(4, 0) = 0.0000; O2I.R(4, 1) = 0.0000; O2I.R(4, 2) = 0.0000;  O2I.R(4, 3) = 0.0000;  O2I.R(4, 4) = 1.0000; O2I.R(4, 5) = 0;       O2I.R(4, 6) = 0;
        //            O2I.R(5, 0) = -0.2143;O2I.R(5, 1) = 0.2063; O2I.R(5, 2) = -0.0076; O2I.R(5, 3) = -0.0565; O2I.R(5, 4) = 0;      O2I.R(5, 5) = 0.9188;  O2I.R(5, 6) = 0.2773;
        //            O2I.R(6, 0) = 0.0025; O2I.R(6, 1) = -0.0266;O2I.R(6, 2) = 0.2700;  O2I.R(6, 3) = 0.0050;  O2I.R(6, 4) = 0;      O2I.R(6, 5) = 0.2773;  O2I.R(6, 6) = 0.9217;

    } else {
        assert(false);
    }

}

void P::Set_itu_umi_channel_parameters_ModeB() {
    double dFrequencyGHz = FX.DRadioFrequencyMHz_Macro * 1e-3;

    double dModifiedFreqGHz
            = (dFrequencyGHz >= 2.0) ? dFrequencyGHz : 2.0;

    Macro2UE_LOS.DelaySpreadAVE = -0.24 * log10(1 + dModifiedFreqGHz) - 7.14;
    //    std::cout << Macro2UE_LOS.DelaySpreadAVE << std::endl;
    Macro2UE_LOS.DelaySpreadSTD = 0.38;
    Macro2UE_NLOS.DelaySpreadAVE = -0.24 * log10(1 + dModifiedFreqGHz) - 6.83;
    Macro2UE_NLOS.DelaySpreadSTD = 0.28 + 0.16 * log10(1 + dModifiedFreqGHz);
    Macro2UE_O2I.DelaySpreadAVE = -6.62;
    Macro2UE_O2I.DelaySpreadSTD = 0.32;

    Macro2UE_LOS.AODSpreadAVE = 1.21 - 0.05 * log10(1 + dModifiedFreqGHz);
    Macro2UE_LOS.AODSpreadSTD = 0.41;
    Macro2UE_NLOS.AODSpreadAVE = 1.53 - 0.23 * log10(1 + dModifiedFreqGHz);
    Macro2UE_NLOS.AODSpreadSTD = 0.33 + 0.11 * log10(1 + dModifiedFreqGHz);
    Macro2UE_O2I.AODSpreadAVE = 1.25;
    Macro2UE_O2I.AODSpreadSTD = 0.42;

    Macro2UE_LOS.AOASpreadAVE = 1.73 - 0.08 * log10(1 + dModifiedFreqGHz);
    Macro2UE_LOS.AOASpreadSTD = 0.28 + 0.014 * log10(1 + dModifiedFreqGHz);
    Macro2UE_NLOS.AOASpreadAVE = 1.81 - 0.08 * log10(1 + dModifiedFreqGHz);
    Macro2UE_NLOS.AOASpreadSTD = 0.3 + 0.05 * log10(1 + dModifiedFreqGHz);
    Macro2UE_O2I.AOASpreadAVE = 1.76;
    Macro2UE_O2I.AOASpreadSTD = 0.16;

    Macro2UE_LOS.EOASpreadAVE = 0.73 - 0.1 * log10(1 + dModifiedFreqGHz);
    Macro2UE_LOS.EOASpreadSTD = 0.34 - 0.04 * log10(1 + dModifiedFreqGHz);
    Macro2UE_NLOS.EOASpreadAVE = 0.92 - 0.04 * log10(1 + dModifiedFreqGHz);
    Macro2UE_NLOS.EOASpreadSTD = 0.41 - 0.07 * log10(1 + dModifiedFreqGHz);
    Macro2UE_O2I.EOASpreadAVE = 1.01;
    Macro2UE_O2I.EOASpreadSTD = 0.43;

    //zhengyi
    Macro2UE_LOS.ShadowFadingSTD = 4;
    Macro2UE_NLOS.ShadowFadingSTD = 8.2;
    Macro2UE_O2I.ShadowFadingSTD = 7;

    Macro2UE_LOS.KFactorDBAVE = 9;
    Macro2UE_LOS.KFactorDBSTD = 5;

    Macro2UE_LOS.DelayScaling = 3;
    Macro2UE_NLOS.DelayScaling = 2.1;
    Macro2UE_O2I.DelayScaling = 2.2;


    Macro2UE_LOS.XPR = 9;
    Macro2UE_LOS.XPR_u = 9;
    Macro2UE_LOS.XPR_sigma = 3;
    Macro2UE_NLOS.XPR = 8;
    Macro2UE_NLOS.XPR_u = 8;
    Macro2UE_NLOS.XPR_sigma = 3;
    Macro2UE_O2I.XPR = 9;
    Macro2UE_O2I.XPR_u = 9;
    Macro2UE_O2I.XPR_sigma = 5;

    Macro2UE_LOS.NumOfCluster = 12;
    Macro2UE_NLOS.NumOfCluster = 19;
    Macro2UE_O2I.NumOfCluster = 12;


    Macro2UE_LOS.ClusterDS = 5;
    Macro2UE_NLOS.ClusterDS = 11;
    Macro2UE_O2I.ClusterDS = 11;

    Macro2UE_LOS.ClusterASD = 3;
    Macro2UE_NLOS.ClusterASD = 10;
    Macro2UE_O2I.ClusterASD = 5;


    Macro2UE_LOS.ClusterASA = 17;
    Macro2UE_NLOS.ClusterASA = 22;
    Macro2UE_O2I.ClusterASA = 8;


    Macro2UE_LOS.ClusterESA = 7;
    Macro2UE_NLOS.ClusterESA = 7;
    Macro2UE_O2I.ClusterESA = 3;

    Macro2UE_LOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_NLOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_O2I.PerClusterShadowingSTDDB = 4;

    Macro2UE_LOS.DSCorrDistM = 7;
    Macro2UE_NLOS.DSCorrDistM = 10;
    Macro2UE_O2I.DSCorrDistM = 10;

    Macro2UE_LOS.ASDCorrDistM = 8;
    Macro2UE_NLOS.ASDCorrDistM = 10;
    Macro2UE_O2I.ASDCorrDistM = 11;

    Macro2UE_LOS.ASACorrDistM = 8;
    Macro2UE_NLOS.ASACorrDistM = 9;
    Macro2UE_O2I.ASACorrDistM = 17;

    Macro2UE_LOS.SFCorrDistM = 10;
    Macro2UE_NLOS.SFCorrDistM = 13;
    Macro2UE_O2I.SFCorrDistM = 7;


    Macro2UE_LOS.KCorrDistM = 15;

    Macro2UE_LOS.ESACorrDistM = 12;
    Macro2UE_NLOS.ESACorrDistM = 10;
    Macro2UE_O2I.ESACorrDistM = 25;

    Macro2UE_LOS.ESDCorrDistM = 12;
    Macro2UE_NLOS.ESDCorrDistM = 10;
    Macro2UE_O2I.ESDCorrDistM = 25;


    //    Macro2UE_O2I.EODSpreadAVE = 0.96;
    //    Macro2UE_O2I.EODSpreadSTD = 0.4;
    //    Macro2UE_O2I.EOASpreadAVE = 1.01;
    //    Macro2UE_O2I.EOASpreadSTD = 0.43;

    //NA
    //    Macro2UE_O2I.ClusterESD = -10000;


    //ZHENGYI
    //    LOS.ClusterESD = 3;
    //    NLOS.ClusterESD = 3;








    //    assert(false);
    //    Macro2UE_O2I.DelaySpreadAVE = -6.62;
    //    Macro2UE_O2I.DelaySpreadSTD = 0.32;
    //    Macro2UE_O2I.AODSpreadAVE = 1.25;
    //    Macro2UE_O2I.AODSpreadSTD = 0.42;
    //    Macro2UE_O2I.AOASpreadAVE = 1.76;
    //    Macro2UE_O2I.AOASpreadSTD = 0.16;
    //        O2I.ShadowFadingSTD = 7;
    //zhengyi
    //    Macro2UE_O2I.ShadowFadingSTD = 7;
    //    Macro2UE_O2I.DelayScaling = 2.2;
    //    Macro2UE_O2I.XPR = 9;
    //    Macro2UE_O2I.XPR_u = 9;
    //    Macro2UE_O2I.XPR_sigma = 5;
    //    Macro2UE_O2I.NumOfCluster = 12;
    //    Macro2UE_O2I.ClusterASD = 5;
    //    Macro2UE_O2I.ClusterASA = 20;
    //    Macro2UE_O2I.ClusterESA = 6;
    //    Macro2UE_O2I.PerClusterShadowingSTDDB = 4;
    //
    //    Macro2UE_O2I.DSCorrDistM = 10;
    //    Macro2UE_O2I.ASDCorrDistM = 11;
    //    Macro2UE_O2I.ASACorrDistM = 17;
    //    Macro2UE_O2I.SFCorrDistM = 7;
    //    Macro2UE_O2I.ESDCorrDistM = 25;
    //    Macro2UE_O2I.ESACorrDistM = 25;
    //    ///elevation
    //
    //    Macro2UE_O2I.EODSpreadAVE = 0.96;
    //    Macro2UE_O2I.EODSpreadSTD = 0.4;
    ////    Macro2UE_O2I.EOASpreadAVE = 1.01;
    ////    Macro2UE_O2I.EOASpreadSTD = 0.43;
    //
    //    //NA
    //    Macro2UE_O2I.ClusterESD = -10000;




    if (P::s().FX.I2DOr3DChannel == P::IS2DCHANNEL) {
        assert(false);
        //高频没有非3d的channel，see 38900
        //2d 参数与低频相同

    } else if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        itpp::mat Macro2UE_LOS_R_square = itpp::mat(7, 7);
        Macro2UE_LOS_R_square = itpp::zeros(7, 7);
        Macro2UE_LOS_R_square =
                "1	0.5	0.8	-0.4	-0.7	0	0.2;"
                "0	1	0.4	-0.5	-0.2	0.5	0.3;"
                "0	0	1	-0.4	-0.3	0	0;"
                "0	0	0	1	0.5	0	0;"
                "0	0	0	0	1	0	0;"
                "0	0	0	0	0	1	0;"
                "0	0	0	0	0	0	1;";

        //from up triangle matrix to symmetric matrix
        itpp::mat aa = Macro2UE_LOS_R_square + Macro2UE_LOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_LOS_R_square));
        //        std::cout << aa << std::endl;
        itpp::cmat bb = itpp::sqrtm(aa);
        //        std::cout << bb << std::endl;

        Macro2UE_LOS.R = itpp::real(bb);
        //        std::cout << LOS.R << std::endl;

        itpp::mat Macro2UE_NLOS_R_square = itpp::mat(7, 7);
        Macro2UE_NLOS_R_square = itpp::zeros(7, 7);
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_NLOS_R_square =
                "1	0	0.4	-0.7	0	-0.5	0;"
                "0	1	0	0	0	0.5	0.5;"
                "0	0	1	-0.4	0	0	0.2;"
                "0	0	0	1	0	0	0;"
                "0	0	0	0	1	0	0;"
                "0	0	0	0	0	1	0;"
                "0	0	0	0	0	0	1;";

        itpp::mat aa_nlos = Macro2UE_NLOS_R_square + Macro2UE_NLOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_NLOS_R_square));
        itpp::cmat bb_nlos = itpp::sqrtm(aa_nlos);
        Macro2UE_NLOS.R = itpp::real(bb_nlos);


        itpp::mat Macro2UE_O2I_R_square = itpp::mat(7, 7);
        Macro2UE_O2I_R_square = itpp::zeros(7, 7);
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_O2I_R_square =
                "1	0.4	0.4	-0.5	0	0	-0.2;"
                "0	1	0	0.2	0	-0.2	0.0;"
                "0	0	1	0.0	0	0	0.5;"
                "0	0	0	1	0	0	0.4;"
                "0	0	0	0	1	0	0;"
                "0	0	0	0	0	1	0.5;"
                "0	0	0	0	0	0	1;";

        itpp::mat aa_o2i = Macro2UE_O2I_R_square + Macro2UE_O2I_R_square.H() - itpp::diag(itpp::diag(Macro2UE_O2I_R_square));
        itpp::cmat bb_o2i = itpp::sqrtm(aa_o2i);
        Macro2UE_O2I.R = itpp::real(bb_o2i);

    } else {
        assert(false);
    }



}

void P::Set_itu_uma_channel_parameters_ModeA() {
    Macro2UE_LOS.DelaySpreadAVE = -7.03;
    Macro2UE_LOS.DelaySpreadSTD = 0.66;
    Macro2UE_NLOS.DelaySpreadAVE = -6.44;
    Macro2UE_NLOS.DelaySpreadSTD = 0.39;
    Macro2UE_O2I.DelaySpreadAVE = -6.62;
    Macro2UE_O2I.DelaySpreadSTD = 0.32;
    Macro2UE_LOS.AODSpreadAVE = 1.15;
    Macro2UE_LOS.AODSpreadSTD = 0.28;
    Macro2UE_NLOS.AODSpreadAVE = 1.41;
    Macro2UE_NLOS.AODSpreadSTD = 0.28;
    Macro2UE_O2I.AODSpreadAVE = 1.25;
    Macro2UE_O2I.AODSpreadSTD = 0.42;
    Macro2UE_LOS.AOASpreadAVE = 1.81;
    Macro2UE_LOS.AOASpreadSTD = 0.20;
    Macro2UE_NLOS.AOASpreadAVE = 1.87;
    Macro2UE_NLOS.AOASpreadSTD = 0.11;
    Macro2UE_O2I.AOASpreadAVE = 1.76;
    Macro2UE_O2I.AOASpreadSTD = 0.16;
    Macro2UE_LOS.ShadowFadingSTD = 4;
    Macro2UE_NLOS.ShadowFadingSTD = 6;
    Macro2UE_O2I.ShadowFadingSTD = 7;
    Macro2UE_LOS.KFactorDBAVE = 9;
    Macro2UE_LOS.KFactorDBSTD = 3.5;
    Macro2UE_LOS.DelayScaling = 2.5;
    Macro2UE_NLOS.DelayScaling = 2.3;
    Macro2UE_O2I.DelayScaling = 2.2;
    Macro2UE_LOS.XPR = 8;
    Macro2UE_NLOS.XPR = 7;
    Macro2UE_O2I.XPR = 9;
    Macro2UE_LOS.XPR_u = 8;
    Macro2UE_NLOS.XPR_u = 7;
    Macro2UE_O2I.XPR_u = 9;
    Macro2UE_LOS.XPR_sigma = 4;
    Macro2UE_NLOS.XPR_sigma = 3;
    Macro2UE_O2I.XPR_sigma = 5;
    Macro2UE_LOS.NumOfCluster = 12;
    Macro2UE_NLOS.NumOfCluster = 20;
    Macro2UE_O2I.NumOfCluster = 12;
    Macro2UE_LOS.ClusterASD = 5;
    Macro2UE_NLOS.ClusterASD = 2;
    Macro2UE_O2I.ClusterASD = 5;
    Macro2UE_LOS.ClusterASA = 11;
    Macro2UE_NLOS.ClusterASA = 15;
    Macro2UE_O2I.ClusterASA = 8;
    Macro2UE_LOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_NLOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_O2I.PerClusterShadowingSTDDB = 4;
    Macro2UE_LOS.DSCorrDistM = 30;
    Macro2UE_NLOS.DSCorrDistM = 40;
    Macro2UE_O2I.DSCorrDistM = 10;
    Macro2UE_LOS.ASDCorrDistM = 18;
    Macro2UE_NLOS.ASDCorrDistM = 50;
    Macro2UE_O2I.ASDCorrDistM = 11;
    Macro2UE_LOS.ASACorrDistM = 15;
    Macro2UE_NLOS.ASACorrDistM = 50;
    Macro2UE_O2I.ASACorrDistM = 17;
    Macro2UE_LOS.SFCorrDistM = 37;
    Macro2UE_NLOS.SFCorrDistM = 50;
    Macro2UE_O2I.SFCorrDistM = 7;
    Macro2UE_LOS.KCorrDistM = 12;
    ///elevation
    Macro2UE_LOS.EODSpreadAVE = 0.85;
    Macro2UE_NLOS.EODSpreadAVE = 0.92;
    Macro2UE_O2I.EODSpreadAVE = 0.96;
    Macro2UE_LOS.EODSpreadSTD = 0.40;
    Macro2UE_NLOS.EODSpreadSTD = 0.49;
    Macro2UE_O2I.EODSpreadSTD = 0.4;
    Macro2UE_LOS.EOASpreadAVE = 0.95;
    Macro2UE_NLOS.EOASpreadAVE = 1.26;
    Macro2UE_O2I.EOASpreadAVE = 1.01;
    Macro2UE_LOS.EOASpreadSTD = 0.16;
    Macro2UE_NLOS.EOASpreadSTD = 0.16;
    Macro2UE_O2I.EOASpreadSTD = 0.43;
    Macro2UE_LOS.ClusterESD = 3;
    Macro2UE_NLOS.ClusterESD = 3;
    Macro2UE_O2I.ClusterESD = 3;
    Macro2UE_LOS.ClusterESA = 7;
    Macro2UE_NLOS.ClusterESA = 7;
    Macro2UE_O2I.ClusterESA = 3;
    Macro2UE_LOS.ESDCorrDistM = 15;
    Macro2UE_NLOS.ESDCorrDistM = 50;
    Macro2UE_O2I.ESDCorrDistM = 25;
    Macro2UE_LOS.ESACorrDistM = 15;
    Macro2UE_NLOS.ESACorrDistM = 50;
    Macro2UE_O2I.ESACorrDistM = 25;

    if (P::s().FX.I2DOr3DChannel == P::IS2DCHANNEL) {
        Macro2UE_LOS.R = itpp::mat(5, 5);
        Macro2UE_LOS.R(0, 0) = 0.8063;
        Macro2UE_LOS.R(0, 1) = 0.2450;
        Macro2UE_LOS.R(0, 2) = 0.4792;
        Macro2UE_LOS.R(0, 3) = -0.1204;
        Macro2UE_LOS.R(0, 4) = -0.2138;
        Macro2UE_LOS.R(1, 0) = 0.2450;
        Macro2UE_LOS.R(1, 1) = 0.9241;
        Macro2UE_LOS.R(1, 2) = -0.1086;
        Macro2UE_LOS.R(1, 3) = -0.2716;
        Macro2UE_LOS.R(1, 4) = 0.0218;
        Macro2UE_LOS.R(2, 0) = 0.4792;
        Macro2UE_LOS.R(2, 1) = -0.1086;
        Macro2UE_LOS.R(2, 2) = 0.8257;
        Macro2UE_LOS.R(2, 3) = -0.2716;
        Macro2UE_LOS.R(2, 4) = -0.0556;
        Macro2UE_LOS.R(3, 0) = -0.1204;
        Macro2UE_LOS.R(3, 1) = -0.2716;
        Macro2UE_LOS.R(3, 2) = -0.2716;
        Macro2UE_LOS.R(3, 3) = 0.9152;
        Macro2UE_LOS.R(3, 4) = -0.0185;
        Macro2UE_LOS.R(4, 0) = -0.2138;
        Macro2UE_LOS.R(4, 1) = 0.0218;
        Macro2UE_LOS.R(4, 2) = -0.0556;
        Macro2UE_LOS.R(4, 3) = -0.0185;
        Macro2UE_LOS.R(4, 4) = 0.9749;

        Macro2UE_NLOS.R = itpp::mat(5, 5);
        Macro2UE_NLOS.R(0, 0) = 0.9139;
        Macro2UE_NLOS.R(0, 1) = 0.1477;
        Macro2UE_NLOS.R(0, 2) = 0.3180;
        Macro2UE_NLOS.R(0, 3) = -0.2044;
        Macro2UE_NLOS.R(0, 4) = 0.0000;
        Macro2UE_NLOS.R(1, 0) = 0.1477;
        Macro2UE_NLOS.R(1, 1) = 0.9139;
        Macro2UE_NLOS.R(1, 2) = 0.2044;
        Macro2UE_NLOS.R(1, 3) = -0.3180;
        Macro2UE_NLOS.R(1, 4) = 0.0000;
        Macro2UE_NLOS.R(2, 0) = 0.3180;
        Macro2UE_NLOS.R(2, 1) = 0.2044;
        Macro2UE_NLOS.R(2, 2) = 0.9231;
        Macro2UE_NLOS.R(2, 3) = 0.0704;
        Macro2UE_NLOS.R(2, 4) = 0.0000;
        Macro2UE_NLOS.R(3, 0) = -0.2044;
        Macro2UE_NLOS.R(3, 1) = -0.3180;
        Macro2UE_NLOS.R(3, 2) = 0.0704;
        Macro2UE_NLOS.R(3, 3) = 0.9231;
        Macro2UE_NLOS.R(3, 4) = 0.0000;
        Macro2UE_NLOS.R(4, 0) = 0.0000;
        Macro2UE_NLOS.R(4, 1) = 0.0000;
        Macro2UE_NLOS.R(4, 2) = 0.0000;
        Macro2UE_NLOS.R(4, 3) = 0.0000;
        Macro2UE_NLOS.R(4, 4) = 1.0000;

        Macro2UE_O2I.R = itpp::mat(5, 5);
        Macro2UE_O2I.R(0, 0) = 0.9139;
        Macro2UE_O2I.R(0, 1) = 0.1477;
        Macro2UE_O2I.R(0, 2) = 0.3180;
        Macro2UE_O2I.R(0, 3) = -0.2044;
        Macro2UE_O2I.R(0, 4) = 0.0000;
        Macro2UE_O2I.R(1, 0) = 0.1477;
        Macro2UE_O2I.R(1, 1) = 0.9139;
        Macro2UE_O2I.R(1, 2) = 0.2044;
        Macro2UE_O2I.R(1, 3) = -0.3180;
        Macro2UE_O2I.R(1, 4) = 0.0000;
        Macro2UE_O2I.R(2, 0) = 0.3180;
        Macro2UE_O2I.R(2, 1) = 0.2044;
        Macro2UE_O2I.R(2, 2) = 0.9231;
        Macro2UE_O2I.R(2, 3) = 0.0704;
        Macro2UE_O2I.R(2, 4) = 0.0000;
        Macro2UE_O2I.R(3, 0) = -0.2044;
        Macro2UE_O2I.R(3, 1) = -0.3180;
        Macro2UE_O2I.R(3, 2) = 0.0704;
        Macro2UE_O2I.R(3, 3) = 0.9231;
        Macro2UE_O2I.R(3, 4) = 0.0000;
        Macro2UE_O2I.R(4, 0) = 0.0000;
        Macro2UE_O2I.R(4, 1) = 0.0000;
        Macro2UE_O2I.R(4, 2) = 0.0000;
        Macro2UE_O2I.R(4, 3) = 0.0000;
        Macro2UE_O2I.R(4, 4) = 1.0000;

    } else if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
        Macro2UE_LOS.R = itpp::mat(7, 7);
        Macro2UE_LOS.R(0, 0) = 0.7361;
        Macro2UE_LOS.R(0, 1) = 0.2725;
        Macro2UE_LOS.R(0, 2) = 0.5189;
        Macro2UE_LOS.R(0, 3) = -0.1762;
        Macro2UE_LOS.R(0, 4) = -0.2305;
        Macro2UE_LOS.R(0, 5) = -0.1294;
        Macro2UE_LOS.R(0, 6) = -0.1170;
        Macro2UE_LOS.R(1, 0) = 0.2725;
        Macro2UE_LOS.R(1, 1) = 0.8404;
        Macro2UE_LOS.R(1, 2) = -0.0979;
        Macro2UE_LOS.R(1, 3) = -0.3322;
        Macro2UE_LOS.R(1, 4) = 0.0301;
        Macro2UE_LOS.R(1, 5) = 0.3045;
        Macro2UE_LOS.R(1, 6) = -0.0768;
        Macro2UE_LOS.R(2, 0) = 0.5189;
        Macro2UE_LOS.R(2, 1) = -0.0979;
        Macro2UE_LOS.R(2, 2) = 0.7825;
        Macro2UE_LOS.R(2, 3) = -0.2146;
        Macro2UE_LOS.R(2, 4) = -0.0472;
        Macro2UE_LOS.R(2, 5) = -0.1176;
        Macro2UE_LOS.R(2, 6) = 0.2161;
        Macro2UE_LOS.R(3, 0) = -0.1762;
        Macro2UE_LOS.R(3, 1) = -0.3322;
        Macro2UE_LOS.R(3, 2) = -0.2146;
        Macro2UE_LOS.R(3, 3) = 0.7376;
        Macro2UE_LOS.R(3, 4) = -0.0281;
        Macro2UE_LOS.R(3, 5) = 0.0410;
        Macro2UE_LOS.R(3, 6) = -0.5158;
        Macro2UE_LOS.R(4, 0) = -0.2305;
        Macro2UE_LOS.R(4, 1) = 0.0301;
        Macro2UE_LOS.R(4, 2) = -0.0472;
        Macro2UE_LOS.R(4, 3) = -0.0281;
        Macro2UE_LOS.R(4, 4) = 0.9707;
        Macro2UE_LOS.R(4, 5) = -0.0225;
        Macro2UE_LOS.R(4, 6) = -0.0158;
        Macro2UE_LOS.R(5, 0) = -0.1294;
        Macro2UE_LOS.R(5, 1) = 0.3045;
        Macro2UE_LOS.R(5, 2) = -0.1176;
        Macro2UE_LOS.R(5, 3) = 0.0410;
        Macro2UE_LOS.R(5, 4) = -0.0225;
        Macro2UE_LOS.R(5, 5) = 0.9346;
        Macro2UE_LOS.R(5, 6) = 0.0311;
        Macro2UE_LOS.R(6, 0) = -0.1170;
        Macro2UE_LOS.R(6, 1) = -0.0768;
        Macro2UE_LOS.R(6, 2) = 0.2161;
        Macro2UE_LOS.R(6, 3) = -0.5158;
        Macro2UE_LOS.R(6, 4) = -0.0158;
        Macro2UE_LOS.R(6, 5) = 0.0311;
        Macro2UE_LOS.R(6, 6) = 0.8164;
        //            LOS.R(0, 0) = 0.7047; LOS.R(0, 1) = 0.2112; LOS.R(0, 2) = 0.5295; LOS.R(0, 3) = -0.1465; LOS.R(0, 4) = -0.2417; LOS.R(0, 5) = 0.3119;  LOS.R(0, 6) = -0.0354;
        //            LOS.R(1, 0) = 0.2112; LOS.R(1, 1) = 0.8819; LOS.R(1, 2) = -0.1144;LOS.R(1, 3) = -0.3139; LOS.R(1, 4) = 0.0025;  LOS.R(1, 5) = 0.2505;  LOS.R(1, 6) = -0.0570;
        //            LOS.R(2, 0) = 0.5295; LOS.R(2, 1) = -0.1144;LOS.R(2, 2) = 0.7809; LOS.R(2, 3) = -0.2940; LOS.R(2, 4) = -0.0555; LOS.R(2, 5) = -0.0669; LOS.R(2, 6) = -0.0514;
        //            LOS.R(3, 0) = -0.1465;LOS.R(3, 1) = -0.3139;LOS.R(3, 2) = -0.2940;LOS.R(3, 3) = 0.8289;  LOS.R(3, 4) = -0.0614; LOS.R(3, 5) = 0.0666;  LOS.R(3, 6) = -0.3134;
        //            LOS.R(4, 0) = -0.2417;LOS.R(4, 1) = 0.0025; LOS.R(4, 2) = -0.0555;LOS.R(4, 3) = -0.0614; LOS.R(4, 4) = 0.9496;  LOS.R(4, 5) = 0.0430;   LOS.R(4, 6) = -0.1766;
        //            LOS.R(5, 0) = 0.3119; LOS.R(5, 1) = 0.2505; LOS.R(5, 2) = -0.0669;LOS.R(5, 3) = 0.0666;  LOS.R(5, 4) = 0.0430;  LOS.R(5, 5) = 0.9102;  LOS.R(5, 6) = 0.0274;
        //            LOS.R(6, 0) = -0.0354;LOS.R(6, 1) = -0.0570;LOS.R(6, 2) = -0.0514;LOS.R(6, 3) = -0.3134; LOS.R(6, 4) = -0.1766; LOS.R(6, 5) = 0.0274;  LOS.R(6, 6) = 0.9288;

        Macro2UE_NLOS.R = itpp::mat(7, 7);
        Macro2UE_NLOS.R(0, 0) = 0.8218;
        Macro2UE_NLOS.R(0, 1) = 0.2402;
        Macro2UE_NLOS.R(0, 2) = 0.3301;
        Macro2UE_NLOS.R(0, 3) = -0.1925;
        Macro2UE_NLOS.R(0, 4) = 0.0000;
        Macro2UE_NLOS.R(0, 5) = -0.3475;
        Macro2UE_NLOS.R(0, 6) = -0.0102;
        Macro2UE_NLOS.R(1, 0) = 0.2402;
        Macro2UE_NLOS.R(1, 1) = 0.7874;
        Macro2UE_NLOS.R(1, 2) = 0.2027;
        Macro2UE_NLOS.R(1, 3) = -0.3698;
        Macro2UE_NLOS.R(1, 4) = 0.0000;
        Macro2UE_NLOS.R(1, 5) = 0.3620;
        Macro2UE_NLOS.R(1, 6) = -0.1157;
        Macro2UE_NLOS.R(2, 0) = 0.3301;
        Macro2UE_NLOS.R(2, 1) = 0.2027;
        Macro2UE_NLOS.R(2, 2) = 0.9179;
        Macro2UE_NLOS.R(2, 3) = 0.0803;
        Macro2UE_NLOS.R(2, 4) = 0.0000;
        Macro2UE_NLOS.R(2, 5) = 0.0210;
        Macro2UE_NLOS.R(2, 6) = 0.0244;
        Macro2UE_NLOS.R(3, 0) = -0.1925;
        Macro2UE_NLOS.R(3, 1) = -0.3698;
        Macro2UE_NLOS.R(3, 2) = 0.0803;
        Macro2UE_NLOS.R(3, 3) = 0.8708;
        Macro2UE_NLOS.R(3, 4) = 0.0000;
        Macro2UE_NLOS.R(3, 5) = 0.0413;
        Macro2UE_NLOS.R(3, 6) = -0.2443;
        Macro2UE_NLOS.R(4, 0) = 0.0000;
        Macro2UE_NLOS.R(4, 1) = 0.0000;
        Macro2UE_NLOS.R(4, 2) = 0.0000;
        Macro2UE_NLOS.R(4, 3) = 0.0000;
        Macro2UE_NLOS.R(4, 4) = 1.0000;
        Macro2UE_NLOS.R(4, 5) = 0;
        Macro2UE_NLOS.R(4, 6) = 0;
        Macro2UE_NLOS.R(5, 0) = -0.3475;
        Macro2UE_NLOS.R(5, 1) = 0.3620;
        Macro2UE_NLOS.R(5, 2) = 0.0210;
        Macro2UE_NLOS.R(5, 3) = 0.0413;
        Macro2UE_NLOS.R(5, 4) = 0;
        Macro2UE_NLOS.R(5, 5) = 0.8633;
        Macro2UE_NLOS.R(5, 6) = 0.0262;
        Macro2UE_NLOS.R(6, 0) = -0.0102;
        Macro2UE_NLOS.R(6, 1) = -0.1157;
        Macro2UE_NLOS.R(6, 2) = 0.0244;
        Macro2UE_NLOS.R(6, 3) = -0.2443;
        Macro2UE_NLOS.R(6, 4) = 0;
        Macro2UE_NLOS.R(6, 5) = 0.0262;
        Macro2UE_NLOS.R(6, 6) = 0.9621;
        //            NLOS.R(0, 0) = 0.8219; NLOS.R(0, 1) = 0.2358; NLOS.R(0, 2) = 0.3317;  NLOS.R(0, 3) = -0.1979; NLOS.R(0, 4) = 0.0000; NLOS.R(0, 5) = -0.3452; NLOS.R(0, 6) = -0.0208;
        //            NLOS.R(1, 0) = 0.2358; NLOS.R(1, 1) = 0.8136; NLOS.R(1, 2) = 0.1965;  NLOS.R(1, 3) = -0.3468; NLOS.R(1, 4) = 0.0000; NLOS.R(1, 5) = 0.3488;  NLOS.R(1, 6) = -0.0448;
        //            NLOS.R(2, 0) = 0.3317; NLOS.R(2, 1) = 0.1965; NLOS.R(2, 2) = 0.9191;  NLOS.R(2, 3) = 0.0759;  NLOS.R(2, 4) = 0.0000; NLOS.R(2, 5) = 0.0243;  NLOS.R(2, 6) = 0.0173;
        //            NLOS.R(3, 0) = -0.1979;NLOS.R(3, 1) = -0.3468;NLOS.R(3, 2) = 0.0759;  NLOS.R(3, 3) = 0.8846;  NLOS.R(3, 4) = 0.0000; NLOS.R(3, 5) = 0.0300;  NLOS.R(3, 6) = -0.2268;
        //            NLOS.R(4, 0) = 0.0000; NLOS.R(4, 1) = 0.0000; NLOS.R(4, 2) = 0.0000;  NLOS.R(4, 3) = 0.0000;  NLOS.R(4, 4) = 1.0000; NLOS.R(4, 5) = 0;       NLOS.R(4, 6) = 0;
        //            NLOS.R(5, 0) = -0.3452;NLOS.R(5, 1) = 0.3488; NLOS.R(5, 2) = 0.0243;  NLOS.R(5, 3) = 0.0300;  NLOS.R(5, 4) = 0;      NLOS.R(5, 5) = 0.8704;  NLOS.R(5, 6) = 0.0080;
        //            NLOS.R(6, 0) = -0.0208;NLOS.R(6, 1) = -0.0448;NLOS.R(6, 2) = 0.0173;  NLOS.R(6, 3) = -0.2268; NLOS.R(6, 4) = 0;      NLOS.R(6, 5) = 0.0080;  NLOS.R(6, 6) = 0.9725;

        Macro2UE_O2I.R = itpp::mat(7, 7);
        Macro2UE_O2I.R(0, 0) = 0.7731;
        Macro2UE_O2I.R(0, 1) = 0.2628;
        Macro2UE_O2I.R(0, 2) = 0.2841;
        Macro2UE_O2I.R(0, 3) = -0.3433;
        Macro2UE_O2I.R(0, 4) = 0.0000;
        Macro2UE_O2I.R(0, 5) = -0.3435;
        Macro2UE_O2I.R(0, 6) = -0.1294;
        Macro2UE_O2I.R(1, 0) = 0.2628;
        Macro2UE_O2I.R(1, 1) = 0.9483;
        Macro2UE_O2I.R(1, 2) = -0.0509;
        Macro2UE_O2I.R(1, 3) = 0.1557;
        Macro2UE_O2I.R(1, 4) = 0.0000;
        Macro2UE_O2I.R(1, 5) = -0.0592;
        Macro2UE_O2I.R(1, 6) = 0.0368;
        Macro2UE_O2I.R(2, 0) = 0.2841;
        Macro2UE_O2I.R(2, 1) = -0.0509;
        Macro2UE_O2I.R(2, 2) = 0.9086;
        Macro2UE_O2I.R(2, 3) = 0.0628;
        Macro2UE_O2I.R(2, 4) = 0.0000;
        Macro2UE_O2I.R(2, 5) = 0.0136;
        Macro2UE_O2I.R(2, 6) = 0.2950;
        Macro2UE_O2I.R(3, 0) = -0.3433;
        Macro2UE_O2I.R(3, 1) = 0.1557;
        Macro2UE_O2I.R(3, 2) = 0.0628;
        Macro2UE_O2I.R(3, 3) = 0.9219;
        Macro2UE_O2I.R(3, 4) = 0.0000;
        Macro2UE_O2I.R(3, 5) = -0.0560;
        Macro2UE_O2I.R(3, 6) = -0.0299;
        Macro2UE_O2I.R(4, 0) = 0.0000;
        Macro2UE_O2I.R(4, 1) = 0.0000;
        Macro2UE_O2I.R(4, 2) = 0.0000;
        Macro2UE_O2I.R(4, 3) = 0.0000;
        Macro2UE_O2I.R(4, 4) = 1.0000;
        Macro2UE_O2I.R(4, 5) = 0;
        Macro2UE_O2I.R(4, 6) = 0;
        Macro2UE_O2I.R(5, 0) = -0.3435;
        Macro2UE_O2I.R(5, 1) = -0.0592;
        Macro2UE_O2I.R(5, 2) = 0.0136;
        Macro2UE_O2I.R(5, 3) = -0.0560;
        Macro2UE_O2I.R(5, 4) = 0;
        Macro2UE_O2I.R(5, 5) = 0.9017;
        Macro2UE_O2I.R(5, 6) = 0.2492;
        Macro2UE_O2I.R(6, 0) = -0.1294;
        Macro2UE_O2I.R(6, 1) = 0.0368;
        Macro2UE_O2I.R(6, 2) = 0.2950;
        Macro2UE_O2I.R(6, 3) = -0.0299;
        Macro2UE_O2I.R(6, 4) = 0;
        Macro2UE_O2I.R(6, 5) = 0.2492;
        Macro2UE_O2I.R(6, 6) = 0.9121;
        //            O2I.R(0, 0) = 0.8219; O2I.R(0, 1) = 0.2358; O2I.R(0, 2) = 0.3317;  O2I.R(0, 3) = -0.1979; O2I.R(0, 4) = 0.0000; O2I.R(0, 5) = -0.3452; O2I.R(0, 6) = -0.0208;
        //            O2I.R(1, 0) = 0.2358; O2I.R(1, 1) = 0.8136; O2I.R(1, 2) = 0.1965;  O2I.R(1, 3) = -0.3468; O2I.R(1, 4) = 0.0000; O2I.R(1, 5) = 0.3488;  O2I.R(1, 6) = -0.0448;
        //            O2I.R(2, 0) = 0.3317; O2I.R(2, 1) = 0.1965; O2I.R(2, 2) = 0.9191;  O2I.R(2, 3) = 0.0759;  O2I.R(2, 4) = 0.0000; O2I.R(2, 5) = 0.0243;  O2I.R(2, 6) = 0.0173;
        //            O2I.R(3, 0) = -0.1979;O2I.R(3, 1) = -0.3468;O2I.R(3, 2) = 0.0759;  O2I.R(3, 3) = 0.8846;  O2I.R(3, 4) = 0.0000; O2I.R(3, 5) = 0.0300;  O2I.R(3, 6) = -0.2268;
        //            O2I.R(4, 0) = 0.0000; O2I.R(4, 1) = 0.0000; O2I.R(4, 2) = 0.0000;  O2I.R(4, 3) = 0.0000;  O2I.R(4, 4) = 1.0000; O2I.R(4, 5) = 0;       O2I.R(4, 6) = 0;
        //            O2I.R(5, 0) = -0.3452;O2I.R(5, 1) = 0.3488; O2I.R(5, 2) = 0.0243;  O2I.R(5, 3) = 0.0300;  O2I.R(5, 4) = 0;      O2I.R(5, 5) = 0.8704;  O2I.R(5, 6) = 0.0080;
        //            O2I.R(6, 0) = -0.0208;O2I.R(6, 1) = -0.0448;O2I.R(6, 2) = 0.0173;  O2I.R(6, 3) = -0.2268; O2I.R(6, 4) = 0;      O2I.R(6, 5) = 0.0080;  O2I.R(6, 6) = 0.9725;

    } else {
        assert(false);
    }
}

void P::Set_itu_uma_channel_parameters_ModeB() {
    double dFrequencyGHz = FX.DRadioFrequencyMHz_Macro * 1e-3;

    //20171213 当频率低于6G时，我们将频率设置为6;频率高于6G时，频率不变
    double dModifiedFreqGHz
            = (dFrequencyGHz >= 6.0) ? dFrequencyGHz : 6.0;

    Macro2UE_LOS.DelaySpreadAVE = -6.955 - 0.0963 * log10(dModifiedFreqGHz);
    Macro2UE_LOS.DelaySpreadSTD = 0.66;
    Macro2UE_NLOS.DelaySpreadAVE = -6.28 - 0.204 * log10(dModifiedFreqGHz);
    Macro2UE_NLOS.DelaySpreadSTD = 0.39;
    Macro2UE_O2I.DelaySpreadAVE = -6.62;
    Macro2UE_O2I.DelaySpreadSTD = 0.32;

    Macro2UE_LOS.AODSpreadAVE = 1.06 + 0.1114 * log10(dModifiedFreqGHz);
    Macro2UE_LOS.AODSpreadSTD = 0.28;
    Macro2UE_NLOS.AODSpreadAVE = 1.5 - 0.1144 * log10(dModifiedFreqGHz);
    Macro2UE_NLOS.AODSpreadSTD = 0.28;
    Macro2UE_O2I.AODSpreadAVE = 1.25;
    Macro2UE_O2I.AODSpreadSTD = 0.42;

    Macro2UE_LOS.AOASpreadAVE = 1.81;
    Macro2UE_LOS.AOASpreadSTD = 0.20;
    Macro2UE_NLOS.AOASpreadAVE = 2.08 - 0.27 * log10(dModifiedFreqGHz);
    Macro2UE_NLOS.AOASpreadSTD = 0.11;
    Macro2UE_O2I.AOASpreadAVE = 1.76;
    Macro2UE_O2I.AOASpreadSTD = 0.16;
    //20171213
    Macro2UE_LOS.EOASpreadAVE = 0.95;
    Macro2UE_LOS.EOASpreadSTD = 0.16;
    Macro2UE_NLOS.EOASpreadAVE = -0.3236 * log10(dModifiedFreqGHz) + 1.512;
    Macro2UE_NLOS.EOASpreadSTD = 0.16;
    Macro2UE_O2I.EOASpreadAVE = 1.01;
    Macro2UE_O2I.EOASpreadSTD = 0.43;
    //        LOS.ShadowFadingSTD = 4;
    //        NLOS.ShadowFadingSTD = 6;
    //        O2I.ShadowFadingSTD = 7;
    //zhengyi

    Macro2UE_LOS.ShadowFadingSTD = 4;
    Macro2UE_NLOS.ShadowFadingSTD = 6;
    Macro2UE_O2I.ShadowFadingSTD = 7;

    Macro2UE_LOS.KFactorDBAVE = 9;
    Macro2UE_LOS.KFactorDBSTD = 3.5;


    Macro2UE_LOS.DelayScaling = 2.5;
    Macro2UE_NLOS.DelayScaling = 2.3;
    Macro2UE_O2I.DelayScaling = 2.2;
    //20171213 这里的XPR没有用上，用的是XPR_u
    Macro2UE_LOS.XPR = 8;
    Macro2UE_NLOS.XPR = 7;
    Macro2UE_O2I.XPR = 9;
    Macro2UE_LOS.XPR_u = 8;
    Macro2UE_NLOS.XPR_u = 7;
    Macro2UE_O2I.XPR_u = 9;
    Macro2UE_LOS.XPR_sigma = 4;
    Macro2UE_NLOS.XPR_sigma = 3;
    Macro2UE_O2I.XPR_sigma = 5;

    Macro2UE_LOS.NumOfCluster = 12;
    Macro2UE_NLOS.NumOfCluster = 20;
    Macro2UE_O2I.NumOfCluster = 12;

    Macro2UE_LOS.ClusterASD = 5;
    Macro2UE_NLOS.ClusterASD = 2;
    Macro2UE_O2I.ClusterASD = 5;

    Macro2UE_LOS.ClusterASA = 11;
    Macro2UE_NLOS.ClusterASA = 15;
    Macro2UE_O2I.ClusterASA = 8; //20171213
    //    Macro2UE_O2I.ClusterASA = 20;

    Macro2UE_LOS.ClusterESA = 7;
    Macro2UE_NLOS.ClusterESA = 7;
    Macro2UE_O2I.ClusterESA = 3; //20171213

    //    Macro2UE_O2I.ClusterESA = 6;

    Macro2UE_LOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_NLOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_O2I.PerClusterShadowingSTDDB = 4;

    Macro2UE_LOS.DSCorrDistM = 30;
    Macro2UE_NLOS.DSCorrDistM = 40;
    Macro2UE_O2I.DSCorrDistM = 10;

    Macro2UE_LOS.ASDCorrDistM = 18;
    Macro2UE_NLOS.ASDCorrDistM = 50;
    Macro2UE_O2I.ASDCorrDistM = 11;

    Macro2UE_LOS.ASACorrDistM = 15;
    Macro2UE_NLOS.ASACorrDistM = 50;
    Macro2UE_O2I.ASACorrDistM = 17;

    Macro2UE_LOS.SFCorrDistM = 37;
    Macro2UE_NLOS.SFCorrDistM = 50;
    Macro2UE_O2I.SFCorrDistM = 7;

    Macro2UE_LOS.KCorrDistM = 12;

    Macro2UE_LOS.ESACorrDistM = 15;
    Macro2UE_NLOS.ESACorrDistM = 50;
    Macro2UE_O2I.ESACorrDistM = 25;

    Macro2UE_LOS.ESDCorrDistM = 15;
    Macro2UE_NLOS.ESDCorrDistM = 50;
    Macro2UE_O2I.ESDCorrDistM = 25;


    //    ///elevation
    //    { //这部分还没跑
    //        Macro2UE_LOS.EODSpreadAVE = 0.85;
    //        Macro2UE_NLOS.EODSpreadAVE = 0.92;
    //        Macro2UE_O2I.EODSpreadAVE = 0.96;
    //        Macro2UE_LOS.EODSpreadSTD = 0.40;
    //        Macro2UE_NLOS.EODSpreadSTD = 0.49;
    //        Macro2UE_O2I.EODSpreadSTD = 0.4;
    //    }

    { //20171212
        Macro2UE_LOS.EODSpreadAVE = -1;
        Macro2UE_NLOS.EODSpreadAVE = -1;
        Macro2UE_O2I.EODSpreadAVE = -1;
        Macro2UE_LOS.EODSpreadSTD = -1;
        Macro2UE_NLOS.EODSpreadSTD = -1;
        Macro2UE_O2I.EODSpreadSTD = -1;
    }

    //    Macro2UE_LOS.EOASpreadAVE = 0.95;
    //    Macro2UE_NLOS.EOASpreadAVE = -0.3236 * log10(dFcGHz) + 1.512;
    //    Macro2UE_O2I.EOASpreadAVE = 1.01;
    //
    //    Macro2UE_LOS.EOASpreadSTD = 0.16;
    //    Macro2UE_NLOS.EOASpreadSTD = 0.16;
    //    Macro2UE_O2I.EOASpreadSTD = 0.43;

    //20171213 没找到
    Macro2UE_LOS.ClusterESD = 3;
    Macro2UE_NLOS.ClusterESD = 3;
    Macro2UE_O2I.ClusterESD = 3;



    if (P::s().FX.I2DOr3DChannel == P::IS2DCHANNEL) {
        assert(false);
    } else if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa

        itpp::mat Macro2UE_LOS_R_square = itpp::mat(7, 7);
        Macro2UE_LOS_R_square = itpp::zeros(7, 7);
        Macro2UE_LOS_R_square(0, 0) = 1;
        Macro2UE_LOS_R_square(1, 1) = 1;
        Macro2UE_LOS_R_square(2, 2) = 1;
        Macro2UE_LOS_R_square(3, 3) = 1;
        Macro2UE_LOS_R_square(4, 4) = 1;
        Macro2UE_LOS_R_square(5, 5) = 1;
        Macro2UE_LOS_R_square(6, 6) = 1;

        Macro2UE_LOS_R_square(0, 1) = 0.4; //ds-asd
        Macro2UE_LOS_R_square(0, 2) = 0.8; //ds-asa
        Macro2UE_LOS_R_square(0, 3) = -0.4; //ds-sf
        Macro2UE_LOS_R_square(0, 4) = -0.4; //ds-k
        Macro2UE_LOS_R_square(0, 5) = -0.2; //ds-zsd
        Macro2UE_LOS_R_square(0, 6) = 0.0; //ds-zsa
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_LOS_R_square(1, 2) = 0.0; //asd-asa
        Macro2UE_LOS_R_square(1, 3) = -0.5; //asd-sf
        Macro2UE_LOS_R_square(1, 4) = -0.0; //asd-k
        Macro2UE_LOS_R_square(1, 5) = 0.5; //asd-zsd
        Macro2UE_LOS_R_square(1, 6) = 0.0; //asd-zsa

        Macro2UE_LOS_R_square(2, 3) = -0.5; //asa-sf
        Macro2UE_LOS_R_square(2, 4) = -0.2; //asa-k
        Macro2UE_LOS_R_square(2, 5) = -0.3; //asa-zsd
        Macro2UE_LOS_R_square(2, 6) = 0.4; //asa-zsa

        Macro2UE_LOS_R_square(3, 4) = 0.0; //sf-k
        Macro2UE_LOS_R_square(3, 5) = 0.0; //sf-zsd
        Macro2UE_LOS_R_square(3, 6) = -0.8; //sf-zsa

        Macro2UE_LOS_R_square(4, 5) = 0.0; //k-zsd
        Macro2UE_LOS_R_square(4, 6) = 0.0; //k-zsa

        Macro2UE_LOS_R_square(5, 6) = 0.0; //zsd-zsa

        //from up triangle matrix to symmetric matrix
        itpp::mat aa = Macro2UE_LOS_R_square + Macro2UE_LOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_LOS_R_square));
        itpp::cmat bb = itpp::sqrtm(aa);
        Macro2UE_LOS.R = itpp::real(bb);

        itpp::mat Macro2UE_NLOS_R_square = itpp::mat(7, 7);
        Macro2UE_NLOS_R_square = itpp::zeros(7, 7);
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_NLOS_R_square(0, 0) = 1;
        Macro2UE_NLOS_R_square(1, 1) = 1;
        Macro2UE_NLOS_R_square(2, 2) = 1;
        Macro2UE_NLOS_R_square(3, 3) = 1;
        Macro2UE_NLOS_R_square(4, 4) = 1;
        Macro2UE_NLOS_R_square(5, 5) = 1;
        Macro2UE_NLOS_R_square(6, 6) = 1;
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_NLOS_R_square(0, 1) = 0.4; //ds-asd
        Macro2UE_NLOS_R_square(0, 2) = 0.6; //ds-asa
        Macro2UE_NLOS_R_square(0, 3) = -0.4; //ds-sf
        Macro2UE_NLOS_R_square(0, 4) = 0.0; //ds-k
        Macro2UE_NLOS_R_square(0, 5) = -0.5; //ds-zsd
        Macro2UE_NLOS_R_square(0, 6) = 0.0; //ds-zsa

        Macro2UE_NLOS_R_square(1, 2) = 0.4; //asd-asa
        Macro2UE_NLOS_R_square(1, 3) = -0.6; //asd-sf
        Macro2UE_NLOS_R_square(1, 4) = 0.0; //asd-k
        Macro2UE_NLOS_R_square(1, 5) = 0.5; //asd-zsd
        Macro2UE_NLOS_R_square(1, 6) = -0.1; //asd-zsa
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_NLOS_R_square(2, 3) = -0.0; //asa-sf
        Macro2UE_NLOS_R_square(2, 4) = 0.0; //asa-k
        Macro2UE_NLOS_R_square(2, 5) = 0.0; //asa-zsd
        Macro2UE_NLOS_R_square(2, 6) = 0.0; //asa-zsa

        Macro2UE_NLOS_R_square(3, 4) = 0.0; //sf-k
        Macro2UE_NLOS_R_square(3, 5) = 0.0; //sf-zsd
        Macro2UE_NLOS_R_square(3, 6) = -0.4; //sf-zsa

        Macro2UE_NLOS_R_square(4, 5) = 0.0; //k-zsd
        Macro2UE_NLOS_R_square(4, 6) = 0.0; //k-zsa

        Macro2UE_NLOS_R_square(5, 6) = 0.0; //zsd-zsa

        itpp::mat aa_nlos = Macro2UE_NLOS_R_square + Macro2UE_NLOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_NLOS_R_square));
        itpp::cmat bb_nlos = itpp::sqrtm(aa_nlos);
        Macro2UE_NLOS.R = itpp::real(bb_nlos);


        itpp::mat Macro2UE_O2I_R_square = itpp::mat(7, 7);
        Macro2UE_O2I_R_square = itpp::zeros(7, 7);

        Macro2UE_O2I_R_square(0, 0) = 1;
        Macro2UE_O2I_R_square(1, 1) = 1;
        Macro2UE_O2I_R_square(2, 2) = 1;
        Macro2UE_O2I_R_square(3, 3) = 1;
        Macro2UE_O2I_R_square(4, 4) = 1;
        Macro2UE_O2I_R_square(5, 5) = 1;
        Macro2UE_O2I_R_square(6, 6) = 1;

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_O2I_R_square(0, 1) = 0.4; //ds-asd
        //        Macro2UE_O2I_R_square(0, 2) = 0.0;//ds-asa
        Macro2UE_O2I_R_square(0, 2) = 0.4; //ds-asa //20171213
        Macro2UE_O2I_R_square(0, 3) = -0.5; //ds-sf
        Macro2UE_O2I_R_square(0, 4) = 0.0; //ds-k
        //        Macro2UE_O2I_R_square(0, 5) = 0.0;//ds-zsd
        Macro2UE_O2I_R_square(0, 5) = -0.6; //ds-zsd 20171213
        //        Macro2UE_O2I_R_square(0, 6) = -0.53;//ds-zsa
        Macro2UE_O2I_R_square(0, 6) = -0.2; //ds-zsa 20171213

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_O2I_R_square(1, 2) = 0.0; //asd-asa
        //        Macro2UE_O2I_R_square(1, 3) = 0;//asd-sf
        Macro2UE_O2I_R_square(1, 3) = 0.2; //asd-sf 20171213
        Macro2UE_O2I_R_square(1, 4) = 0.0; //asd-k
        //        Macro2UE_O2I_R_square(1, 5) = 0.0;//asd-zsd
        Macro2UE_O2I_R_square(1, 5) = -0.2; //asd-zsd 20171213
        //        Macro2UE_O2I_R_square(1, 6) = 0.42;//asd-zsa
        Macro2UE_O2I_R_square(1, 6) = 0.0; //asd-zsa 20171213

        //        Macro2UE_O2I_R_square(2, 3) = 0.53;//asa-sf
        Macro2UE_O2I_R_square(2, 3) = 0.0; //asa-sf
        Macro2UE_O2I_R_square(2, 4) = 0.0; //asa-k
        Macro2UE_O2I_R_square(2, 5) = 0.0; //asa-zsd
        //        Macro2UE_O2I_R_square(2, 6) = 0.0;//asa-zsa
        Macro2UE_O2I_R_square(2, 6) = 0.5; //asa-zsa 20171213

        Macro2UE_O2I_R_square(3, 4) = 0.0; //sf-k 
        Macro2UE_O2I_R_square(3, 5) = 0.0; //sf-zsd
        //        Macro2UE_O2I_R_square(3, 6) = 0.4;//sf-zsa
        Macro2UE_O2I_R_square(3, 6) = 0.4; //sf-zsa 20171213

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_O2I_R_square(4, 5) = 0.0; //k-zsd 
        Macro2UE_O2I_R_square(4, 6) = 0.0; //k-zsa

        //        Macro2UE_O2I_R_square(5, 6) = 0.0;//zsd-zsa
        Macro2UE_O2I_R_square(5, 6) = 0.0; //zsd-zsa 20171213

        itpp::mat aa_o2i = Macro2UE_O2I_R_square + Macro2UE_O2I_R_square.H() - itpp::diag(itpp::diag(Macro2UE_O2I_R_square));
        itpp::cmat bb_o2i = itpp::sqrtm(aa_o2i);
        Macro2UE_O2I.R = itpp::real(bb_o2i);

    } else {
        assert(false);
    }



    //    Macro2UE_NLOS.EODSpreadAVE = 1.37; //1.06;
    //    Macro2UE_NLOS.EODSpreadSTD = 0.38; //0.21;
    // 20171127
    //    Macro2UE_NLOS.EODSpreadAVE = 1.08;
    //    Macro2UE_NLOS.EODSpreadSTD = 0.36;

}

void P::Set_itu_rma_channel_parameters_ModelA() {
    Macro2UE_LOS.DelaySpreadAVE = -7.49;
    Macro2UE_LOS.DelaySpreadSTD = 0.55;
    Macro2UE_NLOS.DelaySpreadAVE = -7.43;
    Macro2UE_NLOS.DelaySpreadSTD = 0.48;
    Macro2UE_O2I.DelaySpreadAVE = -7.47;
    Macro2UE_O2I.DelaySpreadSTD = 0.24;

    Macro2UE_LOS.AODSpreadAVE = 0.90;
    Macro2UE_LOS.AODSpreadSTD = 0.38;
    Macro2UE_NLOS.AODSpreadAVE = 0.95;
    Macro2UE_NLOS.AODSpreadSTD = 0.45;
    Macro2UE_O2I.AODSpreadAVE = 0.67;
    Macro2UE_O2I.AODSpreadSTD = 0.18;

    Macro2UE_LOS.AOASpreadAVE = 1.52;
    Macro2UE_LOS.AOASpreadSTD = 0.24;
    Macro2UE_NLOS.AOASpreadAVE = 1.52;
    Macro2UE_NLOS.AOASpreadSTD = 0.13;
    Macro2UE_O2I.AOASpreadAVE = 1.66;
    Macro2UE_O2I.AOASpreadSTD = 0.21;

    Macro2UE_LOS.EOASpreadAVE = 0.47;
    Macro2UE_LOS.EOASpreadSTD = 0.40;
    Macro2UE_NLOS.EOASpreadAVE = 0.58;
    Macro2UE_NLOS.EOASpreadSTD = 0.37;
    Macro2UE_O2I.EOASpreadAVE = 0.93;
    Macro2UE_O2I.EOASpreadSTD = 0.22;

    Macro2UE_LOS.ShadowFadingSTD = 6;
    Macro2UE_NLOS.ShadowFadingSTD = 8;
    Macro2UE_O2I.ShadowFadingSTD = 8;

    Macro2UE_LOS.KFactorDBAVE = 7;
    Macro2UE_LOS.KFactorDBSTD = 4;

    Macro2UE_LOS.DelayScaling = 3.8;
    Macro2UE_NLOS.DelayScaling = 1.7;
    Macro2UE_O2I.DelayScaling = 1.7;

    Macro2UE_LOS.XPR_u = 12;
    Macro2UE_NLOS.XPR_u = 7;
    Macro2UE_O2I.XPR_u = 7;

    Macro2UE_LOS.XPR_sigma = 4;
    Macro2UE_NLOS.XPR_sigma = 3;
    Macro2UE_O2I.XPR_sigma = 5;

    Macro2UE_LOS.NumOfCluster = 11;
    Macro2UE_NLOS.NumOfCluster = 10;
    Macro2UE_O2I.NumOfCluster = 10;

    Macro2UE_LOS.ClusterASD = 2;
    Macro2UE_NLOS.ClusterASD = 2;
    Macro2UE_O2I.ClusterASD = 2;

    Macro2UE_LOS.ClusterASA = 3;
    Macro2UE_NLOS.ClusterASA = 3;
    Macro2UE_O2I.ClusterASA = 3;

    Macro2UE_LOS.ClusterESA = 3;
    Macro2UE_NLOS.ClusterESA = 3;
    Macro2UE_O2I.ClusterESA = 3;

    Macro2UE_LOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_NLOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_O2I.PerClusterShadowingSTDDB = 3;

    Macro2UE_LOS.DSCorrDistM = 50;
    Macro2UE_NLOS.DSCorrDistM = 36;
    Macro2UE_O2I.DSCorrDistM = 36;

    Macro2UE_LOS.ASDCorrDistM = 25;
    Macro2UE_NLOS.ASDCorrDistM = 30;
    Macro2UE_O2I.ASDCorrDistM = 30;

    Macro2UE_LOS.ASACorrDistM = 35;
    Macro2UE_NLOS.ASACorrDistM = 40;
    Macro2UE_O2I.ASACorrDistM = 40;

    Macro2UE_LOS.SFCorrDistM = 37;
    Macro2UE_NLOS.SFCorrDistM = 120;
    Macro2UE_O2I.SFCorrDistM = 120;

    Macro2UE_LOS.KCorrDistM = 40;

    Macro2UE_LOS.ESACorrDistM = 15;
    Macro2UE_NLOS.ESACorrDistM = 50;
    Macro2UE_O2I.ESACorrDistM = 50;

    Macro2UE_LOS.ESDCorrDistM = 15;
    Macro2UE_NLOS.ESDCorrDistM = 50;
    Macro2UE_O2I.ESDCorrDistM = 50;

    ///elevation(need to check)
    {
        Macro2UE_LOS.EODSpreadAVE = -1;
        Macro2UE_LOS.EODSpreadSTD = -1;
        Macro2UE_LOS.EODoffsetAVE = -1;


        Macro2UE_NLOS.EODSpreadAVE = -1;
        Macro2UE_NLOS.EODSpreadSTD = -1;
        Macro2UE_NLOS.EODoffsetAVE = -1;

        Macro2UE_O2I.EODSpreadAVE = -1;
        Macro2UE_O2I.EODSpreadSTD = -1;
        Macro2UE_O2I.EODSpreadAVE = -1;
    }
    //    Macro2UE_LOS.EOASpreadAVE = 1.08;
    //    Macro2UE_NLOS.EOASpreadAVE = 1.00;
    //    Macro2UE_LOS.EOASpreadSTD = 0.16;
    //    Macro2UE_NLOS.EOASpreadSTD = 0.16;
    Macro2UE_LOS.ClusterESD = 3;
    Macro2UE_NLOS.ClusterESD = 3;
    Macro2UE_O2I.ClusterESD = 3;

    //    Macro2UE_LOS.ClusterESA = 7;
    //    Macro2UE_NLOS.ClusterESA = 7;
    //    Macro2UE_LOS.ESDCorrDistM = 15;
    //    Macro2UE_NLOS.ESDCorrDistM = 50;
    //    Macro2UE_LOS.ESACorrDistM = 15;
    //    Macro2UE_NLOS.ESACorrDistM = 50;

    if (P::s().FX.I2DOr3DChannel == P::IS2DCHANNEL) {
        Macro2UE_LOS.R = itpp::mat(5, 5);
        Macro2UE_LOS.R(0, 0) = 0.9659;
        Macro2UE_LOS.R(0, 1) = 0.0000;
        Macro2UE_LOS.R(0, 2) = 0.0000;
        Macro2UE_LOS.R(0, 3) = -0.2588;
        Macro2UE_LOS.R(0, 4) = 0.0000;
        Macro2UE_LOS.R(1, 0) = 0.0000;
        Macro2UE_LOS.R(1, 1) = 1.0000;
        Macro2UE_LOS.R(1, 2) = 0.0000;
        Macro2UE_LOS.R(1, 3) = 0.0000;
        Macro2UE_LOS.R(1, 4) = 0.0000;
        Macro2UE_LOS.R(2, 0) = 0.0000;
        Macro2UE_LOS.R(2, 1) = 0.0000;
        Macro2UE_LOS.R(2, 2) = 1.0000;
        Macro2UE_LOS.R(2, 3) = 0.0000;
        Macro2UE_LOS.R(2, 4) = 0.0000;
        Macro2UE_LOS.R(3, 0) = -0.2588;
        Macro2UE_LOS.R(3, 1) = 0.0000;
        Macro2UE_LOS.R(3, 2) = 0.0000;
        Macro2UE_LOS.R(3, 3) = 0.9659;
        Macro2UE_LOS.R(3, 4) = 0.0000;
        Macro2UE_LOS.R(4, 0) = 0.0000;
        Macro2UE_LOS.R(4, 1) = 0.0000;
        Macro2UE_LOS.R(4, 2) = 0.0000;
        Macro2UE_LOS.R(4, 3) = 0.0000;
        Macro2UE_LOS.R(4, 4) = 1.0000;

        Macro2UE_NLOS.R = itpp::mat(5, 5);
        Macro2UE_NLOS.R(0, 0) = 0.9556;
        Macro2UE_NLOS.R(0, 1) = -0.1735;
        Macro2UE_NLOS.R(0, 2) = 0.0000;
        Macro2UE_NLOS.R(0, 3) = -0.2384;
        Macro2UE_NLOS.R(0, 4) = 0.0000;
        Macro2UE_NLOS.R(1, 0) = -0.1735;
        Macro2UE_NLOS.R(1, 1) = 0.9380;
        Macro2UE_NLOS.R(1, 2) = 0.0000;
        Macro2UE_NLOS.R(1, 3) = 0.3001;
        Macro2UE_NLOS.R(1, 4) = 0.0000;
        Macro2UE_NLOS.R(2, 0) = 0.0000;
        Macro2UE_NLOS.R(2, 1) = 0.0000;
        Macro2UE_NLOS.R(2, 2) = 1.0000;
        Macro2UE_NLOS.R(2, 3) = 0.0000;
        Macro2UE_NLOS.R(2, 4) = 0.0000;
        Macro2UE_NLOS.R(3, 0) = -0.2384;
        Macro2UE_NLOS.R(3, 1) = 0.3001;
        Macro2UE_NLOS.R(3, 2) = 0.0000;
        Macro2UE_NLOS.R(3, 3) = 0.9237;
        Macro2UE_NLOS.R(3, 4) = 0.0000;
        Macro2UE_NLOS.R(4, 0) = 0.0000;
        Macro2UE_NLOS.R(4, 1) = 0.0000;
        Macro2UE_NLOS.R(4, 2) = 0.0000;
        Macro2UE_NLOS.R(4, 3) = 0.0000;
        Macro2UE_NLOS.R(4, 4) = 1.0000;

    } else if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {


        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa

        itpp::mat Macro2UE_LOS_R_square = itpp::mat(7, 7);
        Macro2UE_LOS_R_square = itpp::zeros(7, 7);
        Macro2UE_LOS_R_square(0, 0) = 1;
        Macro2UE_LOS_R_square(1, 1) = 1;
        Macro2UE_LOS_R_square(2, 2) = 1;
        Macro2UE_LOS_R_square(3, 3) = 1;
        Macro2UE_LOS_R_square(4, 4) = 1;
        Macro2UE_LOS_R_square(5, 5) = 1;
        Macro2UE_LOS_R_square(6, 6) = 1;

        Macro2UE_LOS_R_square(0, 1) = 0.0; //ds-asd
        Macro2UE_LOS_R_square(0, 2) = 0.0; //ds-asa
        Macro2UE_LOS_R_square(0, 3) = -0.5; //ds-sf
        Macro2UE_LOS_R_square(0, 4) = 0.0; //ds-k
        Macro2UE_LOS_R_square(0, 5) = -0.05; //ds-zsd
        Macro2UE_LOS_R_square(0, 6) = 0.27; //ds-zsa
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_LOS_R_square(1, 2) = -0.14; //asd-asa
        Macro2UE_LOS_R_square(1, 3) = 0.0; //asd-sf
        Macro2UE_LOS_R_square(1, 4) = 0.0; //asd-k
        Macro2UE_LOS_R_square(1, 5) = 0.73; //asd-zsd
        Macro2UE_LOS_R_square(1, 6) = -0.14; //asd-zsa

        Macro2UE_LOS_R_square(2, 3) = 0.0; //asa-sf
        Macro2UE_LOS_R_square(2, 4) = 0.0; //asa-k
        Macro2UE_LOS_R_square(2, 5) = -0.2; //asa-zsd
        Macro2UE_LOS_R_square(2, 6) = 0.24; //asa-zsa

        Macro2UE_LOS_R_square(3, 4) = 0.0; //sf-k
        Macro2UE_LOS_R_square(3, 5) = 0.01; //sf-zsd
        Macro2UE_LOS_R_square(3, 6) = -0.17; //sf-zsa

        Macro2UE_LOS_R_square(4, 5) = 0.0; //k-zsd
        Macro2UE_LOS_R_square(4, 6) = -0.02; //k-zsa

        Macro2UE_LOS_R_square(5, 6) = -0.07; //zsd-zsa

        //from up triangle matrix to symmetric matrix
        itpp::mat aa = Macro2UE_LOS_R_square + Macro2UE_LOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_LOS_R_square));
        itpp::cmat bb = itpp::sqrtm(aa);
        Macro2UE_LOS.R = itpp::real(bb);

        itpp::mat Macro2UE_NLOS_R_square = itpp::mat(7, 7);
        Macro2UE_NLOS_R_square = itpp::zeros(7, 7);
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_NLOS_R_square(0, 0) = 1;
        Macro2UE_NLOS_R_square(1, 1) = 1;
        Macro2UE_NLOS_R_square(2, 2) = 1;
        Macro2UE_NLOS_R_square(3, 3) = 1;
        Macro2UE_NLOS_R_square(4, 4) = 1;
        Macro2UE_NLOS_R_square(5, 5) = 1;
        Macro2UE_NLOS_R_square(6, 6) = 1;
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_NLOS_R_square(0, 1) = -0.4; //ds-asd
        Macro2UE_NLOS_R_square(0, 2) = 0.0; //ds-asa
        Macro2UE_NLOS_R_square(0, 3) = -0.5; //ds-sf
        Macro2UE_NLOS_R_square(0, 4) = 0.0; //ds-k
        Macro2UE_NLOS_R_square(0, 5) = -0.1; //ds-zsd
        Macro2UE_NLOS_R_square(0, 6) = -0.4; //ds-zsa

        Macro2UE_NLOS_R_square(1, 2) = 0.0; //asd-asa
        Macro2UE_NLOS_R_square(1, 3) = 0.6; //asd-sf
        Macro2UE_NLOS_R_square(1, 4) = 0.0; //asd-k
        Macro2UE_NLOS_R_square(1, 5) = 0.42; //asd-zsd
        Macro2UE_NLOS_R_square(1, 6) = -0.27; //asd-zsa
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_NLOS_R_square(2, 3) = 0.0; //asa-sf
        Macro2UE_NLOS_R_square(2, 4) = 0.0; //asa-k
        Macro2UE_NLOS_R_square(2, 5) = -0.18; //asa-zsd
        Macro2UE_NLOS_R_square(2, 6) = 0.26; //asa-zsa

        Macro2UE_NLOS_R_square(3, 4) = 0.0; //sf-k
        Macro2UE_NLOS_R_square(3, 5) = -0.04; //sf-zsd
        Macro2UE_NLOS_R_square(3, 6) = -0.25; //sf-zsa

        Macro2UE_NLOS_R_square(4, 5) = 0.0; //k-zsd
        Macro2UE_NLOS_R_square(4, 6) = 0.0; //k-zsa

        Macro2UE_NLOS_R_square(5, 6) = -0.27; //zsd-zsa

        itpp::mat aa_nlos = Macro2UE_NLOS_R_square + Macro2UE_NLOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_NLOS_R_square));
        itpp::cmat bb_nlos = itpp::sqrtm(aa_nlos);
        Macro2UE_NLOS.R = itpp::real(bb_nlos);


        itpp::mat Macro2UE_O2I_R_square = itpp::mat(7, 7);
        Macro2UE_O2I_R_square = itpp::zeros(7, 7);

        Macro2UE_O2I_R_square(0, 0) = 1;
        Macro2UE_O2I_R_square(1, 1) = 1;
        Macro2UE_O2I_R_square(2, 2) = 1;
        Macro2UE_O2I_R_square(3, 3) = 1;
        Macro2UE_O2I_R_square(4, 4) = 1;
        Macro2UE_O2I_R_square(5, 5) = 1;
        Macro2UE_O2I_R_square(6, 6) = 1;

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_O2I_R_square(0, 1) = 0.0; //ds-asd
        Macro2UE_O2I_R_square(0, 2) = 0.0; //ds-asa 
        Macro2UE_O2I_R_square(0, 3) = 0.0; //ds-sf
        Macro2UE_O2I_R_square(0, 4) = 0.0; //ds-k
        Macro2UE_O2I_R_square(0, 5) = 0.0; //ds-zsd 
        Macro2UE_O2I_R_square(0, 6) = 0.0; //ds-zsa 

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_O2I_R_square(1, 2) = -0.7; //asd-asa
        Macro2UE_O2I_R_square(1, 3) = 0.0; //asd-sf 
        Macro2UE_O2I_R_square(1, 4) = 0.0; //asd-k
        Macro2UE_O2I_R_square(1, 5) = 0.66; //asd-zsd 
        Macro2UE_O2I_R_square(1, 6) = 0.47; //asd-zsa 

        Macro2UE_O2I_R_square(2, 3) = 0.0; //asa-sf
        Macro2UE_O2I_R_square(2, 4) = 0.0; //asa-k
        Macro2UE_O2I_R_square(2, 5) = -0.55; //asa-zsd
        Macro2UE_O2I_R_square(2, 6) = -0.22; //asa-zsa 

        Macro2UE_O2I_R_square(3, 4) = 0.0; //sf-k 
        Macro2UE_O2I_R_square(3, 5) = 0.0; //sf-zsd
        Macro2UE_O2I_R_square(3, 6) = 0.0; //sf-zsa 

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_O2I_R_square(4, 5) = 0.0; //k-zsd 
        Macro2UE_O2I_R_square(4, 6) = 0.0; //k-zsa
        Macro2UE_O2I_R_square(5, 6) = 0.0; //zsd-zsa 

        itpp::mat aa_o2i = Macro2UE_O2I_R_square + Macro2UE_O2I_R_square.H() - itpp::diag(itpp::diag(Macro2UE_O2I_R_square));
        itpp::cmat bb_o2i = itpp::sqrtm(aa_o2i);
        Macro2UE_O2I.R = itpp::real(bb_o2i);

        //        
        //        Macro2UE_LOS.R = itpp::mat(7, 7);
        //        Macro2UE_LOS.R(0, 0) = 0.9659;
        //        Macro2UE_LOS.R(0, 1) = 0.0000;
        //        Macro2UE_LOS.R(0, 2) = 0.0000;
        //        Macro2UE_LOS.R(0, 3) = -0.2588;
        //        Macro2UE_LOS.R(0, 4) = 0.0000;
        //        Macro2UE_LOS.R(0, 5) = 0;
        //        Macro2UE_LOS.R(0, 6) = 0;
        //        Macro2UE_LOS.R(1, 0) = 0.0000;
        //        Macro2UE_LOS.R(1, 1) = 1.0000;
        //        Macro2UE_LOS.R(1, 2) = 0.0000;
        //        Macro2UE_LOS.R(1, 3) = 0.0000;
        //        Macro2UE_LOS.R(1, 4) = 0.0000;
        //        Macro2UE_LOS.R(1, 5) = 0;
        //        Macro2UE_LOS.R(1, 6) = 0;
        //        Macro2UE_LOS.R(2, 0) = 0.0000;
        //        Macro2UE_LOS.R(2, 1) = 0.0000;
        //        Macro2UE_LOS.R(2, 2) = 1.0000;
        //        Macro2UE_LOS.R(2, 3) = 0.0000;
        //        Macro2UE_LOS.R(2, 4) = 0.0000;
        //        Macro2UE_LOS.R(2, 5) = 0;
        //        Macro2UE_LOS.R(2, 6) = 0;
        //        Macro2UE_LOS.R(3, 0) = -0.2588;
        //        Macro2UE_LOS.R(3, 1) = 0.0000;
        //        Macro2UE_LOS.R(3, 2) = 0.0000;
        //        Macro2UE_LOS.R(3, 3) = 0.9659;
        //        Macro2UE_LOS.R(3, 4) = 0.0000;
        //        Macro2UE_LOS.R(3, 5) = 0;
        //        Macro2UE_LOS.R(3, 6) = 0;
        //        Macro2UE_LOS.R(4, 0) = 0.0000;
        //        Macro2UE_LOS.R(4, 1) = 0.0000;
        //        Macro2UE_LOS.R(4, 2) = 0.0000;
        //        Macro2UE_LOS.R(4, 3) = 0.0000;
        //        Macro2UE_LOS.R(4, 4) = 1.0000;
        //        Macro2UE_LOS.R(4, 5) = 0;
        //        Macro2UE_LOS.R(4, 6) = 0;
        //        Macro2UE_LOS.R(5, 0) = 0;
        //        Macro2UE_LOS.R(5, 1) = 0;
        //        Macro2UE_LOS.R(5, 2) = 0;
        //        Macro2UE_LOS.R(5, 3) = 0;
        //        Macro2UE_LOS.R(5, 4) = 0;
        //        Macro2UE_LOS.R(5, 5) = 1;
        //        Macro2UE_LOS.R(5, 6) = 0;
        //        Macro2UE_LOS.R(6, 0) = 0;
        //        Macro2UE_LOS.R(6, 1) = 0;
        //        Macro2UE_LOS.R(6, 2) = 0;
        //        Macro2UE_LOS.R(6, 3) = 0;
        //        Macro2UE_LOS.R(6, 4) = 0;
        //        Macro2UE_LOS.R(6, 5) = 0;
        //        Macro2UE_LOS.R(6, 6) = 1;
        //
        //        Macro2UE_NLOS.R = itpp::mat(7, 7);
        //        Macro2UE_NLOS.R(0, 0) = 0.9556;
        //        Macro2UE_NLOS.R(0, 1) = -0.1735;
        //        Macro2UE_NLOS.R(0, 2) = 0.0000;
        //        Macro2UE_NLOS.R(0, 3) = -0.2384;
        //        Macro2UE_NLOS.R(0, 4) = 0.0000;
        //        Macro2UE_NLOS.R(0, 5) = 0;
        //        Macro2UE_NLOS.R(0, 6) = 0;
        //        Macro2UE_NLOS.R(1, 0) = -0.1735;
        //        Macro2UE_NLOS.R(1, 1) = 0.9380;
        //        Macro2UE_NLOS.R(1, 2) = 0.0000;
        //        Macro2UE_NLOS.R(1, 3) = 0.3001;
        //        Macro2UE_NLOS.R(1, 4) = 0.0000;
        //        Macro2UE_NLOS.R(1, 5) = 0;
        //        Macro2UE_NLOS.R(1, 6) = 0;
        //        Macro2UE_NLOS.R(2, 0) = 0.0000;
        //        Macro2UE_NLOS.R(2, 1) = 0.0000;
        //        Macro2UE_NLOS.R(2, 2) = 1.0000;
        //        Macro2UE_NLOS.R(2, 3) = 0.0000;
        //        Macro2UE_NLOS.R(2, 4) = 0.0000;
        //        Macro2UE_NLOS.R(2, 5) = 0;
        //        Macro2UE_NLOS.R(2, 6) = 0;
        //        Macro2UE_NLOS.R(3, 0) = -0.2384;
        //        Macro2UE_NLOS.R(3, 1) = 0.3001;
        //        Macro2UE_NLOS.R(3, 2) = 0.0000;
        //        Macro2UE_NLOS.R(3, 3) = 0.9237;
        //        Macro2UE_NLOS.R(3, 4) = 0.0000;
        //        Macro2UE_NLOS.R(3, 5) = 0;
        //        Macro2UE_NLOS.R(3, 6) = 0;
        //        Macro2UE_NLOS.R(4, 0) = 0.0000;
        //        Macro2UE_NLOS.R(4, 1) = 0.0000;
        //        Macro2UE_NLOS.R(4, 2) = 0.0000;
        //        Macro2UE_NLOS.R(4, 3) = 0.0000;
        //        Macro2UE_NLOS.R(4, 4) = 1.0000;
        //        Macro2UE_NLOS.R(4, 5) = 0;
        //        Macro2UE_NLOS.R(4, 6) = 0;
        //        Macro2UE_NLOS.R(5, 0) = 0;
        //        Macro2UE_NLOS.R(5, 1) = 0;
        //        Macro2UE_NLOS.R(5, 2) = 0;
        //        Macro2UE_NLOS.R(5, 3) = 0;
        //        Macro2UE_NLOS.R(5, 4) = 0;
        //        Macro2UE_NLOS.R(5, 5) = 1;
        //        Macro2UE_NLOS.R(5, 6) = 0;
        //        Macro2UE_NLOS.R(6, 0) = 0;
        //        Macro2UE_NLOS.R(6, 1) = 0;
        //        Macro2UE_NLOS.R(6, 2) = 0;
        //        Macro2UE_NLOS.R(6, 3) = 0;
        //        Macro2UE_NLOS.R(6, 4) = 0;
        //        Macro2UE_NLOS.R(6, 5) = 0;
        //        Macro2UE_NLOS.R(6, 6) = 1;

    } else {
        assert(false);
    }
}

void P::Set_itu_rma_channel_parameters_ModelB() {
    //20171220 Rural下ModelA和ModelB 参数相同

    Macro2UE_LOS.DelaySpreadAVE = -7.49;
    Macro2UE_LOS.DelaySpreadSTD = 0.55;
    Macro2UE_NLOS.DelaySpreadAVE = -7.43;
    Macro2UE_NLOS.DelaySpreadSTD = 0.48;
    Macro2UE_O2I.DelaySpreadAVE = -7.47;
    Macro2UE_O2I.DelaySpreadSTD = 0.24;

    Macro2UE_LOS.AODSpreadAVE = 0.90;
    Macro2UE_LOS.AODSpreadSTD = 0.38;
    Macro2UE_NLOS.AODSpreadAVE = 0.95;
    Macro2UE_NLOS.AODSpreadSTD = 0.45;
    Macro2UE_O2I.AODSpreadAVE = 0.67;
    Macro2UE_O2I.AODSpreadSTD = 0.18;

    Macro2UE_LOS.AOASpreadAVE = 1.52;
    Macro2UE_LOS.AOASpreadSTD = 0.24;
    Macro2UE_NLOS.AOASpreadAVE = 1.52;
    Macro2UE_NLOS.AOASpreadSTD = 0.13;
    Macro2UE_O2I.AOASpreadAVE = 1.66;
    Macro2UE_O2I.AOASpreadSTD = 0.21;

    Macro2UE_LOS.EOASpreadAVE = 0.47;
    Macro2UE_LOS.EOASpreadSTD = 0.40;
    Macro2UE_NLOS.EOASpreadAVE = 0.58;
    Macro2UE_NLOS.EOASpreadSTD = 0.37;
    Macro2UE_O2I.EOASpreadAVE = 0.93;
    Macro2UE_O2I.EOASpreadSTD = 0.22;

    Macro2UE_LOS.ShadowFadingSTD = 6;
    Macro2UE_NLOS.ShadowFadingSTD = 8;
    Macro2UE_O2I.ShadowFadingSTD = 8;

    Macro2UE_LOS.KFactorDBAVE = 7;
    Macro2UE_LOS.KFactorDBSTD = 4;

    Macro2UE_LOS.DelayScaling = 3.8;
    Macro2UE_NLOS.DelayScaling = 1.7;
    Macro2UE_O2I.DelayScaling = 1.7;

    Macro2UE_LOS.XPR_u = 12;
    Macro2UE_NLOS.XPR_u = 7;
    Macro2UE_O2I.XPR_u = 7;

    Macro2UE_LOS.XPR_sigma = 4;
    Macro2UE_NLOS.XPR_sigma = 3;
    Macro2UE_O2I.XPR_sigma = 5;

    Macro2UE_LOS.NumOfCluster = 11;
    Macro2UE_NLOS.NumOfCluster = 10;
    Macro2UE_O2I.NumOfCluster = 10;

    Macro2UE_LOS.ClusterASD = 2;
    Macro2UE_NLOS.ClusterASD = 2;
    Macro2UE_O2I.ClusterASD = 2;

    Macro2UE_LOS.ClusterASA = 3;
    Macro2UE_NLOS.ClusterASA = 3;
    Macro2UE_O2I.ClusterASA = 3;

    Macro2UE_LOS.ClusterESA = 3;
    Macro2UE_NLOS.ClusterESA = 3;
    Macro2UE_O2I.ClusterESA = 3;

    Macro2UE_LOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_NLOS.PerClusterShadowingSTDDB = 3;
    Macro2UE_O2I.PerClusterShadowingSTDDB = 3;

    Macro2UE_LOS.DSCorrDistM = 50;
    Macro2UE_NLOS.DSCorrDistM = 36;
    Macro2UE_O2I.DSCorrDistM = 36;

    Macro2UE_LOS.ASDCorrDistM = 25;
    Macro2UE_NLOS.ASDCorrDistM = 30;
    Macro2UE_O2I.ASDCorrDistM = 30;

    Macro2UE_LOS.ASACorrDistM = 35;
    Macro2UE_NLOS.ASACorrDistM = 40;
    Macro2UE_O2I.ASACorrDistM = 40;

    Macro2UE_LOS.SFCorrDistM = 37;
    Macro2UE_NLOS.SFCorrDistM = 120;
    Macro2UE_O2I.SFCorrDistM = 120;

    Macro2UE_LOS.KCorrDistM = 40;

    Macro2UE_LOS.ESACorrDistM = 15;
    Macro2UE_NLOS.ESACorrDistM = 50;
    Macro2UE_O2I.ESACorrDistM = 50;

    Macro2UE_LOS.ESDCorrDistM = 15;
    Macro2UE_NLOS.ESDCorrDistM = 50;
    Macro2UE_O2I.ESDCorrDistM = 50;

    ///elevation(need to check)
    {
        Macro2UE_LOS.EODSpreadAVE = -1;
        Macro2UE_LOS.EODSpreadSTD = -1;
        Macro2UE_LOS.EODoffsetAVE = -1;


        Macro2UE_NLOS.EODSpreadAVE = -1;
        Macro2UE_NLOS.EODSpreadSTD = -1;
        Macro2UE_NLOS.EODoffsetAVE = -1;

        Macro2UE_O2I.EODSpreadAVE = -1;
        Macro2UE_O2I.EODSpreadSTD = -1;
        Macro2UE_O2I.EODSpreadAVE = -1;
    }
    //    Macro2UE_LOS.EOASpreadAVE = 1.08;
    //    Macro2UE_NLOS.EOASpreadAVE = 1.00;
    //    Macro2UE_LOS.EOASpreadSTD = 0.16;
    //    Macro2UE_NLOS.EOASpreadSTD = 0.16;
    Macro2UE_LOS.ClusterESD = 3;
    Macro2UE_NLOS.ClusterESD = 3;
    Macro2UE_O2I.ClusterESD = 3;


    if (P::s().FX.I2DOr3DChannel == P::IS2DCHANNEL) {
        Macro2UE_LOS.R = itpp::mat(5, 5);
        Macro2UE_LOS.R(0, 0) = 0.9659;
        Macro2UE_LOS.R(0, 1) = 0.0000;
        Macro2UE_LOS.R(0, 2) = 0.0000;
        Macro2UE_LOS.R(0, 3) = -0.2588;
        Macro2UE_LOS.R(0, 4) = 0.0000;
        Macro2UE_LOS.R(1, 0) = 0.0000;
        Macro2UE_LOS.R(1, 1) = 1.0000;
        Macro2UE_LOS.R(1, 2) = 0.0000;
        Macro2UE_LOS.R(1, 3) = 0.0000;
        Macro2UE_LOS.R(1, 4) = 0.0000;
        Macro2UE_LOS.R(2, 0) = 0.0000;
        Macro2UE_LOS.R(2, 1) = 0.0000;
        Macro2UE_LOS.R(2, 2) = 1.0000;
        Macro2UE_LOS.R(2, 3) = 0.0000;
        Macro2UE_LOS.R(2, 4) = 0.0000;
        Macro2UE_LOS.R(3, 0) = -0.2588;
        Macro2UE_LOS.R(3, 1) = 0.0000;
        Macro2UE_LOS.R(3, 2) = 0.0000;
        Macro2UE_LOS.R(3, 3) = 0.9659;
        Macro2UE_LOS.R(3, 4) = 0.0000;
        Macro2UE_LOS.R(4, 0) = 0.0000;
        Macro2UE_LOS.R(4, 1) = 0.0000;
        Macro2UE_LOS.R(4, 2) = 0.0000;
        Macro2UE_LOS.R(4, 3) = 0.0000;
        Macro2UE_LOS.R(4, 4) = 1.0000;

        Macro2UE_NLOS.R = itpp::mat(5, 5);
        Macro2UE_NLOS.R(0, 0) = 0.9556;
        Macro2UE_NLOS.R(0, 1) = -0.1735;
        Macro2UE_NLOS.R(0, 2) = 0.0000;
        Macro2UE_NLOS.R(0, 3) = -0.2384;
        Macro2UE_NLOS.R(0, 4) = 0.0000;
        Macro2UE_NLOS.R(1, 0) = -0.1735;
        Macro2UE_NLOS.R(1, 1) = 0.9380;
        Macro2UE_NLOS.R(1, 2) = 0.0000;
        Macro2UE_NLOS.R(1, 3) = 0.3001;
        Macro2UE_NLOS.R(1, 4) = 0.0000;
        Macro2UE_NLOS.R(2, 0) = 0.0000;
        Macro2UE_NLOS.R(2, 1) = 0.0000;
        Macro2UE_NLOS.R(2, 2) = 1.0000;
        Macro2UE_NLOS.R(2, 3) = 0.0000;
        Macro2UE_NLOS.R(2, 4) = 0.0000;
        Macro2UE_NLOS.R(3, 0) = -0.2384;
        Macro2UE_NLOS.R(3, 1) = 0.3001;
        Macro2UE_NLOS.R(3, 2) = 0.0000;
        Macro2UE_NLOS.R(3, 3) = 0.9237;
        Macro2UE_NLOS.R(3, 4) = 0.0000;
        Macro2UE_NLOS.R(4, 0) = 0.0000;
        Macro2UE_NLOS.R(4, 1) = 0.0000;
        Macro2UE_NLOS.R(4, 2) = 0.0000;
        Macro2UE_NLOS.R(4, 3) = 0.0000;
        Macro2UE_NLOS.R(4, 4) = 1.0000;

    } else if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {


        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa

        itpp::mat Macro2UE_LOS_R_square = itpp::mat(7, 7);
        Macro2UE_LOS_R_square = itpp::zeros(7, 7);
        Macro2UE_LOS_R_square(0, 0) = 1;
        Macro2UE_LOS_R_square(1, 1) = 1;
        Macro2UE_LOS_R_square(2, 2) = 1;
        Macro2UE_LOS_R_square(3, 3) = 1;
        Macro2UE_LOS_R_square(4, 4) = 1;
        Macro2UE_LOS_R_square(5, 5) = 1;
        Macro2UE_LOS_R_square(6, 6) = 1;

        Macro2UE_LOS_R_square(0, 1) = 0.0; //ds-asd
        Macro2UE_LOS_R_square(0, 2) = 0.0; //ds-asa
        Macro2UE_LOS_R_square(0, 3) = -0.5; //ds-sf
        Macro2UE_LOS_R_square(0, 4) = 0.0; //ds-k
        Macro2UE_LOS_R_square(0, 5) = -0.05; //ds-zsd
        Macro2UE_LOS_R_square(0, 6) = 0.27; //ds-zsa
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_LOS_R_square(1, 2) = -0.14; //asd-asa
        Macro2UE_LOS_R_square(1, 3) = 0.0; //asd-sf
        Macro2UE_LOS_R_square(1, 4) = 0.0; //asd-k
        Macro2UE_LOS_R_square(1, 5) = 0.73; //asd-zsd
        Macro2UE_LOS_R_square(1, 6) = -0.14; //asd-zsa

        Macro2UE_LOS_R_square(2, 3) = 0.0; //asa-sf
        Macro2UE_LOS_R_square(2, 4) = 0.0; //asa-k
        Macro2UE_LOS_R_square(2, 5) = -0.2; //asa-zsd
        Macro2UE_LOS_R_square(2, 6) = 0.24; //asa-zsa

        Macro2UE_LOS_R_square(3, 4) = 0.0; //sf-k
        Macro2UE_LOS_R_square(3, 5) = 0.01; //sf-zsd
        Macro2UE_LOS_R_square(3, 6) = -0.17; //sf-zsa

        Macro2UE_LOS_R_square(4, 5) = 0.0; //k-zsd
        Macro2UE_LOS_R_square(4, 6) = -0.02; //k-zsa

        Macro2UE_LOS_R_square(5, 6) = -0.07; //zsd-zsa

        //from up triangle matrix to symmetric matrix
        itpp::mat aa = Macro2UE_LOS_R_square + Macro2UE_LOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_LOS_R_square));
        itpp::cmat bb = itpp::sqrtm(aa);
        Macro2UE_LOS.R = itpp::real(bb);

        itpp::mat Macro2UE_NLOS_R_square = itpp::mat(7, 7);
        Macro2UE_NLOS_R_square = itpp::zeros(7, 7);
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_NLOS_R_square(0, 0) = 1;
        Macro2UE_NLOS_R_square(1, 1) = 1;
        Macro2UE_NLOS_R_square(2, 2) = 1;
        Macro2UE_NLOS_R_square(3, 3) = 1;
        Macro2UE_NLOS_R_square(4, 4) = 1;
        Macro2UE_NLOS_R_square(5, 5) = 1;
        Macro2UE_NLOS_R_square(6, 6) = 1;
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_NLOS_R_square(0, 1) = -0.4; //ds-asd
        Macro2UE_NLOS_R_square(0, 2) = 0.0; //ds-asa
        Macro2UE_NLOS_R_square(0, 3) = -0.5; //ds-sf
        Macro2UE_NLOS_R_square(0, 4) = 0.0; //ds-k
        Macro2UE_NLOS_R_square(0, 5) = -0.1; //ds-zsd
        Macro2UE_NLOS_R_square(0, 6) = -0.4; //ds-zsa

        Macro2UE_NLOS_R_square(1, 2) = 0.0; //asd-asa
        Macro2UE_NLOS_R_square(1, 3) = 0.6; //asd-sf
        Macro2UE_NLOS_R_square(1, 4) = 0.0; //asd-k
        Macro2UE_NLOS_R_square(1, 5) = 0.42; //asd-zsd
        Macro2UE_NLOS_R_square(1, 6) = -0.27; //asd-zsa
        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_NLOS_R_square(2, 3) = 0.0; //asa-sf
        Macro2UE_NLOS_R_square(2, 4) = 0.0; //asa-k
        Macro2UE_NLOS_R_square(2, 5) = -0.18; //asa-zsd
        Macro2UE_NLOS_R_square(2, 6) = 0.26; //asa-zsa

        Macro2UE_NLOS_R_square(3, 4) = 0.0; //sf-k
        Macro2UE_NLOS_R_square(3, 5) = -0.04; //sf-zsd
        Macro2UE_NLOS_R_square(3, 6) = -0.25; //sf-zsa

        Macro2UE_NLOS_R_square(4, 5) = 0.0; //k-zsd
        Macro2UE_NLOS_R_square(4, 6) = 0.0; //k-zsa

        Macro2UE_NLOS_R_square(5, 6) = -0.27; //zsd-zsa

        itpp::mat aa_nlos = Macro2UE_NLOS_R_square + Macro2UE_NLOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_NLOS_R_square));
        itpp::cmat bb_nlos = itpp::sqrtm(aa_nlos);
        Macro2UE_NLOS.R = itpp::real(bb_nlos);


        itpp::mat Macro2UE_O2I_R_square = itpp::mat(7, 7);
        Macro2UE_O2I_R_square = itpp::zeros(7, 7);

        Macro2UE_O2I_R_square(0, 0) = 1;
        Macro2UE_O2I_R_square(1, 1) = 1;
        Macro2UE_O2I_R_square(2, 2) = 1;
        Macro2UE_O2I_R_square(3, 3) = 1;
        Macro2UE_O2I_R_square(4, 4) = 1;
        Macro2UE_O2I_R_square(5, 5) = 1;
        Macro2UE_O2I_R_square(6, 6) = 1;

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_O2I_R_square(0, 1) = 0.0; //ds-asd
        Macro2UE_O2I_R_square(0, 2) = 0.0; //ds-asa 
        Macro2UE_O2I_R_square(0, 3) = 0.0; //ds-sf
        Macro2UE_O2I_R_square(0, 4) = 0.0; //ds-k
        Macro2UE_O2I_R_square(0, 5) = 0.0; //ds-zsd 
        Macro2UE_O2I_R_square(0, 6) = 0.0; //ds-zsa 

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_O2I_R_square(1, 2) = -0.7; //asd-asa
        Macro2UE_O2I_R_square(1, 3) = 0.0; //asd-sf 
        Macro2UE_O2I_R_square(1, 4) = 0.0; //asd-k
        Macro2UE_O2I_R_square(1, 5) = 0.66; //asd-zsd 
        Macro2UE_O2I_R_square(1, 6) = 0.47; //asd-zsa 

        Macro2UE_O2I_R_square(2, 3) = 0.0; //asa-sf
        Macro2UE_O2I_R_square(2, 4) = 0.0; //asa-k
        Macro2UE_O2I_R_square(2, 5) = -0.55; //asa-zsd
        Macro2UE_O2I_R_square(2, 6) = -0.22; //asa-zsa 

        Macro2UE_O2I_R_square(3, 4) = 0.0; //sf-k 
        Macro2UE_O2I_R_square(3, 5) = 0.0; //sf-zsd
        Macro2UE_O2I_R_square(3, 6) = 0.0; //sf-zsa 

        //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
        Macro2UE_O2I_R_square(4, 5) = 0.0; //k-zsd 
        Macro2UE_O2I_R_square(4, 6) = 0.0; //k-zsa
        Macro2UE_O2I_R_square(5, 6) = 0.0; //zsd-zsa 

        itpp::mat aa_o2i = Macro2UE_O2I_R_square + Macro2UE_O2I_R_square.H() - itpp::diag(itpp::diag(Macro2UE_O2I_R_square));
        itpp::cmat bb_o2i = itpp::sqrtm(aa_o2i);
        Macro2UE_O2I.R = itpp::real(bb_o2i);


    } else {
        assert(false);
    }
}
//
//void P::Set_itu_rma_channel_parameters_ModelB() {
//    //所有参数都没有更新
//    Macro2UE_LOS.DelaySpreadAVE = -7.49;
//    Macro2UE_LOS.DelaySpreadSTD = 0.55;
//    Macro2UE_NLOS.DelaySpreadAVE = -7.43;
//    Macro2UE_NLOS.DelaySpreadSTD = 0.48;
//    Macro2UE_LOS.AODSpreadAVE = 0.90;
//    Macro2UE_LOS.AODSpreadSTD = 0.38;
//    Macro2UE_NLOS.AODSpreadAVE = 0.95;
//    Macro2UE_NLOS.AODSpreadSTD = 0.45;
//    Macro2UE_LOS.AOASpreadAVE = 1.52;
//    Macro2UE_LOS.AOASpreadSTD = 0.24;
//    Macro2UE_NLOS.AOASpreadAVE = 1.52;
//    Macro2UE_NLOS.AOASpreadSTD = 0.13;
//    Macro2UE_LOS.ShadowFadingSTD = 4;
//    Macro2UE_NLOS.ShadowFadingSTD = 8;
//    Macro2UE_LOS.KFactorDBAVE = 7;
//    Macro2UE_LOS.KFactorDBSTD = 4;
//    Macro2UE_LOS.DelayScaling = 3.8;
//    Macro2UE_NLOS.DelayScaling = 1.7;
//    Macro2UE_LOS.XPR = 12;
//    Macro2UE_NLOS.XPR = 7;
//    Macro2UE_LOS.NumOfCluster = 11;
//    Macro2UE_NLOS.NumOfCluster = 10;
//    Macro2UE_LOS.ClusterASD = 2;
//    Macro2UE_NLOS.ClusterASD = 2;
//    Macro2UE_LOS.ClusterASA = 3;
//    Macro2UE_NLOS.ClusterASA = 3;
//    Macro2UE_LOS.PerClusterShadowingSTDDB = 3;
//    Macro2UE_NLOS.PerClusterShadowingSTDDB = 3;
//    Macro2UE_LOS.DSCorrDistM = 50;
//    Macro2UE_NLOS.DSCorrDistM = 36;
//    Macro2UE_LOS.ASDCorrDistM = 25;
//    Macro2UE_NLOS.ASDCorrDistM = 30;
//    Macro2UE_LOS.ASACorrDistM = 35;
//    Macro2UE_NLOS.ASACorrDistM = 40;
//    Macro2UE_LOS.SFCorrDistM = 37;
//    Macro2UE_NLOS.SFCorrDistM = 120;
//    Macro2UE_LOS.KCorrDistM = 40;
//    ///elevation(need to check)
//    Macro2UE_LOS.EODSpreadAVE = 0.70;
//    Macro2UE_NLOS.EODSpreadAVE = 0.90;
//    Macro2UE_LOS.EODSpreadSTD = 0.2;
//    Macro2UE_NLOS.EODSpreadSTD = 0.2;
//    Macro2UE_LOS.EOASpreadAVE = 1.08;
//    Macro2UE_NLOS.EOASpreadAVE = 1.00;
//    Macro2UE_LOS.EOASpreadSTD = 0.16;
//    Macro2UE_NLOS.EOASpreadSTD = 0.16;
//    Macro2UE_LOS.ClusterESD = 3;
//    Macro2UE_NLOS.ClusterESD = 3;
//    Macro2UE_LOS.ClusterESA = 7;
//    Macro2UE_NLOS.ClusterESA = 7;
//    Macro2UE_LOS.ESDCorrDistM = 15;
//    Macro2UE_NLOS.ESDCorrDistM = 50;
//    Macro2UE_LOS.ESACorrDistM = 15;
//    Macro2UE_NLOS.ESACorrDistM = 50;
//
//    if (P::s().FX.I2DOr3DChannel == P::IS2DCHANNEL) {
//        Macro2UE_LOS.R = itpp::mat(5, 5);
//        Macro2UE_LOS.R(0, 0) = 0.9659;
//        Macro2UE_LOS.R(0, 1) = 0.0000;
//        Macro2UE_LOS.R(0, 2) = 0.0000;
//        Macro2UE_LOS.R(0, 3) = -0.2588;
//        Macro2UE_LOS.R(0, 4) = 0.0000;
//        Macro2UE_LOS.R(1, 0) = 0.0000;
//        Macro2UE_LOS.R(1, 1) = 1.0000;
//        Macro2UE_LOS.R(1, 2) = 0.0000;
//        Macro2UE_LOS.R(1, 3) = 0.0000;
//        Macro2UE_LOS.R(1, 4) = 0.0000;
//        Macro2UE_LOS.R(2, 0) = 0.0000;
//        Macro2UE_LOS.R(2, 1) = 0.0000;
//        Macro2UE_LOS.R(2, 2) = 1.0000;
//        Macro2UE_LOS.R(2, 3) = 0.0000;
//        Macro2UE_LOS.R(2, 4) = 0.0000;
//        Macro2UE_LOS.R(3, 0) = -0.2588;
//        Macro2UE_LOS.R(3, 1) = 0.0000;
//        Macro2UE_LOS.R(3, 2) = 0.0000;
//        Macro2UE_LOS.R(3, 3) = 0.9659;
//        Macro2UE_LOS.R(3, 4) = 0.0000;
//        Macro2UE_LOS.R(4, 0) = 0.0000;
//        Macro2UE_LOS.R(4, 1) = 0.0000;
//        Macro2UE_LOS.R(4, 2) = 0.0000;
//        Macro2UE_LOS.R(4, 3) = 0.0000;
//        Macro2UE_LOS.R(4, 4) = 1.0000;
//
//        Macro2UE_NLOS.R = itpp::mat(5, 5);
//        Macro2UE_NLOS.R(0, 0) = 0.9556;
//        Macro2UE_NLOS.R(0, 1) = -0.1735;
//        Macro2UE_NLOS.R(0, 2) = 0.0000;
//        Macro2UE_NLOS.R(0, 3) = -0.2384;
//        Macro2UE_NLOS.R(0, 4) = 0.0000;
//        Macro2UE_NLOS.R(1, 0) = -0.1735;
//        Macro2UE_NLOS.R(1, 1) = 0.9380;
//        Macro2UE_NLOS.R(1, 2) = 0.0000;
//        Macro2UE_NLOS.R(1, 3) = 0.3001;
//        Macro2UE_NLOS.R(1, 4) = 0.0000;
//        Macro2UE_NLOS.R(2, 0) = 0.0000;
//        Macro2UE_NLOS.R(2, 1) = 0.0000;
//        Macro2UE_NLOS.R(2, 2) = 1.0000;
//        Macro2UE_NLOS.R(2, 3) = 0.0000;
//        Macro2UE_NLOS.R(2, 4) = 0.0000;
//        Macro2UE_NLOS.R(3, 0) = -0.2384;
//        Macro2UE_NLOS.R(3, 1) = 0.3001;
//        Macro2UE_NLOS.R(3, 2) = 0.0000;
//        Macro2UE_NLOS.R(3, 3) = 0.9237;
//        Macro2UE_NLOS.R(3, 4) = 0.0000;
//        Macro2UE_NLOS.R(4, 0) = 0.0000;
//        Macro2UE_NLOS.R(4, 1) = 0.0000;
//        Macro2UE_NLOS.R(4, 2) = 0.0000;
//        Macro2UE_NLOS.R(4, 3) = 0.0000;
//        Macro2UE_NLOS.R(4, 4) = 1.0000;
//
//    } else if (P::s().FX.I2DOr3DChannel == P::IS3DCHANNEL) {
//        Macro2UE_LOS.R = itpp::mat(7, 7);
//        Macro2UE_LOS.R(0, 0) = 0.9659;
//        Macro2UE_LOS.R(0, 1) = 0.0000;
//        Macro2UE_LOS.R(0, 2) = 0.0000;
//        Macro2UE_LOS.R(0, 3) = -0.2588;
//        Macro2UE_LOS.R(0, 4) = 0.0000;
//        Macro2UE_LOS.R(0, 5) = 0;
//        Macro2UE_LOS.R(0, 6) = 0;
//        Macro2UE_LOS.R(1, 0) = 0.0000;
//        Macro2UE_LOS.R(1, 1) = 1.0000;
//        Macro2UE_LOS.R(1, 2) = 0.0000;
//        Macro2UE_LOS.R(1, 3) = 0.0000;
//        Macro2UE_LOS.R(1, 4) = 0.0000;
//        Macro2UE_LOS.R(1, 5) = 0;
//        Macro2UE_LOS.R(1, 6) = 0;
//        Macro2UE_LOS.R(2, 0) = 0.0000;
//        Macro2UE_LOS.R(2, 1) = 0.0000;
//        Macro2UE_LOS.R(2, 2) = 1.0000;
//        Macro2UE_LOS.R(2, 3) = 0.0000;
//        Macro2UE_LOS.R(2, 4) = 0.0000;
//        Macro2UE_LOS.R(2, 5) = 0;
//        Macro2UE_LOS.R(2, 6) = 0;
//        Macro2UE_LOS.R(3, 0) = -0.2588;
//        Macro2UE_LOS.R(3, 1) = 0.0000;
//        Macro2UE_LOS.R(3, 2) = 0.0000;
//        Macro2UE_LOS.R(3, 3) = 0.9659;
//        Macro2UE_LOS.R(3, 4) = 0.0000;
//        Macro2UE_LOS.R(3, 5) = 0;
//        Macro2UE_LOS.R(3, 6) = 0;
//        Macro2UE_LOS.R(4, 0) = 0.0000;
//        Macro2UE_LOS.R(4, 1) = 0.0000;
//        Macro2UE_LOS.R(4, 2) = 0.0000;
//        Macro2UE_LOS.R(4, 3) = 0.0000;
//        Macro2UE_LOS.R(4, 4) = 1.0000;
//        Macro2UE_LOS.R(4, 5) = 0;
//        Macro2UE_LOS.R(4, 6) = 0;
//        Macro2UE_LOS.R(5, 0) = 0;
//        Macro2UE_LOS.R(5, 1) = 0;
//        Macro2UE_LOS.R(5, 2) = 0;
//        Macro2UE_LOS.R(5, 3) = 0;
//        Macro2UE_LOS.R(5, 4) = 0;
//        Macro2UE_LOS.R(5, 5) = 1;
//        Macro2UE_LOS.R(5, 6) = 0;
//        Macro2UE_LOS.R(6, 0) = 0;
//        Macro2UE_LOS.R(6, 1) = 0;
//        Macro2UE_LOS.R(6, 2) = 0;
//        Macro2UE_LOS.R(6, 3) = 0;
//        Macro2UE_LOS.R(6, 4) = 0;
//        Macro2UE_LOS.R(6, 5) = 0;
//        Macro2UE_LOS.R(6, 6) = 1;
//
//        Macro2UE_NLOS.R = itpp::mat(7, 7);
//        Macro2UE_NLOS.R(0, 0) = 0.9556;
//        Macro2UE_NLOS.R(0, 1) = -0.1735;
//        Macro2UE_NLOS.R(0, 2) = 0.0000;
//        Macro2UE_NLOS.R(0, 3) = -0.2384;
//        Macro2UE_NLOS.R(0, 4) = 0.0000;
//        Macro2UE_NLOS.R(0, 5) = 0;
//        Macro2UE_NLOS.R(0, 6) = 0;
//        Macro2UE_NLOS.R(1, 0) = -0.1735;
//        Macro2UE_NLOS.R(1, 1) = 0.9380;
//        Macro2UE_NLOS.R(1, 2) = 0.0000;
//        Macro2UE_NLOS.R(1, 3) = 0.3001;
//        Macro2UE_NLOS.R(1, 4) = 0.0000;
//        Macro2UE_NLOS.R(1, 5) = 0;
//        Macro2UE_NLOS.R(1, 6) = 0;
//        Macro2UE_NLOS.R(2, 0) = 0.0000;
//        Macro2UE_NLOS.R(2, 1) = 0.0000;
//        Macro2UE_NLOS.R(2, 2) = 1.0000;
//        Macro2UE_NLOS.R(2, 3) = 0.0000;
//        Macro2UE_NLOS.R(2, 4) = 0.0000;
//        Macro2UE_NLOS.R(2, 5) = 0;
//        Macro2UE_NLOS.R(2, 6) = 0;
//        Macro2UE_NLOS.R(3, 0) = -0.2384;
//        Macro2UE_NLOS.R(3, 1) = 0.3001;
//        Macro2UE_NLOS.R(3, 2) = 0.0000;
//        Macro2UE_NLOS.R(3, 3) = 0.9237;
//        Macro2UE_NLOS.R(3, 4) = 0.0000;
//        Macro2UE_NLOS.R(3, 5) = 0;
//        Macro2UE_NLOS.R(3, 6) = 0;
//        Macro2UE_NLOS.R(4, 0) = 0.0000;
//        Macro2UE_NLOS.R(4, 1) = 0.0000;
//        Macro2UE_NLOS.R(4, 2) = 0.0000;
//        Macro2UE_NLOS.R(4, 3) = 0.0000;
//        Macro2UE_NLOS.R(4, 4) = 1.0000;
//        Macro2UE_NLOS.R(4, 5) = 0;
//        Macro2UE_NLOS.R(4, 6) = 0;
//        Macro2UE_NLOS.R(5, 0) = 0;
//        Macro2UE_NLOS.R(5, 1) = 0;
//        Macro2UE_NLOS.R(5, 2) = 0;
//        Macro2UE_NLOS.R(5, 3) = 0;
//        Macro2UE_NLOS.R(5, 4) = 0;
//        Macro2UE_NLOS.R(5, 5) = 1;
//        Macro2UE_NLOS.R(5, 6) = 0;
//        Macro2UE_NLOS.R(6, 0) = 0;
//        Macro2UE_NLOS.R(6, 1) = 0;
//        Macro2UE_NLOS.R(6, 2) = 0;
//        Macro2UE_NLOS.R(6, 3) = 0;
//        Macro2UE_NLOS.R(6, 4) = 0;
//        Macro2UE_NLOS.R(6, 5) = 0;
//        Macro2UE_NLOS.R(6, 6) = 1;
//
//    } else {
//        assert(false);
//    }
//
//
//
//}

void P::Set_itu_indoor_factory_channel_parameters() {
    double dFrequencyGHz = FX.DRadioFrequencyMHz_Macro * 1e-3;
    //V和S目前不知到怎么处理 38.901 7.8-7
    double V = 120 * 60 * 10; //size=120*60, height=10
    double S = (120 * 60 + 120 * 10 + 60 * 10)*2;
    //DS
    Macro2UE_LOS.DelaySpreadAVE = log10(26 * (V / S) + 14) - 9.35;
    Macro2UE_LOS.DelaySpreadSTD = 0.15;
    Macro2UE_NLOS.DelaySpreadAVE = log10(30 * (V / S) + 32) - 9.44;
    Macro2UE_NLOS.DelaySpreadSTD = 0.19;
    //AOD
    Macro2UE_LOS.AODSpreadAVE = 1.56;
    Macro2UE_LOS.AODSpreadSTD = 0.25;
    Macro2UE_NLOS.AODSpreadAVE = 1.57;
    Macro2UE_NLOS.AODSpreadSTD = 0.2;
    //AOA
    Macro2UE_LOS.AOASpreadAVE = -0.18 * log10(1 + dFrequencyGHz) + 1.78;
    Macro2UE_LOS.AOASpreadSTD = 0.12 * log10(1 + dFrequencyGHz) + 0.2;
    Macro2UE_NLOS.AOASpreadAVE = 1.72;
    Macro2UE_NLOS.AOASpreadSTD = 0.3;
    //没有ZOA？
    //ShadowFading_inF-DH
    Macro2UE_LOS.ShadowFadingSTD = 4;
    Macro2UE_NLOS.ShadowFadingSTD = 4;
    //K-factor，只有LOS
    Macro2UE_LOS.KFactorDBAVE = 7;
    Macro2UE_LOS.KFactorDBSTD = 8;
    //DelayScaling，r_tao
    Macro2UE_LOS.DelayScaling = 2.7;
    Macro2UE_NLOS.DelayScaling = 3;
    //XPR
    Macro2UE_LOS.XPR_u = 12;
    Macro2UE_NLOS.XPR_u = 11;
    Macro2UE_LOS.XPR_sigma = 6;
    Macro2UE_NLOS.XPR_sigma = 6;
    //NumOfCluster
    Macro2UE_LOS.NumOfCluster = 25;
    Macro2UE_NLOS.NumOfCluster = 25;
    //ClusterASD
    Macro2UE_LOS.ClusterASD = 5;
    Macro2UE_NLOS.ClusterASD = 5;
    //ClusterASA
    Macro2UE_LOS.ClusterASA = 8;
    Macro2UE_NLOS.ClusterASA = 8;
    //PerClusterShadowing
    Macro2UE_LOS.PerClusterShadowingSTDDB = 4;
    Macro2UE_NLOS.PerClusterShadowingSTDDB = 3;
    //DSCorrDistM 
    Macro2UE_LOS.DSCorrDistM = 10;
    Macro2UE_NLOS.DSCorrDistM = 10;
    //ASDCorrDistM
    Macro2UE_LOS.ASDCorrDistM = 10;
    Macro2UE_NLOS.ASDCorrDistM = 10;
    //ASACorrDistM
    Macro2UE_LOS.ASACorrDistM = 10;
    Macro2UE_NLOS.ASACorrDistM = 10;
    //SFCorrDistM
    Macro2UE_LOS.SFCorrDistM = 10;
    Macro2UE_NLOS.SFCorrDistM = 10;
    //KCorrDistM
    Macro2UE_LOS.KCorrDistM = 10;
    //没有ZSA和ZSD

    //下面这些参数(关于elevation的参数)在协议中找不到，
    //还有那个矩阵R也不知道从哪里找到ZOD就是EOD
    ///elevation  
    Macro2UE_LOS.EODSpreadAVE = 1.35; //0.88;
    Macro2UE_NLOS.EODSpreadAVE = 1.2; //1.06;

    Macro2UE_LOS.EODSpreadSTD = 0.35; //0.31;
    Macro2UE_NLOS.EODSpreadSTD = 0.55; //0.21;

    Macro2UE_LOS.EOASpreadAVE = -0.2 * log10(1 + dFrequencyGHz) + 1.5; //0.94;
    Macro2UE_NLOS.EOASpreadAVE = -0.13 * log10(1 + dFrequencyGHz) + 1.45; //1.10;

    Macro2UE_LOS.EOASpreadSTD = 0.35; //0.26;
    Macro2UE_NLOS.EOASpreadSTD = 0.45; //0.17;
    //协议里没有给出ZSD，没有用到
    Macro2UE_LOS.ClusterESD = 9;
    Macro2UE_NLOS.ClusterESD = 9;

    Macro2UE_LOS.ClusterESA = 9; //3;
    Macro2UE_NLOS.ClusterESA = 9; //3;

    Macro2UE_LOS.ESDCorrDistM = 10; //6;
    Macro2UE_NLOS.ESDCorrDistM = 10; //5;

    Macro2UE_LOS.ESACorrDistM = 10; //2;
    Macro2UE_NLOS.ESACorrDistM = 10; //3;

    //直接把rma_modeA中的R复制过来进行修改
    //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa //N/A的按0处理
    itpp::mat Macro2UE_LOS_R_square = itpp::mat(7, 7);
    Macro2UE_LOS_R_square = itpp::zeros(7, 7);
    Macro2UE_LOS_R_square(0, 0) = 1;
    Macro2UE_LOS_R_square(1, 1) = 1;
    Macro2UE_LOS_R_square(2, 2) = 1;
    Macro2UE_LOS_R_square(3, 3) = 1;
    Macro2UE_LOS_R_square(4, 4) = 1;
    Macro2UE_LOS_R_square(5, 5) = 1;
    Macro2UE_LOS_R_square(6, 6) = 1;

    Macro2UE_LOS_R_square(0, 1) = 0.0; //ds-asd
    Macro2UE_LOS_R_square(0, 2) = 0.0; //ds-asa
    Macro2UE_LOS_R_square(0, 3) = 0.0; //ds-sf
    Macro2UE_LOS_R_square(0, 4) = -0.7; //ds-k
    Macro2UE_LOS_R_square(0, 5) = 0.0; //ds-zsd
    Macro2UE_LOS_R_square(0, 6) = 0.0; //ds-zsa
    //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
    Macro2UE_LOS_R_square(1, 2) = 0.0; //asd-asa
    Macro2UE_LOS_R_square(1, 3) = 0.0; //asd-sf
    Macro2UE_LOS_R_square(1, 4) = -0.5; //asd-k
    Macro2UE_LOS_R_square(1, 5) = 0.0; //asd-zsd
    Macro2UE_LOS_R_square(1, 6) = 0.0; //asd-zsa

    Macro2UE_LOS_R_square(2, 3) = 0.0; //asa-sf
    Macro2UE_LOS_R_square(2, 4) = 0.0; //asa-k
    Macro2UE_LOS_R_square(2, 5) = 0.0; //asa-zsd
    Macro2UE_LOS_R_square(2, 6) = 0.0; //asa-zsa

    Macro2UE_LOS_R_square(3, 4) = 0.0; //sf-k
    Macro2UE_LOS_R_square(3, 5) = 0.0; //sf-zsd
    Macro2UE_LOS_R_square(3, 6) = 0.0; //sf-zsa

    Macro2UE_LOS_R_square(4, 5) = 0.0; //k-zsd
    Macro2UE_LOS_R_square(4, 6) = 0.0; //k-zsa

    Macro2UE_LOS_R_square(5, 6) = 0.0; //zsd-zsa

    //from up triangle matrix to symmetric matrix
    itpp::mat aa = Macro2UE_LOS_R_square + Macro2UE_LOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_LOS_R_square));
    itpp::cmat bb = itpp::sqrtm(aa);
    Macro2UE_LOS.R = itpp::real(bb);

    itpp::mat Macro2UE_NLOS_R_square = itpp::mat(7, 7);
    Macro2UE_NLOS_R_square = itpp::zeros(7, 7);
    //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
    Macro2UE_NLOS_R_square(0, 0) = 1;
    Macro2UE_NLOS_R_square(1, 1) = 1;
    Macro2UE_NLOS_R_square(2, 2) = 1;
    Macro2UE_NLOS_R_square(3, 3) = 1;
    Macro2UE_NLOS_R_square(4, 4) = 1;
    Macro2UE_NLOS_R_square(5, 5) = 1;
    Macro2UE_NLOS_R_square(6, 6) = 1;
    //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
    Macro2UE_NLOS_R_square(0, 1) = 0.0; //ds-asd
    Macro2UE_NLOS_R_square(0, 2) = 0.0; //ds-asa
    Macro2UE_NLOS_R_square(0, 3) = 0.0; //ds-sf
    Macro2UE_NLOS_R_square(0, 4) = 0.0; //ds-k
    Macro2UE_NLOS_R_square(0, 5) = 0.0; //ds-zsd
    Macro2UE_NLOS_R_square(0, 6) = 0.0; //ds-zsa

    Macro2UE_NLOS_R_square(1, 2) = 0.0; //asd-asa
    Macro2UE_NLOS_R_square(1, 3) = 0.0; //asd-sf
    Macro2UE_NLOS_R_square(1, 4) = 0.0; //asd-k
    Macro2UE_NLOS_R_square(1, 5) = 0.0; //asd-zsd
    Macro2UE_NLOS_R_square(1, 6) = 0.0; //asd-zsa
    //0,ds //1,asd  //2,asa  //3,sf  //4,k //5,zsd //6,zsa
    Macro2UE_NLOS_R_square(2, 3) = 0.0; //asa-sf
    Macro2UE_NLOS_R_square(2, 4) = 0.0; //asa-k
    Macro2UE_NLOS_R_square(2, 5) = 0.0; //asa-zsd
    Macro2UE_NLOS_R_square(2, 6) = 0.0; //asa-zsa

    Macro2UE_NLOS_R_square(3, 4) = 0.0; //sf-k
    Macro2UE_NLOS_R_square(3, 5) = 0.0; //sf-zsd
    Macro2UE_NLOS_R_square(3, 6) = 0.0; //sf-zsa

    Macro2UE_NLOS_R_square(4, 5) = 0.0; //k-zsd
    Macro2UE_NLOS_R_square(4, 6) = 0.0; //k-zsa

    Macro2UE_NLOS_R_square(5, 6) = 0.0; //zsd-zsa

    itpp::mat aa_nlos = Macro2UE_NLOS_R_square + Macro2UE_NLOS_R_square.H() - itpp::diag(itpp::diag(Macro2UE_NLOS_R_square));
    itpp::cmat bb_nlos = itpp::sqrtm(aa_nlos);
    Macro2UE_NLOS.R = itpp::real(bb_nlos);
}