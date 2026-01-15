///@file Statistician.cpp
///@brief 用来统计数据的函数
///@author dushaofeng

#include "../Parameters/Parameters.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../BaseStation/BTSID.h"
#include "../MobileStation/MSID.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "../MobileStation/MS.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../Scheduler/SchedulerDL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/BTS.h"
#include "../BaseStation/BS.h"
#include "../NetworkDrive/BSManager.h"
#include "MSData_DL.h"
#include "MSData_UL.h"
#include "BSData.h"
#include "Directory.h"
#include "Statistician.h"
#include "../NetworkDrive/Clock.h"
#include<numeric>
// 构造函数的实现。打开下行或者上行需要统计数据的各个文件

Statistician::Statistician()
	:m_MSData_DL(Parameters::Instance().BASIC.ITotalMSNum),
	m_MSData_UL(),
	m_BTSData(Parameters::Instance().BASIC.INumBSs)
{
	m_dOH_DMRS_Rate = 0.0;
	m_dOH_DMRS_Rate_Old = 0.0;

	boost::filesystem::path filename;

	//初始化下行输出流
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
		//获取该文件的真实路径
		filename = Directory::Instance().GetPath("MSResultsDL.txt"); //.directory_string();
		//打开记录系统下行移动台统计数据的文件
		DL.fMS.open(filename);
		//获取该文件的真实路径
        //hyl
//		filename = Directory::Instance().GetPath("MSResultsDL_eMBB.txt"); //.directory_string();
//		//打开记录系统下行移动台统计数据的文件
//		DL.fMSeMBB.open(filename);
//		//获取该文件的真实路径
//		filename = Directory::Instance().GetPath("MSResultsDL_RedCap.txt"); //.directory_string();
//		//打开记录系统下行移动台统计数据的文件
//		DL.fMSRedCap.open(filename);
		filename = Directory::Instance().GetPath("BSResultsDL.txt"); //.directory_string();
		//打开记录系统下行基站统计数据的文件
		DL.fBS.open(filename);
		filename = Directory::Instance().GetPath("MCS_Statistics.txt"); //.directory_string();
		//打开记录MCS数据文件
		DL.fMCS.open(filename);
//		filename = Directory::Instance().GetPath("MCS_Statistics_eMBB.txt"); //.directory_string();
//		// RedCap :打开记录MCS数据文件
//		DL.fMCS_eMBB.open(filename);
//		filename = Directory::Instance().GetPath("MCS_Statistics_RedCap.txt"); //.directory_string();
//		// RedCap :打开记录MCS数据文件
//		DL.fMCS_RedCap.open(filename);
//
//		filename = Directory::Instance().GetPath("OverHead_DMRS.txt");
//		DL.fOH_DMRS.open(filename);
//
//		if (Parameters::Instance().BASIC.IScenarioModel == Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER) {
//			filename = Directory::Instance().GetPath("MacroMSResultsDL.txt"); //.directory_string();
//			//打开记录系统下行移动台统计数据的文件
//			DL.fMacroMS.open(filename);
//			filename = Directory::Instance().GetPath("PicoMSResultsDL.txt"); //.directory_string();
//			//打开记录系统下行移动台统计数据的文件
//			DL.fPicoMS.open(filename);
//			filename = Directory::Instance().GetPath("MacroMCS_Statistics.txt"); //.directory_string();
//			//打开记录系统下行移动台统计数据的文件
//			DL.fMacroMCS.open(filename);
//			filename = Directory::Instance().GetPath("PicoMCS_Statistics.txt"); //.directory_string();
//			//打开记录系统下行移动台统计数据的文件
//			DL.fPicoMCS.open(filename);
//		}
	}

	//初始化上行输出流
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::DL) {
		filename = Directory::Instance().GetPath("MSResultsUL.txt"); //.directory_string();
		//打开记录系统上行移动台统计数据的文件
		UL.fMS.open(filename);
		filename = Directory::Instance().GetPath("BSResultsUL.txt"); //.directory_string();
		//打开记录系统上行基站统计数据的文件
		UL.fBS.open(filename);
//		filename = Directory::Instance().GetPath("BSIoTResultsUL.txt"); //.directory_string();
//		//打开记录系统上行基站统计数据的文件
//		UL.fIoT.open(filename);
//		filename = Directory::Instance().GetPath("MSResultsULeMBB.txt"); //.directory_string();
//		//打开记录系统上行移动台统计数据的文件
//		UL.fMSeMBB.open(filename);
//		filename = Directory::Instance().GetPath("MSResultsULRedCap.txt"); //.directory_string();
//		//打开记录系统上行移动台统计数据的文件
//		UL.fMSRedCap.open(filename);
	}
}
// 初始化流程
void Statistician::Initialize() {
	//清除已有的数据
	Clear();
	//初始化基站的统计数据，上下行共用
	for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
		m_BTSData[btsid.GetTxID()] = BSData();
	}

	//初始化下行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
		//初始化移动台的统计数据
		for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
			m_MSData_DL[msid.ToInt()] = MSData_DL();
		}
	}

	//初始化上行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::DL) {
		//初始化移动台的统计数据
		for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
			m_MSData_UL[msid.ToInt()] = MSData_UL();
		}
	}

}

///清除数据的函数

void Statistician::Clear() {
	//clear下行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
		///清除移动台的统计数据
		m_MSData_DL.clear();
		//        ///清除基站的统计数据
		//        m_BTSData.clear();
		m_vdOH_DMRS_Rate.clear();
		m_vdOH_DMRS_Rate_Old.clear();
	}

	//clear上行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::DL) {
		///清除移动台的统计数据
		m_MSData_UL.clear();
	}

	///清除基站的统计数据，上下行共用
	m_BTSData.clear();
	m_vdOH_DMRS_Rate.clear();
	m_vdOH_DMRS_Rate_Old.clear();
}

///实现重置操作的函数

void Statistician::Reset() {
	//    for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
	//        m_MSData_DL[msid].m_dAveRateKbps = 0;
	//        m_MSData_DL[msid].m_dAveRateKbps_Converted = 0;
	//        m_MSData_DL[msid].m_dAveSINR = 0;
	//        m_MSData_DL[msid].m_dCRSSINR = 0;
	//        m_MSData_DL[msid].m_iSINRHit = 0;
	//        m_MSData_DL[msid].m_iCRSSINRHit = 0;
	//        m_MSData_DL[msid].m_dAvePostSINR = 0;
	//        m_MSData_DL[msid].m_dLogAvePostSINR = 0;
	//        mm_MSData_DL[msid].m_iPostSINRHit = 0;
	//        for (int iMCS = 0; iMCS < 29; ++iMCS) {
	//            m_MSData_DL[msid].m_iMCSHit[iMCS] = 0;
	//        }

	//reset下行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
		for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
			m_MSData_DL[msid.ToInt()].DL.m_dAveRateKbps = 0;
			m_MSData_DL[msid.ToInt()].DL.m_dAveRateKbps_Converted = 0;
			m_MSData_DL[msid.ToInt()].DL.m_dAveSINR = 0;
			m_MSData_DL[msid.ToInt()].DL.m_dCRSSINR = 0;
			m_MSData_DL[msid.ToInt()].DL.m_iSINRHit = 0;
			m_MSData_DL[msid.ToInt()].DL.m_iCRSSINRHit = 0;
			m_MSData_DL[msid.ToInt()].DL.m_dAvePostSINR = 0;
			m_MSData_DL[msid.ToInt()].DL.m_dLogAvePostSINR = 0;
			m_MSData_DL[msid.ToInt()].DL.m_iPostSINRHit = 0;
			for (int iMCS = 0; iMCS < 29; ++iMCS) {//MCS等级最大29
				m_MSData_DL[msid.ToInt()].DL.m_iMCSHit[iMCS] = 0;
			}

			m_MSData_DL[msid.ToInt()].DL.m_iRankHitNum[0] = 0;
			m_MSData_DL[msid.ToInt()].DL.m_iRankHitNum[2] = 0;
			m_MSData_DL[msid.ToInt()].DL.m_iRankHitNum[3] = 0;
			m_MSData_DL[msid.ToInt()].DL.m_iTM3HitNum = 0;

			for (int i = 0; i < 5; ++i) {
				m_MSData_DL[msid.ToInt()].DL.m_dBLERTx[i] = -1;
				m_MSData_DL[msid.ToInt()].DL.m_iCorrectBlockHit[i] = 0;
				m_MSData_DL[msid.ToInt()].DL.m_iCorruptBlockHit[i] = 0;
			}
		}

		for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
			m_BTSData[btsid.GetTxID()].DL.m_dThroughputKbps = 0;
			m_BTSData[btsid.GetTxID()].DL.m_dThroughputKbps_Converted = 0;

            m_BTSData[btsid.GetTxID()].DL.m_iNewTxNum = 0;
            m_BTSData[btsid.GetTxID()].DL.m_iNewTxErrorNum = 0;

            m_BTSData[btsid.GetTxID()].DL.m_iScheMSNum = 0;
            m_BTSData[btsid.GetTxID()].DL.m_iScheTime = 0;
		}
	}

	//reset上行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::DL) {
		for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
			m_MSData_UL[msid.ToInt()].UL.m_dAveRateKbpsUL = 0;
			m_MSData_UL[msid.ToInt()].m_dAVE_preSINR = 0;
			m_MSData_UL[msid.ToInt()].m_dAVE_preSINR_Index = 0;
			m_MSData_UL[msid.ToInt()].m_dAVE_SINR = 0;
			//            m_MSData_UL[msid].m_dAVE_SINR_test = 0; //zhengyi
			m_MSData_UL[msid.ToInt()].m_dAVE_SINR_Index = 0;
			//            m_MSData_UL[msid].m_iTwoStreamHitTimes = 0;
			//            m_MSData_UL[msid].m_iOneStreamHitTimes = 0;
			m_MSData_UL[msid.ToInt()].m_dTxPowerMw = 0;
			m_MSData_UL[msid.ToInt()].m_dTxPowerMwIndex = 0;
			m_MSData_UL[msid.ToInt()].m_dRBCount = 0;
			m_MSData_UL[msid.ToInt()].m_dRBCount2 = 0;
			m_MSData_UL[msid.ToInt()].m_iSchCount = 0;
			m_MSData_UL[msid.ToInt()].m_iSchCount2 = 0;
			m_MSData_UL[msid.ToInt()].m_iRBSupported = 0;
			m_MSData_UL[msid.ToInt()].m_iOccupiedRBS = 0;
			m_MSData_UL[msid.ToInt()].m_iOccupiedRBS2 = 0;
			//m_MSData[msid].m_d2D_Distance=0;//szx
			//m_MSData[msid].m_iIsLOS = -1;
			//m_MSData[msid].m_dShadowfading = 0;
			// m_MSData[msid].m_AODLOSDEG=0;//szx
			//m_MSData[msid].m_dUE2BSTiltDEG=0;//szx
			//m_MSData[msid].m_dAveTxAntennaPatternDB=0;//SZX
			//m_MSData[msid].m_dPlusOffsetDEG=0;//SZX
			for (int iRankindex = 0; iRankindex < 2; ++iRankindex) {//最大2流
				m_MSData_UL[msid.ToInt()].m_iRank[iRankindex] = 0;
			}
			for (int ii = 0; ii < 5; ++ii) {//重传次数0-4
				m_MSData_UL[msid.ToInt()].m_iReTransTimePerMS[ii] = 0;
			}
			for (int i = 0; i < 3; i++) {
				m_MSData_UL[msid.ToInt()].phr_trigger[i] = 0;
			}
			for (int k = 0; k < 2; ++k) {//MCS最大29
				for (int m = 0; m < 29; ++m) {
					// 将下行错帧和对帧的点击数置0
					//                    m_MSData_UL[msid].DL.m_iFormat2TrueFalseFrameHitDL[k][m] = 0;
					// 将上行错帧和对帧的点击数置0
					m_MSData_UL[msid.ToInt()].UL.m_iFormat2TrueFalseFrameHitUL[k][m] = 0;
				}
			}
			//            for (int k = 0; k < 50; ++k) {
			//                m_MSData_UL[msid].RBnBeenUsed[k] = 0;
			//            }
			//            for (int l = 0; l < 4; ++l) {
			//                m_MSData_UL[msid].m_iPacketResultDL[l] = 0;
			//                m_MSData_UL[msid].m_iPacketResultUL[l] = 0;
			//            }
			for (int i = 0; i < 5; ++i) {
				m_MSData_UL[msid.ToInt()].UL.m_iCorrectBlockHit[i] = 0;
				m_MSData_UL[msid.ToInt()].UL.m_iCorruptBlockHit[i] = 0;
			}
			for (int i = 0; i < 4; ++i) {
				m_MSData_UL[msid.ToInt()].UL.m_dBLERTx[i] = 0;
			}
		}

		for (BTSID btsid = BTSID::Begin(); btsid != BTSID::End(); ++btsid) {

			//m_BTSData[btsid].DL.m_dThroughputKbps = 0;
			m_BTSData[btsid.GetTxID()].UL.m_dThroughputKbps = 0;

			m_BTSData[btsid.GetTxID()].UL.m_dIoT = 0;
			//        m_dIoT_load
			m_BTSData[btsid.GetTxID()].UL.m_dIoT_load = 0;
			m_BTSData[btsid.GetTxID()].UL.m_Iiot_count = 0;
			//m_Iiot_count_load
			m_BTSData[btsid.GetTxID()].UL.m_Iiot_count_load = 0;


            m_BTSData[btsid.GetTxID()].UL.m_iNewTxNum = 0;
            m_BTSData[btsid.GetTxID()].UL.m_iNewTxErrorNum = 0;

            m_BTSData[btsid.GetTxID()].UL.m_iScheMSNum = 0;
            m_BTSData[btsid.GetTxID()].UL.m_iScheTime = 0;

		}
	}
}

