#pragma once
#include "../ChannelModel/PathLoss.h"
#include "RIS2MSPathLossUrbanMacroOutdoorNLOS.h"

/*
 * File:   RIS2MSPathLossUrbanMacroIndoor.h
 * Author: oumali
 *
 * Created on 2024年1月12日, 上午10:27
 */
namespace cm{

    class RIS2MSPathLossUrbanMacroIndoor : public PathLoss{
    public:
        ///计算该场景下PathLoss的DB值
        double Db( double _dDisM ,double _dUEHeightM = 1.5);
    public:
        ///构造函数
        RIS2MSPathLossUrbanMacroIndoor(double _dInDoorDisM, bool _bIsLOS, bool _iIslowloss, double _dPenetrationSFdb );
        ///析构函数
        ~RIS2MSPathLossUrbanMacroIndoor( void );
    private:
        double m_dInDoorDisM;
        bool m_bIsLOS;
        bool m_iIsLowloss;
        double m_dPenetrationSFdb;
    protected:
        RIS2MSPathLossUrbanMacroOutdoorLOS m_PL_LOS;
        RIS2MSPathLossUrbanMacroOutdoorNLOS m_PL_NLOS;
    };
}

