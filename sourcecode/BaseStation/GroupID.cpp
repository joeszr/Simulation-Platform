///@file GroupID.cpp
///@brief  GroupID类函数实现
///@author 

#include "../Utility/IDType.h"
#include "GroupID.h"
#include "../MobileStation/MSID.h"
#include "Group.h"
#include "BS.h"
#include "BSID.h"

GroupID::GroupID(int _iBSID, int _iSectorIndex, int _iGroupIndex) : BSID(_iBSID) {
    m_iSectorIndex = _iSectorIndex;
    m_iGroupIndex = _iGroupIndex;
}

BSID GroupID::GetBSID() {
    BSID _bsid(m_iID);
    return _bsid;
}

int GroupID::GetSectorIndex() const{
    return m_iSectorIndex;
}

int GroupID::GetGroupIndex() const{
    return m_iGroupIndex;
}

bool operator ==(const GroupID& _a, const GroupID& _b) {
    return _a.m_iID == _b.m_iID && _a.m_iSectorIndex == _b.m_iSectorIndex && _a.m_iGroupIndex == _b.m_iGroupIndex;
}

bool operator !=(const GroupID& _a, const GroupID& _b) {
    return _a.m_iID != _b.m_iID || _a.m_iSectorIndex != _b.m_iSectorIndex || _a.m_iGroupIndex != _b.m_iGroupIndex;
}

bool GroupID::operator ==(GroupID& _b) {
    bool result = ((m_iID == _b.m_iID) && (m_iSectorIndex == _b.m_iSectorIndex) && (m_iGroupIndex == _b.m_iGroupIndex));
    return result;
}

bool GroupID::operator !=(GroupID& _b) {
    bool result = ((m_iID != _b.m_iID) || (m_iSectorIndex != _b.m_iSectorIndex) || (m_iGroupIndex != _b.m_iGroupIndex));
    return result;
}

std::ostream & operator <<(std::ostream& _o, GroupID& _GroupID) {
    _o << "(" << _GroupID.m_iID << "," << _GroupID.GetSectorIndex() << "," << _GroupID.GetGroupIndex()<< ")";
    return _o;
}