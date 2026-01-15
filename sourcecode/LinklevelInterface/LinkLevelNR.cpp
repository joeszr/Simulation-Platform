/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "LinkLevelNR.h"
#include <algorithm>
#include"../NetworkDrive/Clock.h"

///@brief LinkLevelNR的构造函数实现
LinkLevelNR::LinkLevelNR(int tableindex):m_RBIR2BLERDL(tableindex){

    if(tableindex == 0 && Parameters::Instance().DaHuaWu.bDl256QamSwitch == false)
    {
        vMaxCBSizeBit = {256, 344,424,568,696,872,1032,1224,1384,1544,1544,1736,2024,2280,2536,2856,3112,3112,3240,3624,4008,4264,4584,4968,5352,5736,5992,6200};
    }
    else
    {
        vMaxCBSizeBit.resize(28, 6912);
    }

    // m_RBIR2BLERDL=RBIR2BLERNR(tableindex);
    vector<double>MCS2SINR = {-6.38, -4.37, -2.07, -0.02, 1.91, 3.75, 4.65, 5.50, 6.54, 7.50, 8.12, 9.20, 10.09, 10.97, 12.06, 13.00, 14.04, 15.00, 15.97, 17.04, 18.01, 18.51, 19.66, 20.58, 21.71, 22.62, 23.96, 25.17};
    MCS2SINRTable.swap(MCS2SINR);

    if(tableindex == 0)
    {
        //DL
        if(Parameters::Instance().DaHuaWu.bDl256QamSwitch)
        {
            filename = const_cast<char*>("./inputfiles/PDSCH_256QAM_RB_TBSize_Table.txt");
        }
        else
        {
            filename = const_cast<char*>("./inputfiles/PDSCH_64QAM_RB_TBSize_Table.txt");
        }
        std::fstream file(filename);
        MaxLayNum = Parameters::Instance().MIMO_CTRL.IMaxRankNum;
        assert(MaxLayNum <= 4);
        RENumTable = {30, 42, 48, 54, 60, 66, 72, 78, 84, 90, 96, 102, 108, 114, 120, 126, 132, 138, 144, 150, 156};
        RELevelNum = 21;
        assert(RENumTable.size() == RELevelNum);
        MaxMCSLevel = 28;
        MaxRBNum = 273;
        Layer_RE_MCS_RBNum2TBSize.resize(MaxLayNum, vector<itpp::imat>(RELevelNum, itpp::imat(MaxMCSLevel, MaxRBNum)));
        int buffer;
        for(int l = 0; l < MaxLayNum; l++){
            for(int s = 0 ; s < RELevelNum; s++){
                for(int m = 0 ; m < MaxMCSLevel; m++){
                    for(int r = 0; r < MaxRBNum; r++){
                        file>>buffer;
                        Layer_RE_MCS_RBNum2TBSize[l][s](m, r) = buffer;
                    }
                }
            }
        }
    }
    else
    {
        //UL
        filename = const_cast<char*>("./inputfiles/PUSCH_64QAM_RB_TBSize_Table.txt");
        std::fstream file(filename);
        MaxLayNum = Parameters::Instance().SIM_UL.UL.iMaxRBFindNum;
        assert(MaxLayNum <= 2);
        RENumTable = {42, 54, 66, 78, 84, 90, 96, 102, 108, 114, 120, 126, 132, 138, 144, 150, 156};
        RELevelNum = 17;
        assert(RENumTable.size() == RELevelNum);
        MaxMCSLevel = 28;
        MaxRBNum = 273;
        Layer_RE_MCS_RBNum2TBSize.resize(MaxLayNum, vector<itpp::imat>(RELevelNum, itpp::imat(MaxMCSLevel, MaxRBNum)));
        int buffer;
        for(int l = 0; l < MaxLayNum; l++){
            for(int s = 0 ; s < RELevelNum; s++){
                for(int m = 0 ; m < MaxMCSLevel; m++){
                    for(int r = 0; r < MaxRBNum; r++){
                        file>>buffer;
                        Layer_RE_MCS_RBNum2TBSize[l][s](m, r) = buffer;
                    }
                }
            }
        }
    }

}

