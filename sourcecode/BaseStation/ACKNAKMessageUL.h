///@file ACKNAKMessageUL.h
///@brief  ACKNAKMessageUL类声明
///@author wangsen
#pragma once
#include"../Utility/Include.h"
class SchedulingMessageUL;

/// @brief ACK/NACK消息类

class ACKNAKMessageUL {
private:
    /// ACKNAK信息
    bool m_bACKNAK;
    /// 对应的SchedulingMessage
    std::shared_ptr<SchedulingMessageUL> m_pSchM;
public:
    /// @brief 设置ACKNAK消息
    void SetACKNAK(bool _bACKNAK);
    /// @brief 获取ACKNAK消息
    bool GetACKNAK() const;
    /// @brief 获取调度信息
    std::shared_ptr<SchedulingMessageUL> GetSchMessage();
public:
    /// ACKNAKMessage类的构造函数
    explicit ACKNAKMessageUL(const std::shared_ptr<SchedulingMessageUL>& _pSchM);
    /// ACKNAKMessage类的析构函数
    ~ACKNAKMessageUL() = default;
};
