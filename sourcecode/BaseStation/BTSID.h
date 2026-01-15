///@file BTSID.h
///@brief  BTSID类声明
///@author wangxiaozhou
#pragma once
#include "../Utility/Include.h"
#include "BSID.h"

class IDType;
class BTS;
class BS;

/// @brief 基站发射器ID类
class BTSID : public BSID {
public:
    ///用于标示BTSID
    int m_iBTSID;

public:
    /// 输出函数
    friend std::ostream & operator <<(std::ostream& _o, BTSID& _btsid);
    /// 由BTSID获得BTS的引用
    BTS& GetBTS() const;
    /// 由BTSID获得BSID的引用
    BSID GetBSID() const;
    /// 返回BTS的序号
    int GetIndex() const;
    ///返回BTS和BS综合序号
    int GetTotalIndex(void)const;

    int GetTxID() const;
    ///@brief 判断两个BTSID是否相等
    ///@return 相等则返回true
    friend bool operator ==(const BTSID& _a, const BTSID& _b);
    ///@brief 判断两个BTSID是否相等
    ///@return 不相等则返回true
    friend bool operator !=(const BTSID& _a, const BTSID& _b);
    ///@brief 比较两个BTSID的大小
    ///@return 前者大于等于后者则返回true
    friend bool operator >=(const BTSID& _a, const BTSID& _b);
    ///@brief 比较两个BTSID的大小
    ///@return 后者大于等于前者则返回true
    friend bool operator <=(const BTSID& _a, const BTSID& _b);
    ///@brief 比较两个BTSID的大小
    ///@return 前者大于后者则返回true
    friend bool operator >(const BTSID& _a, const BTSID& _b);
    ///@brief 比较两个BTSID的大小
    ///@return 前者大于后者则返回true
    friend bool operator <(const BTSID& _a, const BTSID& _b);

    ///@brief 判断形参与当前BTSID是否相等
    ///@return 不相等则返回true
    bool operator !=(BTSID& _b);
    ///@brief 比较形参与当前BTSID的大小
    ///@return 前者大于等于后者则返回true
    bool operator >=(BTSID& _b);
    ///@brief 比较形参与当前BTSID的大小
    ///@return 后者大于等于前者则返回true
    bool operator <=(BTSID& _b);
    ///@brief 比较形参与当前BTSID的大小
    ///@return 前者大于后者则返回true
    bool operator >(BTSID& _b);
    ///@brief 比较形参与当前BTSID的大小
    ///@return 后者大于前者则返回true
    bool operator <(BTSID& _b);
    ///BTSID增加1
    BTSID operator ++();
    ///返回编号最小的BTSID
    static BTSID Begin();
    ///返回编号最大的BTSID
    static BTSID End();
public:
    /// @brief 构造函数
    /// @param _bsid 标示BS的ID
    /// @param _iBTSID 标示BTS的ID
    explicit BTSID(const BSID& _bsid, int _iBTSID = 0);
    /// @brief 构造函数
    /// @param _iBSID 标示BS的ID
    /// @param _iBTSID 标示BTS的ID
    explicit BTSID(const int& _iBSID = 0, int _iBTSID = 0);
    /// @brief 析构函数
    ~BTSID() override = default;
};

typedef BTSID MacroID;
typedef BTSID PicoID;

bool operator ==(const BTSID& p1,const BTSID& p2);