///@file NetWorkDrive.h
///@brief 系统驱动模块
///@author wangxiaozhou
#pragma once

/// @brief 系统驱动模块的类
#include "../Utility/Random.h"

class NetWorkDrive {
public:
    /// @brief 仿真过程启动的入口点函数
    void SimulateRun();
private:

    Random random;
    /// @brief 信道参数初始化的函数
    void ChannelParameterInitialize();
    /// @brief 系统初始化过程函数
    void SystemInitialize();
    /// @brief 系统结束函数
    void SystemEnd();
    /// @brief Drop初始化过程函数
    void DropInitialize();
    /// @brief Drop结束函数
    void DropEnd();
    /// @brief 时隙仿真流程
    void SlotRun();
    //仿真之前打印一些文件的表头
    void PrintHead();

private:
    /// NetWorkDrive类型的指针
    static NetWorkDrive* m_pNetWorkDrive;
public:
    /// @brief 返回唯一的系统驱动对象实例
    static NetWorkDrive& Instance();
    void simthread();
private:
    /// @brief 构造函数
    NetWorkDrive() {};
    /// @brief 拷贝构造函数
    NetWorkDrive(const NetWorkDrive&);
    /// @brief 赋值构造函数
    NetWorkDrive & operator =(const NetWorkDrive&);
    /// @brief 析构函数
    ~NetWorkDrive() = default;
    long int begin = 0, end = 0;
};

