///@file HARQTxStateMS.h
///@brief MS端HARQ发射状态管理类声明
///@author zhengyi

#pragma once

#include "../Utility/Include.h"

class ACKNAKMessageUL;

/// @brief 位于MS的HARQ发送状态机

class HARQTxStateMS {
private:

    /// 从HARQ进程ID到ACKNAK消息的映射
    //    std::vector<std::shared_ptr<ACKNAKMessageUL> > m_vACKNAKMes;

    deque<std::shared_ptr<ACKNAKMessageUL> > m_qACKNAKMesQueue;
public:
    /// @brief 是否需要重传
    bool IsNeedReTx();
    /// @brief 获得当前HARQID
    //    int GetHARQID(void);
    /// @brief 时间驱动接口，完成每个TTI需要完成的操作
    //    void WorkSlot(void);
    /// @brief 注册重发消息
    void RegistReTxMessage(const std::shared_ptr<ACKNAKMessageUL>& _pACKNAKMesUL);
    /// @brief 取重发消息
    std::shared_ptr<ACKNAKMessageUL> GetReTxMessage();
    ///
    //    void Reset();
    ///@brief 取重发消息 将所有进程内的重传消息清零，防止drop间消息传递
    //    void Clear();
public:
    /// @brief 构造函数
    HARQTxStateMS() = default;
    /// @brief 析构函数
    ~HARQTxStateMS() = default;
};




