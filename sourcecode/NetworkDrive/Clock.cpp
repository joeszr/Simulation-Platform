///@file  Clock.cpp
///@brief 时钟类实现
///@author wangxiaozhou

#include "../Parameters/Parameters.h"
#include "Clock.h"
#include "../Statistician/Statistician.h"

///Clock构造函数实现

Clock::Clock() {
    m_iTime = 0;
    m_iReportPeriod = 20;
}

///推动时钟前进函数
void Clock::Forward() {
    ++m_iTime;
}

int Clock::GetTimeSlot()const {
    return m_iTime;
}
int Clock::GetTimeSlotInSF() const {
    return (m_iTime-1) % Parameters::Instance().BASIC.ISlotNumPerSFN;
}
int Clock::GetSFN() const{
    int sfn = (m_iTime - 1)/Parameters::Instance().BASIC.ISlotNumPerSFN;
    return sfn % 1024;
}
int Clock::GetSubFrame() const{
    int subframe = (m_iTime-1)/(Parameters::Instance().BASIC.ISlotNumPerSFN/10);
    return subframe%10;
}

void Clock::Reset() {
    m_iTime = 0;
}

Clock& Clock::Instance() {
    static Clock& m_Clock = *(new Clock);
    return m_Clock;
}

double Clock::GetTimeSec()const {
    return (Parameters::Instance().BASIC.DSlotDuration_ms / 1e3) * GetTimeSlot();
}