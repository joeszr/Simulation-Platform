/// COPRIGHT NOTICE
/// Copyright (c) 2010,
/// All right reserved.
///
///@file ChannelState.h
///@brief  ChannelState类声明
///
///包括构造函数，析构函数，初始化函数和每个时隙的工作函数的声明
///
///@author wangfei
#pragma once
//@threads
#include "BasicChannelState.h"
#include "AntennaPanel.h"
#include "SpaceChannelState.h"
#include "Tx.h"
#include "RIS/RIS.h"

namespace cm {
    class Tx;
    class Rx;
    class BasicChannelState;
    class SpaceChannelState;

    /// 信道状态类，记录一个发送者与一个接收者之间的信道状态
    class ChannelState {
    public:
        /// 记录基本信道信息的成员变量（记录大尺度信道信息）
        BasicChannelState m_BCS;
        /// 记录空时信道信息的成员变量，使用了boost库的智能指针
        std::shared_ptr<SpaceChannelState> m_pSCS;
        /// 标记本链路是否是强链路的标记量，如果是强链路本类就会记录空时信道信息，如果是弱链路则只记录基本信道信息
        bool m_bIsStrong;
        // 小尺度初始化是否完毕
        bool SCSfinished;
    public:
        /// @brief 初始化函数
        /// @param _tx 发送者引用
        /// @param _rx 接收者引用
        void Initialize(Tx& _tx, Rx& _rx);
        void Initialize(Tx& _tx, RIS& _ris);
        void Initialize(RIS& _ris, Rx& _rx);
//        /// @brief 初始化空时信道信息
//        void InitAllSCM();
        /// @为了简化信道初始化的复杂度，先跟据LoS径选择强链路，然后只需要初始化强链路的小尺度
        void InitStrongSCM() ;
        ///
        void CalH();
        void CalH(std::pair<int, int> beampair);

        void SetStrong();

        void InitSmallScale();

        /// @brief 更新信道信息到指定的时间节点
        /// @param _dTimeSec 信道信息对应的时刻，单位秒
        void WorkSlot(double _dTimeSec) const;

    public:
        /// @brief 构造函数
        ChannelState();
        /// @brief 析构函数
        ~ChannelState() = default;
    };
}
