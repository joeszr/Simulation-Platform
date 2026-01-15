//
// Created by ChTY on 2022/10/9.
//

#include "LCG.h"
#include <algorithm>
#include <iostream>
#include "Parameters/Parameters.h"
#include "../NetworkDrive/Clock.h"

double LogicalChannel::BufferGrow(double arrived) {
    if (!havenew) {
        havenew = (buffersize == 0);//若更新前大小为0，且之前没有新数据，记录当前逻辑信道有新数据到达
    }
    buffersize += arrived;
    //assert(CheckValid());//调试时错误时可用于检测
    return buffersize;
}

double LogicalChannel::BufferReduce(double reduced) {//reduced时逻辑信道这次可传输大小
    //add by zhaoyan begin
    assert(reduced >= 0);
    m_iLastTime = Clock::Instance().GetTimeSlot();//更新最后传输时间
    m_dBucketVar -= reduced;//令牌桶变量改变
    //add by zhaoyan end
    assert(buffersize >= 0);//当前逻辑信道要减小缓存应保证数据包队列不为空且记录的总大小buffersize>0
    assert(!packetqueue.empty());
    while (!packetqueue.empty() && reduced > 0) {//没有待传输的包或用完可传输大小，退出循环
        //chty 1031 begin
        auto firstpacket = packetqueue.front();
        double remain = firstpacket->GetOriginSizeKbits() - firstpacket->GetSuccessRecieved_SizeKbits();
        if (remain >= reduced) {//不能传完包，就把可传输大小用完
            firstpacket->RecordSuccessRecievedSizeKbits(reduced);
            buffersize -= reduced;
            reduced = 0;
            //assert(CheckValid());//调试时错误时可用于检测
        } else {//能传完包，包要出队并记录
            firstpacket->RecordSuccessRecievedSizeKbits(remain);
            firstpacket->LogFinishedPacket();
            packetqueue.pop_front();
            reduced -= remain;
            buffersize -= remain;
            //assert(CheckValid());//调试时错误时可用于检测
        }
    }
    //chty 1031 end
    return buffersize;
}

//add by zhaoyan
void LogicalChannel::UpdateBucketVar(double &dBucketSize) {//令牌桶更新方法
    int iTime = Clock::Instance().GetTimeSlot() - m_iLastTime;
    m_dBucketVar += iTime * m_dPBR;
    m_dBucketVar = min(m_dBucketVar, m_dPBR * m_iBSD);
    dBucketSize = buffersize;
    return;
}
//add by zhaoyan

//chty 1031 begin
bool LogicalChannel::CheckValid() const {//检查队列实际大小和buffersize是否一致
    double actualsize = 0.0;
    for (const auto &it: packetqueue) {
        actualsize += it->GetOriginSizeKbits() - it->GetSuccessRecieved_SizeKbits();
    }
    return abs(actualsize - buffersize) <= 0.01;
}

void LogicalChannel::DropTimeout() {//检查数据包是否超时，超时则出队并记录
    auto it = packetqueue.begin();
    int currenttime = Clock::Instance().GetTimeSlot();
    while (it != packetqueue.end()) {
        if ((*it)->IsTimeout(currenttime)) {
            (*it)->DropPacket_DueToTimeout();
            (*it)->LogFinishedPacket();
            buffersize -= ((*it)->GetOriginSizeKbits() - (*it)->GetSuccessRecieved_SizeKbits());
            it = packetqueue.erase(it);
            assert(CheckValid());
        } else {
            it++;
        }
    }
}
//chty 1031 end



LogicalChannel::LogicalChannel(const int &_q) {
    QoS = _q;
    buffersize = 0;
    havenew = false;
    //chty 1031 begin
    m_dBucketVar = 0;
    m_iLastTime = 0;
    m_dPBR = 128 * 8 * Parameters::Instance().BASIC.DSlotDuration_ms;//128*8*10^6bit/s=128*8kb/ms
    m_iBSD = 1000 / Parameters::Instance().BASIC.DSlotDuration_ms;//1s
    //chty 1031 end
}