///MCS到调制阶数的映射
///@param _iMCS  MCS标识
///@param _iDLorUL 上下行标识
///@return 调制阶数

int LinkLevelNR::MCS2ModLevel(int _iMCSIndex, int _iDLorUL) {
    //    assert(_iMCSIndex >= 0 && _iMCSIndex <= Parameters::NRMCSNumDL - 1);
    //    assert(_iMCSIndex >= 0 && _iMCSIndex <= Parameters::Instance().BASIC.NRMCSNumDL - 1);
    //    assert(_iDLorUL == 1);
    if (_iDLorUL == Parameters::DL)
    {
        if(Parameters::Instance().DaHuaWu.bDl256QamSwitch) {
            int ModulationOrderDL[28] = {2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, 8, 8, 8};
            return ModulationOrderDL[_iMCSIndex];
        }
        else
        {
            int ModulationOrderDL[28] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
            return ModulationOrderDL[_iMCSIndex];
        }
    }
    else if (_iDLorUL == Parameters::UL) {
        int iNRMCSNumUL = 28;
        iNRMCSNumUL = Parameters::Instance().BASIC.INRMCSNumUL;
        int ModulationOrder = 0;
        if (Parameters::Instance().BASIC.ISINR2BLERTable == 1) {
            int ModulationOrderUL[iNRMCSNumUL] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
            ModulationOrder = ModulationOrderUL[_iMCSIndex];
        } else if (Parameters::Instance().BASIC.ISINR2BLERTable == 2) {
            int ModulationOrderUL[iNRMCSNumUL] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6};
            ModulationOrder = ModulationOrderUL[_iMCSIndex];
        } else {
            assert(false);
        }
        return ModulationOrder;
    } 
    else if(_iDLorUL == Parameters::ULandDL){
        int iTime=Clock::Instance().GetTimeSlot();
        if(DownOrUpLink(iTime)==0||DownOrUpLink(iTime)==2){
            if(Parameters::Instance().DaHuaWu.bDl256QamSwitch) {
                int ModulationOrderDL[28] = {2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, 8, 8, 8};
                return ModulationOrderDL[_iMCSIndex];
            }
            else
            {
                int ModulationOrderDL[28] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
                return ModulationOrderDL[_iMCSIndex];
            }
        }
        else if(DownOrUpLink(iTime)==1){
            int iNRMCSNumUL = 28;
            iNRMCSNumUL = Parameters::Instance().BASIC.INRMCSNumUL;
            int ModulationOrder = 0;
            if (Parameters::Instance().BASIC.ISINR2BLERTable == 1) {
                int ModulationOrderUL[iNRMCSNumUL] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
                ModulationOrder = ModulationOrderUL[_iMCSIndex];
            } else if (Parameters::Instance().BASIC.ISINR2BLERTable == 2) {
                int ModulationOrderUL[iNRMCSNumUL] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6};
                ModulationOrder = ModulationOrderUL[_iMCSIndex];
            } else {
                assert(false);
            }
            return ModulationOrder;
        }
    }
    else {
        assert(false);
        return 0;
    }
    assert(false);
    return -1;
}

