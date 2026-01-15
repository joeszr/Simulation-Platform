/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file SoundingMessage.h
///@brief Sounding消息类的声明
///
///@author wangfei
#pragma once
#include "../Utility/Include.h"
//@threads
#include "../SafeUnordered_map.h"
#include "../Utility/RBID.h"
class RBID;
class MSID;

/// @brief Sounding 消息类
class SoundingMessage {
private:
    /// 移动台的ID
    MSID m_MSID;
    /// 记录本消息的生成时间
    int m_iBornTime;
    int m_iAntennaID;
    /// 对应每个RB上的信道矩阵
    //@threads
    std::unordered_map<int, cmat> m_mH;
public:
    /// @brief 返回对应RB上的信道矩阵
    cmat GetH(RBID _RBID);
    /// @brief 设置对应RB上，指定天线的信道响应
    void SetH(const RBID& _RBID, cmat _mH, int _iAnt);
    /// @brief
    void SetH(const RBID& _RBID, cmat _mH);
    /// @brief 返回本消息的生成时间
    int GetBornTime() const;
    /// @brief 返回本消息对应的天线ID
    int GetAntennaID() const;
    /// @brief 返回发送本消息的移动台ID
    MSID GetMSID();
public:
    /// @brief 构造函数
    explicit SoundingMessage(MSID _msid);
    /// @brief 析构函数
    ~SoundingMessage() = default;
};