#pragma once
#include "Rx.h"
namespace cm {
    /// @brief 发射器基类

    class RISRxNode : public Rx{
    public:
        ///Rx的构造函数
        RISRxNode(Point& _point);
        ///RX的拷贝构造函数
        RISRxNode(const RISRxNode& _risrx);
        ///虚析构函数
        virtual ~RISRxNode(void);
    };

}