double LinkLevelNR::MCS2Rate(int _iMCSIndex) {
    //    assert(_iMCSIndex >= 0 && _iMCSIndex <= Parameters::NRMCSNumDL - 1);
    //    assert(_iMCSIndex >= 0 && _iMCSIndex <= Parameters::Instance().BASIC.NRMCSNumDL - 1);
    if (Parameters::Instance().BASIC.IDLORUL == Parameters::DL) {
        if(Parameters::Instance().DaHuaWu.bDl256QamSwitch) {
            double RateDL[28] = {0.12, 0.19, 0.30, 0.44, 0.59, 0.37, 0.42, 0.48, 0.54, 0.60, 0.64, 0.46, 0.50, 0.55,
                                 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.67, 0.69, 0.74, 0.78, 0.82, 0.86, 0.90, 0.93};
            return RateDL[_iMCSIndex];
        }
        else
        {
            double RateDL[28] = {0.12, 0.15, 0.19, 0.25, 0.30, 0.37, 0.44, 0.51, 0.59, 0.66, 0.33, 0.37, 0.42, 0.48,
                                 0.54, 0.60, 0.64, 0.43, 0.46, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.89};
            return RateDL[_iMCSIndex];
        }
    }
    else if (Parameters::Instance().BASIC.IDLORUL == Parameters::UL) {
        int iNRMCSNumUL = 28;
        iNRMCSNumUL = Parameters::Instance().BASIC.INRMCSNumUL;
        double Rate = 0;
        if (Parameters::Instance().BASIC.ISINR2BLERTable == 1) {
            double RateUL[iNRMCSNumUL] = {0.12,0.15,0.19,0.25, 0.30,0.37,0.44,0.51,0.59,0.66,0.33,0.37,0.42,0.48,0.54,0.60,0.64,0.46,0.50,0.55,0.60,0.65,0.70,0.75,0.80,0.85,0.89,0.93};
            Rate = RateUL[_iMCSIndex];

        } else if (Parameters::Instance().BASIC.ISINR2BLERTable == 2) {
            double RateUL[iNRMCSNumUL] = {0.03,0.04,0.06,0.08,0.10,0.12,0.15,0.19,0.25,0.30,0.37,0.44,0.51,0.59,0.66,0.37,0.42,0.49,0.54,0.60,0.64,0.68,0.75,0.55,0.60,0.65,0.75};
            Rate = RateUL[_iMCSIndex];
        } else {
            assert(false);
        }
        return Rate;

    } 
    else if(Parameters::Instance().BASIC.IDLORUL == Parameters::ULandDL)
    {
        int iTime=Clock::Instance().GetTimeSlot();
        if(DownOrUpLink(iTime)==0||DownOrUpLink(iTime)==2)
        {
            if(Parameters::Instance().DaHuaWu.bDl256QamSwitch)
            {
                double RateDL[28] = {0.12, 0.19, 0.30, 0.44, 0.59, 0.37, 0.42, 0.48, 0.54, 0.60, 0.64, 0.46, 0.50, 0.55,
                                     0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.67, 0.69, 0.74, 0.78, 0.82, 0.86, 0.90, 0.93};
                return RateDL[_iMCSIndex];
            }
            else
            {
                double RateDL[28] = {0.12, 0.15, 0.19, 0.25, 0.30, 0.37, 0.44, 0.51, 0.59, 0.66, 0.33, 0.37, 0.42, 0.48,
                                     0.54, 0.60, 0.64, 0.43, 0.46, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.89};
                return RateDL[_iMCSIndex];
            }
        }
        else if(DownOrUpLink(iTime)==1)
        {
            double Rate=0;
            int iNRMCSNumUL = 28;
            iNRMCSNumUL = Parameters::Instance().BASIC.INRMCSNumUL;
            if (Parameters::Instance().BASIC.ISINR2BLERTable == 1)
            {
                double RateUL[iNRMCSNumUL] = {0.12, 0.15, 0.19, 0.25, 0.30, 0.37, 0.44, 0.51, 0.59, 0.66, 0.33, 0.37, 0.42, 0.48, 0.54, 0.60, 0.64, 0.46, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.89, 0.93};
                Rate = RateUL[_iMCSIndex];

            } else if (Parameters::Instance().BASIC.ISINR2BLERTable == 2)
            {
                double RateUL[iNRMCSNumUL] = {0.03, 0.04, 0.06, 0.08, 0.10, 0.12, 0.15, 0.19, 0.25, 0.30, 0.37, 0.44, 0.51, 0.59, 0.66, 0.37, 0.42, 0.49, 0.54, 0.60, 0.64, 0.68, 0.75, 0.55, 0.60, 0.65, 0.75};
                Rate = RateUL[_iMCSIndex];
            } else
            {
                assert(false);
            }
            return Rate;
        }
    }
    else
    {
        assert(false);
        return 0.0;
    }
    assert(false);
    return 0;
}
//MCS到CQI的映射，根据38214， 用于下行
int LinkLevelNR::MCS2CQI(int _iMCSIndex)
{
    if(Parameters::Instance().DaHuaWu.bDl256QamSwitch)
    {
        static int MCS2CQI[28] = {1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 12, 12, 13, 13, 14, 14, 15};
        return MCS2CQI[_iMCSIndex];
    }
    else
    {
        static int MCS2CQI[28] = {2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7, 8, 8, 9, 9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14};
        return MCS2CQI[_iMCSIndex];
    }
}

