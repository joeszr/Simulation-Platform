///@file SchedulingMessageUL.h
///@brief SchedulingMessageUL类的声明
///@author wangsen

#pragma once
#include"../Utility/Include.h"
#include "../MobileStation/MSID.h"
#include "../Utility/RBID.h"
#include "../TrafficModel/Packet.h"
using namespace std;
using namespace itpp;

class RBID;

/// @brief 调度消息类

class SchedulingMessageUL {
private:
    int CCELevel;
    vector<int> CCESB;
    /// 被调度MSID
    MSID m_MSID;
    /// 本次调度分配的进程号
    int m_iHARQID;
    /// 被调度MS使用的调制编码方式
    int m_iMCS;
    imat m_mMCS;
    /// 分配给MS的RB资源
    vector<RBID> m_vRBUsed; //清零？
    /// 记录该调度信息被发送次数
    int m_iSendNum;
    /// 记录调度信息的发送次数
//    imat m_mSendNum;
    /// 本消息产生的时间点
    int m_iBornTime;
    
    int m_iRank;
    ///保存配对用户的信息
    vector<MSID> m_vVmimoGroup;

    //保存RBS到ms的映射关系
    std::unordered_map<int, vector<MSID> > m_mRB2vMSID; //zhengyi

    double m_iTBSize;

    std::shared_ptr<Packet> pPacket;

    std::unordered_map<int, cmat> m_mSCID2CodeWord;

public:
    int m_iK2Slot;
    double EstimateSINR;

    void SetCCE(const int _ccelevel, vector<int>& _cceSB);
    int GetCCELevel();
    vector<int>& GetCCESB();

    void SetSINR(double);
    std::shared_ptr<Packet>GetFirstPacket();
    /// @brief 获得调度信息的born time
    int GetBornTime() const;
    /// @brief 设置被调度MS使用的调制编码方式
    void SetMCS(int _iMCS);
    void SetMCSmat(const imat& _mMCS);
    /// @brief 设置分配给MS的RB资源
    void SetRBUsed(const vector<RBID>& _vRBUsed);
    /// @brief 增加调度信息发送次数
    void IncreaseSendNum();

    /// @brief 获得被调度MSID
    MSID GetMSID();
    /// @brief 获得本次调度分配的进程号
    int GetHARQID() const;

    imat GetMCSmat();
    /// @brief 获得分配给MS的RB组
    vector<RBID>& GetRBUsed();
    /// @brief 返回调度消息的发送次数
    int GetSendNum() const;
    /// @brief 处理重传的调度信息
    /// @return  返回表示是否需要重传
    bool ReSchMessageAdjust(bool _bACK);

    //VmimoGroup应该是在计算MUSINR会赋值，不需要在构造调度函数时添加
    vector<MSID> GetVmimoGroup();

    void SetRB2vMSID(const std::unordered_map<int, vector<MSID> >& _mRBS2MS); //ZHENGYI

    std::unordered_map<int, vector<MSID> >& GetRB2vMSID(); //ZHENGYI

    void SetTBSizeKbit();
    void SetTBSizeKbitWithBuffer(double RemainUntransmittedBuffer);
    void SetTBSizeKbit_withPacket();

    double GetTBSizeKbit() const;
    
    int GetRank() const ;
    //@threads
    void SetRank(int _iRI);
    void SetBornTime(int Time);

    void SetvVmimoGroup(vector<MSID> _vVmimoGroup);

    void SetCodeWord(const RBID& _rbid, const cmat& _mCodeWord);
    cmat GetCodeWord(const RBID& _rbid);

public:
    /// @brief 构造函数
    SchedulingMessageUL(MSID _msid, int _iHARQID);
    SchedulingMessageUL(MSID _msid, int _iHARQID, std::shared_ptr<Packet> packet);
    /// @brief 析构函数
    ~SchedulingMessageUL() = default;
};