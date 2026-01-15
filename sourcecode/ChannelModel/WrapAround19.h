///@file WrapAround19.h
///@brief 实现实现19小区拓扑结构的函数
///@author wangfei
#pragma once
#include "WrapAround.h"


namespace cm {
    /// @brief 在19小区服务区中实现WrapAround

    class WrapAround19 : public WrapAround {
        friend class WrapAround;
    private:
        /// 8个参考点
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
        WrapAround19();

        /// @brief 赋值构造函数
        WrapAround19 & operator=(const WrapAround19&);
        /// @brief 析构函数
        ~WrapAround19() override = default;
    };
}