///@brief 根据SINR查询链路级数据得到BLER
///
///@return  BLER值

double LinkLevelNR::BLER(const vector<double>& _vSINR, int _iMCSIndex, double _dTBSizeKbit) {
    if (_iMCSIndex == -1) {
        return 1.0;
    }
    //    assert(_iMCSIndex >= 0 && _iMCSIndex <= Parameters::NRMCSNumDL - 1);
    assert(_iMCSIndex >= 0 && _iMCSIndex <= Parameters::Instance().BASIC.NRMCSNumDL - 1);

    int iModulation = MCS2ModLevel(_iMCSIndex, Parameters::Instance().BASIC.IDLORUL);
    double dRBIR = MIESM(_vSINR, iModulation);

    int iTBSizeBit = _dTBSizeKbit * 1000;
    int MaxCBSizeBit = vMaxCBSizeBit[_iMCSIndex];
    double dCorrectRate = 1;
    int quotient = iTBSizeBit/MaxCBSizeBit;
    int remainder = iTBSizeBit % MaxCBSizeBit;

    if(quotient > 0)
    {
        if(remainder > 0)
        {
            quotient = quotient+1;
        }
        dCorrectRate *= pow((1 - m_RBIR2BLERDL.FER(_iMCSIndex, MaxCBSizeBit, dRBIR)), quotient);
    }
    else if(remainder > 0)
    {
        dCorrectRate *= (1 - m_RBIR2BLERDL.FER(_iMCSIndex, remainder, dRBIR));
    }
    return 1 - dCorrectRate;

//    return m_RBIR2BLERDL.FER(_iMCSIndex, static_cast<int> (_dTBSizeKbit * 1000), dRBIR);
}
double LinkLevelNR::BLERmat(const vector<mat>& _vSINR, int _iMCSIndex, double _dTBSizeKbit) {
    if (_iMCSIndex == -1) {
        return 1.0;
    }
    //    assert(_iMCSIndex >= 0 && _iMCSIndex <= Parameters::NRMCSNumDL - 1);
    assert(_iMCSIndex >= 0 && _iMCSIndex <= Parameters::Instance().BASIC.NRMCSNumDL - 1);

//    int iModulation = MCS2ModLevel(_iMCSIndex, Parameters::Instance().BASIC.IDLORUL);
    // lc
    // 因为这个BLER函数只在下行的调度中用到过，因此直接去掉后面一个参数，采用默认的下行参数
    int iModulation = MCS2ModLevel(_iMCSIndex,Parameters::Instance().BASIC.IDLORUL);
    
    double dRBIR = MIESM(_vSINR, iModulation);

    int iTBSizeBit = _dTBSizeKbit * 1000;
    int MaxCBSizeBit = vMaxCBSizeBit[_iMCSIndex];
    double dCorrectRate = 1;
    int quotient = iTBSizeBit/MaxCBSizeBit;
    int remainder = iTBSizeBit % MaxCBSizeBit;

    if(quotient > 0)
    {
        if(remainder > 0)
        {
            quotient = quotient+1;
        }
        dCorrectRate *= pow((1 - m_RBIR2BLERDL.FER(_iMCSIndex, MaxCBSizeBit, dRBIR)), quotient);
    }
    else if(remainder > 0)
    {
        dCorrectRate *= (1 - m_RBIR2BLERDL.FER(_iMCSIndex, remainder, dRBIR));
    }
    return 1 - dCorrectRate;

//    return m_RBIR2BLERDL.FER(_iMCSIndex, static_cast<int> (_dTBSizeKbit * 1000), dRBIR);
}

//int LinkLevelNR::GetRENum_SingleSB(int _iusedRBnum) {
////    int iMUNumber = static_cast<int> (size());
//    int iLayerNum = 0;
////    for (int i = 0; i < iMUNumber; ++i) {
////        MSID& msid = (*this)[i];
////        iLayerNum += (m_mMSID2Rank[msid] + 1);
////    }
////    //todo PDCCH
//    int iSymbolNumPerSlot = 14;
//    int iRENumPerPRB_DMRS = (iLayerNum + 1) / 2 * 4;
//
//    int iRENum_temp = Parameters::Instance().BASIC.IRBSize * iSymbolNumPerSlot - iRENumPerPRB_DMRS;
//    int iRENum_SingleSB = min(iRENum_temp, 156) * _iusedRBnum;
//    return iRENum_SingleSB;
//    
//    
//}

