/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   XRmultiModel.h
 * Author: chty
 *
 * Created on 2021年10月19日, 下午12:07
 */

#ifndef XRMULTIMODEL_H
#define XRMULTIMODEL_H
#include "./TrafficModel.h"
#include"./Packet.h"
#include"./MSProperty_XRmulti.h"
class XRmultiModel final: public TrafficModel {
public:
    XRmultiModel();
     ~XRmultiModel() override = default;
    void WorkSlot() final;
    void OutputTrafficInfo() final;
private:
    deque<std::shared_ptr<Packet> > m_qTotalPacketQueueDL;
    std::unordered_map<int, MSProperty_XRmulti> m_MSData;
    double alpha=2;//average size ratio between one I-frame/slice and one P-frame/slice
    int K;
    double fps;
    double SumRate;
    double meanI;
    double meanP;
    
};


#endif /* XRMULTIMODEL_H */

