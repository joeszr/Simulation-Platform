///@file Tx.h
///@brief  Tx类声明
///
#pragma once
#include "Tx.h"

namespace cm {
    /// @brief 发射器基类

    class BSTxNode : public Tx{
    protected:
        void Build_BS_Antenna();
    public:
        /// @brief 构造函数
        BSTxNode(Point& _point);
        /// @brief 拷贝构造函数
        BSTxNode(const BSTxNode& _bstx);
        /// @brief 虚析构函数
        virtual ~BSTxNode(void);
    };
}