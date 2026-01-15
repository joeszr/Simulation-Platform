//
// Created by ChTY on 2022/10/10.
//

#include "MAC_BTS.h"
#include "../Parameters/Parameters.h"

void MAC_BTS::RecieveBSR(const std::shared_ptr<BSR> &bsr) {//接收BSR后更新缓存信息
    if (LCGbuffer.count(bsr->msid) == 0) {//没有记录过当前用户的情况下，向unordered_map中插入
        addmslock->lock();
        LCGbuffer.insert(std::make_pair(bsr->msid, unordered_map<int, double>()));
        addmslock->unlock();
    }
    unordered_map<int, double> &LCGsforMS = LCGbuffer[bsr->msid];//更新缓存信息
    for (auto i = 0; i < bsr->LCGID.size(); i++) {
        LCGsforMS[bsr->LCGID[i]] = BSRIndexConvertion5bits(bsr->buffersizes[i]);
    }
}

//add by zhaoyan begin
int MAC_BTS::checkLCGBuffer(const int& iUserId, double &dBufferSize) {
    int iRet = -1;
    dBufferSize = 0.0;
    const auto &autoIter = LCGbuffer.find(iUserId);
    if (autoIter != LCGbuffer.end()) {//若iUserId用户对应有缓存返回0
        for (auto &autoIterLCGBuffer: autoIter->second) {
            if (autoIterLCGBuffer.second > 0) {
                iRet = 0;
                break;
            }
        }
    }
    return iRet;
}
// add by chuwenqiang begin
void MAC_BTS::ReduceBuffer(const int& msid, double datasize){
    if (LCGbuffer.count(msid) == 0) {
        assert(false);
    }
    unordered_map<int, double> &LCGsforMS = LCGbuffer[msid];//更新缓存信息
    for (auto &it:LCGsforMS) {
        int LCGID = it.first;
        LCG2MSID_HistoryThroughput[LCGID][msid] += datasize;
        if(it.second >= datasize){
            it.second -= datasize;
            break;
        }
        else{
            int LCGID = it.first;
            LCG2MSID_HistoryThroughput[LCGID][msid] += it.second;
            datasize -= it.second;
            it.second = 0;
        }
    }
}
double MAC_BTS::GetSumBuffer(const int& msid){
    double sum = 0;
    unordered_map<int, double> &LCGsforMS = LCGbuffer[msid];//更新缓存信息
    for (auto &it:LCGsforMS) {
        sum += it.second;
    }
    return sum;
}
bool MAC_BTS::HaveAnyData(const int& msid)
{
    return GetSumBuffer(msid) > 0.0001;
}
double MAC_BTS::GetBufferOfHighestPriority(const int& msid){ //这个函数跟上面的checkLCGBuffer应该可以合并成一个
    const auto &autoIter = LCGbuffer.find(msid);
    if (autoIter != LCGbuffer.end()) {
        for (auto &autoIterLCGBuffer: autoIter->second) {
            if (autoIterLCGBuffer.second > 0.001) {
                return autoIterLCGBuffer.second;
            }
        }
    }
//
    return 0;
}

int MAC_BTS::GetLCGIDOfHighestPriority(const int& msid){
    const auto &autoIter = LCGbuffer.find(msid);
    if (autoIter != LCGbuffer.end()) {
        for (auto &autoIterLCGBuffer: autoIter->second) {
            if (autoIterLCGBuffer.second > 0.001) {
                return autoIterLCGBuffer.first;
            }
        }
    }
    return -1;
}

unordered_map<int,unordered_map<int,double>>& MAC_BTS::GetLGCbuffer(){
    return LCGbuffer;
}
double MAC_BTS::BSRIndexConvertion5bits(const int& index){
    vector<int> mapping{0, 10, 14, 20, 28, 38, 53, 74, 102, 142, 198, 276, 384, 535, 745, 1038, 1446, 2014, 2806, 3909,
                        5446, 7587, 10570, 14726, 20516, 28581, 39818, 55474, 77284, 107669, 150000};
    assert(index < mapping.size());
    double buffersize = mapping[index];
    buffersize = buffersize * 8 / 1000;
    return buffersize;
}
// add by chuwenqiang end
//add by zhaoyan end

MAC_BTS::MAC_BTS() {
    addmslock = std::shared_ptr<std::mutex>(new std::mutex);
    //初始化历史吞吐量,确保正比例分母不为0;LCGNum处理应该优化;MSNum也应该优化
    int LCGNum = 4;
    for(int i = 0;i < LCGNum;i++){
        //考虑了移动性，所以将msid的维度设成全部用户，不设成小区用户
        for(int msid = 0; msid < Parameters::Instance().BASIC.ITotalMSNum; msid++){
            LCG2MSID_HistoryThroughput[i][msid] = 0.001;
        }
    }
}

unordered_map<int,unordered_map<int,double>>&  MAC_BTS::GetHistoryThroughput(){
    return LCG2MSID_HistoryThroughput;
}

void MAC_BTS::AccumulateSuccessRxKbit_LCG(const int& msid,  int LCGID, double _dKbit){
    LCG2MSID_HistoryThroughput[LCGID][msid] += _dKbit;
}