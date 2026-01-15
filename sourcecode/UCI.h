//
// Created by AAA on 2022/9/17.
//
///@file DCI.h
///@brief  DCI类函数声明
///@author chuwenqiang

#pragma once

#include "./Utility/Include.h"
#include"./MobileStation/MSID.h"
#include "SafeDeque.h"
#include "SR.h"

class ACKNAKMessageDL;
class CQIMessage;
class BSR;



class UCI{//Uplink Control Information

private:

    MSID msid;

    SafeDeque<std::shared_ptr<ACKNAKMessageDL> > ACKMessageQueueDL;
    SafeDeque<std::shared_ptr<CQIMessage> > CQIMesQueue;
    SafeDeque<std::pair<MSID, int> > PHRMesQueue;
    SafeDeque<SRMessage>SRMesQueue;
    SafeDeque<std::shared_ptr<BSR> > BSRQueue;

public:

    void CollectACKDL(const std::shared_ptr<ACKNAKMessageDL>& _pACKNAKMessageDL);
    void CollectCQIMes(const std::shared_ptr<CQIMessage>& _pCQIMes);
    void CollectPHR(const std::pair<MSID, int>& PHRMes);
    void CollectSR(const SRMessage& SRMes);
    void CollectBSR(const std::shared_ptr<BSR>& _pBSRMessage);



    SafeDeque<std::shared_ptr<ACKNAKMessageDL> >* GetACKMesQueDL();
    SafeDeque<std::shared_ptr<CQIMessage> >* GetCQIMEsQue();
    SafeDeque<std::pair<MSID, int> >* GetPHRMesQueue();
    SafeDeque<SRMessage >* GetSRMesQue();


    void SendACKMesDL();
    void SendCQIMes();
    void SendPHRMes();
    void SendSRMes();
    void SendBSR();

    void WorkSlot();
    void Reset();

    void initilize(const MSID& id);

    UCI() = default;
    ~UCI();
};
