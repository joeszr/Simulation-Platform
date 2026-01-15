/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file DistributeMSAlgo.h
///@brief 移动台撒点的函数
///
///@author dushaofeng
#pragma once
#include "../Utility/Random.h"
///移动台管理者类的声明
class MSManager;
///基站管理者类的声明
class BSManager;
//移动台类声明
class MS;

///实现移动台撒点算法的基类
class DistributeMSAlgo {
public:
    Random random;
    // 20171204
    virtual void DistributeMS_with_distance_check(MS& _ms);

    
public:
    ///实现移动台撒点算法的成员函数，是一个虚函数
    virtual void DistributeMS(MSManager& _msm) = 0;
    ///单个MS撒点
    virtual void DistributeMS(MS& _ms) = 0;
    ///单个MS撒点
    virtual void DistributeMS(MS& _ms, int _bsid, int _btsindex);
public:
    ///此类的构造函数
    DistributeMSAlgo() = default;
    ///此类的析构函数
    virtual ~DistributeMSAlgo() = default;
};
