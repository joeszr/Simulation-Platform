//
// Created by AAA on 2022/9/20.
//
#pragma once

#include "../MobileStation/MSID.h"

class PHR {

private:
    MSID msid;

    bool PHR_On;   //开关

    int PeriodicTimer;  //周期计时器
    int Period;        //周期
    int ProhibitTimer;  //禁止计时器
    int ProhibitPeriod;

    double PowerFactorChange_dB;  //路损变化门限
    double LastPLdB;
    double LastTxPower_dBm;

    bool IsTriggered;



public:
    void timerRun();
/**********************************************************************
* 函数名称：Workslot
* 功能描述：判断是否满足触发PHR的条件，如果满足，触发PHR
* 输入参数：max_TxPower_dBm:UE 最大发射功率
          EstimateTxPower_dBm:UE根据功控公式计算的发射功率
          actual_TxPower_mW：UE的实际发射功率
          pathloss：UE和BS的路损
          newResource：是否新传调度
* 输出参数： 无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void Workslot(double max_TxPower_dBm, double EstimateTxPower_dBm, double actual_TxPower_mW, double pathloss, bool newResource);
/**********************************************************************
* 函数名称：GeneratePHR
* 功能描述：产生PHR消息
* 输入参数：max_TxPower_dBm:UE 最大发射功率
          EstimateTxPower_dBm:UE根据功控公式计算的发射功率
* 输出参数：phr_index：上报的PHR等级
* 返 回 值：phr_index
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    int GeneratePHR(double MaxTxPower_dBm, double EstimatedTxPower_dBm);

    void initialize(const MSID& id);
    void Reset();
    void SetConfig(int Period, int ProhibitPeriod, double PowerFactorChange_dB);

    PHR() = default;
    ~PHR() = default;
};

