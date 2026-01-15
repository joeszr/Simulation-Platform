//
// Created by chent on 2022/9/3.
//

#include "ChannelUpdateThread.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../Parameters/Parameters.h"
#include "../ChannelModel/Rx.h"
#include "../ChannelModel/Tx.h"
#include "../ChannelModel/ChannelState.h"
#include "Clock.h"
#include "Thread_control.h"

using namespace cm;

void ChannelUpdateTaskFunc(void* arg)
{
    ChannelUpdateThread* ch = (ChannelUpdateThread*)arg;
    ch->runThreadTask();
}

void ChannelUpdateThread::runThreadTask()
{
    tc=&Thread_control::Instance();

    LinkMatrix& lm = LinkMatrix::Instance();
    int allRx = Rx::CountRx();
    int allTx = Tx::CountTx();
    double _dTimeSec = Clock::Instance().GetTimeSec();
    cm::LinkMatrix::TxRxCS & m_TxRx2CS=LinkMatrix::Instance().m_TxRx2CS;
    std::unordered_map<int,vector<cm::ChannelState*>> used;
    for (int i = 0; i <= allRx; ++i)
    {
        if ((i % cm::LinkMatrix::threadnum) != m_id)
        {
            continue;
        }

        for (int j = 0; j < allTx; ++j)
        {
            cm::LinkMatrix::TxRxID txrxid = std::make_pair(j, i);
            used[i].push_back(&m_TxRx2CS[txrxid]);
        }
    }

    for (auto& it: used)
    {
        for (auto it1 = it.second.begin(); it1 != it.second.end(); it1++)
        {
            (*it1)->WorkSlot(_dTimeSec);
        }
    }

}

ChannelUpdateThread::ChannelUpdateThread(int id)
{
    channel_mutex=std::shared_ptr<std::mutex>(new std::mutex);
    channel_cond=std::shared_ptr<std::condition_variable>(new std::condition_variable);
    state=idle;
    tc=nullptr;
    m_id = id;
}
