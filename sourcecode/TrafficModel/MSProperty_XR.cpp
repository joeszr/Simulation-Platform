/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "./MSProperty_XR.h"

MSProperty_XR::MSProperty_XR(double _dPeriod_ms) {
    double dPeriod_slot = _dPeriod_ms / Parameters::Instance().BASIC.DSlotDuration_ms;
    m_dInitOffset_slot = r.xUniform(0, dPeriod_slot);
    m_iFrameNo = 0;
    m_jitter_packet_slot = ceil(m_dInitOffset_slot);
    m_originalbornslot = m_jitter_packet_slot;
}

MSProperty_XR::MSProperty_XR() {
    m_dInitOffset_slot = 0;
    m_iFrameNo = 0;
}