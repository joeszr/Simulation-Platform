/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BTS_UL.h
 * Author: cyh
 *
 * Created on 2021年1月24日, 下午8:24
 */
#pragma once

#include "../Utility/Include.h"
#include "../Utility/functions.h"
#include"BTSID.h"
#include "../SafeUnordered_map.h"
#include"HARQRxStateBTS.h"
#include "../MobileStation/MSID.h"
#include "MSRxBufferBTS.h"
#include "../SR.h"
//class BTSID;
class BTSRxer;
class SchedulerUL;
class SchedulingMessageUL;
//class ACKNAKMessageUL;
//class HARQRxStateBTS;

class BTS_UL {
public:
    BTSID btsId; //通过id获取BTS的上下行共用的属性和方法
    /// BTS类的发送调度信息队列，上行
    deque<std::shared_ptr<SchedulingMessageUL> >* m_qSchedulingMessageQueueUL;
    /// BTS的HARQ接收管理器,用于上行
    HARQRxStateBTS m_HARQRxStateBTS;
    /// BTS的接收机管理类
    /// 存在于BTS上的对应每个MS的接收缓存单元,用于上行
    std::unordered_map<int, MSRxBufferBTS> m_RxBufferBTS;
    /// 上行调度器
    std::shared_ptr<SchedulerUL> m_SchUL;
    ///根据RBS的指示，来判断RB的起始ID和结束ID
    std::unordered_map<int, pair<int, int > > m_RBS2RBID;
public:
    //0304chty
    vector<vector<bool>> vRBUsedFlag;
    vector< vector<vector<bool>> > vRBUsedFlagPerSlot;

    unordered_map<int, int> MSID2LastK2Slot;

    int iTotalRBNum;
    int iTotalUsedRBNum;
    ///保存RB的集合，根据用户数将资源进行平均分配
    vector<int> m_vRBSetUL;
    std::shared_ptr<BTSRxer> m_pRxer;

    unordered_map<int, int> MSID2PHR;
    unordered_map<int, bool> PhrUpdateFlag;
    unordered_map<int, int> MSID2RBNumSupported;
    void ReceivePHR(const std::pair<MSID, int>& phr);
    unordered_map<int, uint> MSID2SRflag;
    int SR_RBNum;
    double RBUseRate;
    void ReceiveSR(const SRMessage& SRMes);

public:
    void SetPRBRate(double);
    //给每个用户配置SR Period、offset等
    void SetSRConfig(void);
    void CalcSRConfig(int& SR_Period, int& SR_RBNum);

    void Construct(BTSID& _btsId);

    void Initialize();
    /// @brief BTS每个上行TTI要执行的内容
    void WorkSlotUL();
    /// @brief 发送上行调度信息
    void SendSchedulingMessageUL();
    /// @brief 重置BTS
    void Reset();
    /// @param 小区用户的激活集
    void SetMSSRSoffset();
public:
    BTS_UL();
    ~BTS_UL() = default;
};
