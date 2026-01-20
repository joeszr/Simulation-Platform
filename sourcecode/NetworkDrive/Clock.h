///@file  Clock.h
///@brief 时钟类声明
///@author wangxiaozhou
#pragma once
#include "../Utility/Include.h"

///时钟类

class Clock {
    friend class NetWorkDrive; //定义时钟类为NetWorkDrive类的友元类
    //@threads
    friend class Thread_control; //定义时钟类为NetWorkDrive类的友元类
private:
    ///当前时间
    int m_iTime;

    int m_iReportPeriod;
private:
    /// @brief 推动时钟前进函数
    void Forward();


public:
    /// @brief 获取当前时间,单位：时隙
    int GetTimeSlot()const;
    int GetTimeSlotInSF()const;
    /// @brief 获取当前时间，单位：秒
    double GetTimeSec() const;
    /// @brief 重置20261108
    void Reset();
    //获取当前系统帧号
    int GetSFN() const;
    int GetSubFrame() const;
public:
    ///返回唯一的Clock对象实例
    static Clock& Instance();
private:
    ///Clock构造函数
    Clock();
    ///Clock拷贝构造函数
    Clock(const Clock&) = delete;
    ///Clock赋值构造函数
    Clock & operator =(const Clock&);
    ///Clock析构函数
    ~Clock() = default;
};
