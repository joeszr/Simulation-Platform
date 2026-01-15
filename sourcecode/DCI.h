//
// Created by cwq on 2022/9/17.
//
///@file DCI.h
///@brief  DCI类函数声明
///@author chuwenqiang

#pragma once


#include"./BaseStation/BTSID.h"
#include"./MobileStation/MSID.h"
#include "SafeDeque.h"
#include "MobileStation/MSTxer.h"

class SchedulingMessageDL;
class SchedulingMessageUL;
class ACKNAKMessageUL;


class DCI{

private:
    BTSID btsID;

    SafeDeque<std::shared_ptr<SchedulingMessageDL> > m_qSchedulingMessageQueueDL;
    std::deque<std::shared_ptr<SchedulingMessageUL> > m_qSchedulingMessageQueueUL;
    SafeDeque<std::shared_ptr<ACKNAKMessageUL> > ACKMessageQueueUL;
    SafeDeque<TPCMessage > TPCMessageQueue;
    std::deque<ULGrant> ULGrantQueue;

public:
    void CollectSchMesDL(const std::shared_ptr<SchedulingMessageDL>& Sch);
    void CollectSchMesUL(const std::shared_ptr<SchedulingMessageUL>& Sch);
    void CollectACKUL(const std::shared_ptr<ACKNAKMessageUL>& _pACKNAKMessageUL);
    void CollectTPC(const TPCMessage& TPCMes);
    void CollectULGrant(const ULGrant& ulgrant);

    SafeDeque<std::shared_ptr<SchedulingMessageDL>> * GetSchMesQueDL();
    std::deque<std::shared_ptr<SchedulingMessageUL> >* GetSchMesQueUL();
    SafeDeque<std::shared_ptr<ACKNAKMessageUL> >* GetACKMesQueUL();
    SafeDeque<TPCMessage >* GetTPCMesQue();

    void SendSchMesDL();
    void SendSchMesUL();
    void SendACKMesUL();
    void SendTPCMes();
    void SendULGrant();

    void WorkSlot();
    void Reset();


    void initialize(const BTSID& id);
    DCI() = default;
    ~DCI();
};
