///@file MSData.cpp
///@brief 用来记录MS统计数据的函数
///@author dushaofeng
#include "../BaseStation/BTSID.h"
#include "MSData_UL.h"

///@brief 记录MS统计数据的类
///
///初始化MS统计数据用到的一些参数的值

MSData_UL::MSData_UL() {
    m_ActiveBTS = BTSID(-1, -1);
    //    m_dAveRateKbpsDL = 0;
    //    UL.m_dAveRateKbps = 0;
    //    DL.m_dAveRateKbps = 0;
    UL.m_dAveRateKbpsUL = 0;
//    DL.m_dPERDL = -1;

    m_dLinkLossDB = 0;
//    m_dPDiscardDL = 0;
//    m_dPDiscardUL = 0;
//    m_dPDelayBelow50msDL = 0;
//    m_dPDelayBelow50msUL = 0;
//    m_dPDelayUp50msDL = 0;
    m_dUE2BSTiltDEG = 0;
//    m_dPDelayUp50msUL = 0;
//    m_SumDelayOfSucceedPacket = 0;
//    m_SucceedPacketNumber = 0;
    m_dGeometryDB = 0;
    m_d2D_Distance = 0;
    m_AODLOSDEG = 0; //szx
    m_dPlusOffsetDEG = 0; ///szx
    m_dAveTxAntennaPatternDB = 0; ///szx
    m_dRxHeight = 0.1; //szx
    m_dAVE_SINR = 0;
//    m_dAVE_SINR_test = 0;
    m_dul_geoemtry = 0; //统计上行空口SINR
	m_dAVE_preSINR = 0;
	m_dAVE_preSINR_Index = 0;
	m_dAVE_SINR = 0;
    m_dAVE_SINR_Index = 0;
    m_dRBCount = 0;
    m_dRBCount2 = 0;
    m_iSchCount = 0;
    m_iSchCount2 = 0;
    ///当前MS支持的RB数
    m_iRBSupported = 0;
    m_iOccupiedRBS = 0;
    m_iOccupiedRBS2 = 0;
//    m_iCompSize = 0;
    m_iMUcount = 0;
    m_iSUcount = 0;
    m_dTxPowerMw = 0;
    m_dTxPowerMwIndex = 0;
//    m_iTwoStreamHitTimes = 0;
//    m_iOneStreamHitTimes = 0;

    m_dAVE_preCQISINR = 0; //test
    m_dAVE_preCQISINR_Index = 0; //test

    //zhengyi
    m_dPathlossDB = 0;
    m_dAvePathlossDB = 0;
    m_iIsO2I = -1;
    m_iIsLOS = -1;
    m_dShadowfading = 0;

    for (int iRankindex = 0; iRankindex < 2; ++iRankindex) {//最多2流
        m_iRank[iRankindex] = 0;
    }

    for (int i = 0; i < 2; ++i) {//2流，每个记录29个MCS等级
        for (int j = 0; j < 29; ++j) {
//            DL.m_iFormat2TrueFalseFrameHitDL[i][j] = 0;
            UL.m_iFormat2TrueFalseFrameHitUL[i][j] = 0;
//            m_iFormat2TrueFalseFrameHitDL[i][j] = 0;
//            m_iFormat2TrueFalseFrameHitUL[i][j] = 0;
        }
    }
    for (int i = 0; i < 5; ++i) {
        UL.m_iCorrectBlockHit[i] = 0;
        UL.m_iCorruptBlockHit[i] = 0;
    }
    for(int i=0;i<3;i++){
        phr_trigger[i] = 0;
    }
    for (int i = 0; i <= 4; ++i) {
        UL.m_dBLERTx[i] = 0;
        UL.m_iCorruptBlockHit[i] = 0;
    }
//    for (int i = 0; i < 50; ++i) {
//        RBnBeenUsed[i] = 0;
//    }
//    for (int j = 0; j < 3; ++j) {
//        m_iPacketResultDL[j] = 0;
//        m_iPacketResultUL[j] = 0;
//    }
}
