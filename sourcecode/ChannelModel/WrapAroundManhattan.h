///@file WrapAroundManhattan.h
///@brief 实现实现曼哈顿模型拓扑结构的函数
///@author wangfei

#pragma once
#include "WrapAround.h"

namespace cm {
    /// @brief 在曼哈顿情况下实现WrapAround

    class WrapAroundManhattan final: public WrapAround {
        friend class WrapAround;
    public:
        /// @brief 保持UE的位置不变，变换基站的位置
        Point WrapTx(const Point& _ue, const Point& _bs);
        /// @brief 保持基站的位置不变，变换UE的位置
        Point WrapRx(const Point& _ue, const Point& _bs);
        /// @brief 保持移动台位置不变，返回不同wrapping基站的位置
        Point WrapTxi( const Point& _bs,int i );
    private:
        /// @brief 构造函数
        WrapAroundManhattan() = default;
        /// @brief 析构函数
        ~WrapAroundManhattan() override = default;
    };
}
