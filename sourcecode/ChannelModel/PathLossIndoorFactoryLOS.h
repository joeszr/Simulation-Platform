/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PathLossIndoorFactoryLOS.h
 * Author: cyh
 *
 * Created on 2020年1月15日, 上午11:30
 */

#pragma once
#include "PathLoss.h"

namespace cm {
    class PathLossIndoorFactoryLOS final: public PathLoss {
    public:
        double Db( double _dDisM , double _dUEHeightM) override;
        PathLossIndoorFactoryLOS() = default;
        ~PathLossIndoorFactoryLOS() override = default;
    };
}



