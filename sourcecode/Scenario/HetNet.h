#pragma once
#include "Scenario.h"

namespace cm{
    class HetNet : public Scenario{
    public:
        virtual void SetMSPara(MS* ms);
        /////////////////////////////////////LinkMatrix调用////////////////////////////////////////////////
        virtual std::shared_ptr<cm::PathLoss> GetPathLossPtr(cm::Tx& _tx, cm::Rx& _rx, bool _bIsLOS);
        virtual std::shared_ptr<cm::PathLoss> GetPathLossPtr(double _dInDoorDisM, bool _bIsLOS, int _iIslowloss, double _dPenetrationSFdb);
        ////////////////////////////////////////BCS调用////////////////////////////////////////////////////
        /// @brief 初始化地图变量
        virtual void InitializeMap();
        virtual bool DecideLOS(double _dDisM_2D, bool _bIsMacro2UE, double _h_UT);
        virtual void ReadMapPos(Point& _tx, Rx& _rx, BasicChannelState* const bcs);
        virtual void SetSCSPara(BasicChannelState* const bcs, double _dTxHeight, double _dRxHeight);
        ////////////////////////////////////////SCS调用////////////////////////////////////////////////////
        virtual void InitializeAOD(SpaceChannelState* pSCS);
        virtual void InitializeAOA(SpaceChannelState* pSCS);
        virtual void InitializeEOD(SpaceChannelState* pSCS);
        virtual void InitializeEOA(SpaceChannelState* pSCS);
    public:
        HetNet(void);
        ~HetNet(void);
    };
}