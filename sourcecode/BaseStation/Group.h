///@file Group.h
///@brief  Group类函数声明
///@author wangfei

#pragma once
#include "../Utility/Include.h"
#include "../ChannelModel/Point.h"

class BTSID;
class GroupID;

class Group: public cm::Point {
private:
    ///
    GroupID m_GroupID;
    ///
    vector<BTSID> m_vPicoIDs;
    
public:
    ///
    void AddPicoIDs(const BTSID& _btsid);
    ///
    vector<BTSID>& GetPicoIDs();
    GroupID GetGroupID();
public:
    /// @brief 构造函数
    explicit Group(const GroupID& _GroupID);
    /// @brief 析构函数
    ~Group() override = default;
};