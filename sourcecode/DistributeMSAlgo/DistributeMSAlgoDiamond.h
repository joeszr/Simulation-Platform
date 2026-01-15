/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file DistributeMSAlgoDiamond.h
///@brief 菱形撒点函数
///
///@author dushaofeng
#pragma once
#include "DistributeMSAlgo.h"

///菱形撒点类，是移动台撒点类的继承类

class DistributeMSAlgoDiamond final : public DistributeMSAlgo {
public:
    ///平菱形撒点算法的成员函数
    void DistributeMS(MSManager& _msm) final;
    ///单个MS撒点
    void DistributeMS(MS& _ms) final;

public:
    ///此类的构造函数
    DistributeMSAlgoDiamond() = default;
    ///此类的析构函数
    ~DistributeMSAlgoDiamond() override = default;
};
