///@file BTSRxer.h
///@brief  BTS接收机的基类声明
///@author zhengyi
#pragma once
#include "../Utility/Include.h"
#include "../Parameters/Parameters.h"
#include "../Utility/Random.h"

using namespace std;
using namespace itpp;

class BTSID;
class MSID;
class SchedulingMessageUL;
class Detector_UL;
//class tmpDetector;
class HARQRxStateMS;
class HARQTxStateMS;
class HARQRxStateBTS;
class MSRxBufferBTS;
class MSTxBufferBTS;
class HARQRxStateBTS;
class SCID;
class RBID;
class SBID;
class SoundingMessage;

///@brief 移动台接收器的接口基类

class BTSRxer {
protected:
    Random random;
    ///@brief BTS的ID
    BTSID m_BTSID;
    ///@brief 指向BTS激活集的指针
    vector<MSID>* m_pActiveSet;
    ///@brief 检测算法指针
    std::shared_ptr<Detector_UL> m_pDetector;
    ///@brief 指向BTS调度消息队列的指针
    deque<std::shared_ptr<SchedulingMessageUL> >* m_pSchedulingMessageQueueUL;
    ///@brief 指向BTS上移动台发送缓存的指针
    ///@brief 指向BTS端的HARQ接收状态机的指针
    HARQRxStateBTS* m_pHARQRxState_BTS_UL;

    ///@brief 保存当前时刻各个子载波上的IoT值,保存线性值
    vector<double> m_vIoT;
    ///@brief 上行业务保存IOT，用作统计
    vector<double> m_vAveIoT; //上下这两个IOT得消除一个

    vector<cmat> m_vReEstimate; //zhengyi

public:
    virtual void RefreshHARQSINR(MSID msid, int harqID) = 0;
    std::unordered_map<int, MSRxBufferBTS>* m_pRxBufferBTS;

    ///@brief 实现每个TTI接收机动作的函数接口
    virtual void WorkSlot() = 0;
    ///@brief 初始化BTSRxer的函数，用于建立BTSRxer与BTS之间的联系
    void Initialize(vector<MSID>* _pActiveSet,
            HARQRxStateBTS* _pHARQRxStateBTS,
            deque<std::shared_ptr<SchedulingMessageUL> > *_q,
            std::unordered_map<int, MSRxBufferBTS>* _pRxBufferBTS);
    virtual vector < mat > ComputeEstimateSINR(const MSID& _msid_1, const RBID& _rbid_1,  const RBID& _rbid_2) =0;
    ///@brief 计算两用户的预测SINR
    virtual vector < vector<mat > > ComputeEstimateSINR(const MSID& _msid_1,const MSID& _msid_2,
                                                        const RBID& _rbid_1,
                                                        const RBID& _rbid_2) = 0;

    virtual vector<vector<mat > > ComputeEstSINR4MultiUE(vector<MSID> _vmsid,
            RBID _rbid_1,
            RBID _rbid_2);
public:
    ///@brief 构造函数
    explicit BTSRxer(const BTSID& _bstid);
    ///@brief 析构函数
    virtual ~BTSRxer() = default;
};