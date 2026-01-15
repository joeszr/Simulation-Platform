//
// Created by AAA on 2022/9/20.
//
#include "../Utility/functions.h"
#include "PHR.h"
#include "../MobileStation/MS.h"
#include "../NetworkDrive/Clock.h"
#include "../Statistician/Statistician.h"
#include <boost/thread.hpp>
boost::mutex PHR_mutex;
void PHR::initialize(const MSID& id){
    msid = id;

    PHR_On = Parameters::Instance().MSS_UL.UL.IsPhrOn;

    int SlotPerSubFrame = pow(2, Parameters::Instance().BASIC.ISCSConfig);

    PeriodicTimer = 0;
    Period = Parameters::Instance().MSS_UL.UL.PeriodicPeriod * SlotPerSubFrame;
    ProhibitTimer = 0;
    ProhibitPeriod = Parameters::Instance().MSS_UL.UL.ProhibitPeriod * SlotPerSubFrame;

    PowerFactorChange_dB = Parameters::Instance().MSS_UL.UL.PowerFactorChange_dB;

    IsTriggered = false;

    LastPLdB = 0;
    LastTxPower_dBm = 0;
}
void PHR::Reset(){
    PeriodicTimer = 0;
    ProhibitTimer = 0;
    LastPLdB = 0;
    LastTxPower_dBm = 0;
    IsTriggered = false;
}
void PHR::SetConfig(int period, int prohibitPeriod, double powerFactorChange_dB){
    PeriodicTimer = 0;
    Period = period;
    ProhibitTimer = 0;
    ProhibitPeriod = prohibitPeriod;
    PowerFactorChange_dB = powerFactorChange_dB;
    IsTriggered = true;
}
void PHR::timerRun(){
    if(PHR_On) {
        PeriodicTimer++;
        ProhibitTimer++;
    }
}
void PHR::Workslot(double max_TxPower_dBm, double EstimateTxPower_dBm, double actual_TxPower_dBm, double pathloss, bool newResource){
    if(PHR_On){
        int reason = -1;
        if(PeriodicTimer >= Period && Period>0){
            IsTriggered = true;
            PeriodicTimer = 0;
            reason = 0;
        }
        if(ProhibitTimer >= ProhibitPeriod && newResource){
            if(abs(LastPLdB - pathloss) > PowerFactorChange_dB && PowerFactorChange_dB>0){
                IsTriggered = true;
                ProhibitTimer = 0;
                reason = 1;
            }
            if(abs(LastTxPower_dBm - actual_TxPower_dBm ) > PowerFactorChange_dB && PowerFactorChange_dB>0){
                IsTriggered = true;
                ProhibitTimer = 0;
                reason = 2;
            }
        }
        int phr_index = -100;

        if(IsTriggered){
            LastPLdB = pathloss;
            LastTxPower_dBm = actual_TxPower_dBm;
            phr_index = GeneratePHR(max_TxPower_dBm, EstimateTxPower_dBm);

        }
        {
            boost::mutex::scoped_lock lock(PHR_mutex);
            Observer::Print("PHRRecord") << Clock::Instance().GetTimeSlot()
                                         << setw(20) << msid.GetMS().GetMainServBTS()
                                         << setw(20) << msid
                                         << setw(20) << IsTriggered
                                         << setw(20) << reason
                                         << setw(20) << phr_index<<endl;
        }
        if(IsTriggered){
            IsTriggered = false;
        }
        if(reason >= 0 && Clock::Instance().GetTimeSlot() >= Parameters::Instance().BASIC.IWarmUpSlot){
            Statistician::Instance().m_MSData_UL[msid.ToInt()].phr_trigger[reason]++;
        }
    }
}
int PHR::GeneratePHR(double MaxTxPower_dBm, double EstimatedTxPower_dBm){
    double phr_dB = MaxTxPower_dBm - EstimatedTxPower_dBm;
    int phr_index = floor(phr_dB + 23);
    phr_index = max(phr_index, 0);
    phr_index = min(phr_index, 63);
    std::pair<MSID, int> PHRMes = std::make_pair(msid, phr_index);
    msid.GetMS().GetUci()->CollectPHR(PHRMes);  //MS里还没加UCI
    return phr_index;
}