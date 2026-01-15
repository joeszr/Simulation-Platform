//
// Created by AAA on 2022/10/4.
//

#include "SR.h"
#include "./MobileStation/MS.h"
#include "NetworkDrive/Clock.h"
SR::SR(){
    ProhibitTimer_ms = Parameters::Instance().MSS_UL.UL.SR_ProhibitPeriod;
    ProhibitTimerKey = false;

    sr_Periodicity_slot = Parameters::Instance().MSS_UL.UL.sr_Periodicity_slot;
    if(Parameters::Instance().BASIC.ISCSConfig == 1){
        assert(sr_Periodicity_slot>=10 &&sr_Periodicity_slot<=80);
    }
    else if(Parameters::Instance().BASIC.ISCSConfig == 2){
        assert(sr_Periodicity_slot>=10 &&sr_Periodicity_slot<=160);
    }
    else if(Parameters::Instance().BASIC.ISCSConfig == 3){
        assert(sr_Periodicity_slot>=20 &&sr_Periodicity_slot<=320);
    }

    TransMax = Parameters::Instance().MSS_UL.UL.SR_TransMax;
    IsSROn = Parameters::Instance().MSS_UL.UL.IsSROn;
    Counter = -1;

    IsTriggered = false;
    PendingSRNum = 0;
    LastTxTime_ms = 0;
}
void SR::CalculateOffset(const int& Periodicity){
    int FirstSlotUL = -1;
    FirstSlotUL = Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot - Parameters::Instance().SIM.FrameStructure4Sim.P1.IULSlot;
    if(Parameters::Instance().SIM.FrameStructure4Sim.IIsSinglePeriod == 0){
        if(FirstSlotUL == Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot){
            FirstSlotUL =   Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot
                            + Parameters::Instance().SIM.FrameStructure4Sim.P2.IPeriodSlot
                            - Parameters::Instance().SIM.FrameStructure4Sim.P2.IULSlot;
        }
    }
    assert(FirstSlotUL < Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot + Parameters::Instance().SIM.FrameStructure4Sim.P2.IPeriodSlot);

    sr_Offset_slot = FirstSlotUL % Periodicity;
}
void SR::initialize(const MSID& id){
    msid = id;
    CalculateOffset(sr_Periodicity_slot);
}
void SR::SetConfig(const double& _ProhibitTimer, const int& _TransMax, const int& _Counter, const int& Period, const int& offset){
    ProhibitTimer_ms = _ProhibitTimer;
    TransMax = _TransMax;
    Counter = _Counter;
    sr_Periodicity_slot = Period;
    sr_Offset_slot = offset;
}
void SR::Reset(){
    PendingSRNum = 0;
    ProhibitTimerKey = false;

    Counter = -1;
}
void SR::GenerateSR(){
    if(IsSROn){
        if(PendingSRNum == 0){
            Counter = 0;
        }
        PendingSRNum++;
    }
}
void SR::SendSR(){
    double Time_ms = Clock::Instance().GetTimeSec() * 1000;
    if (IsTriggered) {
        int BornTime = Clock::Instance().GetTimeSlot();
        bool State = true;
        SRMessage SRMes = {BornTime, msid, State};

        msid.GetMS().GetUci()->CollectSR(SRMes);
        Counter++;
        LastTxTime_ms = Time_ms;
        IsTriggered = false;
    }
}
bool SR::IsPending(){
    return PendingSRNum != 0;
}
void SR::WorkSlot(bool IsSchM) {
    if (IsSROn) {

        if(PendingSRNum > 0){

            if(IsSchM)
            {
                Reset();
                return;
            }
            int sfn = Clock::Instance().GetSFN();
            int Time_slot = Clock::Instance().GetTimeSlot();
            double Time_ms = Clock::Instance().GetTimeSec()*1000;
            bool AbleToSend = ((sfn*Parameters::Instance().BASIC.ISlotNumPerSFN + (Time_slot-1)%Parameters::Instance().BASIC.ISlotNumPerSFN - sr_Offset_slot)%sr_Periodicity_slot == 0);
            //满足SR发送时机
            if(AbleToSend){
                if(Counter == 0){
                    ProhibitTimerKey = true;
                    IsTriggered = true;
                }
                else if(Time_ms - LastTxTime_ms >= ProhibitTimer_ms){
                    assert(ProhibitTimerKey == true);
                    if(Counter < TransMax)
                    {
                        IsTriggered = true;
                    }
                    else
                    {
                        Reset();
                    }
                }
            }

        }

        SendSR();
    }
}
void SR::ReceiveULGrant(ULGrant & Ulgrant)
{
    if(Ulgrant.IsAllowed)
    {
        msid.GetMS().m_clsMacMs.SetUL_grant(true);
        Reset();
    }
}