/// 打印输出文件的表头

void Statistician::PrintHead() {
	//输出下行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
     //hyl 冗余
		//输出DMRS开销的表头
//		DL.fOH_DMRS << "OH_DMRS_Rate"
//			<< setw(20) << "OH_DMRS_Rate_Old" << endl;
//		//定义输出格式
		boost::format out_ms("%-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s");
		//输出下行移动台的表头
		DL.fMS << out_ms % "MSID" % "BTSID" % "POSX(m)" % "POSY(m)" % "POSZ(m)" % "AveRate(kbps)" % "BLERTx1"
			% "BLERTx2" % "BLERTx3" % "BLERTx4" % "LinkLossDB" % "AveSINRDB" % "PostSINRDB" % "PostSINRDBLOG"
			% "GeometryDB" % "Rank1Num" % "Rank2Num" % "Rank3Num" % "Rank4Num" % "TM3Num" % "AveAntGainDB" % "CRSSINRDB" % "ESD" % "ESA";
		DL.fMS << endl;
//hyl 冗余
//		// RedCap :输出下行移动台的表头
//		DL.fMSeMBB << out_ms % "MSID" % "BTSID" % "POSX(m)" % "POSY(m)" % "POSZ(m)" % "AveRate(kbps)" % "BLERTx1"
//			% "BLERTx2" % "BLERTx3" % "BLERTx4" % "LinkLossDB" % "AveSINRDB" % "PostSINRDB" % "PostSINRDBLOG"
//			% "GeometryDB" % "Rank1Num" % "Rank2Num" % "Rank3Num" % "Rank4Num" % "TM3Num" % "AveAntGainDB" % "CRSSINRDB" % "ESD" % "ESA";
//		DL.fMSeMBB << endl;
//
//		// RedCap :输出下行移动台的表头
//		DL.fMSRedCap << out_ms % "MSID" % "BTSID" % "POSX(m)" % "POSY(m)" % "POSZ(m)" % "AveRate(kbps)" % "BLERTx1"
//			% "BLERTx2" % "BLERTx3" % "BLERTx4" % "LinkLossDB" % "AveSINRDB" % "PostSINRDB" % "PostSINRDBLOG"
//			% "GeometryDB" % "Rank1Num" % "Rank2Num" % "Rank3Num" % "Rank4Num" % "TM3Num" % "AveAntGainDB" % "CRSSINRDB" % "ESD" % "ESA";
//		DL.fMSRedCap << endl;

		//输出下行基站的表头
        boost::format out_bts("%-30s %-30s %-30s %-30s %-30s");
        DL.fBS << out_bts % "BTSID" % "NumServMS" % "AveRate(kbps)" % "BLER" % "AveScheMSNumPerSlot";
		DL.fBS << endl;

		//输出MCS统计的表头
		boost::format out_mcs("%-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s");
		DL.fMCS << out_mcs % "MSID" % "MCS1(QPSK1)" % "MCS2(QPSK2)" % "MCS3(QPSK3)" % "MCS4(QPSK4)" % "MCS5(QPSK5)"
			% "MCS6(QPSK6)" % "MCS7(QPSK7)" % "MCS8(QPSK8)" % "MCS9(QPSK9)" % "MCS10(QPSK10)"
			% "MCS11(16QAM1)" % "MCS12(16QAM2)" % "MCS13(16QAM3)" % "MCS14(16QAM4)" % "MCS15(16QAM5)"
			% "MCS16(16QAM6)" % "MCS17(16QAM7)" % "MCS18(64QAM1)" % "MCS19(64QAM2)" % "MCS20(64QAM3)"
			% "MCS21(64QAM4)" % "MCS22(64QAM5)" % "MCS23(64QAM6)" % "MCS24(64QAM7)" % "MCS25(64QAM8)"
			% "MCS26(16QAM9)" % "MCS27(16QAM10)" % "MCS28(64QAM11)" % "MCS29(64QAM12)";
		DL.fMCS << endl;
        //hyl 冗余
//		DL.fMCS_eMBB << out_mcs % "MSID" % "MCS1(QPSK1)" % "MCS2(QPSK2)" % "MCS3(QPSK3)" % "MCS4(QPSK4)" % "MCS5(QPSK5)"
//			% "MCS6(QPSK6)" % "MCS7(QPSK7)" % "MCS8(QPSK8)" % "MCS9(QPSK9)" % "MCS10(QPSK10)"
//			% "MCS11(16QAM1)" % "MCS12(16QAM2)" % "MCS13(16QAM3)" % "MCS14(16QAM4)" % "MCS15(16QAM5)"
//			% "MCS16(16QAM6)" % "MCS17(16QAM7)" % "MCS18(64QAM1)" % "MCS19(64QAM2)" % "MCS20(64QAM3)"
//			% "MCS21(64QAM4)" % "MCS22(64QAM5)" % "MCS23(64QAM6)" % "MCS24(64QAM7)" % "MCS25(64QAM8)"
//			% "MCS26(16QAM9)" % "MCS27(16QAM10)" % "MCS28(64QAM11)" % "MCS29(64QAM12)";
//		DL.fMCS_eMBB << endl;
//
//		DL.fMCS_RedCap << out_mcs % "MSID" % "MCS1(QPSK1)" % "MCS2(QPSK2)" % "MCS3(QPSK3)" % "MCS4(QPSK4)" % "MCS5(QPSK5)"
//			% "MCS6(QPSK6)" % "MCS7(QPSK7)" % "MCS8(QPSK8)" % "MCS9(QPSK9)" % "MCS10(QPSK10)"
//			% "MCS11(16QAM1)" % "MCS12(16QAM2)" % "MCS13(16QAM3)" % "MCS14(16QAM4)" % "MCS15(16QAM5)"
//			% "MCS16(16QAM6)" % "MCS17(16QAM7)" % "MCS18(64QAM1)" % "MCS19(64QAM2)" % "MCS20(64QAM3)"
//			% "MCS21(64QAM4)" % "MCS22(64QAM5)" % "MCS23(64QAM6)" % "MCS24(64QAM7)" % "MCS25(64QAM8)"
//			% "MCS26(16QAM9)" % "MCS27(16QAM10)" % "MCS28(64QAM11)" % "MCS29(64QAM12)";
//		DL.fMCS_RedCap << endl;

//		if (Parameters::Instance().BASIC.IScenarioModel == Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER) {
//			//输出下行Macro下的移动台的表头
//			DL.fMacroMS << out_ms % "MSID" % "BTSID" % "POSX(m)" % "POSY(m)" % "AveRate(kbps)" % "BLERTx1"
//				% "BLERTx2" % "BLERTx3" % "BLERTx4" % "LinkLossDB" % "AveSINRDB" % "PostSINRDB"
//				% "GeometryDB" % "Rank1Num" % "Rank2Num" % "Rank3Num" % "Rank4Num" % "TM3Num" % "AveAntGainDB" % "CRSSINRDB";
//			DL.fMacroMS << endl;
//
//			//输出下行Pico下的移动台的表头
//			DL.fPicoMS << out_ms % "MSID" % "BTSID" % "POSX(m)" % "POSY(m)" % "AveRate(kbps)" % "BLERTx1"
//				% "BLERTx2" % "BLERTx3" % "BLERTx4" % "LinkLossDB" % "AveSINRDB" % "PostSINRDB"
//				% "GeometryDB" % "Rank1Num" % "Rank2Num" % "Rank3Num" % "Rank4Num" % "TM3Num" % "AveAntGainDB" % "CRSSINRDB";
//			DL.fPicoMS << endl;
//
//			//输出Macro下的MCS统计的表头
//			DL.fMacroMCS << out_mcs % "MSID" % "MCS1(QPSK1)" % "MCS2(QPSK2)" % "MCS3(QPSK3)" % "MCS4(QPSK4)" % "MCS5(QPSK5)"
//				% "MCS6(QPSK6)" % "MCS7(QPSK7)" % "MCS8(QPSK8)" % "MCS9(QPSK9)" % "MCS10(QPSK10)"
//				% "MCS11(16QAM1)" % "MCS12(16QAM2)" % "MCS13(16QAM3)" % "MCS14(16QAM4)" % "MCS15(16QAM5)"
//				% "MCS16(16QAM6)" % "MCS17(16QAM7)" % "MCS18(64QAM1)" % "MCS19(64QAM2)" % "MCS20(64QAM3)"
//				% "MCS21(64QAM4)" % "MCS22(64QAM5)" % "MCS23(64QAM6)" % "MCS24(64QAM7)" % "MCS25(64QAM8)"
//				% "MCS26(16QAM9)" % "MCS27(16QAM10)" % "MCS28(64QAM11)" % "MCS29(64QAM12)";
//			DL.fMacroMCS << endl;
//
//			//输出Pico下的MCS统计的表头
//			DL.fPicoMCS << out_mcs % "MSID" % "MCS1(QPSK1)" % "MCS2(QPSK2)" % "MCS3(QPSK3)" % "MCS4(QPSK4)" % "MCS5(QPSK5)"
//				% "MCS6(QPSK6)" % "MCS7(QPSK7)" % "MCS8(QPSK8)" % "MCS9(QPSK9)" % "MCS10(QPSK10)"
//				% "MCS11(16QAM1)" % "MCS12(16QAM2)" % "MCS13(16QAM3)" % "MCS14(16QAM4)" % "MCS15(16QAM5)"
//				% "MCS16(16QAM6)" % "MCS17(16QAM7)" % "MCS18(64QAM1)" % "MCS19(64QAM2)" % "MCS20(64QAM3)"
//				% "MCS21(64QAM4)" % "MCS22(64QAM5)" % "MCS23(64QAM6)" % "MCS24(64QAM7)" % "MCS25(64QAM8)"
//				% "MCS26(16QAM9)" % "MCS27(16QAM10)" % "MCS28(64QAM11)" % "MCS29(64QAM12)";
//			DL.fPicoMCS << endl;
//		}
	}

	//输出上行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::DL) {
		int width = 30;
		//只保留上行
		UL.fMS << "MSID"
			<< setw(width) << "ActiveBSID"
			<< setw(width) << "POSX"
			<< setw(width) << "POSY"
            << setw(width) << "POSZ"
			<< setw(width) << "AveRate(kbps)"
			<< setw(width) << "BLERTx1"
			<< setw(width) << "BLERTx2"
			<< setw(width) << "BLERTx3"
			<< setw(width) << "BLERTx4"
			<< setw(width) << "LinkLossDB"
			<< setw(width) << "dAvePathlossDB"//zhengyi
			<< setw(width) << "dPathlossDB"//zhengyi
			<< setw(width) << "AveCQIPreSINRDB"//test
			<< setw(width) << "m_dAVE_preCQISINR_Index"//test
			<< setw(width) << "AvePreSINRDB"//zhengyi
			<< setw(width) << "AveSINRDB"
			<< setw(width) << "phr_trigger_0"
			<< setw(width) << "phr_trigger_1"
			<< setw(width) << "phr_trigger_2"
			//                << setw(width) << "AveSINRDB_test"
			<< setw(width) << "UL_GeometryDB"
			<< setw(width) << "m_d2D_Distance"//szx
			<< setw(width) << "m_AODLOSDEG"//szx
			<< setw(width) << "m_dUE2BSTiltDEG"//szx
			<< setw(width) << "AveTxAntennaPatternDB"//szx
			<< setw(width) << "PlusOffsetDEG"//szx
			<< setw(width) << "RxHeightM"//szx
			<< setw(width) << "MSTxPowerMw"//zhengyi
			<< setw(width) << "GeometryDB"
			<< setw(width) << "Rank1Num"
			<< setw(width) << "Rank2Num"
			<< setw(width) << "0ReTransTimes"
			<< setw(width) << "1ReTransTimes"
			<< setw(width) << "2ReTransTimes"
			<< setw(width) << "3ReTransTimes"
			<< setw(width) << "TimesForDrop"
			<< setw(width) << "QPSK_1"
			<< setw(width) << "QPSK_2"
			<< setw(width) << "QPSK_3"
			<< setw(width) << "QPSK_4"
			<< setw(width) << "QPSK_5"
			<< setw(width) << "QPSK_6"
			<< setw(width) << "QPSK_7"
			<< setw(width) << "QPSK_8"
			<< setw(width) << "QPSK_9"
			<< setw(width) << "QPSK_10"
			<< setw(width) << "16QAM_1"
			<< setw(width) << "16QAM_2"
			<< setw(width) << "16QAM_3"
			<< setw(width) << "16QAM_4"
			<< setw(width) << "16QAM_5"
			<< setw(width) << "16QAM_6"
			<< setw(width) << "16QAM_7"
			<< setw(width) << "64QAM_1"
			<< setw(width) << "64QAM_2"
			<< setw(width) << "64QAM_3"
			<< setw(width) << "64QAM_4"
			<< setw(width) << "64QAM_5"
			<< setw(width) << "64QAM_6"
			<< setw(width) << "64QAM_7"
			<< setw(width) << "64QAM_8"
			<< setw(width) << "64QAM_9"
			<< setw(width) << "64QAM_10"
			<< setw(width) << "64QAM_11"
			<< setw(width) << "64QAM_12"
			<< setw(width) << "RBcount1"
			<< setw(width) << "RBcount2"
			<< setw(width) << "RBSupported"
			<< setw(width) << "SchduleFreq1"
			<< setw(width) << "SchduleFreq2"
			<< setw(width) << "m_iOccupiedRBS1"
			<< setw(width) << "m_iOccupiedRBS2"
			//                << setw(width) << "Comp_size"//zhengyi
			<< setw(width) << "MU_count"//zhengyi
			<< setw(width) << "SU_count"//zhengyi
			<< setw(width) << "is_indoor"//zhengyi
			<< setw(width) << "is_LOS"//nml
			<< setw(width) << "ShadowFading"//nml
			<< endl;
		///输出下行基站的表头
        UL.fBS << setw(width) << "BTSID"
               << setw(width) << "NumServMS"
               << setw(width) << "AveRate(kbps)"
               << setw(width) << "BLER"
               << setw(width) << "AveScheMSNumPerSlot"
               << endl;
        //hyl  冗余输出
