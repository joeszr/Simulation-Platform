///@file WrapAround0.h
///@brief 实现实现一个小区结构的函数
///
///@author wangfei
#pragma once
#include "WrapAround.h"

namespace cm{

/// @brief 本类不实现任何WrapAround的功能，但为了保持代码的一致性而使用本类，在室内服务区模型中使用本类

class WrapAround0  final: public WrapAround {
public:
    /// @see WrapAround::WrapTx
    Point WrapTx(const Point& _ue, const Point& _bs);
    /// @see WrapAround::WrapRx
    Point WrapRx(const Point& _ue, const Point& _bs);
    /// @brief 保持移动台位置不变，返回不同wrapping基站的位置
    Point WrapTxi( const Point& _bs,int i );
public:
    /// 构造函数
    WrapAround0() = default;
    /// 析构函数
    ~WrapAround0() override = default;
};
}
