/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file DistributeMSAlgoFix.h
///@brief 按照定点的方式撒点的函数
///
///@author dushaofeng
#pragma once
#include "DistributeMSAlgo.h"

///按照定点的方式撒点的类
class DistributeMSAlgoFix final: public DistributeMSAlgo {
public:
    ///按照定点的方式撒点算法的成员函数
    void DistributeMS(MSManager &_msm) override;
    ///单个MS撒点
    void DistributeMS(MS& _ms) final;
public:
    ///此类的构造函数
    DistributeMSAlgoFix() = default;
    ///此类的析构函数
    ~DistributeMSAlgoFix() override= default;
};
