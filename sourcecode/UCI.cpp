//
// Created by cwq on 2022/9/17.
//
#include "UCI.h"
#include "MobileStation/ACKNAKMessageDL.h"
#include "NetworkDrive/Clock.h"
#include "MobileStation/MS.h"
#include "BaseStation/BTS.h"
#include "BaseStation/ACKNAKMessageUL.h"
#include "Scheduler/SchedulingMessageDL.h"
#include"./MobileStation/CQIMessage.h"

///@file DCI.h
///@brief  BTS类函数类实现
///@author chuwenqiang

UCI::~UCI(){
    ACKMessageQueueDL.clear();
    CQIMesQueue.clear();
    PHRMesQueue.clear();
    SRMesQueue.clear();
    BSRQueue.clear();
}

void UCI::initilize(const MSID& id){
    msid = id;
    BSRQueue.clear();
}
void UCI::Reset(){
    ACKMessageQueueDL.clear();
    CQIMesQueue.clear();
    PHRMesQueue.clear();
    SRMesQueue.clear();
    BSRQueue.clear();
}

void UCI::WorkSlot(){
    SendACKMesDL();
    SendCQIMes();
    SendPHRMes();
    SendSRMes();
}



void UCI::CollectACKDL(const std::shared_ptr<ACKNAKMessageDL>& _pACKNAKMessageDL){
    ACKMessageQueueDL.push_back(_pACKNAKMessageDL);
}
void UCI::CollectCQIMes(const std::shared_ptr<CQIMessage>& _pCQIMes){
    CQIMesQueue.push_back(_pCQIMes);
}
void UCI::CollectPHR(const std::pair<MSID, int>& PHRMes){
    PHRMesQueue.push_back(PHRMes);
}
void UCI::CollectSR(const SRMessage& SRMes){
    SRMesQueue.push_back(SRMes);
}
void UCI::CollectBSR(const std::shared_ptr<BSR>& _pBSRMessage) {
    BSRQueue.push_back(_pBSRMessage);
}




SafeDeque<std::shared_ptr<ACKNAKMessageDL> >* UCI::GetACKMesQueDL(){
    return &ACKMessageQueueDL;
}
SafeDeque<std::shared_ptr<CQIMessage> >* UCI::GetCQIMEsQue(){
    return &CQIMesQueue;
}
SafeDeque<std::pair<MSID, int> >* UCI::GetPHRMesQueue(){
    return &PHRMesQueue;
}
SafeDeque<SRMessage >* UCI::GetSRMesQue(){
    return &SRMesQueue;
}


void UCI::SendACKMesDL(){
    while(!ACKMessageQueueDL.empty()){
        std::shared_ptr<ACKNAKMessageDL> pACK = ACKMessageQueueDL.front();
        std::shared_ptr<SchedulingMessageDL> pSch = pACK->GetScheduleMes();
        BTS& bts = msid.GetMS().GetMainServBTS().GetBTS();
        bts.ReceiveACKNAKMes(pACK);
        ACKMessageQueueDL.pop_front();
    }
}
void UCI::SendCQIMes(){
    BTS& bts = msid.GetMS().GetMainServBTS().GetBTS();
    while(!CQIMesQueue.empty()){
        std::shared_ptr<CQIMessage> pCQIMes = CQIMesQueue.front();
        bts.PushCQIMessage(msid, pCQIMes);
        CQIMesQueue.pop_front();
    }
}
void UCI::SendPHRMes(){
    BTS& bts = msid.GetMS().GetMainServBTS().GetBTS();
    while(!PHRMesQueue.empty()){
        std::pair<MSID, int> phr = PHRMesQueue.front();
        bts.UL.ReceivePHR(phr);
        PHRMesQueue.pop_front();
    }
}
void UCI::SendSRMes(){
    BTS& bts = msid.GetMS().GetMainServBTS().GetBTS();
    while(!SRMesQueue.empty()){
        SRMessage SRMes = SRMesQueue.front();
        SRMesQueue.pop_front();
        bts.UL.ReceiveSR(SRMes);
    }
}
void UCI::SendBSR(){
    BTS& bts = msid.GetMS().GetMainServBTS().GetBTS();
    while(!BSRQueue.empty()){
        std::shared_ptr<BSR> bsr = BSRQueue.front();
        BSRQueue.pop_front();
        bts.m_clsMac.RecieveBSR(bsr);
    }
}



