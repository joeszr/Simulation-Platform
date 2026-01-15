///@file BTSTXer.cpp
///@brief  BTS发送模块类实现
///@author wangxiaozhou

#include "../Utility/IDType.h"
#include "../Utility/SCID.h"
#include "../Utility/RBID.h"
#include "../Utility/SBID.h"
#include "BSID.h"
#include "BTSID.h"
#include "../BaseStation/CodeBook.h"
#include "../MobileStation/MSID.h"
#include "BTSTxer.h"

BTSTxer::BTSTxer(const BTSID& _btsid)
:m_MSID2PanelAndvBeamIndex(Parameters::Instance().BASIC.ITotalMSNum)
{
    m_pActiveSet= nullptr;
    m_BTSID = _btsid;
}

void BTSTxer::Initialize(SafeDeque<std::shared_ptr<SchedulingMessageDL> > *_q, std::unordered_map<int, MSTxBufferBTS>* _pTxBufferBTS, vector<MSID>* _pActiveSet) {
    m_pScheduleMessageQueue = _q;
    m_pTxBufferBTS = _pTxBufferBTS;
    m_pActiveSet = _pActiveSet;
}

void BTSTxer::ReceiveSoundingMessage() {
}

void BTSTxer::PushSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage) {
}

void BTSTxer::SetApSoundingMessage(const std::shared_ptr<SoundingMessage>& _pSoundingMessage) {
}

void BTSTxer::ReceiveApSoundingMessage(){
}

cmat BTSTxer::GetSoundingMat(MSID _msid, RBID _rbid) {
	assert(false);
    //return itpp::zeros_c(1);
}

//std::shared_ptr<SchedulingMessageDL> BTSTxer::GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, vector<SBID> _vSBID, std::map<SBID, Competor> _mSBID2Competor) {
//    assert(false);
//    return std::shared_ptr<SchedulingMessageDL > ();
//}

//std::shared_ptr<SchedulingMessageDL> BTSTxer::GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, int mMCS, vector<SBID> _vSBUsed, std::map<SBID, CompetorRankA> _mSBID2Competor){
//    assert(false);
//    return std::shared_ptr<SchedulingMessageDL > ();
//}

//std::shared_ptr<SchedulingMessageDL> BTSTxer::GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed, std::map<SBID, CompetorRankA> _mSBID2Competor, map<MSID,pair<int,vector<int> > > _mMSID2PanelAndvBSBeam){
//    assert(false);
//    return std::shared_ptr<SchedulingMessageDL > ();
//}

//std::shared_ptr<SchedulingMessageDL> BTSTxer::GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed, std::map<SBID, CompetorRankA> _mSBID2Competor, vector<int> _vSelectedBeamIndex){
//    assert(false);
//    return std::shared_ptr<SchedulingMessageDL > ();
//}
//
//std::shared_ptr<SchedulingMessageDL> BTSTxer::GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed, std::map<SBID, Competor> _mSBID2Competor, vector<vector<pair<shared_ptr<Packet>,double> > > _vpPacketAndSizeKbits){
//    assert(false);
//    return std::shared_ptr<SchedulingMessageDL > ();
//}
//
//std::shared_ptr<SchedulingMessageDL> BTSTxer::GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed, std::map<SBID, CompetorRankA> _mSBID2Competor, vector<vector<pair<std::shared_ptr<Packet>,double> > > _vpPacketAndSizeKbits){
//    assert(false);
//    return std::shared_ptr<SchedulingMessageDL > ();
//}

//std::shared_ptr<SchedulingMessageDL> BTSTxer::GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed) {
//    assert(false);
//    return std::shared_ptr<SchedulingMessageDL > ();
//}

//std::shared_ptr<SchedulingMessageDL> BTSTxer::GenerateSchedulingMessageDL(MSID _msid, int _iHARQID, int _iRank, imat mMCS, vector<SBID> _vSBUsed,vector<vector<pair<shared_ptr<Packet>,double> > > _vpPacketAndSizeKbits) {
//    assert(false);
//    return std::shared_ptr<SchedulingMessageDL > ();
//}

vector<MSID> BTSTxer::GetSchedvMSID(SBID _sbid){
    assert(false);
//    cout<<"Error in BTSTxer::GetSchedvMSID()!"<<endl;
//    vector<MSID> vMSID;
//    return vMSID;
}
