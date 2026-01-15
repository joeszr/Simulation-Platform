//
// Created by CWQ on 2022/11/21.
//

#include "DRX.h"
#include "NetworkDrive/Clock.h"
#include "MobileStation/MS.h"
#include "BaseStation/BTSID.h"

void DRXTimer::SetConfig(bool _IsTimerOn, int _MaxVal, int _offset) {
    IsTimerOn = _IsTimerOn;
    MaxVal = _MaxVal;
    value = MaxVal;
    offset = _offset;
    state = "NA";
}
void DRXTimer::Start(){
    IsTimerOn = true;
    value = MaxVal;
    value--;
    state = "start";
}
void DRXTimer::StartAfterOffset(int _offset) {
    IsTimerOn = true;
    value = MaxVal;
    value--;
    offset = _offset;
    if(offset == 0){
        state = "start";
    }
    else{
        state = "NA";
    }
}
void DRXTimer::Reset(){
    IsTimerOn = false;
    value = MaxVal;
    offset = 0;
}

void DRXTimer::run() {
    if(IsTimerOn){
        if(offset>0){
            offset--;
            if(offset == 0){
                state = "start";
            }
        }
        else if(value>=0){
            value--;
            state = "run";
        }

        if(value == 0){
            state = "end";
        }
        if(value == -1){
            IsTimerOn = false;
            state = "NA";
        }
    }
}
bool DRXTimer::IsExpired() {
    return value == -1;
}
bool DRXTimer::IsRunning(){
    return IsTimerOn && (offset==0);
}



DRX::DRX(){
    int SlotPerSubFrame = pow(2, Parameters::Instance().BASIC.ISCSConfig);

    drxSwitch = Parameters::Instance().DRX.Switch;

    LongCycle = Parameters::Instance().DRX.LongCycle * SlotPerSubFrame;
    LongOffset = 0;

    DRXShortSwitch = Parameters::Instance().DRX.ShortSwitch;
    ShortCycle = Parameters::Instance().DRX.ShortCycle * SlotPerSubFrame;
    ShortOffset = 0;


    drxStartOffset  = 0;
    onDurationTimer.SetConfig(false, Parameters::Instance().DRX.OnDurationTimer*SlotPerSubFrame);
    InactivityTimer.SetConfig(false, Parameters::Instance().DRX.InactivityTimer*SlotPerSubFrame);
    ShortCycleTimer.SetConfig(false, Parameters::Instance().DRX.ShortCycle * Parameters::Instance().DRX.ShortCycleTimer * SlotPerSubFrame);
    NewPDCCH = false;

}
void DRX::initialize(MSID _msid) {
    msid = _msid;
    int SlotPerSubFrame = pow(2, Parameters::Instance().BASIC.ISCSConfig);
    LongOffset = (msid.ToInt() * SlotPerSubFrame) % LongCycle;
    ShortOffset = LongOffset % ShortCycle;

    CycleState = state_long;
    onDurationTimer.StartAfterOffset(LongOffset);

//    if(DRXShortSwitch){
//        CycleState = state_short;
//        ShortCycleTimer.StartAfterOffset(ShortOffset);
//    }
//    else{
//        CycleState = state_long;
//        onDurationTimer.StartAfterOffset(LongOffset);
//    }


}
void DRX::DRXstate_WorkSlot(){
    if(drxSwitch){

        ShortCycleTimer.run();
        onDurationTimer.run();

        int SlotPerSubFrame = pow(2, Parameters::Instance().BASIC.ISCSConfig);

        int SFN = Clock::Instance().GetSFN();
        int subframe = Clock::Instance().GetSubFrame();
        int Slot = (SFN*10+subframe) * SlotPerSubFrame;   //当前子帧号

        if(CycleState == state_long || ShortCycleTimer.IsExpired()){
            if(CycleState == state_short){     //短周期且超时
                CycleState = state_long;        //转换为长周期
                ShortCycleTimer.Reset();
            }
            if(!onDurationTimer.IsRunning()){  //长周期
                if(Slot % LongCycle == LongOffset){
                    onDurationTimer.StartAfterOffset(drxStartOffset);   //目前StartOffset=0
                }
            }
        }
        else{                               //短周期且未超时
            if(!onDurationTimer.IsRunning()){
                if(Slot % ShortCycle == ShortOffset){
                    onDurationTimer.StartAfterOffset(drxStartOffset);
                }
            }
        }

        //判断drx状态
        if(onDurationTimer.IsRunning() || InactivityTimer.IsRunning()){
            drx_state = state_active;
        }
        else{
            drx_state = state_sleep;
        }
    }
    else{
        drx_state = state_active;
    }
}
std::mutex DRXmutex;
void DRX::InactivityTimer_WorkSlot(bool _NewPDCCH){
    NewPDCCH = _NewPDCCH;
    if(drxSwitch){

        if(drx_state == state_active){
            if(InactivityTimer.IsTimerOn){
                InactivityTimer.run();
                if(NewPDCCH){ //有新传PDCCH
                    //重启InactivityTimer
                    InactivityTimer.Start();
                }
                else{
                    if(InactivityTimer.IsExpired()){
                        if(DRXShortSwitch){
                            //重启短周期定时器
                            ShortCycleTimer.Start();
                            CycleState = state_short;
                        }
                        else{
                            //长周期  如果没有使用短周期，直接进入长周期？
                            onDurationTimer.StartAfterOffset(LongOffset);
                            CycleState = state_long;
                        }
                    }
                }
            }
            else{
                if(NewPDCCH){ //有新传PDCCH
                    //重启InactivityTimer
                    InactivityTimer.Start();
                }
            }
        }


    }
}
void DRX::DRXRecord() {
    if(drxSwitch){
        string newDL;
        if(NewPDCCH){
            newDL = "DL";
        }
        else{
            newDL = "NA";
        }
        NewPDCCH = false;
        string cycle;
        if(CycleState == state_long){
            cycle = "LongDRX";
        }
        else{
            cycle = "ShortDRX";
        }
        BTSID btsid = msid.GetMS().GetMainServBTS();
        DRXmutex.lock();
        Observer::Print("DRXrecord")<< btsid
                                    << setw(20) << msid.ToInt()
                                    << setw(20) << Clock::Instance().GetSFN()
                                    << setw(20) << (Clock::Instance().GetTimeSlot()-1)
                                    << setw(20) << onDurationTimer.state
                                    << setw(20) << InactivityTimer.state
                                    << setw(20) << cycle
                                    << setw(20) << newDL
                                    << setw(20) << drx_state<<endl;

        DRXmutex.unlock();
    }

}
