///@file GroupID.h
///@brief  GroupID类声明
///@author 
#pragma once
#include "../Utility/Include.h"
#include "BSID.h"
#include "BTSID.h"

class IDType;
class Group;
class BS;

/// @brief 基站发射器ID类
class GroupID : public BSID{
public:
    int m_iSectorIndex;
    int m_iGroupIndex;

public:
    /// 输出函数
    friend std::ostream & operator <<(std::ostream& _o, GroupID& _groupid);

    ///
    BSID GetBSID();
    ///
    int GetSectorIndex() const;
    ///
    int GetGroupIndex() const;
    ///@brief 判断两个GroupID是否相等
    ///@return 相等则返回true
    friend bool operator ==(const GroupID& _a, const GroupID& _b);
    ///@brief 判断两个GroupID是否相等
    ///@return 不相等则返回true
    friend bool operator !=(const GroupID& _a, const GroupID& _b);  
    ///@brief 判断形参与当前GroupID是否相等
    ///@return 相等则返回true
    bool operator ==(GroupID& _b);
    ///@brief 判断形参与当前GroupID是否相等
    ///@return 不相等则返回true
    bool operator !=(GroupID& _b);
public:
    /// @brief 构造函数
    GroupID(int _iBSID, int _iSectorIndex, int _iGroupIndex);
    /// @brief 析构函数
    ~GroupID() override = default;
};