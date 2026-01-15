//
// Created by ChTY on 2022/10/9.
//

#ifndef CHTY_LCG_H
#define CHTY_LCG_H
///应该能够处理不连续的QoS的问题，但是目前默认初始化QoS为0到TRAFFIC.IQoSLevel
///肯定不支持同一QoS对应多个逻辑信道的情况。
///目前MS没写初始化，故直接默认初始化
///timer各不同周期没有实现
#include <vector>
#include <unordered_map>
#include "NetworkDrive/MyTimer.h"
#include "../../TrafficModel/Universal_Packet.h"
#include <memory>
#include <deque>
class MS;
using namespace std;
class LogicalChannel{
    friend class LCG;
    friend class MAC_MS;
private:
    int LCGID;
    int QoS;
    double buffersize;
    bool havenew;
    //add by zhaoyan
    double m_dPBR;        //优先比特率PBR
    int    m_iBSD;        //桶大小持续时间BSD
    double m_dBucketVar;  //令牌桶变量B  （令牌桶变量单位kb）
    int    m_iLastTime;   //上次传输时间
    //add by zhaoyan
    std::deque<std::shared_ptr<Universal_Packet>> packetqueue;
public:
    //缓存长度增加，返回减小后的缓存大小值
    double BufferGrow(double arrived);
    //缓存长度减小，返回减小后的缓存大小值
    double BufferReduce(double reduced);
    //add by zhaoyan
    //更新令牌桶变量
    void   UpdateBucketVar(double& dBucketSize);
    //add by zhaoyan
    //chty 1031 begin
    //检查记录的缓存队列长度和实际的数据包队列是否吻合
    bool CheckValid() const;
    //超时丢包
    void DropTimeout();
    //chty 1031 end
    //指定QoS
    explicit LogicalChannel(const int&);
    //默认构造函数不应该调用，但为了unordered_map能使用[]，需要保留
    LogicalChannel();
};
class LCG {
    friend class MAC_MS;
private:
    //QoS-逻辑信道，一一对应，已存在的QoS不会插入新的逻辑信道
    unordered_map<int,LogicalChannel> m_LogicalChannels;
public:
    //返回m_LogicalChannels
//    unordered_map<int,LogicalChannel> GetLogicalChannel();
    //chty 1031 begin
    //当前LCG的最大QoS
    int m_iHighestQoS;
    //当前LCG的最小QoS
    int m_iLowestQoS;
    //chty 1031 end
    //添加一条指定QoS的逻辑信道，返回添加的位置和是否成功插入
    std::pair<unordered_map<int,LogicalChannel>::iterator,bool> AddLogicalChannel(const int& qos);
    //删除指定QoS的逻辑信道
    void RemoveLogicalChannel(const int& _qos);
    //缓存队列添加数据，返回更新后队列长度
    double ArrivePacket(const std::shared_ptr<Universal_Packet>& packet);//缓存队列添加数据，返回更新后队列长度
    //减小指定大小
    void ReduceBuffer(double &_datasize);
    //检测是否存在有数据的逻辑信道
    bool HaveAnyData();
    //检测是否有优先的数据
    bool HaveNewPirority(const int& maxqos);
    //计算整个LCG的buffer总大小
    double CalBuffer();
    //chty 1031 begin
    //超时丢包
    void DropTimeout();
    //chty 1031 end
    //构造函数指定最高QoS和最低QoS
    explicit LCG(const int& lowestqos , const int& highestqos);
    //默认构造函数不应该调用，但为了unordered_map能使用[]，需要保留
    LCG();
};

class BSR{
public:
    //发送的用户
    int msid;
    //记录LCG的ID
    vector<int> LCGID;
    //对应LCGID的buffer总大小
    vector<double> buffersizes;
    BSR(const int&, const vector<int>&, const vector<double>&);
};

#endif //CHTY_LCG_H