//		///输出上行基站IoT
//		UL.fIoT << setw(width) << "BTSid"
//			<< setw(width) << "IoT" << endl;
//
//		///RedCap：用来记录类型一移动台统计数据的文件(上行)的表头
//		UL.fMSeMBB << "MSID"
//			<< setw(width) << "ActiveBSID"
//			<< setw(width) << "POSX"
//			<< setw(width) << "POSY"
//            << setw(width) << "POSZ"
//			<< setw(width) << "AveRate(kbps)"
//			<< setw(width) << "BLERTx1"
//			<< setw(width) << "BLERTx2"
//			<< setw(width) << "BLERTx3"
//			<< setw(width) << "BLERTx4"
//			<< setw(width) << "LinkLossDB"
//			<< setw(width) << "dAvePathlossDB"//zhengyi
//			<< setw(width) << "dPathlossDB"//zhengyi
//			<< setw(width) << "AveCQIPreSINRDB"//test
//			<< setw(width) << "m_dAVE_preCQISINR_Index"//test
//			<< setw(width) << "AvePreSINRDB"//zhengyi
//			<< setw(width) << "AveSINRDB"
//			<< setw(width) << "phr_trigger_0"
//			<< setw(width) << "phr_trigger_1"
//			<< setw(width) << "phr_trigger_2"
//			//                << setw(width) << "AveSINRDB_test"
//			<< setw(width) << "UL_GeometryDB"
//			<< setw(width) << "m_d2D_Distance"//szx
//			<< setw(width) << "m_AODLOSDEG"//szx
//			<< setw(width) << "m_dUE2BSTiltDEG"//szx
//			<< setw(width) << "AveTxAntennaPatternDB"//szx
//			<< setw(width) << "PlusOffsetDEG"//szx
//			<< setw(width) << "RxHeightM"//szx
//			<< setw(width) << "MSTxPowerMw"//zhengyi
//			<< setw(width) << "GeometryDB"
//			<< setw(width) << "Rank1Num"
//			<< setw(width) << "Rank2Num"
//			<< setw(width) << "0ReTransTimes"
//			<< setw(width) << "1ReTransTimes"
//			<< setw(width) << "2ReTransTimes"
//			<< setw(width) << "3ReTransTimes"
//			<< setw(width) << "TimesForDrop"
//			<< setw(width) << "QPSK_1"
//			<< setw(width) << "QPSK_2"
//			<< setw(width) << "QPSK_3"
//			<< setw(width) << "QPSK_4"
//			<< setw(width) << "QPSK_5"
//			<< setw(width) << "QPSK_6"
//			<< setw(width) << "QPSK_7"
//			<< setw(width) << "QPSK_8"
//			<< setw(width) << "QPSK_9"
//			<< setw(width) << "QPSK_10"
//			<< setw(width) << "16QAM_1"
//			<< setw(width) << "16QAM_2"
//			<< setw(width) << "16QAM_3"
//			<< setw(width) << "16QAM_4"
//			<< setw(width) << "16QAM_5"
//			<< setw(width) << "16QAM_6"
//			<< setw(width) << "16QAM_7"
//			<< setw(width) << "64QAM_1"
//			<< setw(width) << "64QAM_2"
//			<< setw(width) << "64QAM_3"
//			<< setw(width) << "64QAM_4"
//			<< setw(width) << "64QAM_5"
//			<< setw(width) << "64QAM_6"
//			<< setw(width) << "64QAM_7"
//			<< setw(width) << "64QAM_8"
//			<< setw(width) << "64QAM_9"
//			<< setw(width) << "64QAM_10"
//			<< setw(width) << "64QAM_11"
//			<< setw(width) << "64QAM_12"
//			<< setw(width) << "RBcount1"
//			<< setw(width) << "RBcount2"
//			<< setw(width) << "RBSupported"
//			<< setw(width) << "SchduleFreq1"
//			<< setw(width) << "SchduleFreq2"
//			<< setw(width) << "m_iOccupiedRBS1"
//			<< setw(width) << "m_iOccupiedRBS2"
//			//                << setw(width) << "Comp_size"//zhengyi
//			<< setw(width) << "MU_count"//zhengyi
//			<< setw(width) << "SU_count"//zhengyi
//			<< setw(width) << "is_indoor"//zhengyi
//			<< setw(width) << "is_LOS"//nml
//			<< setw(width) << "ShadowFading"//nml
//			<< endl;
//
//		///RedCap：用来记录类型二移动台统计数据的文件(上行)的表头
//		UL.fMSRedCap << "MSID"
//			<< setw(width) << "ActiveBSID"
//			<< setw(width) << "POSX"
//			<< setw(width) << "POSY"
//            << setw(width) << "POSZ"
//			<< setw(width) << "AveRate(kbps)"
//			<< setw(width) << "BLERTx1"
//			<< setw(width) << "BLERTx2"
//			<< setw(width) << "BLERTx3"
//			<< setw(width) << "BLERTx4"
//			<< setw(width) << "LinkLossDB"
//			<< setw(width) << "dAvePathlossDB"//zhengyi
//			<< setw(width) << "dPathlossDB"//zhengyi
//			<< setw(width) << "AveCQIPreSINRDB"//test
//			<< setw(width) << "m_dAVE_preCQISINR_Index"//test
//			<< setw(width) << "AvePreSINRDB"//zhengyi
//			<< setw(width) << "AveSINRDB"
//			<< setw(width) << "phr_trigger_0"
//			<< setw(width) << "phr_trigger_1"
//			<< setw(width) << "phr_trigger_2"
//			//                << setw(width) << "AveSINRDB_test"
//			<< setw(width) << "UL_GeometryDB"
//			<< setw(width) << "m_d2D_Distance"//szx
//			<< setw(width) << "m_AODLOSDEG"//szx
//			<< setw(width) << "m_dUE2BSTiltDEG"//szx
//			<< setw(width) << "AveTxAntennaPatternDB"//szx
//			<< setw(width) << "PlusOffsetDEG"//szx
//			<< setw(width) << "RxHeightM"//szx
//			<< setw(width) << "MSTxPowerMw"//zhengyi
//			<< setw(width) << "GeometryDB"
//			<< setw(width) << "Rank1Num"
//			<< setw(width) << "Rank2Num"
//			<< setw(width) << "0ReTransTimes"
//			<< setw(width) << "1ReTransTimes"
//			<< setw(width) << "2ReTransTimes"
//			<< setw(width) << "3ReTransTimes"
//			<< setw(width) << "TimesForDrop"
//			<< setw(width) << "QPSK_1"
//			<< setw(width) << "QPSK_2"
//			<< setw(width) << "QPSK_3"
//			<< setw(width) << "QPSK_4"
//			<< setw(width) << "QPSK_5"
//			<< setw(width) << "QPSK_6"
//			<< setw(width) << "QPSK_7"
//			<< setw(width) << "QPSK_8"
//			<< setw(width) << "QPSK_9"
//			<< setw(width) << "QPSK_10"
//			<< setw(width) << "16QAM_1"
//			<< setw(width) << "16QAM_2"
//			<< setw(width) << "16QAM_3"
//			<< setw(width) << "16QAM_4"
//			<< setw(width) << "16QAM_5"
//			<< setw(width) << "16QAM_6"
//			<< setw(width) << "16QAM_7"
//			<< setw(width) << "64QAM_1"
//			<< setw(width) << "64QAM_2"
//			<< setw(width) << "64QAM_3"
//			<< setw(width) << "64QAM_4"
//			<< setw(width) << "64QAM_5"
//			<< setw(width) << "64QAM_6"
//			<< setw(width) << "64QAM_7"
//			<< setw(width) << "64QAM_8"
//			<< setw(width) << "64QAM_9"
//			<< setw(width) << "64QAM_10"
//			<< setw(width) << "64QAM_11"
//			<< setw(width) << "64QAM_12"
//			<< setw(width) << "RBcount1"
//			<< setw(width) << "RBcount2"
//			<< setw(width) << "RBSupported"
//			<< setw(width) << "SchduleFreq1"
//			<< setw(width) << "SchduleFreq2"
//			<< setw(width) << "m_iOccupiedRBS1"
//			<< setw(width) << "m_iOccupiedRBS2"
//			//                << setw(width) << "Comp_size"//zhengyi
//			<< setw(width) << "MU_count"//zhengyi
//			<< setw(width) << "SU_count"//zhengyi
//			<< setw(width) << "is_indoor"//zhengyi
//			<< setw(width) << "is_LOS"//nml
//			<< setw(width) << "ShadowFading"//nml
//			<< endl;
	}
}

