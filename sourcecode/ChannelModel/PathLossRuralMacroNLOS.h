/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file PathLossRuralMacroNLOS.h
///@brief  PathLossRuralMacroNLOS类声明
///
///包括构造函数，析构函数，计算该场景下DB值的函数声明
///
///@author wangfei

#pragma once
#include "PathLoss.h"
#include "PathLossRuralMacroLOS.h"

namespace cm{
    ///PathLossRuralMacroNLOS Class
    class PathLossRuralMacroNLOS_ModeA final : public PathLoss{
    public:
        double Db( double _dDisM , double _dUEHeightM) override;
    protected:
        PathLossRuralMacroLOS_ModeA PL_LOS;
    public:
        ///构造函数
        PathLossRuralMacroNLOS_ModeA() = default;
        ///析构函数
        ~PathLossRuralMacroNLOS_ModeA() override = default;
    };
}
