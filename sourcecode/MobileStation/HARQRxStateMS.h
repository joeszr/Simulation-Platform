///@file HARQRxStateMS.h
///@brief  HARQRxStateMS类声明
///@author wangfei

#pragma once
#include "../Utility/Include.h"

///HARQRxStateMS类

class HARQRxStateMS {
private:
    ///当前的HARQ进程号
//    int m_iHARQID;
    ///<<HARQID，第几流>，SINR>
    SafeUnordered_map<pair<int, int>, vector<double> > m_mHarqRank2SINR;
public:
    ///@brief 每个TTI需要完成的操作
//    void WorkSlot(void);
    ///@brief 合并重传的SINR
    void CombineSINR(int _iHARQID, int _iRankID, const vector<double>& _vSINR);
    ///@brief 获取合并后的SINR
    vector<double>& GetCombinedSINR(int _iHARQID, int _iRankID);
    ///@brief 获取HARQID
//    int GetHARQID();
    ///@brief 刷新
    void Refresh(int _iHARQID,int _iRankID);
    ///@brief reset
    void Reset();
public:
    HARQRxStateMS();
    ~HARQRxStateMS() = default;
};

