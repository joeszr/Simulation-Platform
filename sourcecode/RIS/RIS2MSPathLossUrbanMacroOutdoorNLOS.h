#pragma once
#include "../ChannelModel/PathLoss.h"
#include "RIS2MSPathLossUrbanMacroOutdoorLOS.h"

namespace cm{

    class RIS2MSPathLossUrbanMacroOutdoorNLOS : public PathLoss{
    public:
        ///计算该场景下PathLoss的DB值
        double Db( double _dDisM , double _dUEHeightM = 1.5);
    public:
        ///构造函数
        RIS2MSPathLossUrbanMacroOutdoorNLOS( void );
        ///析构函数
        ~RIS2MSPathLossUrbanMacroOutdoorNLOS( void );

    protected:
        RIS2MSPathLossUrbanMacroOutdoorLOS m_PL_LOS;
    };
}
