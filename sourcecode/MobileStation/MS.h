///@file MS.h
///@brief MS类的声明
///@author wangfei
#pragma once

#include "../Utility/Include.h"
#include "../ChannelModel/Rx.h"
#include"HARQTxStateMS.h"
#include"HARQRxStateMS.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "../Utility/SCID.h"
#include "MSID.h"
#include"../UCI.h"
#include "../BaseStation/BTSID.h"
#include "MSTxBufferMS.h"
#include "MobileStation/MAC/LCG.h"
#include "MobileStation/MAC/MAC_MS.h"
//@threads
#include <mutex>
#include <condition_variable>

#include "MSRx.h"
#include "RIS/RISID.h"
class Thread_control;

class BTSID;
class SCID;
class TrafficModel;
class DetectAlgorithm;
//class HARQTxStateMS;
//class HARQRxStateMS;
//class AntennaOrientGain;
class AOGOmni;
class SchedulingMessageDL;
class SchedulingMessageUL;
class MSRxer;
class MSTxer;
class SRSTxer;

struct MS_UL {

    MSID msId;
    /// 移动台发射起的指针接口
    std::shared_ptr<MSTxer> m_pMSTxer; //add to UL
    /// Geometry
    double m_dGeometryDB; //add to UL
    /// HARQ发射状态机
    HARQTxStateMS m_HARQTxState_MS_UL; //add to UL

    HARQTxStateBTS m_HARQTxState_BTS_DL;
    /// 所有子载波上的SINR
    vector<mat> m_vSINR; //add to UL

    int packettime;
    MSTxBufferMS MSTxBuffer;
    //上行独有的函数
    ///@brief UL TTI的操作
    void WorkSlotUL(); //add to UL
    ///@brief 接收上行的调度消息，调度信息MS保存，MSTXER也有保存
    ///@param _pSchM 表示调度信息的智能指针
    ///@return  无返回值
    void ReceiveSchedulingMessage(std::shared_ptr<SchedulingMessageUL> _pSchedulingMes); //add to UL
    /// @brief 获得发射端数据信号
    cmat GetTxDataSymbol(SCID _scid); //add to UL
    cmat GetTxDataSymbol(SCID _scid,int iPortNum);
    /// @brief 获得发射端参考信号
    cmat GetTxSRS(SCID _scid); //add to UL
    cmat GetTxSRS(SCID _scid,int _iPortNum);
    cmat GetLastTxP(); //add to UL
    ///@brief 配置移动台发送SRS参数
    void ConfigTxSRS(const int _SRSTxCyclicSCID, const int _iSRSTxSpace); //add to UL
    /// @brief 接收上行ACKNAK信息
    /// @param _pACKNAKMessage 上行ACKNAK消息
    void ReceiveACK(std::shared_ptr<ACKNAKMessageUL> _pACKNAKMessageUL); //add to UL
    /// @brief 获取用户SRS占用的子载波编号（mod 4）
    int GetSRSTxCyclicSCID(); //add to UL
    /// @brief 获取用户的Geometry
    double GetGeometryDB(); //in VMIMOSchedulerTrial_3::GenerateCompetitorList useless
    /// @brief 获得用户当前支持调度的RB数
    int GetRBNumSupport(); //add to UL
    ///@brief 设置用户SRS占用的带宽的偏置
    void ConfigSRSBWIndex(int _iSRSIndex); //add to UL
    ///@brief 获得用户SRS占用的带宽的偏置
    int GetSRSBWIndex(); //add to UL
    ///@brief 设置用户SRS占用的带宽的起点和终点
    void SetSRSBWspan(int _ifirst, int _isecond); //add to UL
    ///@brief 获得用户SRS占用的带宽的起点和终点
    pair<int, int> GetSRSBWspan(); //add to UL

    //用于合并的函数
    ///@brief 初始化操作
    ///@return MS初始化成功则返回true,否则返回false
    void Initialize();
    ///@brief 重置操作
    void Reset();
    void Construct(MSID&);
};

///移动台MS类

