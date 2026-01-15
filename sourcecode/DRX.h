//
// Created by CWQ on 2022/11/21.
//
#include "MobileStation/MSID.h"

#ifndef CWQ_DRX_H
#define CWQ_DRX_H
enum DRXState{
    state_active,
    state_sleep
};
enum DRXCycleState{
    state_long,
    state_short
};
struct DRXTimer{
    bool IsTimerOn;
    int value;
    int MaxVal;
    int offset;

    std::string state;
/**********************************************************************
* 函数名称：SetConfig
* 功能描述：计时器参数配置
* 输入参数：_IsTimerOn:计时器开关
          _MaxVal:计时器超时门限
          _offset：计时器启动延迟
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void SetConfig(bool _IsTimerOn, int _MaxVal , int _offset = 0);
    void Start();
    void StartAfterOffset(int);
    void Reset();
    void run();
    bool IsExpired();
    bool IsRunning();
};
class DRX {
private:
    MSID msid;
private:
    bool drxSwitch;

    DRXTimer onDurationTimer;
    DRXTimer InactivityTimer;

    bool DRXShortSwitch;
    int ShortCycle;
    DRXTimer ShortCycleTimer;
    int ShortOffset;

    int LongCycle;
    int LongOffset;

    int drxStartOffset;

    bool NewPDCCH;

private:
    DRXState drx_state;
    DRXCycleState CycleState;

public:
    DRX();
/**********************************************************************
* 函数名称：initialize
* 功能描述：DRX功能初始化
* 输入参数：_msid:DRX实体对应的MSID
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void initialize(MSID _msid);
/**********************************************************************
* 函数名称：DRXstate_WorkSlot
* 功能描述：DRX计时器更新，根据计时器状态确定DRX状态
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void DRXstate_WorkSlot();
/**********************************************************************
* 函数名称：InactivityTimer_WorkSlot
* 功能描述：DRX InactivityTimer更新
* 输入参数：_NewPDCCH：当前时隙是否有新传调度
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void InactivityTimer_WorkSlot(bool _NewPDCCH);

    inline DRXState GetDRXState(){
        return drx_state;
    }
    void DRXRecord();
};


#endif //CHTY_DRX_H
