///@file BSID.h
///@brief 基类BSID的类声明，标志BS的唯一ID,继承于IDType类
///@author wangxiaozhou
#pragma once
#include "../Utility/Include.h"
#include "../Utility/IDType.h"

class BS;
class IDType;

/// @brief 基站ID类
class BSID : public IDType {
public:
    /// @brief 本函数实现了由BSID获得BS的引用功能
    BS& GetBS() const;
    /// @brief 返回编号最小的BSID
    static BSID Begin();
    /// @brief 返回编号最大的BSID+1
    static BSID End();

public:
    /// @brief 构造函数
    explicit BSID(int _id);
    /// @brief 析构函数
    ~BSID() override = default;
};
bool operator==(const BSID& bsid1,const BSID& bsid2);
