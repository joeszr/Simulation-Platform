#pragma once
#include "../ChannelModel/PathLoss.h"

namespace cm{

    class RIS2MSPathLossUrbanMacroOutdoorLOS : public PathLoss{
    public:
        ///计算该场景下PathLoss的DB值
        double Db( double _dDisM , double _dUEHeightM = 1.5);
    public:
        ///构造函数
        RIS2MSPathLossUrbanMacroOutdoorLOS( void );
        ///析构函数
        ~RIS2MSPathLossUrbanMacroOutdoorLOS( void );
    };
}
