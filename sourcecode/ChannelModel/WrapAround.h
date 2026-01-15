///@file WrapAround.h
///@brief 实现WrapAround功能的函数
///@author wangfei

#pragma once
#include "P.h"

namespace cm {

/// @brief 实现WrapAround的接口
///
/// WrapAround 可以消除系统级仿真中的边界效应
class WrapAround{
public:
    /// @brief 保持移动台位置不变，变换基站的位置
    /// @param _ue 代表移动台的位置.
    /// @param _bs 代表基站的位置.
    /// @return 经过WrapAround之后虚拟基站的位置.
    virtual Point WrapTx( const Point& _ue,const Point& _bs ) = 0; 
    /// @brief 保持基站位置不变，变换移动台的位置
    /// @param _ue 代表移动台的位置.
    /// @param _bs 代表基站的位置.
    /// @return 经过WrapAround之后虚拟移动台的位置.
    virtual Point WrapRx( const Point& _ue,const Point& _bs ) = 0;
    /// @brief 保持移动台位置不变，返回不同wrapping基站的位置
    /// @param _bs 代表基站的位置.
    /// @param i代表wrapping基站的序号.
    /// @return 经过WrapAround之后虚拟基站的位置.
    virtual Point WrapTxi( const Point& _bs,int i ) = 0;

private:
    /// 实现WrapAround单件模式的指针
    static WrapAround* m_pWA; 
public:
    /// @brief 实现单件模式的函数
    /// @return 唯一的本类的对象
    static WrapAround& Instance();
protected:
    /// 构造函数
    WrapAround() = default;
    /// 析构函数
    virtual ~WrapAround() = default;
};

}