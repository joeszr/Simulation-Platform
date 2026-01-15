
#pragma once
#include "Rx.h"


//用户信道接收端
namespace cm {

    class MSRxNode : public Rx {
    public:
    protected:
        void Build_UE_Antenna();
    public:
        virtual bool IsLowloss() {return m_iIsLowloss;}
        virtual int GetSpecial() {return m_iSpecial;}
        virtual double GetInCarLossDB() {return m_dInCarLossDB;}
    public:
        ///Rx的构造函数
        MSRxNode(Point& _point);
        ///RX的拷贝构造函数
        MSRxNode(const MSRxNode& _msrx);
        ///虚析构函数
        virtual ~MSRxNode(void);
    };
}