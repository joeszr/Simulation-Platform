#include "../Utility/Include.h"

class MSEBBRank1Rxer;
class MSID;

class MSMUEBBRankARxer : public MSEBBRank1Rxer {
protected:
    void ComputeEstimateSINR();
    void ComputeEstimateSINR_1TxSRSEnhanceCQI_AverageI(int _iRank);
    void ComputeEstimateSINR_TxDCQI_AverageI(int _iRank=0);
    void ComputeReceiveSINR();
    ///@brief MUEBB的DMRS误差建模
    cmat DMRSErrorModel(const vector<MSID>& _vMSID, const cmat& _mS, const cmat& _mH, const cmat& _mP,
            const vector<cmat>& _vSIS, const vector<cmat>& _vSIH,
            const vector<cmat>& _vSIP, const vector<cmat>& _vWIS,
            const vector<double>& _vWIL);
public:
    MSMUEBBRankARxer(BTSID _MainservBTSID, MSID _msid, vector<mat> *_pSINR, HARQRxStateMS* _pHARQRxStateMS);
    ~MSMUEBBRankARxer() override = default;
};

