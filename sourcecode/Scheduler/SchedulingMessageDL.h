///@file SchedulingMessageDL.h
///@brief SchedulingMessage类的声明
///@author wangxiaozhou

#pragma once
#include"../Utility/Include.h"
#include "../Utility/SBID.h"
#include "../MobileStation/MSID.h"
#include "../CalcRENumperRB.h"
using namespace std;
using namespace itpp;

class MSID;
class SBID;
class RBID;
class Packet;
class Universal_Packet;
/// @brief 调度消息类

class SchedulingMessageDL {
private:
    int CCELevel;
    vector<int> CCESB;
    ///计算每个PRB可用的RE数
//    CalcRENum _mCalcRENum;
    /// 被调度MSID
    MSID m_MSID;
    /// 本次调度分配的进程号
    int m_iHARQID;
    /// 被调度MS使用的调制编码方式
    imat m_mMCS;
    int iMCS;
    /// 分配给MS的Subband资源
    vector<SBID> m_vSBUsed;
    //20230212
    /// 存放被调度MS在的该Subband上的Subband Precode CodeWord.格式：<SBID,codeword>
    std::unordered_map<int, cmat> m_mSBID2CodeWord;

    std::unordered_map<int, int> m_mSBID2LayerNum;
    ///存放TBS的信息
    mat m_mTBSize;
    double dTBSize;
    /// 存放被调度MS在全带宽上的预编码码字
    cmat m_mWideBandCodeWord;
    /// 记录该调度信息被发送次数，单流时为1x1矩阵，双流时为2x1矩阵
    imat m_mSendNum;
    int iSendNum;
    /// 流数
    int m_iRank;
    ///
    bool m_bIsTM3;
    // m_vpPacketAndSizeKbits[rankId] --> vector<pair<std::shared_ptr<Packet>, double> > A;
    // A[n] --> <Packetn pointer, The transmitting (On the Air) Packet's part SizeKbit in this Message >
    vector<vector<pair<std::shared_ptr<Packet>, double> > > m_vpPacketAndSizeKbits;

    vector<vector<pair<std::shared_ptr<Universal_Packet>, double> > > m_vpUniversal_PacketAndSizeKbits;
    std::unordered_map<int, pair<int, vector<int> > > m_MSID2PanelAndvBSBeam;

    int m_iBornTimeSlot;

    int m_iLatestTransTimeSlot;

    int m_ik1_slot;

    //    int m_ik3_slot;
public:
    void SetCCE(const int _ccelevel, vector<int>& _cceSB);
    int GetCCELevel();
    vector<int>& GetCCESB();
    void SetUniversal_PacketAndSizeKbits(vector<vector<pair<std::shared_ptr<Universal_Packet>, double>>>& _vpPacketAndSizeKbits);
    vector<vector<pair<std::shared_ptr<Universal_Packet>, double>>>& GetUniversal_PacketAndSizeKbits();
    double EstimateSINR;
    double CQISINR;
    void SetSINR(double);
    int ID;
    /// @brief 设置被调度MS使用的调制编码方式
    void SetMCS(const imat& _mMCS);
    void SetMCS(int _mMCS);
    /// @brief 设置分配给MS的SB资源
    void SetSBUsed(const vector<SBID>& _vSBUsed);
    /// @brief 设置被调度MS在Subband上的预编码码字
    void SetCodeWord(const std::unordered_map<int, cmat>& _mSBID2CodeWord);
    /// @brief 设置被调度MS在Subband上的预编码码字
    void SetCodeWord(const SBID& _sbid, const cmat& _mCodeWord);
    /// @brief 设置被调度MS在全带宽上的预编码码字
    void SetCodeWord(const cmat& _mWideBandCodeWord);

    void SetLayerNum(const SBID& _sbid, int _iLayerNum);
    /// @brief 增加调度信息发送次数
    /// @param _iStreamIndex 传输流的序号，0---第1流；1---第2流
    void IncreaseSendNum(int _iRankID);
    void IncreaseSendNum();
    /// @brief 将发送次数复位
    /// @param _iStreamIndex 传输流的序号，0---第1流；1---第2流

    /// @brief 获得被调度MSID
    MSID GetMSID();

    int GetMSIDInt();
    /// @brief 获得本次调度分配的进程号
    int GetHARQID()const ;
    /// @brief 获得被调度MS使用的调制编码方式
    imat GetMCS();
    int GetiMCS() const;
    /// @brief 获得分配给MS的SB组
    vector<SBID> GetSBUsed();
    /// @brief 获得分配给MS的RB组
    cmat GetCodeWord();
    /// @brief 获得预编码时MS使用的subband码字
    /// @param 分配给被调度MS的SBID
    /// @return  返回该Subband上使用的预编码码字，考虑了有可能是双流的情况
    cmat GetCodeWord(const SBID& _sbid);
    /// @brief 返回调度消息的发送次数
    imat GetSendNum();
    /// @brief 返回调度消息的发送次数
    int GetSendNum(int _iRankID);
    int GetiSendNum() const;
    /// @brief 返回发送信号的流数（rank）
    int GetRank() const;

    std::unordered_map<int, int> GetLayerNum();

    mat GetTBSize();
    double GetdTBSize() const;

    int GetLayerNum(const SBID& _sbid);
    /// @brief 返回使用的RB数
    int CountRBNum();
    ///
    bool IsTM3TB() const;
    ///
    void SetMap_MSID2PanelAndBSBeam(const std::unordered_map<int, pair<int, vector<int> > >& mMSID2PanelAndBSBeam);

    std::unordered_map<int, pair<int, vector<int> > > GetMap_MSID2PanelAndvBSBeam();

    vector<vector<pair<std::shared_ptr<Packet>, double> > >& GetPacketAndSizeKbits();

    void SetTBSizeKbit();
    void SetTBSizeKbit_TDD();
    void SetdTBSize(double _dTBSize);
    int GetTBSize_AllRank();
    int GetBornTime() const;

    void SetLatestTransTime(int _iLatestTransTime);

    int GetLatestTransTime()const;

    bool CanReTx(int _iTime) const;

    void Setk1_slot(int _iGap);

    int Getk1_slot() const;

    bool CanHARQProcess(int _iTime) const;

    std::shared_ptr<Packet> GetFirstPacket();

    double GetCarriedPacket_SizeKbits();

public:
    /// @brief 构造函数
    SchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank);
    SchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, const vector<vector<pair<std::shared_ptr<Packet>, double> > >& _vpPacketAndSizeKbits);
    /// @brief 析构函数
    ~SchedulingMessageDL();
};