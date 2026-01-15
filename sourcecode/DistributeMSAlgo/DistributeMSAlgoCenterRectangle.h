/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file DistributeMSAlgoCenterRectangle.h
///@brief 中心小区矩形撒点函数声明
///
///@author wangxiaozhou
#pragma once
#include "DistributeMSAlgo.h"

///中心小区矩形撒点，是移动台撒点类的继承类

class DistributeMSAlgoCenterRectangle final: public DistributeMSAlgo {
public:
    ///中心小区矩形撒点算法的成员函数
    void DistributeMS(MSManager& _msm) final;
    ///单个MS撒点
    void DistributeMS(MS& _ms) final;

public:
    ///此类的构造函数
    DistributeMSAlgoCenterRectangle() = default;
    ///此类的析构函数
    ~DistributeMSAlgoCenterRectangle() override = default;
};
