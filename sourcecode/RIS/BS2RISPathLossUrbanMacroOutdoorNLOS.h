#pragma once
#include "../ChannelModel/PathLoss.h"
#include "BS2RISPathLossUrbanMacroOutdoorLOS.h"

namespace cm{

    class BS2RISPathLossUrbanMacroOutdoorNLOS : public PathLoss{
    public:
        ///计算该场景下PathLoss的DB值
        double Db( double _dDisM, double _dRISHeightM = 1.5);
    public:
        ///构造函数
        BS2RISPathLossUrbanMacroOutdoorNLOS( void );
        ///析构函数
        ~BS2RISPathLossUrbanMacroOutdoorNLOS( void );
    protected:
        BS2RISPathLossUrbanMacroOutdoorLOS m_PL_LOS;
    };
}
