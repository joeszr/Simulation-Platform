///@file RISID.h
///@brief 基类RISID的类声明，标志RIS的唯一ID,继承于IDType类
///@author wangxiaozhou
#pragma once
#include "../Utility/Include.h"
#include "../Utility/IDType.h"
#include "../BaseStation/BTSID.h"

class IDType;
class RIS;

/// @brief RISID类
class RISID : public BTSID {
public:
    int m_iRISID;
public:
    /// @brief 本函数实现了由RISID获得RIS的引用功能
    RIS& GetRIS(void);
    /// @brief 返回编号最小的RISID
    static RISID Begin();
    /// @brief 返回编号最大的RISID+1
    static RISID End();
    int GetBSIndex();
    int GetBTSIndex();
    int GetRISIndex();
    // 返回RIS在整个系统中的序号
    int GetTotalIndex();
    RISID operator ++();
public:
    /// @brief 构造函数
    RISID(int bsid = 0, int btsid = 0, int _id = 0);
    RISID(BTSID btsid, int _id = 0);
    /// @brief 析构函数
    virtual ~RISID(void);
};


