/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file DistributeMSAlgoHexagon.h
///@brief 六边形撒点函数
///
///@author dushaofeng
#pragma once
#include "DistributeMSAlgo.h"

///正六边形撒点类，是移动台撒点类的继承类

class DistributeMSAlgoHexagon  final: public DistributeMSAlgo {
public:
    ///正六边形撒点算法的成员函数
    void DistributeMS(MSManager& _msm) final;
    ///单个MS撒点
    void DistributeMS(MS& _ms) final;
    void DistributeMS(MS& _ms, int _bsid, int _btsindex) final;
public:
    ///此类的构造函数
    DistributeMSAlgoHexagon() = default;
    ///此类的析构函数
    ~DistributeMSAlgoHexagon() override = default;
};
