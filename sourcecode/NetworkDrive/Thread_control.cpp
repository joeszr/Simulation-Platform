/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Thread_control.cpp
 * Author: chty
 * 
 * Created on 2022年8月23日, 下午4:51
 */

#include "Thread_control.h"
#include<mutex>
#include<condition_variable>
#include"../Parameters/Parameters.h"
#include "BSManager.h"
#include "MSManager.h"
#include"../BaseStation/BTS.h"
#include"../MobileStation/MS.h"
#include "Clock.h"
#include"../ChannelModel/LinkMatrix.h"
#include"../TrafficModel/TrafficManager.h"
#include"../NetworkDrive/NetWorkDrive.h"
#include"../Statistician/Statistician.h"
extern int G_ICurDrop;

Thread_control *Thread_control::m_pThread_control = nullptr;

extern void BtsTaskFunc(void* arg);
extern void MsTaskFunc(void* arg);
extern void ChannelUpdateTaskFunc(void* arg);

Thread_control::Thread_control() : DL(0), UL(1), Special(2) {
    slots = 0;
    slot_type = -1;

    pCommonThreadpool = new ThreadPool(cm::LinkMatrix::threadnum);

}

void Thread_control::run()
{
    Initialize();
    vector<BS> &allBS = BSManager::Instance().m_BSs;
    vector<MS> &allMS = MSManager::Instance().m_vMS;

    while(slots < Parameters::Instance().BASIC.ISlotPerDrop)
    {
        slots++;
        cout << "slot: " << slots << "/" << Parameters::Instance().BASIC.ISlotPerDrop << endl;
        slot_type = DownOrUpLink(slots);
        Clock::Instance().Forward();
        //下行
        if(slot_type == DL || slot_type == Special)
        {
            cout<<"start ch"<<endl;
            for (auto i = 0; i < cm::LinkMatrix::threadnum; ++i)
            {
                pCommonThreadpool->Add(ChannelUpdateTaskFunc, (void *) &chthreads[i]);
            }
            TrafficManager::Instance().WorkSlot();
            pCommonThreadpool->join();


            cout<<"start bts"<<endl;
            for (auto &bs: allBS) {
                for (int j = 0; j < bs.CountBTS(); ++j)
                {
                    pCommonThreadpool->Add(BtsTaskFunc, (void *) &bs.GetBTS(j));
                }
            }
            pCommonThreadpool->join();

            cout<<"start ms"<<endl;
            for (auto &ms: allMS)
            {
                pCommonThreadpool->Add(MsTaskFunc, (void *) &ms);
            }
            pCommonThreadpool->join();
        }
            //上行
        else
        {
            cout<<"start ch"<<endl;
            for (auto i = 0; i < cm::LinkMatrix::threadnum; ++i)
            {
                pCommonThreadpool->Add(ChannelUpdateTaskFunc, (void *) &chthreads[i]);
            }
            TrafficManager::Instance().WorkSlot();
            pCommonThreadpool->join();

            cout<<"start ms"<<endl;
            for (auto &ms: allMS)
            {
                pCommonThreadpool->Add(MsTaskFunc, (void *) &ms);
            }

            pCommonThreadpool->join();

            cout<<"start bts"<<endl;
            for (auto &bs: allBS)
            {
                for (int j = 0; j < bs.CountBTS(); ++j)
                {
                    pCommonThreadpool->Add(BtsTaskFunc, (void *) &bs.GetBTS(j));
                }
            }
            pCommonThreadpool->join();
        }
    }
}


Thread_control &Thread_control::Instance() {
    if (!m_pThread_control)
        m_pThread_control = new Thread_control;
    return *m_pThread_control;
}


void Thread_control::Reset() {

    slots = 0;
    slot_type = -1;

}

void Thread_control::Initialize() {
    for (auto i = 0; i < cm::LinkMatrix::threadnum; ++i) {
        chthreads.emplace_back(std::move(ChannelUpdateThread(i)));
    }
}