class MS : public cm::Rx {
    friend struct MS_UL;
    //多线程
    friend Thread_control;
public:
    void runThreadTask();
private:
    Thread_control* tc;
public:
    void UE_attach();
    void UE_attach_new();
    void Initialize_for_smallcell();
    ///@return MS初始化成功则返回true,否则返回false
    bool Initialize();
///RIS类
    RISID m_vMainServRIS0;
    RISID m_vMainServRIS2;
    RISID m_vMainServRIS3;
    RISID m_vMainServRIS_New;
    RISID m_vMainServRIS_Comp;
    int Best_RIS_H0;
    int Best_RIS_V0;
    int Best_RIS_H3;
    int Best_RIS_V3;
    //服务RIS的振子相位20250117
    itpp::cmat tempR_PRIS;
    //作为生成信道的接收节点
    std::shared_ptr<cm::MSRxNode> m_pRxNode;
    //逻辑信道相关
public:
    MAC_MS m_clsMacMs;

    //业务模型相关
public:
    int packettime;

    //运行
public:
    ///@brief 每个TTI的操作
    void WorkSlot();
    ///@brief DL TTI的操作
    void WorkSlotDL();
    ///@brief UL TTI的操作
    void WorkSlotUL();
    ///@brief 重置操作
    void Reset();

    //
    void RefreshHARQSINR(int harqID, int iRank);

    //工具成员
public:
    MS_UL UL;
private:
    /// 移动台接收器的指针接口
    std::shared_ptr<MSRxer> m_pMSRxer;
    /// 移动台发射起的指针接口
    std::shared_ptr<MSTxer> m_pMSTxer;
    /// 移动台SRS发射机的指针接口
    std::shared_ptr<SRSTxer> m_pSRSTxer;
    /// HARQ接收状态机
    HARQRxStateMS m_HARQRxState;

    //属性信息
public:
    /// MSID
    MSID m_ID;
    /// 主服务基站ID
    BTSID m_MainServBTS;
    ///@brief 获取主服务基站ID
    BTSID GetMainServBTS();
    ///@brief 获取MSID
    MSID GetID();


private:
    UCI uci;
    //zhengyi
    double m_dGeometryDB_SNR;
    double m_dGeometryDB_SINR;
    double m_dGeometrydb100MHz;
    double m_dSIR;
    double m_dINR;
    /// 所有子载波上的SINR
    vector<mat> m_vSINR;

    /// HARQ发射状态机
    //    HARQTxStateMS m_HARQTxState;
    ///
    bool bReInitialize;
    bool bHasTraffic;
    //double m_SpeedSacleFactor;
    //20190917
public:
    std::unordered_map<int, cmat>ms_SCPrecodeMat;
    int iBestRank_UL;

    UCI* GetUci(){
        return &uci;
    }

    vector<BTSID> GetAllServBTSs();
    ///@brief 接收调度消息
    ///@param _pSchM 表示调度信息的智能指针
    ///@return  无返回值
    void ReceiveSchedulingMessage(std::shared_ptr<SchedulingMessageDL> _pSchedulingMes);

    ///@brief 获得该MS的geometry
    ///@param void
    ///@return  返回geometry
    double GetGeometry();
    ///@brief 返回数据符号
    cmat GetTxDataSymbol(SCID _scid);
    ///@brief 返回参考符号
    cmat GetTxSRS(SCID _scid);
    ///@brief 返回SRS发射机的指针

    std::shared_ptr<SRSTxer> GetpSRSTxer() {
        return m_pSRSTxer;
    };
    double GetOllA(int _iRank, int _iRankID);
    double GetOllA(int _iRank);

    bool GetReInitialize() {
        return bReInitialize;
    };;

    bool GetTrafficState() {
        return bHasTraffic;
    };

    void SetTrafficState(bool _bTrafficState) {
        bHasTraffic = _bTrafficState;
    };
    cmat GetCovR(SCID _scid);
    void BeginApSounding();
protected:
    void CalcSINR_for_Calibration_new(
            double& _RSRP_dB_out, double& _CouplingLoss_dB_out,
            double& _SNR_dB_out, double& _SINR_dB_out, double& _Interf_dB_out);

    //    void ITU_Phase1_Stat_after_UE_attach();

    void ITU_Phase1_Stat_after_UE_attach_new();

    void BasicOutput_Statistican();


public:

    explicit MS(int _id);
    MS(int _id,int _type);
    ~MS() override = default;
};
