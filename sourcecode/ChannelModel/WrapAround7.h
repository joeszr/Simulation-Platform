///@file WrapAround7.h
///@brief 实现实现7小区拓扑结构的函数
///@author wangfei

#pragma once
#include "WrapAround.h"

namespace cm {
    /// @brief 在7小区的情况下实现WrapAround

    class WrapAround7 : public WrapAround {
        friend class WrapAround;
    private:
        Point m_p[8];
    public:
        /// @brief 保持UE的位置不变，变换基站的位置
        Point WrapTx(const Point& _ue, const Point& _bs);
        /// @brief 保持基站的位置不变，变换UE的位置
        Point WrapRx(const Point& _ue, const Point& _bs);
        /// @brief 保持移动台位置不变，返回不同wrapping基站的位置
        Point WrapTxi( const Point& _bs,int i );
       
    private:
        /// @brief 构造函数
        WrapAround7();
        /// @brief 拷贝构造函数
        WrapAround7(const WrapAround7&);
        /// @brief 赋值构造函数
        WrapAround7 & operator=(const WrapAround7&);
        /// @brief 析构函数
        ~WrapAround7() override = default;
    };
}
