/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PathLossIndoorFactoryDHNLOS.cpp
 * Author: cyh
 * 
 * Created on 2020年1月15日, 上午11:30
 */

#include "PathLossIndoorFactoryDHNLOS.h"
#include "P.h"

using namespace cm;

double PathLossIndoorFactoryDHNLOS::Db(double _dDisM, double _dUEHeightM) {
    static double PL_LOS = 31.84
            + 21.50 * std::log10(_dDisM)
            + 19.00 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
    
    static double PL = 33.63
            + 21.90 * std::log10(_dDisM)
            + 20.00 * std::log10(P::s().FX.DRadioFrequencyMHz_Macro / 1e3);
    
    double res = std::max(PL, PL_LOS);
    
    return -1 * res;
}

