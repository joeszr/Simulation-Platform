///@file Group.cpp
///@brief  Group类函数实现
///@author wangfei

#include "../Utility/IDType.h"
#include "GroupID.h"
#include "BTSID.h"
#include "Group.h"

Group::Group(const GroupID& _GroupID) : m_GroupID ( _GroupID) {

}

void Group::AddPicoIDs(const BTSID& _btsid) {
    m_vPicoIDs.push_back(_btsid);
}

vector<BTSID>& Group::GetPicoIDs() {
    return m_vPicoIDs;
}

GroupID Group::GetGroupID() {
    return m_GroupID;
}