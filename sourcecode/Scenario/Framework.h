///@brief  架构类声明
///
///针对不同网络架构，实现不同初始化流程
///
///@author wanghanning

#pragma once
#include "../Utility/functions.h"
#include "../NetworkDrive/BSManager.h"
#include "../NetworkDrive/MSManager.h"
#include "../DistributeMSAlgo/DistributeMSAlgo.h"


class Framework{
public:
public:
    std::shared_ptr<DistributeMSAlgo> m_pDMSA;
    bool systemInitFinished;
    virtual void InitFrame();
    void PrintHead();
    /// @brief 信道参数初始化的函数
    void ChannelParameterInitialize();
    //7、19扇区撒点
    void StandardDistribute();
    ///////////////////可重构////////////////////
    //移动台位置分布初始化
    virtual void DistributeMSs();
    virtual void DistributeMSs(vector<int> _vmsid);
    ///
    virtual void ConstructPicoGroups();
    //初始化用户
    virtual void InitializeMSs();
    virtual void InitializeMSs(vector<int> _vmsid);
    //初始化基站
    virtual void InitializeBSs();
    ///////////////////必须重构///////////////////
    //初始化BS分布位置
    virtual void DistributeBSs() = 0;
protected:
    static Framework* m_pFramework;
public:
    /// @brief 构造函数
    Framework(void);
    /// @breif 析构函数
    ~Framework(void);
    //单例
    static Framework& Instance();
};