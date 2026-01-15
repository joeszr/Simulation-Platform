//
// Created by ChTY on 2022/10/7.
//

#include "MixedTraffic.h"
#include "../MobileStation/MSID.h"
#include "../MobileStation/MS.h"
void MixedTraffic::WorkSlot()
{
    if(Clock::Instance().GetTimeSlot()==1)
    {
        MSID2Traffic.clear();
        for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid)
        {
            MSID2Traffic[msid.ToInt()].initialize(msid.ToInt(), xrSingleOn, xrMultiOn, ftp3On, fullbufferOn, DatabaseOn);
        }
    }
    for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid)
    {
        MSID2Traffic[msid.ToInt()].WorkSlot();
    }
}
void MixedTraffic::OutputTrafficInfo()
{
    for (MSID msid = MSID::Begin(); msid <= MSID::End(); ++msid)
    {
        MSID2Traffic[msid.ToInt()].OutputTrafficInfo();
    }
}