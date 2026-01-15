//
// Created by ChTY on 2022/10/10.
//

#ifndef CHTY_MAC_MS_H
#define CHTY_MAC_MS_H
#include "LCG.h"
#include <unordered_map>
#include "../../TrafficModel/Universal_Packet.h"
#include <memory>


class MAC_MS {
private:
public:
    MAC_MS();
private:
    MS* ms;
public:
    void setMs(MS *ms);

private:
    bool shouldSendBSR;
    unordered_map<int,LCG> LCs;
    //chty 1031 begin
    int LCGNum;
    //chty 1031 end
    int minpirority;
    MyTimer ReTxBSRTimer;
    MyTimer PeriodicBSRTimer;
    vector<int> RegularBRR_ID;
    vector<double> RegularBRR_size;

    //cwq
    bool UL_Grant;


    void setBSRTimers();
/**********************************************************************
* 函数名称：SendBSR
* 功能描述：发送BSR
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void SendBSR();
/**********************************************************************
* 函数名称：CheckBSR
* 功能描述：检查BSR触发条件，确认是否需要发送BSR
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void CheckBSR();
public:

    bool IsBucketAvailable();

    void TimerRun();
    void SetUL_grant(bool);
    //add by zhaoyan
/**********************************************************************
* 函数名称：ReduceBuffer
* 功能描述：减少用户的缓存
* 输入参数：datasize：减少的缓存大小
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void ReduceBuffer(double& datasize);
/**********************************************************************
* 函数名称：ArrivePacket
* 功能描述：接收业务模型产生的数据包
* 输入参数：packet：业务包指针
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void ArrivePacket(const std::shared_ptr<Universal_Packet>& packet);
    //add by zhaoyan
/**********************************************************************
* 函数名称：DropTimeout
* 功能描述：丢弃缓存中超时的包
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 全局变量：无
* 修改记录：
* 其他说明：
* 修改日期        版 本 号    修 改 人        修改内容
* 2023.11.19     V1.0       楚文强          Create
************************************************************************/
    void DropTimeout();
    //chty 1031 end
    void WorkSlot(bool IsScheduled);
    double GetBuffer(int LCGID)
    {
        return LCs[LCGID].CalBuffer();
    }
};


#endif //CHTY_MAC_MS_H
