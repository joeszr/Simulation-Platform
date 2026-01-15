//
// Created by AAA on 2023/3/7.
//
#include "../NetworkDrive/Clock.h"
#include "PDCCH_Allocator.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../MobileStation/MS.h"
#include "../BaseStation/BTS.h"
#include "../Statistician/Statistics.h"
bool PDCCH_Allocator::Allocate(MSID msid, vector<vector<bool>>& ResourceGrid, unordered_map<int, int>& MSID2CCELevel, unordered_map<int, vector<int>>& MSID2CCESB, int DLorUL)
{
    return true;
    ///小区级指标新增
    int BTS2TxID = m_BTSID.GetBTS().GetTxID();
    if(DLorUL == Parameters::UL){
        Statistics::Instance().m_BS_UL_PDCCHCCE_Times[BTS2TxID] += 1;
        Statistics::Instance().m_BS_UL_PDCCHCCEAllocate_FailureRate_Total[BTS2TxID] += 1;
    }
    else if(DLorUL == Parameters::DL){
        Statistics::Instance().m_BS_DL_PDCCHCCE_Times[BTS2TxID] += 1;
        Statistics::Instance().m_BS_DL_PDCCHCCEAllocate_FailureRate_Total[BTS2TxID] += 1;
    }

    if(DLorUL == Parameters::UL)
    {
        if(m_BTSID.GetBTS().UlCceOccupiedNum >= m_BTSID.GetBTS().UlCceMaxNum)
        {
            Statistics::Instance().m_BS_UL_PDCCHCCEAllocate_FailureRate_Use[BTS2TxID] += 1;
            return false;
        }
    }
    int CoresetID = 1;
    bool Interleaved = false;
    MS& ms = msid.GetMS();
    BTS& bts = ms.GetMainServBTS().GetBTS();
    double PL = cm::LinkMatrix::Instance().GetCouplingLossDB(bts, ms);
    int CceLevel = bts.MSID2CCELevel[ms.GetID().ToInt()];
    int CandicateOfCceLevel;
    int crossCarrierIndicator = 0;
    int CoresetDuration = Parameters::Instance().CoresetDuration;
    if(CceLevel == 1)
    {
        CandicateOfCceLevel = 2;
    }
    else if(CceLevel == 2)
    {
        CandicateOfCceLevel = 2;
    }
    else if(CceLevel == 4)
    {
        CandicateOfCceLevel = 4;
    }
    else if(CceLevel == 8)
    {
        CandicateOfCceLevel = 2;
    }
    else if(CceLevel == 16)
    {
        CandicateOfCceLevel = 1;
    }
    else
    {
        cerr<<"wrong ccelevel!"<<endl;
    }

    int coresetRbLen = Parameters::Instance().BASIC.IRBNum/6*6;
    int regMaxNum = coresetRbLen * CoresetDuration;
    int CceMaxNum = regMaxNum / 6;
    if(CceLevel > CceMaxNum)
    {
        return false;
    }
//    assert(CceLevel <= CceMaxNum);//带宽太小

    int L = CceLevel;
    int n_CI = crossCarrierIndicator;
    int p      = CoresetID;
    int N_CCE  = CceMaxNum;
    int M_s    = CandicateOfCceLevel;
    int D      = 65537;

    long long A_p;
    if( p%3 == 0){
        A_p = 39827;
    }
    else if(p%3 == 1){
        A_p = 39829;
    }
    else{
        A_p = 39839;
    }
    bool AllocationSuccess = false;
    //这个值应该随机生成，这里先用MSID
    int n_RNTI = msid.ToInt()+1;
    int Y_p = n_RNTI;
    int iSlot = Clock::Instance().GetTimeSlotInSF();
    long long tempY_p = 0;
    for(int i=0; i<=iSlot; i++){
        tempY_p = A_p * Y_p;
        Y_p = tempY_p % D;
    }
    vector<int> vCceStartIndex(M_s);
    for(int m_s=0; m_s<M_s; m_s++){
        vCceStartIndex[m_s] = L*( (Y_p + m_s*N_CCE/(L*M_s) + n_CI) % (N_CCE/L) );
    }
    for(int CceStart=0; CceStart<M_s; CceStart++)
    {
        if(!Interleaved)
        {
            int iCceStartIndex = vCceStartIndex[CceStart];
            int RegBundleSize = 6;
            int AllocateCceNum = 0;
            vector<vector<bool>>ResourceGrid_temp = ResourceGrid;

            for(int iCce=0; iCce<CceLevel; iCce++)
            {
                int RegBundleIndex = 6*(iCceStartIndex + iCce)/RegBundleSize;
                vector<int>RegIndex(RegBundleSize);
                for(int iReg=0; iReg<RegBundleSize; iReg++){
                    RegIndex[iReg] = RegBundleIndex * RegBundleSize + iReg;
                }

//                    vector<vector<bool>>CceMapping(coresetRbLen, vector<bool>(CoresetDuration));
                vector<pair<int, int>>CceMapping;
                int maxReg = RegBundleSize/CoresetDuration;
                for(int iReg=0; iReg<maxReg; iReg++){
                    int RbIndex = RegIndex[iReg*CoresetDuration]/CoresetDuration;
                    for(int i=0; i<CoresetDuration; i++){
                        CceMapping.push_back(std::make_pair(RbIndex,i));
//                            CceMapping[RbIndex][i] = true;
                    }
                }
                bool flag = true;
                for(auto pos:CceMapping){
                    //如果已被占用
                    if(ResourceGrid_temp[pos.first][pos.second] == true || pos.first >= Parameters::Instance().DaHuaWu.iOccupiedRbNum){
                        flag = false;
                        break;
                    }
                }
                if(flag)
                {
                    for(auto pos:CceMapping){
                        ResourceGrid_temp[pos.first][pos.second] = true;
                    }
                    AllocateCceNum++;
                }
                else
                {
                    break;
                }
            }
            if(AllocateCceNum == CceLevel)
            {
                MSID2CCELevel[msid.ToInt()] = CceLevel;
                for(int i=0;i<ResourceGrid_temp.size(); i++)
                {
                    if(ResourceGrid[i][0] == false && ResourceGrid_temp[i][0] == true)
                    {
                        MSID2CCESB[msid.ToInt()].push_back(i);
                    }
//                    for(int j = 0; j<CoresetDuration; j++){
//                        if(ResourceGrid[i][j] == false && ResourceGrid_temp[i][j] == true)
//                        {
//                            MSID2CCESB[msid.ToInt()].push_back(i);
//                        }
//                    }
                }
                assert(MSID2CCESB[msid.ToInt()].size() == 6*CceLevel);
                ResourceGrid = ResourceGrid_temp;
                AllocationSuccess = true;

                if(DLorUL == Parameters::UL)
                {
                    m_BTSID.GetBTS().UlCceOccupiedNum += CceLevel;
                }

                if(DLorUL == Parameters::DL)
                {
                    m_BTSID.GetBTS().DlCceOccupiedNum += CceLevel;
                }
                break;
            }
        }
        else
        {
            assert(false);
        }
    }
    if(AllocationSuccess)
    {
        ///小区上行PDCCH CCE分配失败比例
        return true;
    }
    if(DLorUL == Parameters::UL){
        Statistics::Instance().m_BS_UL_PDCCHCCEAllocate_FailureRate_Use[BTS2TxID] += 1;
    }
    else if(DLorUL == Parameters::DL){
        Statistics::Instance().m_BS_DL_PDCCHCCEAllocate_FailureRate_Use[BTS2TxID] += 1;
    }
    return false;
}