#include "../Utility/Include.h"
#include "./BTSEBBRank1Txer.h"
class SBID;
class SCID;
class RBID;

class BTSMUEBBTxer : public BTSEBBRank1Txer {
public:
    /// @brief
   // std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, vector<SBID> _vSBUsed, std::map<SBID, Competor> _mSBID2Competor);
    std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, int mMCS, const vector<SBID>& _vSBUsed, std::unordered_map<int, CompetorRankA> _mSBID2Competor);
   //used in MUProportionalFairRBRankAScheduler
    std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, imat mMCS, const vector<SBID>& _vSBUsed, std::unordered_map<int, CompetorRankA> _mSBID2Competor, std::unordered_map<int,pair<int,vector<int> > > _mMSID2PanelAndvBSBeam);
   // std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed, std::map<SBID, Competor> _mSBID2Competor, vector<vector<pair<std::shared_ptr<Packet>,double> > > _vpPacketAndSizeKbits);
    std::shared_ptr<SchedulingMessageDL> GenerateSchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank, int mMCS, const vector<SBID>& _vSBUsed, std::unordered_map<int, CompetorRankA> _mSBID2Competor, const vector<vector<pair<std::shared_ptr<Packet>,double> > >& _vpPacketAndSizeKbits);
public:
    /// @brief 执行BTS发送操作，由BTS的WorkSlot()调用
    void WorkSlot() override;
public:
    explicit BTSMUEBBTxer(const BTSID& _btsid);
    ~BTSMUEBBTxer() = default;
};

