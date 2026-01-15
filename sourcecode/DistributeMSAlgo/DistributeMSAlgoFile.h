/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file DistributeMSAlgoFile.h
///@brief 按照输入文件规定的点值撒点函数
///
///@author dushaofeng
#pragma once
#include "DistributeMSAlgo.h"

///按照输入文件规定的点值撒点算法的类，输入文件定义了所有的点值座标
class DistributeMSAlgoFile  final: public DistributeMSAlgo {
public:
    ///按照输入文件规定的点值撒点的成员函数
    void DistributeMS(MSManager& _msm) final;
    ///单个MS撒点
    void DistributeMS(MS& _ms) final;
public:
    ///此类的构造函数
    DistributeMSAlgoFile() = default;
    ///此类的析构函数
    ~DistributeMSAlgoFile() override = default;
};
