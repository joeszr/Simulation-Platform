
///@brief  RIS网络
///
///针对不同网络架构，实现不同初始化流程
///
/*
 * File:   SceRIS.h
 * Author: wanghanning
 *
 * Created on 2023年7月18日, 上午9:23
 */

#pragma once
#include "OneLayerNetwork.h"
#include"NetworkDrive/MSManager.h"

class FrameRIS : public Framework{
public:
    virtual void DistributeBSs();
    //virtual void InitializeMSs();
public:
    /// @brief 构造函数
    FrameRIS(void);
    /// @breif 析构函数
    ~FrameRIS(void);

    void PrintHead();

    void InitFrame();


};

