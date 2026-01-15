#include "../Utility/Include.h"
#include "../BaseStation/CodeBook.h"
#include "../BaseStation/NR_CodeBook.h"
//@threads
class MSID;
class SBID;
class RBID;
class MSTxBufferBTS;

class CompetorRankA : public vector<MSID> {
public:
    /// 码本
    std::shared_ptr<CodeBook> m_pCodeBook;
    std::unordered_map<int, int> m_mMSID2Rank;

    //double dSuccessRxKbit;
    //map<std::pair<MSID, SBID>, mat> m_mMSIDSBID2SINR;
    std::unordered_map<int,vector<double> > m_vMSID2SINR;
    std::unordered_map<int, mat> m_mMSID2SINR;
    std::unordered_map<int, double> m_dMSID2SINR;

    
    //map<std::pair<MSID, SBID>, imat> m_mMSIDSBID2MCS;
    std::unordered_map<int, imat> m_mMSID2MCS;
    std::unordered_map<int, int> m_iMSID2MCS;
    // map<SBID, cmat> m_mSBID2CodeWord;
       cmat m_CodeWord;
    //map<SBID, double> m_mSBID2Priority;
       double m_Priority;
    std::unordered_map<int, double> m_mSBID2Capacity;
       double m_Capacity;

    std::unordered_map<int, double> m_mMSID2Capacity;
    
public:
    double GetPerformanceMetric(bool _bCompareWithPriority = true) const;
    
//2019.6.13 平台更新
//    cmat m_CodeWord;
//    double m_Priority;
//    double m_Capacity;
private:
    //@threads
    int ReComputeSINR(const MSID& _msid, const cmat& _mPA, const cmat& _mP, const SBID& _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS, double _dTxDSINR);
    vector<double> ReComputeVSINR(const MSID& _msid, const cmat& _mPA, const cmat& _mP, const SBID& _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS, double _dTxDSINR);
    vector<double> ReComputeVSINR_simplified(MSID _msid, SBID _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS, double _dTxDSINR);
    imat ReComputeIdealSINR(const MSID& _msid, const cmat& _mPA, const cmat& _mP, const SBID& _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS, double _dTxDSINR);
    //@threads
    void ComputeCompeteDataEigenBasedBF(const SBID& _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS);

    double GetRENum_SingleSB();
    


public:
    double GetTBSizeKbit(int _iMCS,int v=1);
    //@threads
    void ComputeCompeteData(int _iMIMOMode, const SBID& _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS, CodeBook* _pCodeBook = nullptr);

    void SetRank(const MSID& _msid, const int& _iRank);

    int GetRank(const MSID& _msid);

    imat GetMCS(const MSID& _msid, const SBID& _sbid);

    void CalcPreCodeWord(const SBID& _sbid, std::unordered_map<int, MSTxBufferBTS>& _mMSID2MSTxBufferBTS);
public:
    CompetorRankA();
    CompetorRankA(const vector<MSID>& _v);
    ~CompetorRankA() = default;
};