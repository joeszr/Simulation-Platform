///@file MSTxBufferBTS.h
///@brief  MSTxBufferBTS类声明
///@author wangxiaozhou

#pragma once
#include "../Utility/Include.h"
#include "../MobileStation/MSID.h"
#include "BTSID.h"

class RBID;
class SBID;
class BTSID;
class CQIMessage;
class Packet;
class Universal_Packet;

class Olla{
private:
    double OllaSINRdB;
    int AckSchedCounter;
    int AckSchedWindow;
    int AckCounter;
    int AckThred;
    int NackSchedCounter;
    int NackSchedWindow;
    int NackCounter;
    int NackThred;
    double SINRDownStep;
    double SINRUpStep;

public:
    void OllaProcess(bool, int);
    double GetOllaSINR();
    void SetConfig(double sinr, int ASC, int AC, int NASC, int NAC, double DownStep, double UpStep);  //对应上面的几个成员变量
    Olla(double sinr = 0.0, int ASC = 100, int AC = 90, int NASC = 100, int NAC = 10, double DownStep = 0.5, double UpStep = 0.05);
    ~Olla() = default;
};
/// @brief 位于BTS上的移动台发送缓存

class MSTxBufferBTS {
private:
    ///
    BTSID m_BTSID;
    /// 移动台的ID
    MSID m_MSID;
    /// CQI消息的队列
    deque<std::shared_ptr<CQIMessage> > m_qCQIMesQueue;
    /// 当前的CQI消息
    std::shared_ptr<CQIMessage> m_pCurrentCQIM;
    /// 移动台成功接收的总比特数
    double m_dSuccessRxKbit;
    /// 基站发送的总比特数
    double m_dTotalTxKbit;
    /// 用于生成波束赋形向量
    std::unordered_map<int, cmat> m_mRBID2H;
    /// 用于存储CovR
    std::unordered_map<int, cmat> m_mSBID2CovR;
    ///包队列
    deque<std::shared_ptr<Packet> > m_qPacketQueueDL;
    //分逻辑信道的包队列
    unordered_map<int, deque<std::shared_ptr<Universal_Packet>> > m_LC2PacketQueueDL;
    unordered_map<int, double> m_LC2Buffer;
    unordered_map<int, double> m_LC2TxKBits;
    unordered_map<int, double> m_LC2TxKBits_ThisSlot;
    int LCNum;
    
    int m_iPacketNum_Right;
    
    int m_iPacketNum_Wrong;

    Olla OllaController;

    vector<double> CQISINR;
    double CQIFilterFactor;

public:
    void UpdateHistoryThrouthput();
    unordered_map<int, double>& GetLC2Buffer(){
        return m_LC2Buffer;
    }
    unordered_map<int, double>& GetLC2TxKBits(){
        return m_LC2TxKBits;
    }
    int GetSmallestLCIndexWithData();//获取存在业务的最下逻辑信道的编号（0~LCNum-1），如果没有数据则返回-1
    double GetSumOfBufferSize();
    bool HaveAnyData();
    vector<vector<pair<std::shared_ptr<Universal_Packet>, double>>> ReduceBuffer(double datasize);//用于混合业务
    void OllaProcess(bool, int);
    double GetOllaSINR();
    double GetCQISINR(int rank);

    /// @brief 将CQI消息加入CQI消息的队列
    void PushCQIMessage(const std::shared_ptr<CQIMessage>& _pCQIM);
    /// @brief 接收CQI消息
    /// 从本类的CQI消息队列中找到最近收到的CQI消息并赋值给当前的CQI消息指针
    void ReceiveCQIMessage();
    /// @brief 返回当前的CQI消息
    std::shared_ptr<CQIMessage> GetCQIMessage();
    /// @brief 取成功传输的比特数
    double GetRateKbit() const;
    /// @brief 取基站发送的比特数
    double GetTotalTxKbit() const;
    /// @brief 累计成功传输的总比特数
    void AccumulateSuccessRxKbit(double _dKbit);
    /// @brief 累计基站发送的比特数
    void AccumulateTotalTxKbit(double _dKbit);
    /// @brief 设置Sounding参考信号反馈得到的信道相应
    void SetH(const RBID& _rbid, const cmat& _mH);
    /// @brief 设置Sounding参考信号反馈得到的信道相应
    void SetH(const RBID& _rbid, const int& _iAntID, const cmat& _mH);
    /// @brief 返回存储的信道相应
    cmat GetH(const RBID& _rbid);
    ///2.29返回每个子带中中间资源块的信道
    cmat GetH(const SBID& _sbid);
    /// @brief 计算信道相关阵
    void ComputeCovRInfo();
    /// @brief 返回存储的信道相关阵
    cmat GetCovR(const SBID& _sbid);
    ///
    void PushPacketDL(const std::shared_ptr<Packet>& _pPacket);
    void PushPacketDL(const std::shared_ptr<Universal_Packet>& _pPacket);
    ///
    int GetDLPacketNum();

    double GetDLPacketSizeKbits();

    double GetDLFirstAvailablePacket_RemainUntransmitted_SizeKbits();

    double GetFirstPacketBorntimeSlot();

    void PopFinishedPacket();
    ///
    //vector<pair<std::shared_ptr<Packet>, double> > ReducePacketBuf(double dLen);
    ///适用于多流
    vector<vector<pair<std::shared_ptr<Packet>, double> > > ReducePacketBuf(vector<double>& _vLen);

    void PopTimeOutPacket(int _Time);

    std::shared_ptr<Packet> GetDLFirstAvailablePacket();
    
    void LogPacketErrorRate();
public:
    /// @brief 构造函数
    explicit MSTxBufferBTS(const BTSID& _btsid = BTSID(-1, -1), const MSID& _msid = MSID(-1));
    /// @brief 析构函数
    ~MSTxBufferBTS() = default;
};
