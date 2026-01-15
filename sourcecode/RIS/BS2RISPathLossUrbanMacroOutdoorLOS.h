#pragma once
#include "../ChannelModel/PathLoss.h"

namespace cm{

    class BS2RISPathLossUrbanMacroOutdoorLOS : public PathLoss{
    public:
        ///计算该场景下PathLoss的DB值
        double Db( double _dDisM, double _dRISHeightM = 1.5);
    public:
        ///构造函数
        BS2RISPathLossUrbanMacroOutdoorLOS( void );
        ///析构函数
        ~BS2RISPathLossUrbanMacroOutdoorLOS( void );
    };
}
