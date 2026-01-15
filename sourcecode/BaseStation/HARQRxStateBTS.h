///@file HARQRxStateBTS.h
///@brief  HARQRxStateBTS类声明
///@author ZHENGYI

#pragma once
#include "../Utility/Include.h"
#include "../MobileStation/MSID.h"
class ACKNAKMessageUL;

class HARQRxStateBTS {
private:
    /// 标示当前HARQ进程ID
    //    int m_iHARQThreadID;

    SafeUnordered_map<pair<MSID, int>, vector<mat> > m_mMSIDHARQID2SINR;

    deque<std::shared_ptr<ACKNAKMessageUL> > m_qACKNAKMesQueue;
public:
    ///@brief 每个TTI需要完成的操作
    //    void WorkSlot();
    ///@brief 合并重传的SINR
    //    void CombineSINR(MSID _msid, const vector<double>& _vSINR);

    void CombineSINR(const MSID& _msid, const int& _HARQID, const vector<mat>& _vSINR);
    ///@brief 获取合并后的SINR
    //    const vector<double>& GetCombinedSINR(MSID _msid);

    const vector<mat>& GetCombinedSINR(const MSID& _msid, const int& _HARQID);
    ///@brief 获取HARQID
    //    int GetHARQID();
    ///@brief 刷新
    //    void Refresh(MSID _msid);

    void Refresh(const MSID& _msid, const int& _HARQID);
    ///@brief reset
    void Reset();
    ///
    //    void Clear(void);
    ///@brief 判断下次传输是否是重传，用于调度中排除重传用户和资源
    bool IsNeedReTx();
    ///@brief 判断本次传输是否是重传，用于本次传输是提取重传信息还是新调度信息  //zhengyi
    bool IsReTxCurrentSF(const MSID&);
    ///
    void RegistReTxMessage(const std::shared_ptr<ACKNAKMessageUL>& _pACKNAKMes);
    ///获取当前采用的重传配置信息
    //    std::shared_ptr<ACKNAKMessageUL> GetReTxMessage();
    ///获取当前用户_msid用户的重传配置信息
    std::shared_ptr<ACKNAKMessageUL> GetReTxMessage(const MSID& _msid);
    ///获得重传队列中的参数配置信息队列
    deque<std::shared_ptr<ACKNAKMessageUL > > GetReTxMsgQue();

    deque<std::shared_ptr<ACKNAKMessageUL > >& GetReTxMsgQueRef();
    //
    std::shared_ptr<ACKNAKMessageUL> GetReTxMsgwithoutDel(const MSID& _msid);

public:
    HARQRxStateBTS();
    ~HARQRxStateBTS() = default;
};