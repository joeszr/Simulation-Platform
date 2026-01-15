///@file BTS.h
///@brief  BTS类函数声明
///@author wangxiaozhou

#pragma once

#include "../Utility/Include.h"
#include "../ChannelModel/Tx.h"
#include "../Utility/SBID.h"
#include"BTS_UL.h"
#include "HARQTxStateBTS.h"
#include "HARQRxStateBTS.h"
#include "../DCI.h"
#include "DRX.h"
#include "../RIS/RIS.h"
//@threads
#include<mutex>
#include <condition_variable>
#include "../SafeDeque.h"
#include "../SafeUnordered_map.h"
#include "MSTxBufferBTS.h"
#include "./MAC/MAC_BTS.h"
#include "../ChannelModel/BSTx.h"
class Thread_control;

class RBID;
class SBID;
class SCID;
class BTSID;
class BTSTxer;
class BTSRxer;
class SchedulerDL;
class SchedulerUL;
class SchedulingMessageDL;
class SchedulingMessageUL;
class ACKNAKMessageDL;
class ACKNAKMessageUL;
//class HARQTxStateBTS;
//class HARQRxStateBTS;
class MSTxBufferBTS;
class CQIMessage;
class SoundingMessage;
class Packet;
class MSID;

/// @brief 基站发射器类

class BTS final: public cm::Tx {
    friend class BTS_UL;
    //@threads
    friend Thread_control;

public:
    BTS_UL UL;
    MAC_BTS m_clsMac;

    std::unordered_map<int, DRX> MSID2DRX;
    std::unordered_map<int, bool> MSID2NewPDCCH;
    std::unordered_map<int, int> MSID2CCELevel;
    int UlCceMaxNum;
    int UlCceOccupiedNum;
    int DlCceOccupiedNum;

private:
    //@threads
    enum State {
        run, done, idle
    } m_state;
    Thread_control* m_Thread_control;
    int times;
    std::shared_ptr<std::mutex> btsmutex;
    std::shared_ptr<std::mutex> connectmutex;
    std::shared_ptr<std::condition_variable> btscond;

    //20220828 cwq
    DCI dci;
    /// BTS类的发送调度信息队列，下行
    SafeDeque<std::shared_ptr<SchedulingMessageDL> >* m_qSchedulingMessageQueue;
    /// BTS类的发送调度信息队列，上行
    //deque<std::shared_ptr<SchedulingMessageUL> > m_qSchedulingMessageQueueUL;
    /// BTS的HARQ发送管理器
    HARQTxStateBTS m_HARQTxStateBTS;
    /// BTS的HARQ接收管理器
    //    HARQRxStateBTS m_HARQRxStateBTS;
    /// BTS的发射机管理类
    std::shared_ptr<BTSTxer> m_pTxer;
    /// BTS的接收机管理类
    std::shared_ptr<BTSRxer> m_pRxer;
    /// 用来实现多线程的辅组变量
    //    int m_iSwitch;
    /// 标示BTSID
    BTSID m_ID;
    ///包含BTS上的对应每个RIS
    vector<std::shared_ptr<RIS> > m_vpRIS;
    /// 存在于BTS上的对应每个MS的接收缓存单元,用于上行
    //    map<MSID, MSRxBufferBTS> m_RxBufferBTS;
    /// 存在于BTS上的对应每个MS的发送缓存单元,用于下行
    std::unordered_map<int, MSTxBufferBTS> m_TxBufferBTS;
    /// 下行调度器
    std::shared_ptr<SchedulerDL> m_SchDL;
    /// 上行调度器
    std::shared_ptr<SchedulerUL> m_SchUL;
    /// BTS服务的用户激活集
    vector<MSID> m_ActiveSet;
    ///SB使用标识
    vector<vector<bool>> vSBUsedFlag;
    vector< vector<vector<bool>> > vSBUsedFlagPerSlot;
    int iTotalSBNum;

    int iTotalUsedSBNum;

    int iFTPPacketLostNum;
    //@threads
    SafeDeque<std::shared_ptr<ACKNAKMessageDL> > m_qACKNAKMesQueue;

private:
    Random random;
public:
    //作为生成信道的发送节点
    std::shared_ptr<cm::BSTxNode> m_pTxNode;
    int m_iMultiplexID;
    vector<int> m_vMultiplexID;
    ///ljq：SRS的天线端口号相关的计算？？？
    void SRSMultiplexID();


    //if false, this HARQ has not been occupied
    std::unordered_map<int, vector<bool> > m_MSID2HARQStateList;

    std::unordered_map<int, vector<bool> > m_MSID2HARQStateList_UL;

    void WorkSlotDL();

    void WorkSlotUL();

    void LogPacketErrorRate();

    void runThreadTask();
public:
    /// @brief BTS每个上行TTI要执行的内容
    //    void WorkSlotUL(void);
    /// @brief 接收MS反馈的CQI信息
    void ReceiveCQIMessage();
    /// @brief 发送下行调度信息
    void SendSchedulingMessage();

