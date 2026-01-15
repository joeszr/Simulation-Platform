///@file BTSID.cpp
///@brief  BTSID类函数实现
///@author wangxiaozhou

#include "../Utility/IDType.h"
#include "BTSID.h"
#include "../MobileStation/MSID.h"
#include "../Utility/RBID.h"
#include "../Scheduler/SchedulerDL.h"
#include "../Scheduler/SchedulerUL.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "BTS.h"
#include "BS.h"
#include "BSID.h"

bool operator ==(const BTSID& p1,const BTSID& p2){
    return p1.m_iBTSID==p2.m_iBTSID&&p1.ToInt()==p2.ToInt();
}
BTSID::BTSID(const int& _iBSID, int _iBTSID) : BSID(_iBSID) {
    m_iBTSID = _iBTSID;
}

BTSID::BTSID(const BSID& _bsid, int _iBTSID) : BSID(_bsid) {
    m_iBTSID = _iBTSID;
}

BTS& BTSID::GetBTS() const{
    BS& _bs = GetBS();
    BTS& _bts = _bs.GetBTS(m_iBTSID);
    return _bts;
}

BSID BTSID::GetBSID() const{
    BSID _bsid(m_iID);
    return _bsid;
}

int BTSID::GetTotalIndex()const {
    return m_iID*Parameters::Instance().BASIC.ITotalBTSNumPerBS+m_iBTSID;
}

int BTSID::GetIndex() const{
    return m_iBTSID;
}
int BTSID::GetTxID() const{
    return m_iID * Parameters::Instance().BASIC.IBTSPerBS + m_iBTSID;
}

bool operator!=(const BTSID& _a, const BTSID& _b) {
    return _a.m_iID != _b.m_iID || _a.m_iBTSID != _b.m_iBTSID;
}

bool operator>=(const BTSID& _a, const BTSID& _b) {
    bool result = ((_a.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _a.m_iBTSID) >= (_b.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _b.m_iBTSID));
    return result;
}

bool operator<=(const BTSID& _a, const BTSID& _b) {
    bool result = ((_a.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _a.m_iBTSID) <= (_b.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _b.m_iBTSID));
    return result;
}

bool operator>(const BTSID& _a, const BTSID& _b) {
    bool result = ((_a.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _a.m_iBTSID) > (_b.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _b.m_iBTSID));
    return result;
}

bool operator<(const BTSID& _a, const BTSID& _b) {
    bool result = ((_a.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _a.m_iBTSID) < (_b.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _b.m_iBTSID));
    return result;
}

BTSID BTSID::operator++() {
    if (m_iBTSID == Parameters::Instance().BASIC.ITotalBTSNumPerBS - 1) {
        m_iBTSID = 0;
        ++m_iID;
    } else {
        ++m_iBTSID;
    }
    return *this;
}

BTSID BTSID::Begin() {
    BTSID result = BTSID(0, 0);
    return result;
}

BTSID BTSID::End() {
    BTSID result = BTSID(Parameters::Instance().BASIC.INumBSs - 1, Parameters::Instance().BASIC.ITotalBTSNumPerBS - 1);
    return result;
}


bool BTSID::operator!=(BTSID& _b) {
    bool result = ((m_iID != _b.GetBSID().ToInt()) || (m_iBTSID != _b.m_iBTSID));
    return result;
}

bool BTSID::operator>=(BTSID& _b) {
    bool result = ((m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + m_iBTSID) >= (_b.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _b.m_iBTSID));
    return result;
}

bool BTSID::operator<=(BTSID& _b) {
    bool result = ((m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + m_iBTSID) <= (_b.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _b.m_iBTSID));
    return result;
}

bool BTSID::operator>(BTSID& _b) {
    bool result = ((m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + m_iBTSID) > (_b.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _b.m_iBTSID));
    return result;
}

bool BTSID::operator<(BTSID& _b) {
    bool result = ((m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + m_iBTSID) < (_b.m_iID * Parameters::Instance().BASIC.ITotalBTSNumPerBS + _b.m_iBTSID));
    return result;
}

std::ostream & operator<<(std::ostream& _o, BTSID& _btsid) {
    _o << "(" +std::to_string( _btsid.ToInt()) + "," + std::to_string(_btsid.GetIndex()) + ")";
    return _o;
}