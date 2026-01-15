/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MSProperty_XRmulti.h
 * Author: chty
 *
 * Created on 2021年10月19日, 上午11:33
 */

#ifndef MSPROPERTY_XRMULTI_H
#define MSPROPERTY_XRMULTI_H

#include "../Utility/Random.h"
class MSProperty_XRmulti {
public:
    Random random;
    double m_dInitOffset_slot;
    int m_iFrameNo;
    int m_jitter_packet_slot;// add for xr; the time slot to generate next packet
    double m_originalbornslot;
    int K;//GOP:K
    int order;//标记当前I帧P帧循环的位置
    MSProperty_XRmulti(){assert(false);}
    MSProperty_XRmulti(double _dPeriod_ms,int K);
    int get_and_renew_order();
    virtual ~MSProperty_XRmulti() = default;
private:

};


#endif /* MSPROPERTY_XRMULTI_H */