int LinkLevelNR::GetRENum(int _iRBnum, int _iLayerNum) {
    int iRENumPerPRB = Parameters::Instance().BASIC.DDLRENumPerRBforData;
    int iRENum_SingleSB =(int)(iRENumPerPRB * _iRBnum);
    return iRENum_SingleSB;
}
int LinkLevelNR::GetRENumInRange(int RBbegin, int RBend)
{
    int RENum = -1;
    if(RBbegin >= Parameters::Instance().DaHuaWu.iOccupiedRbNum)
    {
        RENum = (RBend - RBbegin + 1) * Parameters::Instance().BASIC.DDLRENumPerRBforDataWithoutPDCCH;
    }
    else if(RBend < Parameters::Instance().DaHuaWu.iOccupiedRbNum)
    {
        RENum = (RBend - RBbegin + 1) * Parameters::Instance().BASIC.DDLRENumPerRBforData;
    }
    else
    {
        RENum = Parameters::Instance().BASIC.DDLRENumPerRBforData * (Parameters::Instance().DaHuaWu.iOccupiedRbNum - RBbegin)
                + Parameters::Instance().BASIC.DDLRENumPerRBforDataWithoutPDCCH * (RBend - Parameters::Instance().DaHuaWu.iOccupiedRbNum + 1);
    }
    return RENum;
}

double LinkLevelNR::MCS2TBSKBit(int _iMCSIndex, int _iRENum_AllSB) {
    double dTBSizeKbit = 0.001 * MCS2TBS(_iMCSIndex, _iRENum_AllSB, 1);

    return dTBSizeKbit;
}

///@brief 使用预测的信噪比预测出最恰当的MCS
///
///在满足目标BLER门限的条件下，选择等效传输速率最高的MCS
///@param _vSINR 预测的SINR的线性值，支持double,vector<double>,以及vector<mat>类型，具体见下面的两个重载函数
///@return MCS值

int LinkLevelNR::SINR2MCS(const vector<double>& _vSINR) {
    int iMCS = -1;
    int iSCNum = static_cast<int> (_vSINR.size());
    int iRBNum = iSCNum * Parameters::Instance().LINK_CTRL.IFrequencySpace / Parameters::Instance().BASIC.IRBSize;
    //    for (int i = Parameters::NRMCSNumDL - 1; i >= 0; --i) {
    for (int i = Parameters::Instance().BASIC.NRMCSNumDL - 1; i >= 0; --i) {
        double dTBSKbit = GetTBSizeKBit_woDMRS(i, iRBNum);
        double dBLER = BLER(_vSINR, i, dTBSKbit);
        if (dBLER < Parameters::Instance().SIM.DL.DBlerTarget) {
            iMCS = i;
            break;
        }
    }
    return max(iMCS,0);
}

///上面int LinkLevelInterface::SINR2MCS(double _dSINR)的重载函数

imat LinkLevelNR::SINR2MCS(const vector<mat>& _vSINR) {
    int iSize = static_cast<int> (_vSINR.size());
    assert(iSize > 0);
    int irow = _vSINR[0].rows();
    int icol = _vSINR[0].cols();
    imat mMCS(irow, icol);
    mMCS.zeros();
    for (int ir = 0; ir < irow; ++ir) {
        for (int ic = 0; ic < icol; ++ic) {
            vector<double> vSINR;
            for (int jj = 0; jj < iSize; jj++) {
                vSINR.push_back(_vSINR[jj](ir, ic));
            }
            mMCS(ir, ic) = SINR2MCS(vSINR);
        }
    }
    return mMCS;
}

///根据预测的MCS，映射到预测的信噪比
///@brief 查询MCS到目标SINR,与RB个数有关
///@param _iMCSIndex  MCS标识
///@param _iRBNum RB个数
///@return 目标SINR的线性值

