///@file  MSManager.h
///@brief 移动台管理类声明
///@author wangxiaozhou

#pragma once
#include "../Utility/Include.h"

class MS;
class BSManager;
class DistributeMSAlgo;
//@threads
class Thread_control;
///移动台管理类

class MSManager {
    //@threads
    friend Thread_control;
    //chty 1103
private:
    void DistributeMSthread(int);
//chty 1103
public:
    ///存储所有的移动台
    vector<MS> m_vMS;
    std::shared_ptr<DistributeMSAlgo> m_pDMSA;
    ///移动台初始化
    void InitializeMSs();
    ///移动台管理类每TTI进行的操作
    void WorkSlot();
    ///由MS的ID得到MS的引用
    MS& GetMS(int _id);
    ///获得系统中MS的数量
    int CountMS()const;
    ///添加移动台
    void AddMS();
    ///移动台位置分布初始化
    void DistributeMSs();

    void DistributeMSs(MS& _ms);

    ///重置移动台管理类
    void Reset();
public:
    //@threads
    static void reInitialze(MS&);
    //@threads
    ///返回唯一的移动台对象实例
    static MSManager& Instance();
private:
    static MSManager* m_pMSM;
private:
    ///MSManager类的构造函数
    MSManager();
    ///MSManager类的拷贝构造函数
    MSManager(const MSManager&);
    ///MSManager类的赋值构造函数
    MSManager & operator =(const MSManager&);
    ///MSManager类的析构函数
    ~MSManager() = default;
};
