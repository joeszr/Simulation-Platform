/*
* Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/*
 * File:   RISTx.h
 * Author: oumali
 *
 * Created on 2023年7月26日, 下午3:48
 */
#pragma once
#include "Tx.h"
namespace cm {
    /// @brief 发射器基类

    class RISTxNode : public Tx{
    public:
        ///Rx的构造函数
        RISTxNode(Point& _point);
        ///RX的拷贝构造函数
        RISTxNode(const RISTxNode& _ristx);
        ///虚析构函数
        virtual ~RISTxNode(void);
    };

}