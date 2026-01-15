///@file MSData.cpp
///@brief 用来记录MS统计数据的函数
///@author dushaofeng

#include "../BaseStation/BTSID.h"
#include "MSData_DL.h"

MSData_DL::MSData_DL() {
    m_x = m_y = m_z = 0;
    m_ActiveBTS = BTSID(-1, -1);
    m_dLinkLossDB = 0;
    m_dAveAntGainDB = 0;
    m_dESA = 0;
    m_dESD = 0;
    DL.m_dGeometryDB = 0;
    DL.m_dAveRateKbps = 0;
    DL.m_dAveSE_20MHz_withoutOverhead_bps_Per_Hz = 0;
    DL.m_dAveRateKbps_Converted = 0;
    DL.m_dAveSE_StatBandWidth_withOverhead_bps_Per_Hz = 0;
    DL.m_dAveSINR = 0;
    DL.m_dCRSSINR = 0;
    DL.m_iSINRHit = 0;
    DL.m_iCRSSINRHit = 0;
    DL.m_dAvePostSINR = 0;
    DL.m_dLogAvePostSINR=0;
    DL.m_iPostSINRHit = 0;
    for (int iMCS = 0; iMCS < 29; ++iMCS) {//MCS最大取28
        DL.m_iMCSHit[iMCS] = 0;
    }
    for (int i = 0; i < 5; ++i) {
        DL.m_dBLERTx[i] = -1;
        DL.m_iCorrectBlockHit[i] = 0;
        DL.m_iCorruptBlockHit[i] = 0;
    }
    DL.m_iRankHitNum[0] = 0;
    DL.m_iRankHitNum[1] = 0;
    DL.m_iRankHitNum[2] = 0;
    DL.m_iRankHitNum[3] = 0;
    DL.m_iTM3HitNum = 0;
    DL.m_iSchedulingNum = 0;
    Packetscounts=0;
    SuccessPackets=0;
    LostPackets=0;
}
