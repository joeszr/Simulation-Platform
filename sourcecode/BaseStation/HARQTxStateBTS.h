///@file HARQTxStateBTS.h
///@brief BTS端HARQ发射状态管理类声明
///@author wangxiaozhou

#pragma once
#include "../Utility/Include.h"
#include "../SafeDeque.h"
class ACKNAKMessageDL;
class MSID;

/// @brief 位于BTS的HARQ发送状态机

class HARQTxStateBTS {
private:
    /// 标示当前HARQ进程ID
    int m_iHARQThreadID;
    /// 存储对应HARQID的HARQ线程状态队列

    /// ACKNAK消息队列
    SafeDeque<std::shared_ptr<ACKNAKMessageDL> > m_qNAKMesQueue;
public:
    /// @brief 判断是否需要进行HARQ重传
    bool IsNeedReTx();
    //记录所属小区
    int BSid;
    int BTSindex;
    void setBTSID(int a,int b){
        BSid=a;
        BTSindex=b;
    }
    //打印重传调度信息内容
    void PrintNAKQueue(bool)const;

    int ReTxNum();
    /// @brief 获得当前HARQID
    int GetHARQID();
    /// @brief 获得需要重发的ACKNAK信息
    std::shared_ptr<ACKNAKMessageDL> GetReTxMessage();
    ///
    std::shared_ptr<ACKNAKMessageDL> CheckReTxMessage();
    ///清除重传ACKNAK信息
    void ClearReTxMessage();
    /// @brief 注册重发消息
    void RegistReTxMessage(const std::shared_ptr<ACKNAKMessageDL>& _pACKNAKMessage);
    /// @brief 时间驱动接口，完成每个TTI需要完成的操作
    //    void WorkSlot(void);
    /// @brief 重置HARQ发送状态机
    void Reset();

    bool CanReTx( const deque<std::shared_ptr<ACKNAKMessageDL> >::iterator& _p);
    
    SafeDeque<std::shared_ptr<ACKNAKMessageDL> >& GetACKNAKMesQueue();

public:
    /// @brief 构造函数
    HARQTxStateBTS() = default;
    /// @brief 析构函数
    ~HARQTxStateBTS() = default;
};


