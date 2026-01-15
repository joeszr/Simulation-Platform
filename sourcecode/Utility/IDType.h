/// COPRIGHT NOTICE    
/// Copyright (c) 2010,
/// All right reserved.
///
///@file IDType.h
///@brief  基类IDType的类声明，BSID的基类
/// 
///
///实现了基类IDType的构造函数、析构函数、IDType类输出运算符重载、IDType类运算操作符重载
///
///@author wangxiaozhou
#pragma once
#include <iostream>

///基类IDType的类

class IDType {
protected:
    ///IDType类的ID标示
    int m_iID;

public:
    ///实现由IDType类型转化为类型
    int ToInt()const;
    ///实现IDType类型输出运算符重载
    friend std::ostream & operator <<(std::ostream& _o, const IDType& _id);

    ///实现IDType类型运算符重载
//    friend IDType operator +(const IDType& _a, const IDType& _b);
//    friend IDType operator -(const IDType& _a, const IDType& _b);
//    friend bool operator>(const IDType& _a, const IDType& _b);
//    friend bool operator<(const IDType& _a, const IDType& _b);
//    friend bool operator <=(const IDType& _a, const IDType& _b);
//    friend bool operator >=(const IDType& _a, const IDType& _b);

    bool operator ==(const IDType& _b) const;
    bool operator !=(const IDType& _b) const;
    bool operator<(const IDType& _b) const;
    bool operator>(const IDType& _b) const;
    bool operator <=(const IDType& _b) const;
    bool operator >=(const IDType& _b) const;
    IDType & operator ++();
    IDType & operator --();

    bool operator ==(const int& _ib) const;
    bool operator !=(const int& _ib) const;
    bool operator<(const int& _ib) const;
    bool operator>(const int& _ib) const;
    bool operator <=(const int& _ib) const;
    bool operator >=(const int& _ib) const;
    void operator +=(const int& _ib);

public:
    explicit IDType(int _id);
    virtual ~IDType() = default;
};
