//
// Created by AAA on 2022/10/4.
//
#pragma once
#include"../MobileStation/MSID.h"
#include"../Utility/Include.h"
//这个ULGrant应该是调度信息，只是现在先不考虑BSR的资源占用，所以先用一个消息代替
struct ULGrant{
    MSID msid;
    int time;
    bool IsAllowed;
};
struct SRMessage{
    int BornTime;
    MSID m_ID;
    bool State;    //0 pending  1 triggered
};
class SR {

private:
    MSID msid;

    bool IsSROn;

    bool IsTriggered;

    int PendingSRNum;

    double ProhibitTimer_ms;
    bool ProhibitTimerKey;

    int sr_Periodicity_slot;
    int sr_Offset_slot;

    int TransMax;
    int Counter;
    double LastTxTime_ms;

    void SendSR();
    void Reset();
    void CalculateOffset(const int& Periodicity);

public:
    void ReceiveULGrant(ULGrant& UlGrant);
    void initialize(const MSID& id);
    void SetConfig(const double& _ProhibitTimer, const int& _TransMax, const int& _Counter, const int& Period, const int& offset);
/**********************************************************************
 * 函数名称：GenerateSR
 * 功能描述：触发SR
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：无
 * 全局变量：无
 * 修改记录：
 * 其他说明：
 * 修改日期        版 本 号    修 改 人        修改内容
 * 2023.11.19     V1.0       楚文强          Create
 ************************************************************************/
    void GenerateSR();
/**********************************************************************
* 函数名称：WorkSlot
* 功能描述：判断是否触发SR，如果已经触发且满足发送条件，发送SR
* 输入参数：IsSchM是否有新传调度
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void WorkSlot(bool IsSchM);
    bool IsPending();
    SR();
    ~SR() = default;
};


