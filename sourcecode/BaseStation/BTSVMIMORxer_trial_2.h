///@file BTSVMIMORxer_trial_2.h
///@brief  BTSMRC接收机的声明
///@author zhengyi

#pragma once
#include "../Utility/Include.h"
#include "../Utility/SCID.h"
#include "../MobileStation/MSID.h"
//chty 1110 b
#include "../SafeMap.h"
//chty 1110 e
class BTSRxer;
class Detector_UL;

///@brief MRC接收机类

class BTSVMIMORxer_trial_2 : public BTSRxer {
private:
    //chty 1110 b
    SafeMap<int, vector<mat>> m_MSID2mSINR4BestRank;//used in ComputeRIandPMI and ComputeEstimateSINR, records the mSINR of all MS in the BTS.
    //chty 1110 e
public:
    //static int threadnum;
protected:
    int m_iMaxRank;
    int m_iBestRank;
    std::unordered_map<int, int> m_mSCPMI;
    std::unordered_map<int, cmat>m_mSCPrecodeMat;
    std::unordered_map<int, int> m_mMSID2Rank;

    std::unordered_map<int, deque<int> >MSID2HistoryTPC;


    void CombineMultiMS(const MSID& _msid, const vector<MSID>& _vMSID, const SCID& _scid,
                                              cmat& _mS,
                                              cmat& _mH,
                                              cmat & _mP);
    void ComputeRIandPMI(const MSID& _MSID);
    
    int CalcBestPMI(const MSID& _MSID, const int& _rank, const SCID& _scid);
    
    void ComputeEstimateSINR_ForSelectedRank_EBBCQI_AverageI(const MSID& _MSID,
        const int& _iRank, vector<mat>& _vmSINR4SelectedRank, std::unordered_map<int, int>& _vBestPMI);
    
    mat CalcSINR_EBBCQI_AverageI(const MSID& _msid, const int& _iRank, const SCID& _scid, const int& _iBestPMI);
    double EstimateWholeBandCapacity_For_mSINR(vector<mat>& _vmSINR4SelectedRank,const MSID& msid);
    ///@brief 估计SRS的SINR
    void ComputeEstimateSINR(const MSID& _MSID);
    ///@brief 估计业务信道的干扰
    ///@brief 计算数据信号的SINR
    void ComputeReceiveSINR(const std::shared_ptr<SchedulingMessageUL>& _pSchM);
    ///@brief BTS接收处理
    void ReceiveProcess();
    ///@brief BTS计算某个用户在某个载波上IoT,适应virtualMIMO
    itpp::mat CalculateIoTafterDet(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const cmat& _mPA,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);
    ///@brief BTS计算某个用户在某个载波上IoT,SRS上干扰的计算还是要这种
    itpp::mat CalculateIoTafterDet(std::shared_ptr<Detector_UL>& _pDetector, const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    double CalculateIoTave(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const cmat& _mPA,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);
    ///
    //    void ComputeVirtualMIMOReceiveSINR(std::shared_ptr<SchedulingMessageUL> _pSchM);

    //    itpp::mat CalculateIntraInf(const cmat& _mS, const cmat& _mH, const cmat& _mP,
    //            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
    //            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
    //            const vector<double>& _vWIL);

    //    itpp::mat CalculateInterInf(const cmat& _mS, const cmat& _mH, const cmat& _mP,
    //            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
    //            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
    //            const vector<double>& _vWIL);

    itpp::mat CalculateInf(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    //    cmat CalculateRe(const cmat& _mS, const cmat& _mH, const cmat& _mP,
    //            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
    //            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
    //            const vector<double>& _vWIL);

    itpp::mat CalculateSINR(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL,
            const cmat& _mRe);

    std::shared_ptr<SchedulingMessageUL> GetSchedulingMsg(const MSID& _msid);

    void SINRInterpolate(const MSID& msid, vector<mat> vSINR, int iSpace, vector<SCID> SCIDUsed);

    void vIoTEstimateInterpolate(const MSID& msid, vector<double>& vSINR, int iSpace, const vector<SCID>& SCidUsed);

    void vInfAfterDetecInterpolate(const MSID& _msid, vector<double>& _vInfAfterDetec, int _iSpace, vector<SCID> SCidUsed);

    void vIOT_forAverageInterpolate(const MSID& _msid, vector<double>& vIOT_forAverage, int iSpace, vector<SCID> SCidUsed);

    void vReEstimateInterpolate(const MSID& _msid, vector<cmat> _vRe, int _iSpace, vector<SCID> SCidUsed);

    vector<vector<mat > > ComputeEstSINR4MultiUE(vector<MSID> _vmsid,
            RBID _rbid_1,
            RBID _rbid_2);

    //    void ComputeReceiveSINR_test(std::shared_ptr<SchedulingMessageUL> _pSchM);

    //    vector<double> GetSINRfromBuffer(vector<RBID> _vRBUsed, MSID _msid);

    //    vector<double> GettestSINRfromBuffer(vector<RBID> _vRBUsed, MSID _msid);

    void ComputeULGeometry(const std::shared_ptr<SchedulingMessageUL>& _pSchM);

public:

    //DMRS ERROR Using SRS error model;
    void GetH4SRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL, mat& _HerrorScale, cmat & _Herror
            );
    //DMRS ERROR Using SRS error model;
    //    vector<double> GetSinr4SRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP,
    //            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
    //            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
    //            const vector<double>& _vWIL);
    mat GetSinr4SRSError(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);

    //no codes there,just for error claim
    void GetH4DMRSError(const MSID& _msid,
            const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL, const mat& _HerrorStd, const cmat & _Herror
            );

    deque<std::shared_ptr<ACKNAKMessageUL> > m_qRetxACKMsgQue_CurrentSlot;
    void BufferACKmsg4retxinMU(const std::shared_ptr<ACKNAKMessageUL>& _temp);
    void ResetACKMsgBuffer();
    void BufferReTxACKmsg();
    bool IsReTx(const MSID& _msid);



public:
    ///@brief 实现每个TTI接收机动作的函数
    void WorkSlot();
    ///@brief 计算两用户的SINR,返回所vector<mat>,这个函数是放在调度器里面用的
    //    vector < vector<double > > ComputeEstimateSINR(MSID _msid_1, MSID _msid_2, SCID _scid_1, SCID _scid_2);
    vector<vector<mat > > ComputeEstimateSINR(const MSID& _msid_1, const MSID& _msid_2, const RBID& _rbid_1, const RBID& _rbid_2);

    std::vector<mat> ComputeEstimateSINR(const MSID& _MSID, const RBID& FirstRBID, const RBID& LastRBID);
    ///@brief 计算检测前的IOT
    void ComputeIoTPreDetection();

    int GenerateTPCCommand_ByUser(const double& SINR, const double& PathLoss);
    int GenerateTPCCommand(const MSID& msid, const double& SINR, const double& PathLoss);
    int Generate_AccumulationTCP(const MSID& msid, const double& SINR, const double& TargetSINR);
    int Generate_absTPC(const MSID& msid, const double& SINR, const double& TargetSINR);

    void ReceivePHR(const std::shared_ptr<SchedulingMessageUL>& pSchM);
    void RefreshHARQSINR(MSID msid, int harqID);


public:
    ///@brief 构造函数
    explicit BTSVMIMORxer_trial_2(const BTSID& _BTSID);
    ///@brief 析构函数
    ~BTSVMIMORxer_trial_2() = default;
};
