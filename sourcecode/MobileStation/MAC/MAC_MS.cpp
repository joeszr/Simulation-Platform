//
// Created by ChTY on 2022/10/10.
//

#include "MAC_MS.h"
#include <assert.h>
#include "Utility/functions.h"
#include "NetworkDrive/Clock.h"
#include "../MS.h"
#include "./MSTxer.h"

void MAC_MS::setBSRTimers() {
    ReTxBSRTimer.setmax(Parameters::Instance().MSS_UL.UL.BSR_RetransTimer * pow(2,Parameters::Instance().BASIC.ISCSConfig));
    ReTxBSRTimer.reset();
    PeriodicBSRTimer.setmax(Parameters::Instance().MSS_UL.UL.BSR_PeriodicTimer * pow(2,Parameters::Instance().BASIC.ISCSConfig));
    PeriodicBSRTimer.reset();
}
void MAC_MS::SetUL_grant(bool grant){
    UL_Grant = grant;
    ReTxBSRTimer.reset();
}
void MAC_MS::TimerRun(){
    ReTxBSRTimer.add();
    PeriodicBSRTimer.add();
}
void MAC_MS::CheckBSR() {

    if(!shouldSendBSR)
    {
//    ReTxBSRTimer.add();
//    PeriodicBSRTimer.add();
        if (ReTxBSRTimer.CheckTimeout() > 0) {
            for (auto lcg: LCs) {
                if (lcg.second.HaveAnyData()) {
                    shouldSendBSR = true;
                    break;
                }
            }
        }
        if (!shouldSendBSR) {
            for (auto lcg: LCs) {
                if (lcg.second.HaveNewPirority(minpirority)) {
                    shouldSendBSR = true;
                    break;
                }
            }
        }
        if (!shouldSendBSR && PeriodicBSRTimer.CheckTimeout() > 0) {
//        PeriodicBSRTimer.reset();
            shouldSendBSR = true;
        }

    }
}

void MAC_MS::SendBSR() {
    vector<int>LCGID;
    vector<double>buffersize;
    for(auto lcg:LCs)
    {
        double buff=lcg.second.CalBuffer();
        LCGID.emplace_back(lcg.first);
        buffersize.emplace_back(buff);
    }
//    vector<int> a{0,1,2,3};
//    vector<double> b{0,0,10,1};
//    std::shared_ptr<BSR> bsr=std::make_shared<BSR>(1,a, b);
    std::shared_ptr<BSR> bsr=std::make_shared<BSR>(ms->m_ID.ToInt(),LCGID, buffersize);
    shouldSendBSR=false;
    ReTxBSRTimer.reset();
    PeriodicBSRTimer.reset();
    ms->GetUci()->CollectBSR(bsr);
    ms->GetUci()->SendBSR();
    ///发送操作没有实现
}
std::mutex MS_buffer_lock;
void MAC_MS::WorkSlot(bool IsScheduled) {
    CheckBSR();
    int time=Clock::Instance().GetTimeSlot();
    //如果关闭SR功能，则默认UE已经获得了上行授权
    if(Parameters::Instance().MSS_UL.UL.IsSROn == false)
    {
        UL_Grant = true;
    }
    if(shouldSendBSR && DownOrUpLink(time) == 1)
    {

        if(IsScheduled || UL_Grant)
//        if(UL_Grant)
        {
            SendBSR();
//            UL_Grant = false;
        }
        else
        {
            (ms->UL.m_pMSTxer)->GetSR().GenerateSR();
        }
    }
    DropTimeout();
    ///不确定是否这样维持
    minpirority=100;
    for(auto lcg:LCs)
    {
        if(lcg.second.HaveAnyData())
        {
            minpirority=min(minpirority,lcg.second.m_iHighestQoS);
        }
    }
    int iTime = Clock::Instance().GetTimeSlot();
    MSID msid = ms->GetID();
    BTSID btsid = ms->GetMainServBTS();
    MS_buffer_lock.lock();
    Observer::Print("MS_buffer_record")<<iTime
                        << setw(20) << btsid
                        << setw(20) << msid
                        << setw(20) << LCs[0].CalBuffer()
                        << setw(20) << LCs[1].CalBuffer()
                        << setw(20) << LCs[2].CalBuffer()
                        << setw(20) << LCs[3].CalBuffer() <<endl;
    MS_buffer_lock.unlock();
}

