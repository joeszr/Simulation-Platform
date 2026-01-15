///@file RISID.cpp
///@brief  基类RISID的函数实现
///@author wangxiaozhou

#include "RISID.h"

#include "../BaseStation/BTS.h"
#include "../BaseStation/BS.h"

RISID::RISID(int bsid, int btsid, int risid) : BTSID(bsid, btsid) {
    m_iRISID = risid;
}

RISID::RISID(BTSID btsid, int risid) : BTSID(btsid.GetBSID(), btsid.GetIndex()) {
    m_iRISID = risid;
}

RISID::~RISID() {
}

RIS& RISID::GetRIS(void) {
    BS& _bs = GetBS();
    BTS& _bts = _bs.GetBTS(m_iBTSID);
    RIS& _ris = _bts.GetRIS(m_iRISID);
    return _ris;
}

RISID RISID::Begin() {
    RISID result = RISID(0,0,0);
    return result;
}

RISID RISID::End() {
    RISID result = RISID(Parameters::Instance().BASIC.INumBSs-1, Parameters::Instance().BASIC.ITotalBTSNumPerBS-1, Parameters::Instance().BASIC.IRISPerBTS-1);
    return result;
}

int RISID::GetBSIndex() {
    return m_iID;
}

int RISID::GetBTSIndex() {
    return m_iBTSID;
}

int RISID::GetRISIndex() {
    return m_iRISID;
}

int RISID::GetTotalIndex() {
    int bts = Parameters::Instance().BASIC.IRISPerBTS;
    int bs = Parameters::Instance().BASIC.ITotalBTSNumPerBS * Parameters::Instance().BASIC.IRISPerBTS;
    int id = m_iRISID + m_iBTSID * bts + m_iID * bs;
    return id;
}

RISID RISID::operator ++() {
    if (m_iRISID == Parameters::Instance().BASIC.IRISPerBTS - 1) {
        m_iRISID = 0;
        if (m_iBTSID == Parameters::Instance().BASIC.ITotalBTSNumPerBS - 1) {
            m_iBTSID = 0;
            ++m_iID;
        } else {
            ++m_iBTSID;
        }
    } else {
        ++m_iRISID;
    }
    return *this;
}