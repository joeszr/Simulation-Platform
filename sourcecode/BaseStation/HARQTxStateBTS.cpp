///@file HARQTxStateBTS.cpp
///@brief BTS端HARQ发射状态管理类函数实现
///@author wangxiaozhou

#include "../NetworkDrive/Clock.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include"../MobileStation/MSID.h"
#include "../Scheduler/SchedulingMessageDL.h"
#include "HARQTxStateBTS.h"
std::mutex PrintNAKQueuelock;

bool HARQTxStateBTS::IsNeedReTx() {
    //    return m_HARQThreadTxState.IsNeedReTx();
    //    return m_vHARQThreadTxState[m_iHARQThreadID].IsNeedReTx();
    //todo
    return !m_qNAKMesQueue.empty();
}
void HARQTxStateBTS::PrintNAKQueue(bool before_or_affter) const {
    /*int time=Clock::Instance().GetTimeSlot();
    std::lock_guard<std::mutex>l(PrintNAKQueuelock);
    for(auto it=m_qNAKMesQueue.cbegin();it!=m_qNAKMesQueue.cend();it++){
        Observer::Print("NAKQueueInfo")<<time<<" ("<<BSid<<","<<BTSindex<<") "<<(before_or_affter?"before ":"after ")<<(*it)->GetScheduleMes()->ID<<" "
                <<(*it)->GetScheduleMes()->GetMSID().ToInt()<<" "<<(*it)->GetScheduleMes()->GetBornTime()<<" "
        <<(*it)->GetScheduleMes()->GetLatestTransTime()<<" "<<(*it)->GetScheduleMes()->GetiSendNum()<<" "<<endl;
    }*/
}



int HARQTxStateBTS::ReTxNum() {
    assert(false);
    //return m_qNAKMesQueue.size();
}

//void HARQTxStateBTS::WorkSlot(void) {
//    m_iHARQThreadID = (++m_iHARQThreadID) % Parameters::Instance().SIM.DL.IHARQProcessNum;
//}

int HARQTxStateBTS::GetHARQID() {
    assert(false);
    //return m_iHARQThreadID;
}

std::shared_ptr<ACKNAKMessageDL> HARQTxStateBTS::GetReTxMessage() {
    //    return m_HARQThreadTxState.GetACKNAKMessage();
    std::shared_ptr<ACKNAKMessageDL> pACKNAKMessage = m_qNAKMesQueue.front();
    m_qNAKMesQueue.pop_front();
    return pACKNAKMessage;
}

std::shared_ptr<ACKNAKMessageDL> HARQTxStateBTS::CheckReTxMessage() {
    //    return m_HARQThreadTxState.CheckACKNAKMessage();
    std::shared_ptr<ACKNAKMessageDL> pACKNAKMessage = m_qNAKMesQueue.front();
    return pACKNAKMessage;
}

void HARQTxStateBTS::ClearReTxMessage() {
    //    m_HARQThreadTxState.ClearACKNAKMesQueue();
    while (!m_qNAKMesQueue.empty()) {
        m_qNAKMesQueue.pop_front();
    }
}

void HARQTxStateBTS::RegistReTxMessage(const std::shared_ptr<ACKNAKMessageDL>& _pACKNAKMessage) {
    //    m_HARQThreadTxState.RegistACKNAKMessage(_pACKNAKMessage);
    m_qNAKMesQueue.push_back(_pACKNAKMessage);
}

void HARQTxStateBTS::Reset() {
    //    m_iHARQThreadID = 0;
    m_qNAKMesQueue.clear();
}

bool HARQTxStateBTS::CanReTx(const deque<std::shared_ptr<ACKNAKMessageDL> >::iterator& _p) {
    int iTime = Clock::Instance().GetTimeSlot();
    return (*_p)->GetScheduleMes()->CanReTx(iTime);
}

SafeDeque<std::shared_ptr<ACKNAKMessageDL> >& HARQTxStateBTS::GetACKNAKMesQueue() {
    return m_qNAKMesQueue;
}