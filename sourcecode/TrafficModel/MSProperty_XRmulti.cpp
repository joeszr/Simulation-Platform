/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "../Parameters/Parameters.h"
#include "./MSProperty_XRmulti.h"
MSProperty_XRmulti::MSProperty_XRmulti(double _dPeriod_ms, int _K) {
    double dPeriod_slot = _dPeriod_ms / Parameters::Instance().BASIC.DSlotDuration_ms;
    m_dInitOffset_slot = random.xUniform(0, dPeriod_slot);
    m_iFrameNo = 0;
    m_jitter_packet_slot=ceil(m_dInitOffset_slot);
    m_originalbornslot=m_jitter_packet_slot;
    K=_K;
    order=random.xUniformInt(0,K);
}
int MSProperty_XRmulti::get_and_renew_order(){
    if(order<K-1){
        return order++;
    }
    else{
        order=0;
        return K-1;
    }
}
