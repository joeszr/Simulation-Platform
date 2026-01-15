/// COPRIGHT NOTICE    
/// Copyright (c) 2010,
/// All right reserved.
///
///@file IDType.cpp
///@brief IDType类的函数实现
///
/// 实现了IDType类的构造函数、析构函数、IDType到int型转换函数、输出运算及运算操作符重载
///
///
///@author wangxiaozhou
#include "IDType.h"

using namespace std;

///@brief 构造函数
IDType::IDType(int _id) {
    m_iID = _id;
}
///@brief 析构函数
///@brief IDType转换为int类型
///@return  返回一个int类型ID
int IDType::ToInt()const {
    return m_iID;
}
///@brief IDType类型"<<"运算符重载
///@return  返回一个输出流对象的引用
std::ostream & operator <<(std::ostream& _o, const IDType& _id) {
    _o << _id.m_iID;
    return _o;
}
///@brief IDType类型运算符重载
///@return  返回重载运算结果
//IDType operator +(const IDType& _a, const IDType& _b) {
//    return IDType(_a.m_iID + _b.m_iID);
//}
//
//
//IDType operator -(const IDType& _a, const IDType& _b) {
//    return IDType(_a.m_iID - _b.m_iID);
//}
//
//
//bool operator>(const IDType& _a, const IDType& _b) {
//    bool result = (_a.ToInt() > _b.ToInt());
//    return result;
//}
//
//
//bool operator<(const IDType& _a, const IDType& _b) {
//    bool result = (_a.ToInt() < _b.ToInt());
//    return result;
//}
//
//
//bool operator <=(const IDType& _a, const IDType& _b) {
//    bool result = (!(_a.m_iID > _b.m_iID));
//    return result;
//}
//
//
//bool operator >=(const IDType& _a, const IDType& _b) {
//    bool result = (!(_a.m_iID < _b.m_iID));
//    return result;
//}


bool IDType::operator ==(const IDType& _b) const {
    bool result = (m_iID == _b.ToInt());
    return result;
}


bool IDType::operator !=(const IDType& _b) const{
    bool result = (m_iID != _b.ToInt());
    return result;
}


bool IDType::operator<(const IDType& _b) const{
    return m_iID < _b.ToInt();
}


bool IDType::operator>(const IDType& _b) const{
    return m_iID > _b.ToInt();
}


bool IDType::operator <=(const IDType& _b) const{
    return !(m_iID > _b.ToInt());
}


bool IDType::operator >=(const IDType& _b) const{
    return !(m_iID < _b.ToInt());
}


IDType& IDType::operator ++() {
    ++m_iID;
    return *this;
}


IDType& IDType::operator --() {
    --m_iID;
    return *this;
}
///@brief IDType与int类型运算符重载
///@return  返回重载运算结果
bool IDType::operator ==(const int& _ib) const{
    bool result = (m_iID == _ib);
    return result;
}


bool IDType::operator !=(const int& _ib) const{
    bool result = (m_iID != _ib);
    return result;
}


bool IDType::operator<(const int& _ib) const{
    bool result = (m_iID < _ib);
    return result;
}


bool IDType::operator>(const int& _ib) const{
    bool result = (m_iID > _ib);
    return result;
}


bool IDType::operator <=(const int& _ib) const{
    return !(m_iID < _ib);
}


bool IDType::operator >=(const int& _ib) const{
    return (!(m_iID < _ib));
}

void IDType::operator +=(const int& _ib) {
    m_iID += _ib;
}