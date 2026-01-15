//
// Created by chent on 2022/9/3.
//

#ifndef MULTITHREAD_CHANNELUPDATETHREAD_H
#define MULTITHREAD_CHANNELUPDATETHREAD_H
#include <memory>
#include <mutex>
#include <condition_variable>
class Thread_control;
class ChannelUpdateThread {
    friend Thread_control;
public:
    void runThreadTask();
    ChannelUpdateThread(int id);

private:
    std::shared_ptr<std::mutex> channel_mutex;
    std::shared_ptr<std::condition_variable> channel_cond;
    enum State {
        run, done,idle
    } state;
    Thread_control* tc;
    int m_id;
};


#endif //MULTITHREAD_CHANNELUPDATETHREAD_H
