#pragma once
#include "Scenario.h"

namespace cm{
    class UrbanMacro : public Scenario{
    public:
        virtual void SetMSPara(MS* ms);
        /////////////////////////////////////LinkMatrix调用////////////////////////////////////////////////
        virtual double GenPos2Din(std::pair<int, int> pos);
        virtual std::shared_ptr<cm::PathLoss> GetPathLossPtr(cm::Tx& _tx, cm::Rx& _rx, bool _bIsLOS);
        virtual std::shared_ptr<cm::PathLoss> GetPathLossPtr(double _dInDoorDisM, bool _bIsLOS, int _iIslowloss, double _dPenetrationSFdb);
        ////////////////////////////////////////BCS调用////////////////////////////////////////////////////
        virtual bool DecideLOS(double _dDisM_2D, bool _bIsMacro2UE = true, double _h_UT = 1.5);
        virtual void ReadMapPos(Point& _tx, Rx& _rx, BasicChannelState* const bcs);
        virtual void SetSCSPara(BasicChannelState* const bcs, double _dTxHeight, double _dRxHeight);
        ////////////////////////////////////////SCS调用////////////////////////////////////////////////////
        virtual void InitializeAOD(SpaceChannelState* pSCS);
        virtual void InitializeAOA(SpaceChannelState* pSCS);
        virtual void InitializeEOD(SpaceChannelState* pSCS);
        virtual void InitializeEOA(SpaceChannelState* pSCS);
    public:
        UrbanMacro(void);
        ~UrbanMacro(void);
    };
}