    void SendSchedulingMessage_afterk0();

public:
    /// @brief 获得当前类型

    //20220828 cwq
    DCI* GetDci(){
        return &dci;
    }
    ///20251117
    RIS& GetRIS(int _iID);
    ///20260115 添加RIS
    void AddRISs();
    /// @brief 获得BTSID
    BTSID GetID();
    /// @brief BTS每个TTI要执行的内容
    void WorkSlot();

    /// @brief 将一个MS接入到本BTS，被本BTS服务
    bool ConnectMS(const MSID& _imsid);

    /// @brief BTS初始化
    void Initialize();
    /// @brief 获得链接到BTS的MS数
    int GetConnectedNum();
    /// @brief 重置BTS
    void Reset();
    /// @brief 接收ACKNAK信息
    /// @param _pACKNAKMessage ACKNAK消息
    void ReceiveACKNAKMes(const std::shared_ptr<ACKNAKMessageDL>& _pACKNAKMessage);
    /// @brief 将CQI消息推入BTS的CQI消息队列
    /// @param _msid 报告CQI消息的移动台ID
    /// @param _pCQIMessage CQI消息
    void PushCQIMessage(const MSID& _msid, const std::shared_ptr<CQIMessage>& _pCQIMessage);
    /// @brief 获得发射端参考信号
    cmat GetTxCRS(int _rank);
    /// @brief 获得对应子载波上BTS发送的数据符号
    /// @param _scid 子载波ID
    /// @return 子载波上发送的数据符号
    cmat GetTxDataSymbol(const SCID& _scid);
    /// @brief 获得对应SB上的预编码码字
    /// @param _sbid Sub band ID
    /// @return 对应一个SB上使用的预编码码字
    cmat GetPrecodeCodeWord(const SBID& _sbid);
    /// @brief 获得对应RB上的被调度的MSID
    /// @param _rbid Resource Block ID
    /// @return 对应一个RB上的被调度的MSID数组
    vector<MSID> GetSchedvMSID(const SBID& _sbid);
    /// @brief 将SoundingMessage消息推入BTS的sounding消息队列
    /// @param _msid 上报Sounding消息的移动台ID
    /// @param _pSoundingMessage sounding消息
    void PushSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage);
    /// @param 接收非周期_pSoundingMessage sounding消息
    void SetApSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage);
    ///
    void ReceiveApSoundingMessage();
    ///获取发射机下不同UE选择的BeamIndex
    vector<int> GetUE2vBSBeam(const MSID& _msid);
    ///获取发射机下不同UE选择的PanelIndicator
    ///0表示用了Panel 0&1，1表示用了2&3，2表示用了4个Panel
    int GetUE2PanelIndicator(const MSID& _msid);
    /// 获取发射机下 所有的UE 选择的PanelIndicator和Beam的Map
    std::unordered_map<int, std::pair<int, vector<int> > > GetMap_MSID2UEPanelAndvBSBeam();

    vector< std::pair<int, vector<int> > > GetvUEPanelAndvBSBeam();

    /// @brief 反馈ActiveSet集合
    vector<MSID>& GetActiveSet();

    /// Get HARQState
    HARQTxStateBTS& GetBTSHARQTxState();

    ///@threads
    std::unordered_map<int, MSTxBufferBTS>& GetMSTxBufferBTS();
    ///
    void ReceivePacketDL(const MSID& _msid, const std::shared_ptr<Packet>& _pPacket);
    //用于混合业务
    void ReceivePacketDL(const MSID& _msid, const std::shared_ptr<Universal_Packet>& _pPacket);

    ///
    bool IsExistTraffic();
    ///SB标识操作
    void ResetSBUsedFlag();
    void SetSBUsedFlag(const SBID& _sbid);
    void SetSBUsedFlag(vector<vector<bool>>& SBUsedMap);
    bool GetSBUsedFlag(const SBID& _sbid);
    int GetFTPPacketLostNum() const;

    void SetFTPPacketLostNum(int _iNum);

    int GetMultiplexID() const{
        return m_iMultiplexID;
    };

    void ProcessACKNAKMesQueue();

    void ProcessACKNAKMes(const std::shared_ptr<ACKNAKMessageDL>& _pACKNAKMes);

    std::unordered_map<int, vector<bool> >& GetMSID2HARQStateList();

    std::unordered_map<int, vector<bool> >& GetMSID2HARQStateList_UL();

    void ReceiveSoundingMessage();

    void UpdateCCELevel(MSID& msid);

    int PL2CCELevel(MSID& msid);

    void CalcULCceRatio();

    int GetRISNum();

public:
    /// @brief 构造函数
    BTS(const BSID& _bsid, const int& _iID);
    /// @brief 析构函数
    ~BTS() override;
};
