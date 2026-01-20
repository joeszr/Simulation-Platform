///@brief  单层网络
///
///针对不同网络架构，实现不同初始化流程
///
///@author wanghanning

#pragma once
#include "OneLayerNetwork.h"

class IndoorNetwork : public OneLayerNetwork{
public:
    virtual void SetMSPara();
    virtual void DistributeBSs();
public:
    /// @brief 构造函数
    IndoorNetwork(void);
    /// @breif 析构函数
    ~IndoorNetwork(void);
};