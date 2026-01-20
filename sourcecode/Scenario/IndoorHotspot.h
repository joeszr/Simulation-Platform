///@brief  架构类声明
///
///针对不同网络架构，实现不同初始化流程
///
///@author wanghanning

#pragma once
#include "Scenario.h"

namespace cm{
    //Indoor场景有单独的信道参数、撒点算法，因此继承frame和scenario
    class IndoorHotspot : public Scenario{
    public:
        virtual void SetMSPara(MS* ms);
        /////////////////////////////////////LinkMatrix调用////////////////////////////////////////////////
        virtual std::shared_ptr<PathLoss> GetPathLossPtr(TxNode& _tx, RxNode& _rx, bool _bIsLOS);
        virtual std::shared_ptr<PathLoss> GetPathLossPtr(double _dInDoorDisM, bool _bIsLOS, int _iIslowloss, double _dPenetrationSFdb);
        ////////////////////////////////////////BCS调用////////////////////////////////////////////////////
        /// @brief 初始化地图变量
        virtual void InitializeMap();
        virtual bool DecideLOS(double _dDisM_2D, bool _bIsMacro2UE = true, double _h_UT = 1.5);
        virtual void ReadMapPos(Point& _tx, RxNode& _rx, BasicChannelState* const bcs);
        virtual void SetSCSPara(BasicChannelState* const bcs, double _dTxHeight, double _dRxHeight);
        ////////////////////////////////////////SCS调用////////////////////////////////////////////////////
        virtual void InitializeAOD(SpaceChannelState* pSCS);
        virtual void InitializeAOA(SpaceChannelState* pSCS);
        virtual void InitializeEOD(SpaceChannelState* pSCS);
        virtual void InitializeEOA(SpaceChannelState* pSCS);
    public:
        IndoorHotspot(void);
        ~IndoorHotspot(void);
    };
}