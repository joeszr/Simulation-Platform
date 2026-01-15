///@file MSRxer.h
///@brief  MS接收机的基类声明
///@author wangfei
#pragma once

#include "../Utility/Include.h"
#include "../Parameters/Parameters.h"
#include "MSID.h"
#include"../BaseStation/BTSID.h"
#include "../Utility/Random.h"
//using namespace std;
//using namespace itpp;

//class BTSID;
//class MSID;
class SchedulingMessageDL;
class Detector_DL;
class HARQRxStateMS;
class SRSTxer;
class SCID;
//class HARQRxStateBTS;

///@brief 移动台接收器的接口基类

class MSRxer {
protected:
    Random random;
    /// 主服务站的ID
    BTSID m_MainServBTS;
    /// 移动台的ID
    MSID m_MSID;
    /// 指向SINR的向量
    vector<mat> *m_pSINR;
    /// 存放接收到的调度信息
    std::shared_ptr<SchedulingMessageDL> m_pScheduleMesDL;
    /// 检测算法指针
    std::shared_ptr<Detector_DL> m_pDetector;
    /// 指向HARQ接收状态机的指针
    //    HARQRxStateMS* m_pHARQRxStateMS;

    HARQRxStateMS* m_pHARQRxStateMS;

    void CalInterference(vector<cmat>& vSIS, vector<cmat>& vSIH, vector<cmat>& vSIP, vector<cmat>& vWIS, vector<double>& vWIL,
            BTSID& _MainServBTS, MSID& _msid, SCID& _scid);

    void CalStrongInterference(vector<cmat>& vSIS, vector<cmat>& vSIH, vector<cmat>& vSIP,
            BTSID& _MainServBTS, MSID& _msid, SCID& _scid);

    void CalWeakInterference(vector<cmat>& vWIS, vector<double>& vWIL,
            BTSID& _MainServBTS, MSID& _msid, SCID& _scid);
public:
    /// OLLA调整偏置（dB)
    //double m_dOLLAOffsetDB;

    vector<double> v_dOLLAOffsetDB;
    //  对于最大rank等于4
    //      m_dOLLAOffsetDB（0，0）代表rank=0时第一流的OLLA值
    //      m_dOLLAOffsetDB（1，0）和m_dOLLAOffsetDB（1，1）分别代表rank=1时第一流和第二流的OLLA值
    //      m_dOLLAOffsetDB（2，0...2）分别代表rank=2时第一、二、三流的OLLA值
    //      m_dOLLAOffsetDB（3，0...3）分别代表rank=3时第一、二、三、四流的OLLA值
    mat m_dOLLAOffsetDB;
    ///
    double m_dOLLAOffset2DB;
    ///
    vector<cmat> m_vCovR;
public:
    ///@brief 接收调度消息的函数接口
    ///@param _pSchM 表示调度信息的智能指针
    void ReceiveScheduleMessage(std::shared_ptr<SchedulingMessageDL> _pScheduleMes);
    ///@brief 实现每个TTI接收机动作的函数接口
    virtual void WorkSlot() = 0;
    virtual void ApSounding();
    virtual void RefreshHARQSINR(int, int) = 0;

public:
    ///@brief 构造函数
    MSRxer(const BTSID& _MainServBTSID, const MSID& _msid, vector<mat> *_pSINR, HARQRxStateMS* _pHARQRxStateMS);
    ///@brief 析构函数
    virtual ~MSRxer() = 0;
};