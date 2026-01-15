///@file MSEBBRankOneRxer.h
///@brief  EBBRankOne接收机的声明
///@author wangxiaozhou

#pragma once
#include "../Utility/Include.h"
#include "../BaseStation/CodeBook.h"
using namespace std;
using namespace itpp;

class MSRxer;
class SCID;

///@brief EBB接收机类

class MSEBBRank1Rxer : public MSRxer {
protected:
    /// 码本
    std::shared_ptr<CodeBook> m_pCodeBook;
    /// 记录各个subband上的PMI信息
    std::unordered_map<int, int> m_mSubBandPMI;
    std::unordered_map<int, itpp::cmat> m_mSubBandPrecode;
    int m_iMaxRank;
    int m_iBestRank;
    cmat m_WCovR;
    std::unordered_map<int, cmat>m_mSubBandPrecodeMat;

protected:
    ///@brief 实现构造和发送CQI信息的函数
    void SendCQIFeedback();
    ///@brief 实现EBB的sounding
    void Sounding(int _iAntIndex);
    ///
    void Sounding();
    ///@brief 估计公共参考信号的SINR
    void ComputeRIandPMI();
    virtual void ComputeEstimateSINR();
    virtual void ComputeEstimateSINR_EBBCQI_AverageI(int _iRank);
    ///@brief 计算数据信号的SINR
    virtual void ComputeReceiveSINR();
    double EstimateWholeBandCapacity();

    ///@brief EBB检测接收过程
    void ReceiveProcess();
    ///@brief SRS误差建模
    cmat SRSErrorModel(const cmat& _mH);
    ///@brief DMRS误差建模
    cmat DMRSErrorModel(const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);
    ///@brief 天线校准误差
    cmat AntennaCalibrationErrorModel(const cmat& _mH);
    void UpdateAverageInterferenceCovR();
    void UpdateCovR();
    //
    int CalcBestPMI(int _rank,const SBID& _sbid);
    mat CalcSINR_EBBCQI_AverageI(int _iRank, SCID _scid, int _iBestPMI);
    double CalcSINR_EBBCQI_AverageI_FDD(int _iRank, SCID _scid, int _iBestPMI);
    void ComputeEstimateSINR_ForSelectedRank_EBBCQI_AverageI(
        int _iRank, vector<mat>& _vmSINR4SelectedRank,map<SBID, int>& _vBestPMI) ;
    double EstimateWholeBandCapacity_For_mSINR(vector<mat>& _vmSINR4SelectedRank);
    double EstimateWholeBandCapacity_For_mSINR(vector<double>& _vmSINR4SelectedRank,int v=1);
    void ComputeEstimateSINR_ForSelectedRank_EBBCQI_AverageI_TDD(int _iRank, vector<double>& _vmSINR4SelectedRank);
    double CalcSINR_EBBCQI_AverageI_TDD(int _iRank, SCID _scid);
public:
    ///@brief 实现每个TTI接收机动作的函数
    void WorkSlot();
    /// 非周期Souding
    void ApSounding();
    void RefreshHARQSINR(int, int);
public:
    ///@brief 构造函数
    MSEBBRank1Rxer(BTSID _MainServBTSID, MSID _msid, vector<mat> *_pSINR, HARQRxStateMS* _pHARQRxStateMS);
    ///@brief 析构函数
    ~MSEBBRank1Rxer() override = default;

private:
    //简化后的TDD流数选择
    void SelectRankTDDNew();

    //TDD时估计时不同流数、不同SC上的SINR
    vector<vector<double>> vRank_SC_SINR;
};