double LinkLevelNR::MCS2SINR(int _iMCSIndex, int _iRBNum, double _dTBSizeKbit) {
    if (_iMCSIndex == -1) {
        return DB2L(-10.0);
    }
    //    assert(_iMCSIndex >= 0 && _iMCSIndex <= Parameters::NRMCSNumDL - 1);
    assert(_iMCSIndex >= 0 && _iMCSIndex <= Parameters::Instance().BASIC.NRMCSNumDL - 1);
    double dSINR = CalculateMCS2SINR(_iMCSIndex, _iRBNum, _dTBSizeKbit);
    return dSINR;
}
double LinkLevelNR::SINRCombineUL2(const vector< mat >& _vSINR) {

    int iSINRSize = static_cast<int> (_vSINR.size());
    assert(iSINRSize > 0);
    
    int iMaxRank = _vSINR[0].rows();
    for(int i = 0; i<iSINRSize; ++i){
        if(iMaxRank < _vSINR[i].rows()){
            iMaxRank = _vSINR[i].rows();
        }
    }
    
    int irow = iMaxRank;
    int icol = _vSINR[0].cols();
    
    mat mSINR(irow,icol);
    for (int row = 0; row < irow; ++row){
        for(int col = 0; col < icol; ++col){
           vector <double> vSINR;
           for (int i = 0; i < iSINRSize; ++i) {
               if(_vSINR[i].rows()-1< row){
                   continue;
               }
               vSINR.push_back(_vSINR[i](row,col)); 
           }
           
           mSINR(row,col)=SINRCombineUL(vSINR);
        }
    }

    double dResult = 0;

    for(int i = 0; i< irow; i++){
        dResult += mSINR(i,0);
    }
    dResult /= irow;
    return dResult;
      
}


mat LinkLevelNR::MCS2SINR(imat _iMCSIndex, int _iRBNum, double _dTBSizeKbit) {
    int irow = _iMCSIndex.rows();
    int icol = _iMCSIndex.cols();
    mat mSINR(irow, icol);
    for (int ir = 0; ir < irow; ++ir) {
        for (int ic = 0; ic < icol; ++ic) {
            mSINR(ir, ic) = MCS2SINR(_iMCSIndex(ir, ic), _iRBNum, _dTBSizeKbit);
        }
    }
    return mSINR;
}

///根据MCS等级和RB数计算SINR门限
///@param _iMCSIndex  MCS等级
///@param _iRBNum RB个数
///@return 目标SINR的线性值

double LinkLevelNR::CalculateMCS2SINR(int _iMCSIndex, int _iRBNum, double _dTBSizeKbit) {
    vector<double> MIESMSINR = m_vMIESMSINR;
    if (MCS2ModLevel(_iMCSIndex,Parameters::Instance().BASIC.IDLORUL) == 8)
        MIESMSINR = m_vMIESMSINR_256QAM;

    int iSize = MIESMSINR.size();
    double dSINRADB = MIESMSINR[0];
    double dSINRBDB = MIESMSINR[iSize - 1];
    double dSINRCDB = 0.5 * (dSINRADB + dSINRBDB);
    bool bFlag = true;
    double dBLER = 0;
    do {
        vector<double> vSINR;
        vSINR.resize(_iRBNum * Parameters::Instance().BASIC.IRBSize / Parameters::Instance().LINK_CTRL.IFrequencySpace, DB2L(dSINRCDB));
        dBLER = BLER(vSINR, _iMCSIndex, _dTBSizeKbit);
        if (dBLER < Parameters::Instance().SIM.DL.DBlerTarget) {
            dSINRBDB = dSINRCDB;
        } else {
            dSINRADB = dSINRCDB;
        }
        dSINRCDB = 0.5 * (dSINRADB + dSINRBDB);
        bFlag = (abs(dBLER - Parameters::Instance().SIM.DL.DBlerTarget) > 0.00001);
    } while (bFlag);
    return DB2L(dSINRCDB + 0.1);
}

double LinkLevelNR::ComputePostSINRDB(const vector<double>& _vSINR, int _iMCS) {
    _iMCS = max(_iMCS, 0);
    int iModLevel = MCS2ModLevel(_iMCS, Parameters::Instance().BASIC.IDLORUL);
    double dRBIR = MIESM(_vSINR, iModLevel);
    double dSI = dRBIR * iModLevel;
    double dSINRDB = SI2SINRDB(dSI, iModLevel);
    return dSINRDB;
}