LogicalChannel::LogicalChannel() {//不允许插入无qos和id的逻辑信道
    assert(false);
}


std::pair<unordered_map<int, LogicalChannel>::iterator, bool> LCG::AddLogicalChannel(const int &_q) {
    assert(_q <= m_iHighestQoS && _q >= m_iLowestQoS);//插入的逻辑信道的QoS合法
    return m_LogicalChannels.insert(pair<int, LogicalChannel>(_q, LogicalChannel(_q)));
}

void LCG::RemoveLogicalChannel(const int &_qos) {
    if (m_LogicalChannels.find(_qos) != m_LogicalChannels.end()) {
        cout << "No LogicalChannel to Remove";
        assert(false);
    }
    m_LogicalChannels.erase(_qos);
}

double LCG::CalBuffer() {//对所有逻辑信道的buffer加和
    double buff = 0;
    for (const auto &lc: m_LogicalChannels) {
        buff += lc.second.buffersize;
    }
    return buff;
}

LCG::LCG(const int &lowestqos, const int &hightestqos) {
    m_iHighestQoS = hightestqos;
    m_iLowestQoS = lowestqos;
//    assert(false);
}

LCG::LCG() { assert(false); }

bool LCG::HaveAnyData() {//遍历包含的逻辑信道进行检查
    for (const auto &lc: m_LogicalChannels) {
        if (lc.second.buffersize > 0.0001) {
            return true;
        }
    }
    return false;
}

bool LCG::HaveNewPirority(const int &maxqos) {//遍历包含的逻辑信道进行检查
    for (auto lc: m_LogicalChannels) {
        if (lc.second.havenew && lc.second.QoS < maxqos) {
            return true;
        }
        lc.second.havenew = false;
    }
    return false;
}

double LCG::ArrivePacket(const std::shared_ptr<Universal_Packet> &packet) {
    int qos = packet->qos;
    double _datasize = packet->GetOriginSizeKbits();
    auto it = find_if(m_LogicalChannels.begin(), m_LogicalChannels.end(), [=](auto &lc) { return lc.second.QoS == packet->qos; });
    if (it == m_LogicalChannels.end()) {
        //chty 1031 begin
        auto &lc = AddLogicalChannel(qos).first->second;
        lc.packetqueue.emplace_back(packet);
        return lc.BufferGrow(_datasize);
        //chty 1031 end
    } else {
        //chty 1031 begin
        it->second.packetqueue.emplace_back(packet);
        return it->second.BufferGrow(_datasize);
        //chty 1031 end
    }
}

//add by zhaoyan
void LCG::ReduceBuffer(double &_datasize) {
    //double ReduceDataSize = _datasize;
    for (auto i = m_iLowestQoS; i <= m_iHighestQoS; i++)
    {        //chty 1031 begin
        auto it = m_LogicalChannels.find(i);
        if (it == m_LogicalChannels.end())continue;
        if (it->second.m_dBucketVar > 0 && it->second.buffersize >= 0.001)//令牌桶变量>0且有待传输数据
            //chty 1031 end
        {
            if (it->second.buffersize >= _datasize) {//用完可传输大小，直接返回
                it->second.BufferReduce(_datasize);
                return;
            } else {
                _datasize -= it->second.buffersize;
                it->second.BufferReduce(it->second.buffersize);//这一逻辑信道的缓存全部传完

            }
        }
    }
    return;
}

//chty 1031 begin
void LCG::DropTimeout() {//让每个逻辑信道进行超时丢包
    for (auto &autoLC: m_LogicalChannels) {
        autoLC.second.DropTimeout();
    }
}
//chty 1031 end
//add by zhaoyan

