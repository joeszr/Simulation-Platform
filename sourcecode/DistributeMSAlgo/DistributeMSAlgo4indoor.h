/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file DistributeMSAlgo4indoor.h
///@brief 室内移动台撒点函数
///
///@author dushaofeng

#pragma once
#include "DistributeMSAlgo.h"
///室内移动台撒点函数的类，是移动台撒点函数的继承
class DistributeMSAlgo4indoor final: public DistributeMSAlgo {
public:
    ///实现撒点算法的成员函数
    void DistributeMS(MSManager& _msm) final;
    ///单个MS撒点
    void DistributeMS(MS& _ms) final;
public:
    ///此继承类的构造函数
    DistributeMSAlgo4indoor() = default;
    ///此继承类的析构函数
    ~DistributeMSAlgo4indoor() override= default;


};