MAC_MS::MAC_MS() {
    UL_Grant = false;  //这里应该初始化为false ，等SR完成后再改
    shouldSendBSR=false;
    setBSRTimers();
    //平台供设置4个LCG，优先级数字小，优先级更高
    LCs.insert(std::make_pair(0,LCG(0,0)));
    LCs.insert(std::make_pair(1,LCG(1,8)));
    LCs.insert(std::make_pair(2,LCG(9,12)));
    LCs.insert(std::make_pair(3,LCG(13,16)));
//    LCs.insert(std::make_pair(0,LCG(0,0)));
//    LCs.insert(std::make_pair(1,LCG(1,1)));
//    LCs.insert(std::make_pair(2,LCG(2,2)));
//    LCs.insert(std::make_pair(3,LCG(3,3)));
    LCGNum = 4;
    minpirority = 100; //初始化一个比较大的值
}

void MAC_MS::ArrivePacket(const std::shared_ptr<Universal_Packet>& packet) {
    int lcgid = -1;
    int p = packet->qos;
    if (p == 0) {
        lcgid = 0;
    } else if (1 <= p && p <= 8) {
        lcgid = 1;
    } else if (9 <= p && p <= 12) {
        lcgid = 2;
    } else if (13 <= p && p <= 16) {
        lcgid = 3;
    } else {
        cout << "invalid Packet QoS" << endl;
        assert(false);
    }
    LCs[lcgid].ArrivePacket(packet);
}

//add by zhaoyan begin
void MAC_MS::ReduceBuffer(double &datasize) {
//    for (auto i = 0; i < LCGNum; i++) {
//        auto &autoLcs = LCs[i];
//    }
    //chty 1031 begin
    for (auto i = 0; i < LCGNum; i++) {
        auto &autoLCG = LCs[i];//MAC初始化时LCG是连续的，一定存在
        double dSumBufferSize = 0.0;
        for (auto &autoLogicalChannel: autoLCG.m_LogicalChannels) {
            double dBufferSize = 0.0;
            autoLogicalChannel.second.UpdateBucketVar(dBufferSize);
            dSumBufferSize += dBufferSize;
        }
        //chty 1031 begin
        if (dSumBufferSize <= 0.0)continue;
        //chty 1031 end
        if (datasize >= dSumBufferSize) {
            autoLCG.ReduceBuffer(dSumBufferSize);
        } else {
            autoLCG.ReduceBuffer(datasize);
            break;
        }
    }
    bool HaveAnyData = false;
    for (auto i = 0; i < LCGNum; i++)
    {
        HaveAnyData = HaveAnyData || LCs[i].HaveAnyData();
    }
    if(HaveAnyData == false)
    {
        UL_Grant = false;
    }
    //chty 1031 end
    return;
}

void MAC_MS::setMs(MS * pms) {
    ms = pms;
}

void MAC_MS::DropTimeout() {
    for(auto& autoLCG : LCs){
        autoLCG.second.DropTimeout();
    }
}
//add by zhaoyan end

//add by ljq begin

//用以剔除由于BJ令牌桶中无法调度的用户
bool MAC_MS::IsBucketAvailable() {

    //遍历所以逻辑信道组中所有的逻辑信道
    for(auto &LogicalChannelGroup: LCs){
        unordered_map<int, LogicalChannel> mLogicalChannels = LogicalChannelGroup.second.m_LogicalChannels;
        for(auto &LC : mLogicalChannels){
//            if(LC.second.m_dBucketVar > 0 && !LC.second.packetqueue.empty())  //确定包队列不为空的逻辑信道，其令牌桶变量>0
            if(LC.second.m_dBucketVar > 0)  //确定包队列不为空的逻辑信道，其令牌桶变量>0
                return true;
        }
        return false;
    }
}
//add by ljq end


