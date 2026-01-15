///@file ACKNAKMessageDL.h
///@brief  ACKNAKMessage类声明
///@author wangfei

#pragma once
#include"../Utility/Include.h"

class SchedulingMessageDL;
//class SchedulingMessage;

/// @brief ACK/NACK消息类

class ACKNAKMessageDL {
private:
    /// ACKNAK信息
    bmat m_mACKNAK;
    bool bACKNAK;
    /// 对应的SchedulingMessage
    std::shared_ptr<SchedulingMessageDL> m_pScheduleMes;

    //    std::shared_ptr<SchedulingMessage> m_pScheduleMes;
public:
    void SetACKNAK(bool _mACKNAK);

    bool GetbACKNAK() const;
    /// @brief 获取调度信息
    std::shared_ptr<SchedulingMessageDL> GetScheduleMes();

public:
    /// ACKNAKMessage类的构造函数
    explicit ACKNAKMessageDL(std::shared_ptr<SchedulingMessageDL> _pSchM);
    /// ACKNAKMessageDL类的析构函数
    ~ACKNAKMessageDL() = default;
};