// 打印输出数据的函数

void Statistician::PrintTable() {

	//输出下行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
        //hyl  冗余输出
//		// 输出DMRS的开销
//		DL.fOH_DMRS << m_dOH_DMRS_Rate
//			<< setw(20) << m_dOH_DMRS_Rate_Old << endl;
		// 输出移动台的数据
		// 定义输出格式
		boost::format out_ms("%-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s");
		for (int i = 0; i < static_cast<int> (m_MSData_DL.size()); ++i) {
			if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
				MSData_DL& msdata = m_MSData_DL[MSID(i).ToInt()];
                string fMS_btsid;
                stringstream ss;
                ss << "(" << msdata.m_ActiveBTS.GetBSID().ToInt() << "," << msdata.m_ActiveBTS.GetIndex() << ")" ;
                fMS_btsid = ss.str();
				DL.fMS << out_ms % i % fMS_btsid % msdata.m_x % msdata.m_y % msdata.m_z % msdata.DL.m_dAveRateKbps % msdata.DL.m_dBLERTx[1]
					% msdata.DL.m_dBLERTx[2] % msdata.DL.m_dBLERTx[3] % msdata.DL.m_dBLERTx[4] % msdata.m_dLinkLossDB % L2DB(msdata.DL.m_dAveSINR) % L2DB(msdata.DL.m_dAvePostSINR) % msdata.DL.m_dLogAvePostSINR
					% msdata.DL.m_dGeometryDB % msdata.DL.m_iRankHitNum[0] % msdata.DL.m_iRankHitNum[1] % msdata.DL.m_iRankHitNum[2] % msdata.DL.m_iRankHitNum[3] % msdata.DL.m_iTM3HitNum % msdata.m_dAveAntGainDB % L2DB(msdata.DL.m_dCRSSINR) % msdata.m_dESD % msdata.m_dESA;
				DL.fMS << endl;
				if (msdata.DL.m_iTM3HitNum > 0) {
					cout << "msdata.DL.m_iTM3HitNum= " << (msdata.DL.m_iTM3HitNum) << endl;
				}
			}
		}
//cwq dahuawu
//		for (int i = 0; i < floor(Parameters::Instance().BASIC.ITotalMSNum * Parameters::Instance().BASIC.type1MSRate + 0.5); ++i) {
//			if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
//				MSData_DL& msdata = m_MSData_DL[MSID(i).ToInt()];
//                string fMS_btsid;
//                stringstream ss;
//                ss << "(" << msdata.m_ActiveBTS.GetBSID().ToInt() << "," << msdata.m_ActiveBTS.GetIndex() << ")" ;
//                fMS_btsid = ss.str();
//				DL.fMSeMBB << out_ms % i % fMS_btsid % msdata.m_x % msdata.m_y % msdata.m_z % msdata.DL.m_dAveRateKbps % msdata.DL.m_dBLERTx[1]
//					% msdata.DL.m_dBLERTx[2] % msdata.DL.m_dBLERTx[3] % msdata.DL.m_dBLERTx[4] % msdata.m_dLinkLossDB % L2DB(msdata.DL.m_dAveSINR) % L2DB(msdata.DL.m_dAvePostSINR) % msdata.DL.m_dLogAvePostSINR
//					% msdata.DL.m_dGeometryDB % msdata.DL.m_iRankHitNum[0] % msdata.DL.m_iRankHitNum[1] % msdata.DL.m_iRankHitNum[2] % msdata.DL.m_iRankHitNum[3] % msdata.DL.m_iTM3HitNum % msdata.m_dAveAntGainDB % L2DB(msdata.DL.m_dCRSSINR) % msdata.m_dESD % msdata.m_dESA;
//				DL.fMSeMBB << endl;
//				if (msdata.DL.m_iTM3HitNum > 0) {
//					cout << "msdata.DL.m_iTM3HitNum= " << (msdata.DL.m_iTM3HitNum) << endl;
//				}
//			}
//		}
//cwq dahuawu
//		for (int i = floor(Parameters::Instance().BASIC.ITotalMSNum * Parameters::Instance().BASIC.type1MSRate + 0.5); i < Parameters::Instance().BASIC.ITotalMSNum; ++i) {
//			if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
//				MSData_DL& msdata = m_MSData_DL[MSID(i).ToInt()];
//                string fMS_btsid;
//                stringstream ss;
//                ss << "(" << msdata.m_ActiveBTS.GetBSID().ToInt() << "," << msdata.m_ActiveBTS.GetIndex() << ")" ;
//                fMS_btsid = ss.str();
//				DL.fMSRedCap << out_ms % i % fMS_btsid % msdata.m_x % msdata.m_y % msdata.m_z % msdata.DL.m_dAveRateKbps % msdata.DL.m_dBLERTx[1]
//					% msdata.DL.m_dBLERTx[2] % msdata.DL.m_dBLERTx[3] % msdata.DL.m_dBLERTx[4] % msdata.m_dLinkLossDB % L2DB(msdata.DL.m_dAveSINR) % L2DB(msdata.DL.m_dAvePostSINR) % msdata.DL.m_dLogAvePostSINR
//					% msdata.DL.m_dGeometryDB % msdata.DL.m_iRankHitNum[0] % msdata.DL.m_iRankHitNum[1] % msdata.DL.m_iRankHitNum[2] % msdata.DL.m_iRankHitNum[3] % msdata.DL.m_iTM3HitNum % msdata.m_dAveAntGainDB % L2DB(msdata.DL.m_dCRSSINR) % msdata.m_dESD % msdata.m_dESA;
//				DL.fMSRedCap << endl;
//				if (msdata.DL.m_iTM3HitNum > 0) {
//					cout << "msdata.DL.m_iTM3HitNum= " << (msdata.DL.m_iTM3HitNum) << endl;
//				}
//			}
//		}



		//输出基站的数据
		boost::format out_bts("%-30s %-30s %-30s %-30s %-30s");
		for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
			if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {

                double BLER = m_BTSData[btsid.GetTxID()].DL.m_iNewTxErrorNum;
                BLER /= m_BTSData[btsid.GetTxID()].DL.m_iNewTxNum;
                double AceScheMsNumPerSlot = m_BTSData[btsid.GetTxID()].DL.m_iScheMSNum;
                AceScheMsNumPerSlot /= m_BTSData[btsid.GetTxID()].DL.m_iScheTime;

                DL.fBS << out_bts % btsid % m_BTSData[btsid.GetTxID()].m_iNumServMS % m_BTSData[btsid.GetTxID()].DL.m_dThroughputKbps % BLER % AceScheMsNumPerSlot;
                DL.fBS << endl;
			}
		}
//cwq dahuawu
		//输出MCS的统计数据
		boost::format out_mcs("%-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s %-30s");
//		for (int i = 0; i < static_cast<int> (m_MSData_DL.size()); ++i) {
//			if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
//				MSData_DL& msdata_DL = m_MSData_DL[MSID(i).ToInt()];
//				DL.fMCS << out_mcs % i % msdata_DL.DL.m_iMCSHit[0] % msdata_DL.DL.m_iMCSHit[1] % msdata_DL.DL.m_iMCSHit[2] % msdata_DL.DL.m_iMCSHit[3]
//					% msdata_DL.DL.m_iMCSHit[4] % msdata_DL.DL.m_iMCSHit[5] % msdata_DL.DL.m_iMCSHit[6] % msdata_DL.DL.m_iMCSHit[7]
//					% msdata_DL.DL.m_iMCSHit[8] % msdata_DL.DL.m_iMCSHit[9] % msdata_DL.DL.m_iMCSHit[10] % msdata_DL.DL.m_iMCSHit[11]
//					% msdata_DL.DL.m_iMCSHit[12] % msdata_DL.DL.m_iMCSHit[13] % msdata_DL.DL.m_iMCSHit[14] % msdata_DL.DL.m_iMCSHit[15]
//					% msdata_DL.DL.m_iMCSHit[16] % msdata_DL.DL.m_iMCSHit[17] % msdata_DL.DL.m_iMCSHit[18] % msdata_DL.DL.m_iMCSHit[19]
//					% msdata_DL.DL.m_iMCSHit[20] % msdata_DL.DL.m_iMCSHit[21] % msdata_DL.DL.m_iMCSHit[22] % msdata_DL.DL.m_iMCSHit[23]
//					% msdata_DL.DL.m_iMCSHit[24] % msdata_DL.DL.m_iMCSHit[25] % msdata_DL.DL.m_iMCSHit[26] % msdata_DL.DL.m_iMCSHit[27]
//					% msdata_DL.DL.m_iMCSHit[28];
//				DL.fMCS << endl;
//			}
//		}

		// RedCap: 输出MCS的统计数据
