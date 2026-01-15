/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MSProperty_XR.h
 * Author: dixinyu
 *
 * Created on 2020年9月10日, 下午4:26
 */

///@file MSData.h
///@brief 用来记录MS统计数据的函数
///@author dushaofeng

#pragma once
#include "../Utility/Include.h"
#include "../Utility/Random.h"
///用来记录移动台统计数据的类

class MSProperty_XR {
public:
    Random r;
    //每个UE的InitOffset_ms可能不一样
    double m_dInitOffset_slot;
    int m_iFrameNo;
    int m_jitter_packet_slot;// add for xr; the time slot to generate next packet
    double m_originalbornslot;//add for xr; the time slot to generate next packet without jitter
public:

    explicit MSProperty_XR(double _dPeriod_ms);
    
    MSProperty_XR();

    ~MSProperty_XR() = default;
};
