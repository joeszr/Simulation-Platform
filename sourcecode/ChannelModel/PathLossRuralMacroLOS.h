/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossRuralMacroLOS.h
///@brief  PathLossRuralMacroLOS类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei

#pragma once
#include "PathLoss.h"

namespace cm{
    ///PathLossRuralMacroLOS Class
    class PathLossRuralMacroLOS_ModeA final : public PathLoss{
    public:
        double Db( double _dDisM , double _dUEHeightM) override;
    public:
        ///构造函数
        PathLossRuralMacroLOS_ModeA() = default;
        ///析构函数
        ~PathLossRuralMacroLOS_ModeA() override = default;
    };
}
