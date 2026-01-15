///@file MSTXer.h
///@brief  MS发送模块声明
///@author zhengyi

#pragma once
#include "../Utility/Include.h"
#include "../Parameters/Parameters.h"
#include "../PHR.h"
#include"../SR.h"
using namespace std;
using namespace itpp;

class BTSID;
class MSID;
class SchedulingMessageUL;
class SCID;
class RBID;
class SBID;
class SoundingMessage;
class MSTxBufferBTS;
class HARQTxStateMS;
class ACKNAKMessageUL;
class SchedulingMessageUL;
class HARQTxStateBTS;

struct TPCMessage{
    MSID msid;
    int time;
    int tpc;
    bool IsAccumulation;
};
class MSTxer {
protected:

    //20220828 cwq
    deque<TPCMessage> TPCQue;  // {msid, time, tpc}
    double SumOfTPCdB; //用于累计式功控;
    double AbsTPCdB; //用于绝对式功控
    double LastTimeSumOfTPCdB;//上次闭环功控调整值
    double LastTimeTxPowermW;//上次发射功率
    PHR phr;
    SR sr;

    ///@brief 主服务站的ID
    BTSID m_MainServBTSID;
    ///@brief 移动台的ID
    MSID m_MSID;
    ///@brief 存放MS接收到的调度信息
    deque<std::shared_ptr<SchedulingMessageUL> > m_qSchedulingMessageQueue;
    ///@brief 取得当前的调度信息
    std::shared_ptr<SchedulingMessageUL> m_pCurrentSchM;
    ///@brief 保存上行MS数据信道每个子载波上的发射功率
    double m_dSCDataTxPowerMW;
    ///@brief 保存上行SRS的每个子载波的发射功率
    double m_dSCSRSTxPowerMW;
    ///@brief MS发送的SRS间隔的子载波
    int m_iSRSTxSpace;
    ///@brief MS发送的SRS所在的子载波号
    int m_iSRSTxCyclicSCID;
    ///@brief MS发送的SRS所在的子载波号
    int m_iSRSBandWidthIndex;
    ///@brief MS发送的SRS的带宽的起始点和终点，因为直接写RBID类型失败，故采用int类型
    pair<int, int> m_pSRSBWrbid;
    ///@brief 指向MS端HARQ状态机
    //    HARQTxStateMS* m_pHARQTxState_MS_UL;

    std::shared_ptr<HARQTxStateMS> m_pHARQTxState_MS_UL;

    std::shared_ptr<HARQTxStateBTS> m_pHARQTxState_BTS_DL;

    ///当前发射功率可以支持调度的最大RB数
    int m_iRBnumTx;

protected:
    ///@brief 从调度消息队列中检出可以被使用的调度消息
    std::shared_ptr<SchedulingMessageUL> ReceiveSchedulingMessage();

public:

    //20220828 cwq
    SR& GetSR();
    PHR& GetPHR();
    void ReceiveTPCCommand(TPCMessage TPC);
    double Generate_f();
    void Accumlation_f_Process(int tpc);
    void Abs_f_Process(int tpc);
    void ResetTPC(); //for Accumlation TPC
    void SetSRConfig(double _ProhibitTimer, int _TransMax, int _Counter, int Period, int offset);

    ///@brief 接收调度消息的函数接口
    ///@param _pSchedulingM 表示调度信息的智能指针
    void PushSchedulingMessage(std::shared_ptr<SchedulingMessageUL> _pSchedulingM);
    ///@brief 实现每个TTI接收机动作的函数接口
    virtual void WorkSlot() = 0;
    ///@brief 获得数据符号的发射功率的函数接口
    virtual cmat GetTxDataSymbol(SCID _scid) = 0;
    virtual cmat GetTxDataSymbol(SCID _scid,int _iPortNum) = 0;
    ///@brief 获得MS发送的souning信号的功率
    virtual cmat GetTxSRS(SCID _scid) = 0;
    virtual cmat GetTxSRS(SCID _scid,int _iPortNum) = 0;
    ///@brief 获得MS发送的souning信号的功率
    virtual cmat GetTxSRS() = 0;
    ///@brief 获得用户上次传输的发射功率
    virtual cmat GetLastTxP() = 0;
    ///@brief 获得用户当前功率可支持传输的RB数
    virtual int GetRBNumSupport() = 0;
    ///@brief 设置MS发送SRS所在的循环子载波号与间隔
    void ConfigTxSRS(const int& _SRSTxCyclicSCID, const int& _iSRSTxSpace);
    /// @brief 接收上行ACKNAK信息
    /// @param _pACKNAKMessage 上行ACKNAK消息
    void ReceiveACK(std::shared_ptr<ACKNAKMessageUL> _pACKNAKMessageUL);
    /// @brief 获取用户SRS占用的SCID(mod4)
    int GetSRSTxCyclicSCID();
    ///@brief 初始化MSTxer
    //    void Initialize(HARQTxStateMS* _pHARQTxStateMS); //zhengyi
    ///@brief 设置用户SRS占用的带宽的偏置
    void ConfigSRSBWIndex(int _iSRSIndex);
    ///@brief 获得用户SRS占用的带宽的偏置
    int GetSRSBWIndex();
    ///@brief 设置用户SRS占用的带宽的的起点和终点
    void SetSRSBWspan(int _ifirst, int _isecond);
    ///@brief 获得用户SRS占用的带宽的起点和终点
    pair<int, int> GetSRSBWspan();

public:
    MSTxer(const BTSID& _MainServBTSID, const MSID& _MSID, std::shared_ptr<HARQTxStateMS> _pHARQTxStateMS);
    virtual ~MSTxer() = default;
};