int BSRIndexConvertion5bits(double buffersize) {//38.321,5bit的buffersize映射，Table 6.1.3.1-1
    double temp = buffersize * 1000 / 8;
    int index = 0;
    vector<int> mapping{0, 10, 14, 20, 28, 38, 53, 74, 102, 142, 198, 276, 384, 535, 745, 1038, 1446, 2014, 2806, 3909,
                        5446, 7587, 10570, 14726, 20516, 28581, 39818, 55474, 77284, 107669, 150000};
    auto it = std::find_if(mapping.begin(), mapping.end(), [&](int t) { return t >= temp; });
    index = it - mapping.begin();
    if(index == mapping.size())
    {
        index--;
    }
    return index;
}

int BSRIndexConvertion8bits(double buffersize) {//38.321,8bit的buffersize映射，Table 6.1.3.1-2
    double temp = buffersize * 1000 / 8;
    int index = 0;
    vector<int> mapping{0, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                        19, 20, 22, 23, 25, 26, 28, 30, 32, 34, 36, 38, 40, 43, 46, 49, 52, 55, 59, 62, 66, 71, 75, 80,
                        85, 91, 97, 103, 110, 117, 124, 132, 141, 150, 160, 170, 181, 193, 205, 218, 233, 248, 264, 281,
                        299, 318, 339, 361, 384, 409, 436, 464, 494, 526,
                        560, 597, 635, 677, 720, 767, 817, 870, 926, 987, 1051, 1119, 1191, 1269, 1351, 1439, 1532,
                        1631, 1737, 1850, 1970, 2098, 2234, 2379, 2533, 2698, 2873, 3059, 3258, 3469, 3694, 3934, 4189,
                        4461, 4751, 5059, 5387, 5737, 6109, 6506, 6928, 7378, 7857, 8367, 8910, 9488, 10104, 10760,
                        11458, 12202, 12994, 13838, 14736, 15692, 16711, 17795, 18951, 20181, 21491, 22885, 24371,
                        25953, 27638, 29431,
                        31342, 33376, 35543, 37850, 40307, 42923, 45709, 48676, 51836, 55200, 58784, 62599, 66663,
                        70990, 75598, 80505, 85730, 91295, 97221, 103532, 110252, 117409, 125030, 133146, 141789,
                        150992, 160793, 171231, 182345, 194182, 206786, 220209, 234503, 249725, 265935, 283197, 301579,
                        321155, 342002, 364202, 387842, 413018, 439827, 468377, 498780,
                        531156, 565634, 602350, 641449, 683087, 727427, 774645, 824928, 878475, 935498, 996222, 1060888,
                        1129752, 1203085, 1281179, 1364342, 1452903, 1547213, 1647644,
                        1754595, 1868488, 1989774, 2118933, 2256475, 2402946, 2558924, 2725027, 2901912, 3090279,
                        3290873, 3504487, 3731968, 3974215, 4232186, 4506902, 4799451, 5110989, 5442750, 5796046,
                        6172275, 6572925, 6999582, 7453933, 7937777, 8453028, 9001725, 9586039, 10208280, 10870913,
                        11576557, 12328006, 13128233, 13980403, 14887889, 15854280, 16883401, 17979324, 19146385,
                        20389201, 21712690, 23122088, 24622972, 26221280, 27923336, 29735875, 31666069, 33721553,
                        35910462, 38241455, 40723756, 43367187, 46182206, 49179951, 52372284, 55771835, 59392055,
                        63247269, 67352729, 71724679, 76380419, 81338368};
    auto it = std::find_if(mapping.begin(), mapping.end(), [&](int t) { return t >= temp; });
    index = it - mapping.begin();
    return index;
}

BSR::BSR(const int &_msid, const vector<int> &LCGIds, const vector<double> &buffer) : msid(_msid), LCGID(LCGIds) {
    buffersizes = vector<double>(buffer.size(), 0);
    for (auto i = 0; i < buffer.size(); i++) {
        buffersizes[i] = BSRIndexConvertion5bits(buffer[i]);
        //buffersizes[i] = (buffer[i]);
    }
}

//unordered_map<int,LogicalChannel> LCG::GetLogicalChannel(){
//    return m_LogicalChannels;
//}