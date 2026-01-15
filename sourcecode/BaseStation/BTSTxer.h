///@file BTSTXer.h
///@brief  BTS发送模块声明
///@author wangxiaozhou

#pragma once
#include "../Utility/Include.h"
#include "../Utility/SBID.h"
#include "../Parameters/Parameters.h"
//@threads
#include"../SafeDeque.h"
#include "../SafeUnordered_map.h"
#include "MSTxBufferBTS.h"
#include "../MobileStation/MSID.h"
using namespace std;
using namespace itpp;

class BTSID;
class SchedulingMessageDL;
class RBID;
class SBID;
class SoundingMessage;
class Packet;
class CompetorRankA;

/// @brief BTS发送模块类

class BTSTxer {
protected:
    /// 所属的BTSID
    BTSID m_BTSID;
    /// 指向BTS上移动台发送缓存的指针
    std::unordered_map<int, MSTxBufferBTS>* m_pTxBufferBTS;
    /// 指向BTS调度消息队列的指针
    //@threads
    SafeDeque<std::shared_ptr<SchedulingMessageDL> >* m_pScheduleMessageQueue;
    /// 指向BTS激活集的指针
    vector<MSID>* m_pActiveSet;
public:
    /// @brief 接收MS发送的Sounding信号
    virtual void ReceiveSoundingMessage();

public:
    vector<int> m_vSelectedBeamIndex;
    //0表示用了0&1，1表示2&3，2表示用了四个Panel
    std::unordered_map<int,pair<int,vector<int> > > m_MSID2PanelAndvBeamIndex;
    vector< pair<int,vector<int> > > m_vUEPanelAndVBSBeam;
    /// @brief BTS发射机初始化
    virtual void Initialize(SafeDeque<std::shared_ptr<SchedulingMessageDL> > *_q, std::unordered_map<int, MSTxBufferBTS>* _pTxBufferBTS, vector<MSID>* _pActiveSet);
    /// @brief 获得发送符号
    virtual cmat GetTxDataSymbol(SCID _scid) = 0;
    /// @brief 获得参考信号
    virtual cmat GetTxCRS(int _iPortNum) = 0;
    /// @brief 获得对应子带上使用的预编码码字
    virtual cmat GetPrecodeCodeWord(SBID _sbid) = 0;
    /// @brief 获得对应RB上的被调度的MSID数组
    virtual vector<MSID> GetSchedvMSID(SBID _sbid);
    /// @brief 执行BTS发送操作，由BTS的WorkSlot()调用
    virtual void WorkSlot() = 0;
    /// @brief 将MS发送的sounding信号压入接收队列
    virtual void PushSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage);
    /// @brief 将MS发送的非周期ApSounding信号放入m_pApSoundingMessage
    virtual void SetApSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage);
    ///
    virtual void ReceiveApSoundingMessage();
    /// @brief 根据MSID和RBID获得Sounding信号
    virtual cmat GetSoundingMat(MSID _msid, RBID _rbid);
    /// @brief 重置BTS发射机
    virtual void Reset() = 0;
    /// @brief 产生调度消息(为MU设计)
    //virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, vector<SBID> _vSBUsed, std::map<SBID, Competor> _mSBID2Competor);
    virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, int mMCS, const vector<SBID>& _vSBUsed, std::unordered_map<int, CompetorRankA> _mSBID2Competor)=0;
   //used in MUProportionalFairRBRankAScheduler
    virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, imat mMCS, const vector<SBID>& _vSBUsed, std::unordered_map<int, CompetorRankA> _mSBID2Competor, std::unordered_map<int,pair<int,vector<int> > > _mMSID2PanelAndvBSBeam)=0;
//    virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed, std::map<SBID, CompetorRankA> _mSBID2Competor, vector<int> _vSelectedBeamIndex);
//    virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed, std::map<SBID, Competor> _mSBID2Competor, vector<vector<pair<std::shared_ptr<Packet>,double> > > _vpPacketAndSizeKbits);
    virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, int mMCS, const vector<SBID>& _vSBUsed, std::unordered_map<int, CompetorRankA> _mSBID2Competor, const vector<vector<pair<std::shared_ptr<Packet>,double> > >& _vpPacketAndSizeKbits)=0;
//    virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed);
    /// @brief 产生调度消息(为SU的Bursty业务设计)
//    virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed, vector<vector<pair<shared_ptr<Packet>,double> > > _vpPacketAndSizeKbits);

public:
    BTSID GetBTSID() const {
        return m_BTSID;
    }
    
public:
    /// @brief 构造函数
    explicit BTSTxer(const BTSID& _btsid);
    /// @brief 析构函数
    virtual ~BTSTxer() = default;
};