//cwq dahuawu
//		for (int i = 0; i < floor(Parameters::Instance().BASIC.ITotalMSNum * Parameters::Instance().BASIC.type1MSRate + 0.5); ++i) {
//			if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
//				MSData_DL& msdata_DL = m_MSData_DL[MSID(i).ToInt()];
//				DL.fMCS_eMBB << out_mcs % i % msdata_DL.DL.m_iMCSHit[0] % msdata_DL.DL.m_iMCSHit[1] % msdata_DL.DL.m_iMCSHit[2] % msdata_DL.DL.m_iMCSHit[3]
//					% msdata_DL.DL.m_iMCSHit[4] % msdata_DL.DL.m_iMCSHit[5] % msdata_DL.DL.m_iMCSHit[6] % msdata_DL.DL.m_iMCSHit[7]
//					% msdata_DL.DL.m_iMCSHit[8] % msdata_DL.DL.m_iMCSHit[9] % msdata_DL.DL.m_iMCSHit[10] % msdata_DL.DL.m_iMCSHit[11]
//					% msdata_DL.DL.m_iMCSHit[12] % msdata_DL.DL.m_iMCSHit[13] % msdata_DL.DL.m_iMCSHit[14] % msdata_DL.DL.m_iMCSHit[15]
//					% msdata_DL.DL.m_iMCSHit[16] % msdata_DL.DL.m_iMCSHit[17] % msdata_DL.DL.m_iMCSHit[18] % msdata_DL.DL.m_iMCSHit[19]
//					% msdata_DL.DL.m_iMCSHit[20] % msdata_DL.DL.m_iMCSHit[21] % msdata_DL.DL.m_iMCSHit[22] % msdata_DL.DL.m_iMCSHit[23]
//					% msdata_DL.DL.m_iMCSHit[24] % msdata_DL.DL.m_iMCSHit[25] % msdata_DL.DL.m_iMCSHit[26] % msdata_DL.DL.m_iMCSHit[27]
//					% msdata_DL.DL.m_iMCSHit[28];
//				DL.fMCS_eMBB << endl;
//			}
//		}
		// RedCap: 输出MCS的统计数据
        //cwq dahuawu
//		for (int i = floor(Parameters::Instance().BASIC.ITotalMSNum * Parameters::Instance().BASIC.type1MSRate + 0.5); i < Parameters::Instance().BASIC.ITotalMSNum; ++i) {
//			if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
//				MSData_DL& msdata_DL = m_MSData_DL[MSID(i).ToInt()];
//				DL.fMCS_RedCap << out_mcs % i % msdata_DL.DL.m_iMCSHit[0] % msdata_DL.DL.m_iMCSHit[1] % msdata_DL.DL.m_iMCSHit[2] % msdata_DL.DL.m_iMCSHit[3]
//					% msdata_DL.DL.m_iMCSHit[4] % msdata_DL.DL.m_iMCSHit[5] % msdata_DL.DL.m_iMCSHit[6] % msdata_DL.DL.m_iMCSHit[7]
//					% msdata_DL.DL.m_iMCSHit[8] % msdata_DL.DL.m_iMCSHit[9] % msdata_DL.DL.m_iMCSHit[10] % msdata_DL.DL.m_iMCSHit[11]
//					% msdata_DL.DL.m_iMCSHit[12] % msdata_DL.DL.m_iMCSHit[13] % msdata_DL.DL.m_iMCSHit[14] % msdata_DL.DL.m_iMCSHit[15]
//					% msdata_DL.DL.m_iMCSHit[16] % msdata_DL.DL.m_iMCSHit[17] % msdata_DL.DL.m_iMCSHit[18] % msdata_DL.DL.m_iMCSHit[19]
//					% msdata_DL.DL.m_iMCSHit[20] % msdata_DL.DL.m_iMCSHit[21] % msdata_DL.DL.m_iMCSHit[22] % msdata_DL.DL.m_iMCSHit[23]
//					% msdata_DL.DL.m_iMCSHit[24] % msdata_DL.DL.m_iMCSHit[25] % msdata_DL.DL.m_iMCSHit[26] % msdata_DL.DL.m_iMCSHit[27]
//					% msdata_DL.DL.m_iMCSHit[28];
//				DL.fMCS_RedCap << endl;
//			}
//		}

        //hyl 冗余
//		if (Parameters::Instance().BASIC.IScenarioModel == Parameters::SCENARIO_5G_DENSE_URBAN_2LAYER && Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {
//			for (int i = 0; i < static_cast<int> (m_MSData_DL.size()); ++i) {
//				MSData_DL& msdata_DL = m_MSData_DL[MSID(i).ToInt()];
//				if (BSManager::IsMacro(msdata_DL.m_ActiveBTS)) {
//                    string fMacroMS_btsid;
//                    stringstream ss;
//                    ss << "(" << msdata_DL.m_ActiveBTS.GetBSID().ToInt() << "," << msdata_DL.m_ActiveBTS.GetIndex() << ")" ;
//                    fMacroMS_btsid = ss.str();
//					DL.fMacroMS << out_ms % i % fMacroMS_btsid % msdata_DL.m_x % msdata_DL.m_y % msdata_DL.DL.m_dAveRateKbps % msdata_DL.DL.m_dBLERTx[1]
//						% msdata_DL.DL.m_dBLERTx[2] % msdata_DL.DL.m_dBLERTx[3] % msdata_DL.DL.m_dBLERTx[4] % msdata_DL.m_dLinkLossDB % L2DB(msdata_DL.DL.m_dAveSINR) % L2DB(msdata_DL.DL.m_dAvePostSINR)
//						% msdata_DL.DL.m_dGeometryDB % msdata_DL.DL.m_iRankHitNum[0] % msdata_DL.DL.m_iRankHitNum[1] % msdata_DL.DL.m_iRankHitNum[2] % msdata_DL.DL.m_iRankHitNum[3] % msdata_DL.DL.m_iTM3HitNum % msdata_DL.m_dAveAntGainDB % L2DB(msdata_DL.DL.m_dCRSSINR);
//					DL.fMacroMS << endl;
//
//					DL.fMacroMCS << out_mcs % i % msdata_DL.DL.m_iMCSHit[0] % msdata_DL.DL.m_iMCSHit[1] % msdata_DL.DL.m_iMCSHit[2] % msdata_DL.DL.m_iMCSHit[3]
//						% msdata_DL.DL.m_iMCSHit[4] % msdata_DL.DL.m_iMCSHit[5] % msdata_DL.DL.m_iMCSHit[6] % msdata_DL.DL.m_iMCSHit[7]
//						% msdata_DL.DL.m_iMCSHit[8] % msdata_DL.DL.m_iMCSHit[9] % msdata_DL.DL.m_iMCSHit[10] % msdata_DL.DL.m_iMCSHit[11]
//						% msdata_DL.DL.m_iMCSHit[12] % msdata_DL.DL.m_iMCSHit[13] % msdata_DL.DL.m_iMCSHit[14] % msdata_DL.DL.m_iMCSHit[15]
//						% msdata_DL.DL.m_iMCSHit[16] % msdata_DL.DL.m_iMCSHit[17] % msdata_DL.DL.m_iMCSHit[18] % msdata_DL.DL.m_iMCSHit[19]
//						% msdata_DL.DL.m_iMCSHit[20] % msdata_DL.DL.m_iMCSHit[21] % msdata_DL.DL.m_iMCSHit[22] % msdata_DL.DL.m_iMCSHit[23]
//						% msdata_DL.DL.m_iMCSHit[24] % msdata_DL.DL.m_iMCSHit[25] % msdata_DL.DL.m_iMCSHit[26] % msdata_DL.DL.m_iMCSHit[27]
//						% msdata_DL.DL.m_iMCSHit[28];
//					DL.fMacroMCS << endl;
//				}
//				else {
//                    string fPicoMS_btsid;
//                    stringstream ss;
//                    ss << "(" << msdata_DL.m_ActiveBTS.GetBSID().ToInt() << "," << msdata_DL.m_ActiveBTS.GetIndex() << ")" ;
//                    fPicoMS_btsid = ss.str();
//					DL.fPicoMS << out_ms % i % fPicoMS_btsid % msdata_DL.m_x % msdata_DL.m_y % msdata_DL.DL.m_dAveRateKbps % msdata_DL.DL.m_dBLERTx[1]
//						% msdata_DL.DL.m_dBLERTx[2] % msdata_DL.DL.m_dBLERTx[3] % msdata_DL.DL.m_dBLERTx[4] % msdata_DL.m_dLinkLossDB % L2DB(msdata_DL.DL.m_dAveSINR) % L2DB(msdata_DL.DL.m_dAvePostSINR)
//						% msdata_DL.DL.m_dGeometryDB % msdata_DL.DL.m_iRankHitNum[0] % msdata_DL.DL.m_iRankHitNum[1] % msdata_DL.DL.m_iRankHitNum[2] % msdata_DL.DL.m_iRankHitNum[3] % msdata_DL.DL.m_iTM3HitNum % msdata_DL.m_dAveAntGainDB % L2DB(msdata_DL.DL.m_dCRSSINR);
//					DL.fPicoMS << endl;
//
//					DL.fPicoMCS << out_mcs % i % msdata_DL.DL.m_iMCSHit[0] % msdata_DL.DL.m_iMCSHit[1] % msdata_DL.DL.m_iMCSHit[2] % msdata_DL.DL.m_iMCSHit[3]
//						% msdata_DL.DL.m_iMCSHit[4] % msdata_DL.DL.m_iMCSHit[5] % msdata_DL.DL.m_iMCSHit[6] % msdata_DL.DL.m_iMCSHit[7]
//						% msdata_DL.DL.m_iMCSHit[8] % msdata_DL.DL.m_iMCSHit[9] % msdata_DL.DL.m_iMCSHit[10] % msdata_DL.DL.m_iMCSHit[11]
//						% msdata_DL.DL.m_iMCSHit[12] % msdata_DL.DL.m_iMCSHit[13] % msdata_DL.DL.m_iMCSHit[14] % msdata_DL.DL.m_iMCSHit[15]
//						% msdata_DL.DL.m_iMCSHit[16] % msdata_DL.DL.m_iMCSHit[17] % msdata_DL.DL.m_iMCSHit[18] % msdata_DL.DL.m_iMCSHit[19]
//						% msdata_DL.DL.m_iMCSHit[20] % msdata_DL.DL.m_iMCSHit[21] % msdata_DL.DL.m_iMCSHit[22] % msdata_DL.DL.m_iMCSHit[23]
//						% msdata_DL.DL.m_iMCSHit[24] % msdata_DL.DL.m_iMCSHit[25] % msdata_DL.DL.m_iMCSHit[26] % msdata_DL.DL.m_iMCSHit[27]
//						% msdata_DL.DL.m_iMCSHit[28];
//					DL.fPicoMCS << endl;
//				}
//			}
//		}
	}

	//输出上行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::DL) {
		// 输出移动台的数据
		int width = 30;
		double m_iTotal = 0;
		// 统计各MCS等级使用次数
		double m_dFormatHitTimes[29];
		// 统计各MCS等级的使用比例
		double m_dFormatHitTimesPercent[29];

		for (int i = 0; i < 29; ++i) {
			m_dFormatHitTimes[i] = 0;
			m_dFormatHitTimesPercent[i] = 0.0;
		}
		vector<std::pair<int, MSData_UL&>> temp;
		for (int i = 0; i < static_cast<int> (m_MSData_UL.size()); ++i) {
			MSData_UL& msdata = m_MSData_UL[MSID(i).ToInt()];
			assert(msdata.m_d2D_Distance != 0);
			UL.fMS << i << setw(width) << "(" << (msdata.m_ActiveBTS.GetBSID().ToInt()) << "," << (msdata.m_ActiveBTS.GetIndex()) << ")"
				<< setw(width) << msdata.m_x
				<< setw(width) << msdata.m_y
                << setw(width) << msdata.m_z
				<< setw(width) << msdata.UL.m_dAveRateKbpsUL
				<< setw(width) << msdata.UL.m_dBLERTx[1]
				<< setw(width) << msdata.UL.m_dBLERTx[2]
				<< setw(width) << msdata.UL.m_dBLERTx[3]
				<< setw(width) << msdata.UL.m_dBLERTx[4]
				<< setw(width) << msdata.m_dLinkLossDB
				//zhengyi
				<< setw(width) << msdata.m_dAvePathlossDB
				<< setw(width) << msdata.m_dPathlossDB;
			double dAveSINRdB = 0;
			double dAverPreSINRdB = 0;
			//            double dAveSINRdB_test = 0;
			double dul_geometry_db = 0;

			double dAverCQIPreSINRdB = 0; //test

			//test
			if (msdata.m_dAVE_preCQISINR_Index != 0) {
				dAverCQIPreSINRdB = L2DB(msdata.m_dAVE_preCQISINR / (msdata.m_dAVE_preCQISINR_Index));
			}
			if (msdata.m_dAVE_preSINR_Index != 0) {
				dAverPreSINRdB = L2DB(msdata.m_dAVE_preSINR / (msdata.m_dAVE_preSINR_Index));
			}
			if (msdata.m_dAVE_SINR_Index != 0) {
				dAveSINRdB = L2DB(msdata.m_dAVE_SINR / (msdata.m_dAVE_SINR_Index));
			}
			//上行geometry,m_dul_geoemtry
			if (msdata.m_dAVE_SINR_Index != 0) {
				dul_geometry_db = L2DB(msdata.m_dul_geoemtry / (msdata.m_dAVE_SINR_Index));
			}

			UL.fMS << setw(width) << dAverCQIPreSINRdB; //test
			UL.fMS << setw(width) << msdata.m_dAVE_preCQISINR_Index; //test
			UL.fMS << setw(width) << dAverPreSINRdB; //UL Per UE Average preSINR 看看估计的SINR是多少，用来评估是否估计上出了问题
			UL.fMS << setw(width) << dAveSINRdB; //UL Per UE Average SINR
			UL.fMS << setw(width) << msdata.phr_trigger[0];
			UL.fMS << setw(width) << msdata.phr_trigger[1];
			UL.fMS << setw(width) << msdata.phr_trigger[2];
			//            UL.fMS << setw(width) << dAveSINRdB_test; //zhengyi test comp
			UL.fMS << setw(width) << dul_geometry_db //20170217 ul geoemtry
				<< setw(width) << msdata.m_d2D_Distance//szx
				<< setw(width) << msdata.m_AODLOSDEG
				<< setw(width) << msdata.m_dUE2BSTiltDEG
				<< setw(width) << msdata.m_dAveTxAntennaPatternDB
				<< setw(width) << msdata.m_dPlusOffsetDEG
				<< setw(width) << msdata.m_dRxHeight;
			if (msdata.m_dTxPowerMwIndex == 0) {
				UL.fMS << setw(width) << 0; //m_MSData[MSID(i)].m_dTxPowerMw / m_MSData[MSID(i)].m_dTxPowerMwIndex;
			}
			else {
				UL.fMS << setw(width) << msdata.m_dTxPowerMw / msdata.m_dTxPowerMwIndex;
			} //UL Per UE Average SINR
			UL.fMS << setw(width) << msdata.m_dGeometryDB
				<< setw(width) << msdata.m_iRank[0]
				<< setw(width) << msdata.m_iRank[1];
			UL.fMS << setw(width) << msdata.m_iReTransTimePerMS[0]
				<< setw(width) << msdata.m_iReTransTimePerMS[1]
				<< setw(width) << msdata.m_iReTransTimePerMS[2]
				<< setw(width) << msdata.m_iReTransTimePerMS[3]
				<< setw(width) << msdata.m_iReTransTimePerMS[4];
			for (int j = 0; j < 29; ++j) {//MCS最大29
				UL.fMS << setw(width) << msdata.m_dFormatFractionUL[j];
			}
			UL.fMS << setw(width) << msdata.m_dRBCount;
			UL.fMS << setw(width) << msdata.m_dRBCount2;
			UL.fMS << setw(width) << msdata.m_iRBSupported;
			UL.fMS << setw(width) << msdata.m_iSchCount;
			UL.fMS << setw(width) << msdata.m_iSchCount2;
			UL.fMS << setw(width) << msdata.m_iOccupiedRBS;
			UL.fMS << setw(width) << msdata.m_iOccupiedRBS2;
			//            UL.fMS << setw(width) << m_MSData_UL[MSID(i)].m_iCompSize;
			UL.fMS << setw(width) << msdata.m_iMUcount
				<< setw(width) << msdata.m_iSUcount;
			UL.fMS << setw(width) << msdata.m_iIsO2I; // is indoor
			UL.fMS << setw(width) << msdata.m_iIsLOS;
			UL.fMS << setw(width) << msdata.m_dShadowfading;
			//Statistician::Instance().m_MSData[m_ID].m_iIsO2I
			UL.fMS << endl;
			//计算各个MCS等级的使用次数
			for (int j = 0; j < 29; ++j) { //这个应该用上行可用的MCS等级数代替
				m_dFormatHitTimes[j] += (msdata.UL.m_iFormat2TrueFalseFrameHitUL[0][j] + msdata.UL.m_iFormat2TrueFalseFrameHitUL[1][j]);
				m_iTotal += (msdata.UL.m_iFormat2TrueFalseFrameHitUL[0][j] + msdata.UL.m_iFormat2TrueFalseFrameHitUL[1][j]);
			}
			//        }
		}

