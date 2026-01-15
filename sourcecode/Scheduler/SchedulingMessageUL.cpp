///@file SchedulingMessageUL.cpp
///@brief SchedulingMessageUL类的实现
///@author wangsen

#include "../Utility/IDType.h"
#include "../MobileStation/MSID.h"
#include "../Utility/RBID.h"
#include "../Utility/SBID.h"
#include "../NetworkDrive/Clock.h"
#include "SchedulingMessageUL.h"
#include "../LinklevelInterface/LinkLevelInterface.h"

SchedulingMessageUL::SchedulingMessageUL(MSID _msid, int _iHARQID) :m_mRB2vMSID(Parameters::Instance().BASIC.IRBNum){
    m_MSID = _msid;
    m_iHARQID = _iHARQID;
    m_iSendNum = 0;
    m_iBornTime = Clock::Instance().GetTimeSlot();
    m_iTBSize = -1;
    m_iK2Slot = -1;
}
SchedulingMessageUL::SchedulingMessageUL(MSID _msid, int _iHARQID, std::shared_ptr<Packet> packet) :m_mRB2vMSID(Parameters::Instance().BASIC.IRBNum){
    m_MSID = _msid;
    m_iHARQID = _iHARQID;
    m_iSendNum = 0;
    m_iBornTime = Clock::Instance().GetTimeSlot();
    //默认另外的用户是不用重传的

    m_iTBSize = -1;

    pPacket = packet;
}

///获得被调度MSID

MSID SchedulingMessageUL::GetMSID() {
    return m_MSID;
}

///获得本次调度分配的进程号

int SchedulingMessageUL::GetHARQID() const{
    return m_iHARQID;
}

///获得被调度MS使用的调制编码方式

imat SchedulingMessageUL::GetMCSmat() {
    return m_mMCS;
}

///获得分配给MS的RB组

vector<RBID>& SchedulingMessageUL::GetRBUsed() {
    return m_vRBUsed;
}

int SchedulingMessageUL::GetSendNum() const{
    return m_iSendNum;
}




void SchedulingMessageUL::SetRB2vMSID(const std::unordered_map<int, vector<MSID> >& _mRB2MS) {
    m_mRB2vMSID = _mRB2MS;
}

std::unordered_map<int, vector<MSID> >& SchedulingMessageUL::GetRB2vMSID() {
    return m_mRB2vMSID;
}

void SchedulingMessageUL::SetMCS(int _iMCS) {
    m_iMCS = _iMCS;
}
void SchedulingMessageUL::SetMCSmat(const imat& _mMCS) {
    m_mMCS = _mMCS;
}

void SchedulingMessageUL::SetRBUsed(const vector<RBID>& _vRBUsed) {
    m_vRBUsed = _vRBUsed;
}

vector<MSID> SchedulingMessageUL::GetVmimoGroup() {
    return m_vVmimoGroup;
}

void SchedulingMessageUL::IncreaseSendNum() {
    m_iSendNum += 1;
}

int SchedulingMessageUL::GetBornTime() const{
    return m_iBornTime;
}
void SchedulingMessageUL::SetSINR(double sinr){
    EstimateSINR = sinr;
}

bool SchedulingMessageUL::ReSchMessageAdjust(bool _bACK) {
    bool bIsSend = false; ///初始不需要重传
    assert(m_iSendNum < Parameters::Instance().SIM_UL.UL.IHARQMaxTransNum);
    if ((!_bACK) && (m_iSendNum < Parameters::Instance().SIM_UL.UL.IHARQMaxTransNum)) {
        bIsSend = true; ///传错需要反馈ACKNAKMessageDL
        IncreaseSendNum(); ///传输次数加1
    }
    return bIsSend;
}

