///@file MRCTXer.h
///@brief  BTS MRC发射机类函数声明
///@author zhengyi
#pragma once
#include "../Utility/Include.h"
using namespace std;
using namespace itpp;

class BTSTxer;
class SCID;
class RBID;
class SBID;
class SoundingMessage;
class MSTxer;
/// @brief BTS MRC发射机类

class MSMRCTxer : public MSTxer {
public:
    ///@brief 实现每个TTI接收机动作的函数
    void WorkSlot();
    ///@brief 获得上行业务信道载波_scid上的功率
    cmat GetTxDataSymbol(SCID _scid);
    cmat GetTxDataSymbol(SCID _scid,int _iPortNum);
    
   
    ///@brief 获得上行SRS信道载波_scid上的功率
    cmat GetTxSRS(SCID _scid);
    cmat GetTxSRS(SCID _scid,int _iPortNum);
    cmat GetTxSRS();

    cmat GetLastTxP(); //zhengyi 用于干扰估计
    ///@brief 获得上行用户调度信息
    void GetSchedulingMsg();
    ///@brief 获得用户当前功率可支持调度的RB数
    int GetRBNumSupport();

private:
    ///@brief 实现上行MS的功控
    void PowerControl();
    ///@brief 实现上行SRS功控
    void PowerControl4SRS();

public:
    ///@brief 构造函数
    MSMRCTxer(BTSID _MainServBTSID, MSID _MSID, std::shared_ptr<HARQTxStateMS> _pHARQTxStateMS);
    ///@brief 析构函数
    ~MSMRCTxer() override = default;
    
};