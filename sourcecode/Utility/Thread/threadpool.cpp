//
// Created by cmcc on 2023/8/14.
//

#include "threadpool.h"
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;
const int NUMBER = 2;

ThreadPool::ThreadPool(int thread_num)
{
    do {
        threadNum = thread_num;
        busyNum = 0;
        liveNum = thread_num;
        exitNum = 0;

        shutdown = false;
        // this:传递给线程入口函数的参数，即线程池
//        managerID = std::thread(manager, this);
        threadIDs.resize(thread_num);
        for (int i = 0; i < thread_num; ++i) {
            threadIDs[i] = std::thread(worker, this);
        }

        return;
    }while (0);
}

ThreadPool::~ThreadPool()
{
    cout << "destruct ThreadPool" << endl;
    shutdown = true;
    //阻塞回收管理者线程
    if (managerID.joinable())
    {
        managerID.join();
    }

    //唤醒阻塞的消费者线程
    cond.notify_all();
    for (int i = 0; i < threadNum; ++i)
    {
        if (threadIDs[i].joinable())
        {
            threadIDs[i].join();
        }
    }
}

void ThreadPool::Add(Task t)
{
    unique_lock<mutex> lk(mutexPool);
    if (shutdown)
    {
        return;
    }
    //添加任务
    taskQ.push(t);
    cond.notify_one();
}

void ThreadPool::Add(callback f, void* a)
{
    unique_lock<mutex> lk(mutexPool);
    if (shutdown)
    {
        return;
    }
    //添加任务
    taskQ.push(Task(f, a));
    cond.notify_one();
}

int ThreadPool::Busynum()
{
    mutexPool.lock();
    int busy = busyNum;
    mutexPool.unlock();
    return busy;
}

int ThreadPool::Alivenum()
{
    mutexPool.lock();
    int alive = liveNum;
    mutexPool.unlock();
    return alive;
}
void ThreadPool::ShutDown()
{
    while(!taskQ.empty())
    {
        cond.notify_one();
    }
}

void ThreadPool::worker(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    // 工作者线程需要不停的获取线程池任务队列，所以使用while
    while (true)
    {
        // 每一个线程都需要对线程池进任务队列行操作，因此线程池是共享资源，需要加锁
        unique_lock<mutex> lk(pool->mutexPool);
        while (pool->taskQ.empty() && !pool->shutdown)
        {
            // 如果任务队列中任务为0,并且线程池没有被关闭,则阻当前工作线程
            pool->cond.wait(lk);
        }

        // 判断线程池是否关闭了
        if (pool->shutdown)
        {
//            cout << "threadid: " << std::this_thread::get_id() << " exit......" << endl;
            return;
        }

        // 从任务队列中去除一个任务
        Task task = pool->taskQ.front();
        pool->taskQ.pop();
        pool->busyNum++;
        // 当访问完线程池队列时，线程池解锁

        // 取出Task任务后，就可以在当前线程中执行该任务了
//        cout << "thread: " << std::this_thread::get_id() << " start working..." << endl;
        lk.unlock();
        task.function(task.arg);

        //(*task.function)(task.arg);
        //free(task.arg);
        //task.arg = nullptr;

        lk.lock();
        // 任务执行完毕,忙线程解锁
//        cout << "thread: " << std::this_thread::get_id() << " end working..." << endl;
        pool->busyNum--;
        pool->cond_task_done.notify_one();
        lk.unlock();
    }
}
void ThreadPool::join()
{
//    std::mutex m;
    while(true)
    {
        unique_lock<mutex> l(mutexPool);
        while (!taskQ.empty() || busyNum > 0)
        {
            cond_task_done.wait(l);
        }
        return;
    }
}
// 检测是否需要添加线程还是销毁线程
void ThreadPool::manager(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    // 管理者线程也需要不停的监视线程池队列和工作者线程
    while (!pool->shutdown)
    {
        //每隔3秒检测一次
        //sleep(3);
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // 取出线程池中任务的数量和当前线程的数量,别的线程有可能在写数据，所以我们需要加锁
        // 目的是添加或者销毁线程
        unique_lock<mutex> lk(pool->mutexPool);
        int queuesize = pool->taskQ.size();
        int livenum = pool->liveNum;
        int busynum = pool->busyNum;
        lk.unlock();
    }
}
