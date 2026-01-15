/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Thread_control.h
 * Author: chty
 *
 * Created on 2022年8月23日, 下午4:51
 */

#ifndef THREAD_CONTROL_H
#define THREAD_CONTROL_H
#include <mutex>
#include<vector>
#include<thread>
#include <boost/thread.hpp>
#include "ChannelUpdateThread.h"
#include "../Utility/Thread/threadpool.h"
class Thread_control {
public:
    static Thread_control& Instance();

    void run();//开始仿真的接口
    void Reset();//drop结束的重置
    Thread_control(const Thread_control& orig) = delete;

    ThreadPool* pCommonThreadpool;

private:
    Thread_control();
    virtual ~Thread_control() = default;
    static Thread_control* m_pThread_control;
    std::vector< std::shared_ptr<std::thread> > g;//保存所有线程
    std::vector<ChannelUpdateThread>chthreads;//信道更新类对象


    int slots;//当前正在运行的时隙
    int slot_type;//当前slot类型（上下行和特殊时隙）
    const int DL;//slot_type的下行的值
    const int UL;//slot_type的上行的值
    const int Special;//slot_type的特殊时隙的值

    void Initialize();//初始化，用于每次仿真添加信道更新类对象

};

#endif /* THREAD_CONTROL_H */