double LinkLevelNR::GetTBSizeKBit_woDMRS(imat _mMCS, int _iRBNum) {
    double dResult = 0.0;
    for (int row = 0; row < _mMCS.rows(); ++row) {
        for (int col = 0; col < _mMCS.cols(); ++col) {
            dResult += GetTBSizeKBit_woDMRS(_mMCS(row, col), _iRBNum);
        }
    }
    return dResult;
}

double LinkLevelNR::GetTBSizeKBit_woDMRS(int _iMCS, int _iRBNum) {
//    int iRENumPerPRB;
//    if(Parameters::Instance().BASIC.RRC_Config_On == 1){
//        iRENumPerPRB = _mCalcRENum.CalculateRENum();
//    }else{
//        iRENumPerPRB = Parameters::Instance().BASIC.DDLRENumPerRBforData;
//    }
    int iRENumPerPRB = Parameters::Instance().BASIC.DDLRENumPerRBforData;
    int iRENum = (int)(iRENumPerPRB * _iRBNum);

    return MCS2TBSKBit(_iMCS, iRENum);
}

double LinkLevelNR::SINRCombineUL(const vector<double>& _vSINR) {

    double dSINRtemp = 0.0;
    int iSINRSize = static_cast<int> (_vSINR.size());
    for (int i = 0; i < iSINRSize; ++i) {

        dSINRtemp += _vSINR[i] / (_vSINR[i] + 1) / iSINRSize;
    }
    double dResult = 1.0 / (1.0 / dSINRtemp - 1.0);
    return dResult;
}

double LinkLevelNR::MCS2TBSKBit2(int _iMCSIndex, int _iRENum_AllSB,int v) {
    double dTBSizeKbit = 0.001 * MCS2TBS(_iMCSIndex, _iRENum_AllSB,v);
    return dTBSizeKbit;
}
int LinkLevelNR::SINR2MCS2(const vector<double>& _vSINR,int v) {
    int iMCS = -1;
    int iSCNum = static_cast<int> (_vSINR.size());
    int iRBNum = iSCNum * Parameters::Instance().LINK_CTRL.IFrequencySpace / Parameters::Instance().BASIC.IRBSize ;
    //    for (int i = Parameters::NRMCSNumDL - 1; i >= 0; --i) {
    for (int i = Parameters::Instance().BASIC.NRMCSNumDL - 1; i >= 0; --i) {
        double dTBSKbit = GetTBSizeKBit_woDMRS2(i, iRBNum,v);
        double dBLER = BLER(_vSINR, i, dTBSKbit);
        if (dBLER < Parameters::Instance().SIM.DL.DBlerTarget) {
            iMCS = i;
            break;
        }
    }
    return iMCS;
}


double LinkLevelNR::GetTBSizeKBit_woDMRS2(imat _mMCS, int _iRBNum,int v) {
    double dResult = 0.0;
    for (int row = 0; row < _mMCS.rows(); ++row) {
        for (int col = 0; col < _mMCS.cols(); ++col) {
            dResult += GetTBSizeKBit_woDMRS2(_mMCS(row, col), _iRBNum,v);
        }
    }
    return dResult;
}

double LinkLevelNR::GetTBSizeKBit_woDMRS2(int _iMCS, int _iRBNum,int v) {
//    int iRENumPerPRB;
//    if(Parameters::Instance().BASIC.RRC_Config_On == 1){
//        iRENumPerPRB = _mCalcRENum.CalculateRENum();
//    }else{
//        iRENumPerPRB = Parameters::Instance().BASIC.DDLRENumPerRBforData;
//    }
    int iRENumPerPRB = Parameters::Instance().BASIC.DDLRENumPerRBforData;
    int iRENum = (int)(iRENumPerPRB * _iRBNum);
    return MCS2TBSKBit2(_iMCS, iRENum,v);
}

