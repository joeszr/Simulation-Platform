///@file BTSEBBRankOneTXer.h
///@brief  BTS EBBRankOne发射机类声明
///@author wangxiaozhou

#pragma once
#include "../Utility/Include.h"
//@threads
#include "../SafeDeque.h"
#include "../Utility/SBID.h"
using namespace std;
using namespace itpp;

class BTSTxer;
class MSID;
class SCID;
class RBID;
class SBID;
class SoundingMessage;

/// @brief BTS Eigen Based Beamforming 发射器类

class BTSEBBRank1Txer : public BTSTxer {
protected:
    /// 接收MS发送的SoundingMessage
    //@threads
    SafeDeque<std::shared_ptr<SoundingMessage> > m_qSoundingMessage;
    /// 接收MS发送的非周期SoundingMessage
    std::shared_ptr<SoundingMessage> m_pApSoundingMessage;
    /// RB到波束赋形向量的映射
    std::unordered_map<int, cmat> m_mSBID2CodeWord;
    /// RB到被调度MS的映射（主要用于MU EBB的DMRS建模时确定LOS还是NLOS）
    std::unordered_map<int, vector<MSID> > m_mSBID2vMSID;
protected:
    /// @brief 接收Sounding消息
    void ReceiveSoundingMessage();

public:
    /// @brief BTS EBB发射机初始化
    void Initialize(SafeDeque<std::shared_ptr<SchedulingMessageDL> > *_q,std::unordered_map<int, MSTxBufferBTS>*  _pTxBufferBTS,vector<MSID>* _pActiveSet);
    /// @brief 取子载波上发送的数据符号
    virtual cmat GetTxDataSymbol(SCID _scid);
    /// @brief 取发送的参考符号
    cmat GetTxCRS(int _iPortNum);
    /// @brief 取SB上的预编码码字
    cmat GetPrecodeCodeWord(SBID _sbid);
    /// @brief 获得对应RB上的被调度的MSID数组
    vector<MSID> GetSchedvMSID(SBID _sbid);
    /// @brief 时间驱动接口，完成一个TTI的操作
    virtual void WorkSlot();
    /// @brief 重置操作
    void Reset();
    /// @brief 压入Sounding消息
    void PushSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage);
    /// @brief SetApSounding消息
    void SetApSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage);
    ///
    void ReceiveApSoundingMessage();
    /// @brief 产生调度消息
    virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, int mMCS, const vector<SBID>& _vSBUsed);

    virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, int mMCS, const vector<SBID>& _vSBUsed, const vector<vector<pair<std::shared_ptr<Packet>,double> > >& _vpPacketAndSizeKbits);
    //used in MUProportionalFairRBRankAScheduler
    virtual std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, imat mMCS, const vector<SBID>& _vSBUsed, std::unordered_map<int, CompetorRankA> _mSBID2Competor, std::unordered_map<int, pair<int, vector<int> > > _mMSID2PanelAndvBSBeam)=0;

public:
    /// @brief 构造函数
    explicit BTSEBBRank1Txer(const BTSID& _btsid);
    /// @brief 析构函数
    ~BTSEBBRank1Txer() = default;
};