//hyl 冗余
//		//RedCap输出（上行） 实际上上行没有进行业务区分
//		if (Parameters::Instance().BASIC.ifcoexist == 1) {
//
//			for (int i = 0; i < floor(Parameters::Instance().BASIC.ITotalMSNum * Parameters::Instance().BASIC.type1MSRate + 0.5); ++i) {
//				MSData_UL& msdata = m_MSData_UL[MSID(i).ToInt()];
//				assert(msdata.m_d2D_Distance != 0);
//				UL.fMSeMBB << i << setw(width) << "(" << (msdata.m_ActiveBTS.GetBSID().ToInt()) << "," << (msdata.m_ActiveBTS.GetIndex()) << ")"
//					<< setw(width) << msdata.m_x
//					<< setw(width) << msdata.m_y
//                    << setw(width) << msdata.m_z
//					<< setw(width) << msdata.UL.m_dAveRateKbpsUL
//					<< setw(width) << msdata.UL.m_dBLERTx[1]
//					<< setw(width) << msdata.UL.m_dBLERTx[2]
//					<< setw(width) << msdata.UL.m_dBLERTx[3]
//					<< setw(width) << msdata.UL.m_dBLERTx[4]
//					<< setw(width) << msdata.m_dLinkLossDB
//					//zhengyi
//					<< setw(width) << msdata.m_dAvePathlossDB
//					<< setw(width) << msdata.m_dPathlossDB;
//				double dAveSINRdB = 0;
//				double dAverPreSINRdB = 0;
//				//            double dAveSINRdB_test = 0;
//				double dul_geometry_db = 0;
//
//				double dAverCQIPreSINRdB = 0; //test
//
//				//test
//				if (msdata.m_dAVE_preCQISINR_Index != 0) {
//					dAverCQIPreSINRdB = L2DB(msdata.m_dAVE_preCQISINR / (msdata.m_dAVE_preCQISINR_Index));
//				}
//				if (msdata.m_dAVE_preSINR_Index != 0) {
//					dAverPreSINRdB = L2DB(msdata.m_dAVE_preSINR / (msdata.m_dAVE_preSINR_Index));
//				}
//				if (msdata.m_dAVE_SINR_Index != 0) {
//					dAveSINRdB = L2DB(msdata.m_dAVE_SINR / (msdata.m_dAVE_SINR_Index));
//				}
//				//上行geometry,m_dul_geoemtry
//				if (msdata.m_dAVE_SINR_Index != 0) {
//					dul_geometry_db = L2DB(msdata.m_dul_geoemtry / (msdata.m_dAVE_SINR_Index));
//				}
//
//				UL.fMSeMBB << setw(width) << dAverCQIPreSINRdB; //test
//				UL.fMSeMBB << setw(width) << msdata.m_dAVE_preCQISINR_Index; //test
//				UL.fMSeMBB << setw(width) << dAverPreSINRdB; //UL Per UE Average preSINR 看看估计的SINR是多少，用来评估是否估计上出了问题
//				UL.fMSeMBB << setw(width) << dAveSINRdB; //UL Per UE Average SINR
//				UL.fMSeMBB << setw(width) << msdata.phr_trigger[0];
//				UL.fMSeMBB << setw(width) << msdata.phr_trigger[1];
//				UL.fMSeMBB << setw(width) << msdata.phr_trigger[2];
//				//            UL.fMSeMBB << setw(width) << dAveSINRdB_test; //zhengyi test comp
//				UL.fMSeMBB << setw(width) << dul_geometry_db //20170217 ul geoemtry
//					<< setw(width) << msdata.m_d2D_Distance//szx
//					<< setw(width) << msdata.m_AODLOSDEG
//					<< setw(width) << msdata.m_dUE2BSTiltDEG
//					<< setw(width) << msdata.m_dAveTxAntennaPatternDB
//					<< setw(width) << msdata.m_dPlusOffsetDEG
//					<< setw(width) << msdata.m_dRxHeight;
//				if (msdata.m_dTxPowerMwIndex == 0) {
//					assert(msdata.m_dTxPowerMw == -1);
//					UL.fMSeMBB << setw(width) << 0; //m_MSData[MSID(i)].m_dTxPowerMw / m_MSData[MSID(i)].m_dTxPowerMwIndex;
//				}
//				else {
//					UL.fMSeMBB << setw(width) << msdata.m_dTxPowerMw / msdata.m_dTxPowerMwIndex;
//				} //UL Per UE Average SINR
//				UL.fMSeMBB << setw(width) << msdata.m_dGeometryDB
//					<< setw(width) << msdata.m_iRank[0]
//					<< setw(width) << msdata.m_iRank[1];
//				UL.fMSeMBB << setw(width) << msdata.m_iReTransTimePerMS[0]
//					<< setw(width) << msdata.m_iReTransTimePerMS[1]
//					<< setw(width) << msdata.m_iReTransTimePerMS[2]
//					<< setw(width) << msdata.m_iReTransTimePerMS[3]
//					<< setw(width) << msdata.m_iReTransTimePerMS[4];
//				for (int j = 0; j < 29; ++j) {//MCS最大29
//					UL.fMSeMBB << setw(width) << msdata.m_dFormatFractionUL[j];
//				}
//				UL.fMSeMBB << setw(width) << msdata.m_dRBCount;
//				UL.fMSeMBB << setw(width) << msdata.m_dRBCount2;
//				UL.fMSeMBB << setw(width) << msdata.m_iRBSupported;
//				UL.fMSeMBB << setw(width) << msdata.m_iSchCount;
//				UL.fMSeMBB << setw(width) << msdata.m_iSchCount2;
//				UL.fMSeMBB << setw(width) << msdata.m_iOccupiedRBS;
//				UL.fMSeMBB << setw(width) << msdata.m_iOccupiedRBS2;
//				//            UL.fMSeMBB << setw(width) << m_MSData_UL[MSID(i)].m_iCompSize;
//				UL.fMSeMBB << setw(width) << msdata.m_iMUcount
//					<< setw(width) << msdata.m_iSUcount;
//				UL.fMSeMBB << setw(width) << msdata.m_iIsO2I; // is indoor
//				UL.fMSeMBB << setw(width) << msdata.m_iIsLOS;
//				UL.fMSeMBB << setw(width) << msdata.m_dShadowfading;
//				//Statistician::Instance().m_MSData[m_ID].m_iIsO2I
//				UL.fMSeMBB << endl;
//				//计算各个MCS等级的使用次数
//				for (int j = 0; j < 29; ++j) { //这个应该用上行可用的MCS等级数代替
//					m_dFormatHitTimes[j] += (msdata.UL.m_iFormat2TrueFalseFrameHitUL[0][j] + msdata.UL.m_iFormat2TrueFalseFrameHitUL[1][j]);
//					m_iTotal += (msdata.UL.m_iFormat2TrueFalseFrameHitUL[0][j] + msdata.UL.m_iFormat2TrueFalseFrameHitUL[1][j]);
//				}
//				//        }
//			}
//
//			for (int i = floor(Parameters::Instance().BASIC.ITotalMSNum * Parameters::Instance().BASIC.type1MSRate + 0.5); i < Parameters::Instance().BASIC.ITotalMSNum; ++i) {
//				MSData_UL& msdata = m_MSData_UL[MSID(i).ToInt()];
//				assert(msdata.m_d2D_Distance != 0);
//				UL.fMSRedCap << i << setw(width) << "(" << (msdata.m_ActiveBTS.GetBSID().ToInt()) << "," << (msdata.m_ActiveBTS.GetIndex()) << ")"
//					<< setw(width) << msdata.m_x
//					<< setw(width) << msdata.m_y
//                    << setw(width) << msdata.m_z
//					<< setw(width) << msdata.UL.m_dAveRateKbpsUL
//					<< setw(width) << msdata.UL.m_dBLERTx[1]
//					<< setw(width) << msdata.UL.m_dBLERTx[2]
//					<< setw(width) << msdata.UL.m_dBLERTx[3]
//					<< setw(width) << msdata.UL.m_dBLERTx[4]
//					<< setw(width) << msdata.m_dLinkLossDB
//					//zhengyi
//					<< setw(width) << msdata.m_dAvePathlossDB
//					<< setw(width) << msdata.m_dPathlossDB;
//				double dAveSINRdB = 0;
//				double dAverPreSINRdB = 0;
//				//            double dAveSINRdB_test = 0;
//				double dul_geometry_db = 0;
//
//				double dAverCQIPreSINRdB = 0; //test
//
//				//test
//				if (msdata.m_dAVE_preCQISINR_Index != 0) {
//					dAverCQIPreSINRdB = L2DB(msdata.m_dAVE_preCQISINR / (msdata.m_dAVE_preCQISINR_Index));
//				}
//				if (msdata.m_dAVE_preSINR_Index != 0) {
//					dAverPreSINRdB = L2DB(msdata.m_dAVE_preSINR / (msdata.m_dAVE_preSINR_Index));
//				}
//				if (msdata.m_dAVE_SINR_Index != 0) {
//					dAveSINRdB = L2DB(msdata.m_dAVE_SINR / (msdata.m_dAVE_SINR_Index));
//				}
//				//上行geometry,m_dul_geoemtry
//				if (msdata.m_dAVE_SINR_Index != 0) {
//					dul_geometry_db = L2DB(msdata.m_dul_geoemtry / (msdata.m_dAVE_SINR_Index));
//				}
//
//				UL.fMSRedCap << setw(width) << dAverCQIPreSINRdB; //test
//				UL.fMSRedCap << setw(width) << msdata.m_dAVE_preCQISINR_Index; //test
//				UL.fMSRedCap << setw(width) << dAverPreSINRdB; //UL Per UE Average preSINR 看看估计的SINR是多少，用来评估是否估计上出了问题
//				UL.fMSRedCap << setw(width) << dAveSINRdB; //UL Per UE Average SINR
//				UL.fMSRedCap << setw(width) << msdata.phr_trigger[0];
//				UL.fMSRedCap << setw(width) << msdata.phr_trigger[1];
//				UL.fMSRedCap << setw(width) << msdata.phr_trigger[2];
//				//            UL.fMSRedCap << setw(width) << dAveSINRdB_test; //zhengyi test comp
//				UL.fMSRedCap << setw(width) << dul_geometry_db //20170217 ul geoemtry
//					<< setw(width) << msdata.m_d2D_Distance//szx
//					<< setw(width) << msdata.m_AODLOSDEG
//					<< setw(width) << msdata.m_dUE2BSTiltDEG
//					<< setw(width) << msdata.m_dAveTxAntennaPatternDB
//					<< setw(width) << msdata.m_dPlusOffsetDEG
//					<< setw(width) << msdata.m_dRxHeight;
//				if (msdata.m_dTxPowerMwIndex == 0) {
//					assert(msdata.m_dTxPowerMw == -1);
//					UL.fMSRedCap << setw(width) << 0; //m_MSData[MSID(i)].m_dTxPowerMw / m_MSData[MSID(i)].m_dTxPowerMwIndex;
//				}
//				else {
//					UL.fMSRedCap << setw(width) << msdata.m_dTxPowerMw / msdata.m_dTxPowerMwIndex;
//				} //UL Per UE Average SINR
//				UL.fMSRedCap << setw(width) << msdata.m_dGeometryDB
//					<< setw(width) << msdata.m_iRank[0]
//					<< setw(width) << msdata.m_iRank[1];
//				UL.fMSRedCap << setw(width) << msdata.m_iReTransTimePerMS[0]
//					<< setw(width) << msdata.m_iReTransTimePerMS[1]
//					<< setw(width) << msdata.m_iReTransTimePerMS[2]
//					<< setw(width) << msdata.m_iReTransTimePerMS[3]
//					<< setw(width) << msdata.m_iReTransTimePerMS[4];
//				for (int j = 0; j < 29; ++j) {//MCS最大29
//					UL.fMSRedCap << setw(width) << msdata.m_dFormatFractionUL[j];
//				}
//				UL.fMSRedCap << setw(width) << msdata.m_dRBCount;
//				UL.fMSRedCap << setw(width) << msdata.m_dRBCount2;
//				UL.fMSRedCap << setw(width) << msdata.m_iRBSupported;
//				UL.fMSRedCap << setw(width) << msdata.m_iSchCount;
//				UL.fMSRedCap << setw(width) << msdata.m_iSchCount2;
//				UL.fMSRedCap << setw(width) << msdata.m_iOccupiedRBS;
//				UL.fMSRedCap << setw(width) << msdata.m_iOccupiedRBS2;
//				//            UL.fMSRedCap << setw(width) << m_MSData_UL[MSID(i)].m_iCompSize;
//				UL.fMSRedCap << setw(width) << msdata.m_iMUcount
//					<< setw(width) << msdata.m_iSUcount;
//				UL.fMSRedCap << setw(width) << msdata.m_iIsO2I; // is indoor
//				UL.fMSRedCap << setw(width) << msdata.m_iIsLOS;
//				UL.fMSRedCap << setw(width) << msdata.m_dShadowfading;
//				//Statistician::Instance().m_MSData[m_ID].m_iIsO2I
//				UL.fMSRedCap << endl;
//				//计算各个MCS等级的使用次数
//				for (int j = 0; j < 29; ++j) { //这个应该用上行可用的MCS等级数代替
//					m_dFormatHitTimes[j] += (msdata.UL.m_iFormat2TrueFalseFrameHitUL[0][j] + msdata.UL.m_iFormat2TrueFalseFrameHitUL[1][j]);
//					m_iTotal += (msdata.UL.m_iFormat2TrueFalseFrameHitUL[0][j] + msdata.UL.m_iFormat2TrueFalseFrameHitUL[1][j]);
//				}
//				//        }
//			}
//		}


		//输出基站的数据
		for (int i = 0; i < Parameters::Instance().BASIC.INumBSs; ++i) {
			for (int j = 0; j < Parameters::Instance().BASIC.IBTSPerBS; ++j) {
				BTSID btsid(i, j);

                double BLER = m_BTSData[btsid.GetTxID()].UL.m_iNewTxErrorNum;
                BLER /= m_BTSData[btsid.GetTxID()].UL.m_iNewTxNum;
                double AceScheMsNumPerSlot = m_BTSData[btsid.GetTxID()].UL.m_iScheMSNum;
                AceScheMsNumPerSlot /= m_BTSData[btsid.GetTxID()].UL.m_iScheTime;

                UL.fBS << setw(width) << "(" << i << "," << j << ")"
                       << setw(width) << m_BTSData[btsid.GetTxID()].m_iNumServMS
                       << setw(width) << m_BTSData[btsid.GetTxID()].UL.m_dThroughputKbps
                       << setw(width) << BLER
                       << setw(width) << AceScheMsNumPerSlot;

//					//zhengyi 之前都没有除以时间上的常数
//					<< setw(width) << L2DB(m_BTSData[btsid.GetTxID()].UL.m_dIoT / m_BTSData[btsid.GetTxID()].UL.m_Iiot_count)
//					<< setw(width) << L2DB(m_BTSData[btsid.GetTxID()].UL.m_dIoT_load / m_BTSData[btsid.GetTxID()].UL.m_Iiot_count_load);
                UL.fBS << endl;
			}
		}
		string s1;
	}
}


