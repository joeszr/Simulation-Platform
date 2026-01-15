//
// Created by cwq on 2022/9/17.
//

#include "DCI.h"
#include "NetworkDrive/Clock.h"
#include "MobileStation/MS.h"
#include "BaseStation/BTS.h"
#include "MobileStation/MSID.h"
#include "MobileStation/MSTxer.h"
#include"./Scheduler/SchedulingMessageDL.h"
#include"./Scheduler/SchedulingMessageUL.h"
#include"./BaseStation/ACKNAKMessageUL.h"

DCI::~DCI(){
    m_qSchedulingMessageQueueDL.clear();
    m_qSchedulingMessageQueueUL.clear();
    ACKMessageQueueUL.clear();
    TPCMessageQueue.clear();
    ULGrantQueue.clear();
}

void DCI::initialize(const BTSID& id){
    btsID = id;
}
void DCI::Reset(){
    m_qSchedulingMessageQueueDL.clear();
    m_qSchedulingMessageQueueUL.clear();
    ACKMessageQueueUL.clear();
    TPCMessageQueue.clear();
    ULGrantQueue.clear();
}

void DCI::WorkSlot(){
    SendSchMesDL();
    SendSchMesUL();
    SendACKMesUL();
    SendTPCMes();
    SendULGrant();
}

void DCI::CollectSchMesDL(const std::shared_ptr<SchedulingMessageDL>& Sch){
    m_qSchedulingMessageQueueDL.push_back(Sch);
}
void DCI::CollectSchMesUL(const std::shared_ptr<SchedulingMessageUL>& Sch){
    m_qSchedulingMessageQueueUL.push_back(Sch);
}
void DCI::CollectACKUL(const std::shared_ptr<ACKNAKMessageUL>& _pACKNAKMessageUL){
    ACKMessageQueueUL.push_back(_pACKNAKMessageUL);
    MSID msid = _pACKNAKMessageUL->GetSchMessage()->GetMSID();
}

void DCI::CollectTPC(const TPCMessage& TPCMes){
    TPCMessageQueue.push_back(TPCMes);
}
void DCI::CollectULGrant(const ULGrant& ulgrant){
    ULGrantQueue.push_back(ulgrant);
}



SafeDeque<std::shared_ptr<SchedulingMessageDL>> * DCI::GetSchMesQueDL(){
    return &m_qSchedulingMessageQueueDL;
}
std::deque<std::shared_ptr<SchedulingMessageUL> >* DCI::GetSchMesQueUL(){
    return &m_qSchedulingMessageQueueUL;
}
SafeDeque<std::shared_ptr<ACKNAKMessageUL> >* DCI::GetACKMesQueUL(){
    return &ACKMessageQueueUL;
}
SafeDeque<TPCMessage>* DCI::GetTPCMesQue(){
    return &TPCMessageQueue;
}



void DCI::SendSchMesDL(){
    BTS& bts = btsID.GetBTS();
    bts.SendSchedulingMessage();
}
void DCI::SendSchMesUL(){
    BTS& bts = btsID.GetBTS();
    bts.UL.SendSchedulingMessageUL();
}
void DCI::SendACKMesUL(){
    while(!ACKMessageQueueUL.empty()){
        std::shared_ptr<ACKNAKMessageUL> ACKM = ACKMessageQueueUL.front();
        std::shared_ptr<SchedulingMessageUL> pSM = ACKM->GetSchMessage();
        MSID msid = pSM->GetMSID();
        MS& ms = msid.GetMS();
        ms.UL.ReceiveACK(ACKM);
        ACKMessageQueueUL.pop_front();
    }
}
void DCI::SendTPCMes(){
    while(!TPCMessageQueue.empty()){
        TPCMessage TPCMes = TPCMessageQueue.front();
        MSID msid = TPCMes.msid;
        MS& ms = msid.GetMS();
        ms.UL.m_pMSTxer->ReceiveTPCCommand(TPCMes);
        TPCMessageQueue.pop_front();
    }
}
void DCI::SendULGrant(){
    while(!ULGrantQueue.empty()){
        ULGrant UlGrant = ULGrantQueue.front();
        MSID msid = UlGrant.msid;
        MS& ms = msid.GetMS();
        ms.UL.m_pMSTxer->GetSR().ReceiveULGrant(UlGrant);
        ULGrantQueue.pop_front();
    }
}