void SchedulingMessageUL::SetTBSizeKbit() {
    int iRENum_AllRB = 0;
//    for (auto& it : m_mRB2vMSID) {
//        int iLayerNum = static_cast<int> (it.second.size())*1;
//        iRENum_AllRB += LinkLevelInterface::Instance_UL().GetRENum(1, iLayerNum);
//    }
    for(auto& it:m_vRBUsed) {
        int iLayerNum = static_cast<int> (m_mRB2vMSID[it.ToInt()].size()) * 1;   //iLayerNum 表示该RB对应的用户数
        iRENum_AllRB += LinkLevelInterface::Instance_UL().GetRENum(1, iLayerNum);  //iLayerNum 在GetRENum这个函数中没有调用
    }
    m_iTBSize = LinkLevelInterface::Instance_UL().MCS2TBSKBit(m_mMCS, iRENum_AllRB);

}
void SchedulingMessageUL::SetTBSizeKbitWithBuffer(double RemainUntransmittedBuffer) {
    int iRENum_AllRB = 0;
//    for (auto& it : m_mRB2vMSID) {
//        int iLayerNum = static_cast<int> (it.second.size())*1;
//        iRENum_AllRB += LinkLevelInterface::Instance_UL().GetRENum(1, iLayerNum);
//    }
    for(auto& it:m_vRBUsed) {
        int iLayerNum = static_cast<int> (m_mRB2vMSID[it.ToInt()].size()) * 1;
        iRENum_AllRB += LinkLevelInterface::Instance_UL().GetRENum(1, iLayerNum);
    }

    m_iTBSize = LinkLevelInterface::Instance_UL().MCS2TBSKBit(m_mMCS, iRENum_AllRB);



    if(m_iTBSize > RemainUntransmittedBuffer){
        m_iTBSize = RemainUntransmittedBuffer;
    }

//    cout  << " m_iTBSize = " << m_iTBSize << endl;
}
void SchedulingMessageUL::SetTBSizeKbit_withPacket() {
    int iRENum_AllRB = 0;
//    for (auto& it : m_mRB2vMSID) {
//        int iLayerNum = static_cast<int> (it.second.size())*1;
//        iRENum_AllRB += LinkLevelInterface::Instance_UL().GetRENum(1, iLayerNum);
//    }
    for(auto& it:m_vRBUsed) {
        int iLayerNum = static_cast<int> (m_mRB2vMSID[it.ToInt()].size())*1;
        iRENum_AllRB += LinkLevelInterface::Instance_UL().GetRENum(1, iLayerNum);
    }
    m_iTBSize = LinkLevelInterface::Instance_UL().MCS2TBSKBit(m_mMCS, iRENum_AllRB);
    if(m_iTBSize > pPacket->GetRemainUntransmitted_SizeKbits()){
        m_iTBSize = pPacket->GetRemainUntransmitted_SizeKbits();
    }
    pPacket->ReduceUntransmittedSizeKbits(m_iTBSize);
}

double SchedulingMessageUL::GetTBSizeKbit() const{
    return m_iTBSize;
}
int SchedulingMessageUL::GetRank() const{
    return m_iRank;
}
//@threads
void SchedulingMessageUL::SetRank(int _iRI){
    m_iRank = _iRI;
}

void SchedulingMessageUL::SetBornTime(int Time){
    assert(Time>m_iBornTime);
    m_iBornTime=Time;
}

void SchedulingMessageUL::SetvVmimoGroup(vector<MSID> _vVmimoGroup){
    m_vVmimoGroup = _vVmimoGroup;
}

std::shared_ptr<Packet> SchedulingMessageUL::GetFirstPacket(){
    return pPacket;
}

void SchedulingMessageUL::SetCCE(const int _ccelevel, vector<int>& _cceSB){
    CCELevel = _ccelevel;
    CCESB = _cceSB;
}
int SchedulingMessageUL::GetCCELevel(){
    return CCELevel;
}
vector<int>& SchedulingMessageUL::GetCCESB(){
    return CCESB;
}

void SchedulingMessageUL::SetCodeWord(const RBID& _rbid, const cmat& _mCodeWord)
{
    m_mSCID2CodeWord[_rbid.ToInt()] = _mCodeWord;
}
cmat SchedulingMessageUL::GetCodeWord(const RBID& _rbid)
{
    return m_mSCID2CodeWord[_rbid.ToInt()];
}