// 实现在输出数据前的数据预处理工作的函数

void Statistician::PreProcess() {
	if (Parameters::Instance().BASIC.IDLORUL == Parameters::DL) {
		for (BTSID btsid = BTSID::Begin(); btsid <= BTSID::End(); ++btsid) {
			msbts.push_back(m_BTSData[btsid.GetTxID()].m_iNumServMS);
		}
	}
//	m_dOH_DMRS_Rate = accumulate(m_vdOH_DMRS_Rate.begin(), m_vdOH_DMRS_Rate.end(), 0.0);
//	m_dOH_DMRS_Rate /= static_cast<double> (m_vdOH_DMRS_Rate.size());

//	m_dOH_DMRS_Rate_Old = accumulate(m_vdOH_DMRS_Rate_Old.begin(), m_vdOH_DMRS_Rate_Old.end(), 0.0);
//	m_dOH_DMRS_Rate_Old /= static_cast<double> (m_vdOH_DMRS_Rate_Old.size());

	double dTimeSlot = (Parameters::Instance().BASIC.ISlotPerDrop - Parameters::Instance().BASIC.IWarmUpSlot) * Parameters::Instance().BASIC.INumSnapShot;
	double dTimeSec = dTimeSlot * Parameters::Instance().BASIC.DSlotDuration_ms / 1000;

	//输出下行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::UL) {

//		m_dOH_DMRS_Rate = accumulate(m_vdOH_DMRS_Rate.begin(), m_vdOH_DMRS_Rate.end(), 0.0);
//		m_dOH_DMRS_Rate /= static_cast<double> (m_vdOH_DMRS_Rate.size());

//		m_dOH_DMRS_Rate_Old = accumulate(m_vdOH_DMRS_Rate_Old.begin(), m_vdOH_DMRS_Rate_Old.end(), 0.0);
//		m_dOH_DMRS_Rate_Old /= static_cast<double> (m_vdOH_DMRS_Rate_Old.size());


		//20200506BYLYC
		//仿真输出的原始吞吐量到频谱效率的转换系数，用于计算开销折算前的20MHz基准带宽频谱效率(不考虑大带宽增益)
		double dRawTP2SE_20MHz_withoutOverhead = 1.0 / Parameters::Instance().BASIC.DBandWidthKHz
			/ Parameters::Instance().SIM.FrameStructure4Sim.Calc_DL_Rate();
		//            / (1.0 - Parameters::Instance().BASIC.DOverHead_LTE);

		//仿真输出的原始吞吐量到频谱效率的转换系数，用于计算开销折算后统计带宽的频谱效率
		double dRawTP2SE_StatBandWidth_withOverhead = dRawTP2SE_20MHz_withoutOverhead
			* Parameters::Instance().BASIC.DGuardBand *
			(1 - Parameters::Instance().BASIC.DTotalFixedOH - m_dOH_DMRS_Rate_Old);


		//预处理移动台的数据
		for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
			MSData_DL& msdata = m_MSData_DL[msid.ToInt()];

			// 计算第1/2/3/4次传输后的残留误帧率
			for (int iSendNum = 1; iSendNum <= Parameters::Instance().SIM.DL.IHARQMaxTransNum; ++iSendNum) {
				int iBlockSum = 0;
				for (int i = 1; i <= iSendNum; ++i) {
					iBlockSum += msdata.DL.m_iCorrectBlockHit[i];
				}
				iBlockSum += msdata.DL.m_iCorruptBlockHit[iSendNum];
				if (iBlockSum == 0) {
					msdata.DL.m_dBLERTx[iSendNum] = 0;
				}
				else {
					msdata.DL.m_dBLERTx[iSendNum] = msdata.DL.m_iCorruptBlockHit[iSendNum] / double(iBlockSum);
				}
			}

			msdata.DL.m_dAveSINR /= msdata.DL.m_iSINRHit;
			if (msdata.DL.m_iCRSSINRHit == 0) {
				msdata.DL.m_dCRSSINR = 1000000;
			}
			else {
				msdata.DL.m_dCRSSINR /= msdata.DL.m_iCRSSINRHit;
			}
			msdata.DL.m_dAvePostSINR /= msdata.DL.m_iPostSINRHit;
			msdata.DL.m_dLogAvePostSINR /= msdata.DL.m_iPostSINRHit;
			msdata.DL.m_dAveRateKbps /= dTimeSec;

		}

		// 预处理基站的数据
		for (int i = 0; i < Parameters::Instance().BASIC.INumBSs; ++i) {
			for (int j = 0; j < Parameters::Instance().BASIC.IBTSPerBS; ++j) {

				BTSID btsid(i, j);
				///计算基站的下行平均吞吐量
				m_BTSData[btsid.GetTxID()].DL.m_dThroughputKbps /= dTimeSec;

				double FrameStructProb =
					1.0 * Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot / (1.0 * Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot);
				//                m_BTSData[btsid].DL.m_dThroughputKbps /= FrameStructProb;

				double RawTP = m_BTSData[btsid.GetTxID()].DL.m_dThroughputKbps / dTimeSec;

				m_BTSData[btsid.GetTxID()].DL.m_dAveSE_20MHz_withoutOverhead_bps_Per_Hz = RawTP * dRawTP2SE_20MHz_withoutOverhead;
				//                m_BTSData[btsid].DL.m_dThroughputKbps = m_BTSData[btsid].DL.m_dAveSE_20MHz_withoutOverhead_bps_Per_Hz * Parameters::Instance().BASIC.DBandWidthKHz;

				m_BTSData[btsid.GetTxID()].DL.m_dAveSE_StatBandWidth_withOverhead_bps_Per_Hz = RawTP * dRawTP2SE_StatBandWidth_withOverhead;
				m_BTSData[btsid.GetTxID()].DL.m_dThroughputKbps_Converted = m_BTSData[btsid.GetTxID()].DL.m_dAveSE_StatBandWidth_withOverhead_bps_Per_Hz * Parameters::Instance().BASIC.DSystemBandWidthKHz;
			}
		}
	}

	//输出上行信息
	if (Parameters::Instance().BASIC.IDLORUL != Parameters::DL) {
		//预处理移动台的数据
		//        for (MSID i(0); i < MSID(m_MSData_UL.size()); ++i) {
		for (MSID i = MSID::Begin(); i <= MSID::End(); ++i) {
			//            int correctdl = 0;
			//            int faildl = 0;
			int Correct_UL = 0; //上行
			int Fail_UL = 0;
			for (int j = 0; j < 29; ++j) {
				//                int counttdl = m_MSData_UL[i].DL.m_iFormat2TrueFalseFrameHitDL[0][j];
				//                int countfdl = m_MSData_UL[i].DL.m_iFormat2TrueFalseFrameHitDL[1][j];
				int Ture_Count_UL = m_MSData_UL[i.ToInt()].UL.m_iFormat2TrueFalseFrameHitUL[0][j];
				int False_Count_UL = m_MSData_UL[i.ToInt()].UL.m_iFormat2TrueFalseFrameHitUL[1][j];
				//累计下行正确传输的帧数
				//                correctdl += counttdl;
				//累计下行错误传输的帧数
				//                faildl += countfdl;
				//累计上行正确传输的帧数
				Correct_UL += Ture_Count_UL;
				//累计上行错误传输的帧数
				Fail_UL += False_Count_UL;
			}



			//计算每一种传输格式的帧被使用的概率
			for (int j = 0; j < 29; ++j) {
				//                int counttdl = m_MSData_UL[i].DL.m_iFormat2TrueFalseFrameHitDL[0][j];
				//                int countfdl = m_MSData_UL[i].DL.m_iFormat2TrueFalseFrameHitDL[1][j];

				int Ture_Count_UL = m_MSData_UL[i.ToInt()].UL.m_iFormat2TrueFalseFrameHitUL[0][j];
				int False_Count_UL = m_MSData_UL[i.ToInt()].UL.m_iFormat2TrueFalseFrameHitUL[1][j];
				//                if (correctdl + faildl == 0) {
				//                    m_MSData_UL[i].m_dFormatFractionDL[j] = 0;
				//                } else {
				//                    m_MSData_UL[i].m_dFormatFractionDL[j] = double(counttdl + countfdl) / double(correctdl + faildl);
				//                }
				if (Correct_UL + Fail_UL == 0) {
					m_MSData_UL[i.ToInt()].m_dFormatFractionUL[j] = 0;
				}
				else {
					//m_MSData_UL[i].m_dFormatFractionUL[j] = static_cast<double> (Ture_Count_UL + False_Count_UL) / static_cast<double> (Correct_UL + Fail_UL);
					m_MSData_UL[i.ToInt()].m_dFormatFractionUL[j] = static_cast<double> (Ture_Count_UL + False_Count_UL);
				}
			}
			/*            int packetnumdl = accumulate(m_MSData_UL[i].m_iPacketResultDL, m_MSData_UL[i].m_iPacketResultDL + 4, 0);
			//            int packetnumul = accumulate(m_MSData_UL[i].m_iPacketResultUL, m_MSData_UL[i].m_iPacketResultUL + 4, 0);
			//            if (packetnumdl == 0) {
			//                m_MSData_UL[i].m_dPDelayBelow50msDL = 0;
			//                m_MSData_UL[i].m_dPDelayUp50msDL = 0;
			//                m_MSData_UL[i].m_dPDiscardDL = 0;
			//            } else {
			//                m_MSData_UL[i].m_dPDelayBelow50msDL = m_MSData_UL[i].m_iPacketResultDL[0] / double(packetnumdl);
			//                m_MSData_UL[i].m_dPDelayUp50msDL = m_MSData_UL[i].m_iPacketResultDL[1] / double(packetnumdl);
			//                m_MSData_UL[i].m_dPDiscardDL = m_MSData_UL[i].m_iPacketResultDL[3] / double(packetnumdl);
			//            }
			//            if (packetnumul == 0) {
			//                m_MSData_UL[i].m_dPDelayBelow50msUL = 0;
			//                m_MSData_UL[i].m_dPDelayUp50msUL = 0;
			//                m_MSData_UL[i].m_dPDiscardUL = 0;
			//
			//            } else {
			//                m_MSData_UL[i].m_dPDelayBelow50msUL = m_MSData_UL[i].m_iPacketResultUL[0] / double(packetnumul);
			//                m_MSData_UL[i].m_dPDelayUp50msUL = m_MSData_UL[i].m_iPacketResultUL[1] / double(packetnumul);
			//                m_MSData_UL[i].m_dPDiscardUL = m_MSData_UL[i].m_iPacketResultUL[3] / double(packetnumul);
			//            }
			//        m_MSData[i].m_dAveRateKbpsDL /= dTimeSec;
			//            m_MSData_UL[i].DL.m_dAveRateKbps /= dTimeSec;
			 */
			m_MSData_UL[i.ToInt()].UL.m_dAveRateKbpsUL /= dTimeSec;
			// assert(m_MSData[i].m_dRBCount<4610);  //need check under 20MHz
			assert(m_MSData_UL[i.ToInt()].m_d2D_Distance != 0);
			            if (m_MSData_UL[i.ToInt()].m_iSchCount > 0) {
			                m_MSData_UL[i.ToInt()].m_dRBCount /= m_MSData_UL[i.ToInt()].m_iSchCount;
//			                m_MSData_UL[i].m_iOccupiedRBS /= m_MSData_UL[i].m_iSchCount;
			            } else {
			                m_MSData_UL[i.ToInt()].m_dRBCount = 0;
//			                m_MSData_UL[i].m_iOccupiedRBS = 0;
			            }
//			            if (m_MSData_UL[i].m_iSchCount2 > 0) {
//			                m_MSData_UL[i].m_dRBCount2 /= m_MSData_UL[i].m_iSchCount2;
//			                m_MSData_UL[i].m_iOccupiedRBS2 /= m_MSData_UL[i].m_iSchCount2;
//			            } else {
//			                m_MSData_UL[i].m_dRBCount2 = 0;
//			                m_MSData_UL[i].m_iOccupiedRBS2 = 0;
//			            }

			 //for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid) {
			MSData_UL& msdata = m_MSData_UL[i.ToInt()];

			// 计算第1/2/3/4次传输后的残留误帧率
			for (int iSendNum = 1; iSendNum <= Parameters::Instance().SIM_UL.UL.IHARQMaxTransNum; ++iSendNum) {
				int iBlockSum = 0;
				for (int s = 1; s <= iSendNum; ++s) {
					iBlockSum += msdata.UL.m_iCorrectBlockHit[s];
				}
				iBlockSum += msdata.UL.m_iCorruptBlockHit[iSendNum];
				if (iBlockSum == 0) {
					msdata.UL.m_dBLERTx[iSendNum] = 0;
				}
				else {
					msdata.UL.m_dBLERTx[iSendNum] = msdata.UL.m_iCorruptBlockHit[iSendNum] / double(iBlockSum);
				}
			}
			//}
		}

		// 预处理基站的数据
		for (int i = 0; i < Parameters::Instance().BASIC.INumBSs; ++i) {
			for (int j = 0; j < Parameters::Instance().BASIC.IBTSPerBS; ++j) {
				BTSID btsid(i, j);
				///计算基站的下行平均吞吐量
				m_BTSData[btsid.GetTxID()].DL.m_dThroughputKbps /= dTimeSec;
				///计算基站的上行平均吞吐量
				m_BTSData[btsid.GetTxID()].UL.m_dThroughputKbps /= dTimeSec;
			}
		}
	}
}

Statistician& Statistician::Instance() {
	static Statistician& m_S = *(new Statistician);
	return m_S;
}
