/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file MSRxBufferBS.h
///@brief  MSRxBufferBS类声明
///
///包括构造函数，析构函数
///
///@author wangxiaozhou
#pragma once
#include "../Utility/Include.h"
#include "../MobileStation/MSID.h"
class MSID;

/// @brief 位于BS上的移动台接收缓存
class MSRxBufferBS {
public:
    /// 移动台的ID
    MSID m_iMSID;

public:
    /// @brief 构造函数
    explicit MSRxBufferBS(const MSID& _id = MSID(0));
    /// @brief 析构函数
    ~MSRxBufferBS() = default;
};