double LinkLevelNR::MCS2TBS(int _iMCSIndex, int _iRENum_AllSB,int v) {
    //如果MCS无效的话，对应的吞吐量应该为0
    if (_iMCSIndex == -1)
        return 0.0;

    //Ninfo = Nre * R * Qm * v
    /*
     其中 Nre 为从的RE数
     * R 为MCS等级对应的码率
     * Qm 为MCS等级对应的调制等级
     * v  为层数
     */

    double InfoNum = _iRENum_AllSB * MCS2Rate(_iMCSIndex) * MCS2ModLevel(_iMCSIndex, Parameters::Instance().BASIC.IDLORUL) * v;

    double dTBS;
    if (InfoNum <= 3824) {
        int n = max(floor(log2(InfoNum)) - 6.0, 3.0);
        double temp = pow(2.0, n);
        InfoNum = max(floor(InfoNum / temp) * temp, 24.0);
        dTBS = InfoNum;
    } else {
        int n = floor(log2(InfoNum - 24.0)) - 5.0;
        double temp = pow(2.0, n);
        InfoNum = max(itpp::round((InfoNum - 24.0) / temp) * temp, 3840.0);

        if (MCS2Rate(_iMCSIndex) <= 0.25) {
            double C = itpp::ceil_i((InfoNum + 24) / 3816);
            dTBS = 8.0 * C * itpp::ceil_i((InfoNum + 24) / 8 / C) - 24;
        } else {
            if (InfoNum > 8424) {
                double C = itpp::ceil_i((InfoNum + 24) / 8424);
                dTBS = 8.0 * C * itpp::ceil_i((InfoNum + 24) / 8 / C) - 24;
            } else {
                dTBS = 8.0 * itpp::ceil_i((InfoNum + 24) / 8) - 24;
            }
        }
    }
    return dTBS;
}

string LinkLevelNR::ModulationOrder(int _iMCSIndex, int iDLorUL) {
    static std::mutex m;
    m.lock();
	int index = LinkLevelNR::MCS2ModLevel(_iMCSIndex, iDLorUL);
	string sModulationOrder;
	switch (index) {
	case 2:
		sModulationOrder = "QPSK";
		break;
	case 4:
		sModulationOrder = "16QAM";
		break;
	case 6:
		sModulationOrder = "64QAM";
		break;
	case 8:
		sModulationOrder = "256QAM";
		break;
	case 0:
		sModulationOrder = "NULL";
		break;
	default:
		cout << "LinkLevelNR::ModulationOrder error!" << endl;
            cout<<_iMCSIndex<<" "<<index<<endl;
		assert(false);
	}
    m.unlock();
	return sModulationOrder;
}
int LinkLevelNR::SINR2MCS_simplified(const vector<double>& vSINR){
    double SINRdB = 0;
    int length = static_cast<int>(vSINR.size());
    for(int i=0;i<length;i++){
        SINRdB += vSINR[i];
    }
    SINRdB /= length;
    SINRdB = L2DB(SINRdB);

    int MaxMCS = static_cast<int>(MCS2SINRTable.size()-1);
    for(int i=MaxMCS;i>=0;i--){
        if(MCS2SINRTable[i] <= SINRdB )
            return i;
    }
    return  0;
}
int LinkLevelNR::SINR2MCS_simplified(double dSINR_dB){
    int MaxMCS = static_cast<int>(MCS2SINRTable.size()-1);
    for(int i=MaxMCS;i>=0;i--){
        if(MCS2SINRTable[i] <= dSINR_dB )
            return i;
    }
    return  0;
}
double LinkLevelNR::MCS2SINR_simplified(const int& _MCS){
    if(_MCS<0)
        int a=0;
    return MCS2SINRTable[_MCS];
}
int LinkLevelNR::GetRBNum_ByTable(const int layer, const int RENumPerRB, const int MCS, const double TBSizeKbit){
    if(TBSizeKbit < 0.001)
        return 0;
    double TBSize = TBSizeKbit*1000;
    int r = -1;
    for(int i=0;i<RELevelNum; i++){
        if(RENumTable[i] == RENumPerRB){
            r = i;
            break;
        }
    }
    assert(r>0);
    for(int i=0; i<MaxRBNum; i++){
        if(Layer_RE_MCS_RBNum2TBSize[layer][r](MCS, i) >= TBSize){
            return i+1;
        }
    }
    return MaxRBNum;
//    assert(false);
}