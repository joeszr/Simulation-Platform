///@file SchedulingMessageDL.cpp
///@brief SchedulingMessage类的实现
///@author wangxiaozhou
int SchedulingMessageDLID=0;
#include "../Utility/IDType.h"
#include "../MobileStation/MSID.h"
#include "../Utility/RBID.h"
#include "../Utility/SBID.h"
#include "../NetworkDrive/Clock.h"
#include "SchedulingMessageDL.h"
#include "../BaseStation/BTSID.h"
#include "../TrafficModel/Packet.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
extern std::mutex coutmutex;

SchedulingMessageDL::SchedulingMessageDL(const MSID& _msid, int _iHARQID, int _iRank)
:m_mSBID2CodeWord(Parameters::Instance().BASIC.ISBNum),
 m_mSBID2LayerNum(Parameters::Instance().BASIC.ISBNum),
 m_MSID2PanelAndvBSBeam(Parameters::Instance().BASIC.DNumMSPerBTS)
{
    ID=++SchedulingMessageDLID;
    m_MSID = _msid;
    m_iHARQID = _iHARQID;
    m_iRank = _iRank;
    m_mSendNum = itpp::ones_i(m_iRank + 1, 1);
    iSendNum = 1;
    m_bIsTM3 = false;
    m_iBornTimeSlot = Clock::Instance().GetTimeSlot();
    m_iLatestTransTimeSlot = Clock::Instance().GetTimeSlot();
    m_ik1_slot =1;
    //    m_ik3_slot = -1;
}

SchedulingMessageDL::SchedulingMessageDL(
        const MSID& _msid, int _iHARQID, int _iRank, const vector<vector<pair<std::shared_ptr<Packet>, double> > >& _vpPacketAndSizeKbits):m_mSBID2CodeWord(Parameters::Instance().BASIC.ISBNum),
                                                                                                                             m_mSBID2LayerNum(Parameters::Instance().BASIC.ISBNum),
                                                                                                                             m_MSID2PanelAndvBSBeam(Parameters::Instance().BASIC.DNumMSPerBTS) {
    ID=++SchedulingMessageDLID;
    m_MSID = _msid;
    m_iHARQID = _iHARQID;
    m_iRank = _iRank;
    m_mSendNum = itpp::ones_i(m_iRank + 1, 1);
    iSendNum = 1;
    m_vpPacketAndSizeKbits = _vpPacketAndSizeKbits;
    m_iBornTimeSlot = Clock::Instance().GetTimeSlot();
    m_iLatestTransTimeSlot = Clock::Instance().GetTimeSlot();

    m_ik1_slot = -1;
    //    m_ik3_slot = -1;
}

SchedulingMessageDL::~SchedulingMessageDL() {
    //hyl 冗余
//    {std::lock_guard<std::mutex>l(coutmutex);
//        Observer::Print("SchedulerMessageDLinfo") << setw(20)<<Clock::Instance().GetTimeSlot() <<setw(20)<< "destoring"<<setw(20)<<"(/,/)"
//        <<setw(20)<< ID << setw(20)
//                                                  << GetMSID().ToInt() << setw(20)
//                                                  << GetBornTime()
//                                                  << setw(20)<<GetLatestTransTime()
//                                                  << setw(20)<< GetiSendNum() << endl;
//    }
}

///获得被调度MSID

MSID SchedulingMessageDL::GetMSID() {
    return m_MSID;
}

int SchedulingMessageDL::GetMSIDInt() {
    return m_MSID.ToInt();
}

///获得本次调度分配的进程号

int SchedulingMessageDL::GetHARQID() const{
    return m_iHARQID;
}

///获得被调度MS使用的调制编码方式

imat SchedulingMessageDL::GetMCS() {
    return m_mMCS;
}
int SchedulingMessageDL::GetiMCS() const{
    return iMCS;
}

///获得分配给MS的Subband组

vector<SBID> SchedulingMessageDL::GetSBUsed() {
    return m_vSBUsed;
}

int SchedulingMessageDL::GetSendNum(int _iRankID) {
    return m_mSendNum(_iRankID, 0);
}
int SchedulingMessageDL::GetiSendNum() const{
    return iSendNum;
}

cmat SchedulingMessageDL::GetCodeWord() {
    return m_mWideBandCodeWord;
}

cmat SchedulingMessageDL::GetCodeWord(const SBID& _sbid) {
    if (m_mSBID2CodeWord.count(_sbid.ToInt())) {
        return m_mSBID2CodeWord[_sbid.ToInt()];
    } else {
        assert(false);
    }
    assert(false);
    return  cmat(1,1);
}

imat SchedulingMessageDL::GetSendNum() {
    return m_mSendNum;
}

std::unordered_map<int, int> SchedulingMessageDL::GetLayerNum() {
    return m_mSBID2LayerNum;

}

mat SchedulingMessageDL::GetTBSize() {
    return m_mTBSize;
}
double SchedulingMessageDL::GetdTBSize() const{
    return dTBSize;
}

int SchedulingMessageDL::GetTBSize_AllRank() {
    int iTBSize_AllRank = 0;
    int irow = m_mTBSize.rows();
    for (int i = 0; i < irow; i++) {
        iTBSize_AllRank += m_mTBSize(i, 0);
    }
    return iTBSize_AllRank;
}

int SchedulingMessageDL::GetLayerNum(const SBID& _sbid) {
    if (m_mSBID2LayerNum.count(_sbid.ToInt())) {
        return m_mSBID2LayerNum[_sbid.ToInt()];
    } else {
        assert(false);
    }
    assert(false);
    return  -1;
}

void SchedulingMessageDL::SetMCS(const imat& _mMCS) {
    m_mMCS = _mMCS;
}
void SchedulingMessageDL::SetMCS(int _iMCS) {
    iMCS = _iMCS;
}
void SchedulingMessageDL::SetSINR(double sinr){
    EstimateSINR = sinr;
}

void SchedulingMessageDL::SetSBUsed(const vector<SBID>& _vSBUsed) {
    m_vSBUsed = _vSBUsed;
}

void SchedulingMessageDL::SetCodeWord(const cmat& _mWideBandCodeWord) {
    m_mWideBandCodeWord = _mWideBandCodeWord;
}

void SchedulingMessageDL::IncreaseSendNum(int _iRankID) {
    m_mSendNum(_iRankID, 0) += 1;
}
void SchedulingMessageDL::IncreaseSendNum() {
    iSendNum += 1;
}

int SchedulingMessageDL::GetRank() const{
    return m_iRank;
}

void SchedulingMessageDL::SetCodeWord(const std::unordered_map<int, cmat>& _mSBID2CodeWord) {
    m_mSBID2CodeWord = _mSBID2CodeWord;
}

void SchedulingMessageDL::SetCodeWord(const SBID& _sbid, const cmat& _mCodeWord) {
    m_mSBID2CodeWord[_sbid.ToInt()] = _mCodeWord;
}

void SchedulingMessageDL::SetLayerNum(const SBID& _sbid, int _iLayerNum) {
    m_mSBID2LayerNum[_sbid.ToInt()] = _iLayerNum;
}

int SchedulingMessageDL::CountRBNum() {
    if (!m_vSBUsed.empty()) {
        return Parameters::Instance().BASIC.ISBSize * m_vSBUsed.size();
    }
    return 0;
}

bool SchedulingMessageDL::IsTM3TB() const{
    return m_bIsTM3;
}

void SchedulingMessageDL::SetMap_MSID2PanelAndBSBeam(const std::unordered_map<int, pair<int, vector<int> > >& mMSID2PanelAndvBSBeam) {
    m_MSID2PanelAndvBSBeam = mMSID2PanelAndvBSBeam;
}

std::unordered_map<int, pair<int, vector<int> > > SchedulingMessageDL::GetMap_MSID2PanelAndvBSBeam() {
    return m_MSID2PanelAndvBSBeam;
}

vector<vector<pair<std::shared_ptr<Packet>, double> > >& SchedulingMessageDL::GetPacketAndSizeKbits() {
    return m_vpPacketAndSizeKbits;
}

int SchedulingMessageDL::GetBornTime() const{
    return m_iBornTimeSlot;
}

void SchedulingMessageDL::SetLatestTransTime(int _iLatestTransTime) {
    m_iLatestTransTimeSlot = _iLatestTransTime;
}

int SchedulingMessageDL::GetLatestTransTime() const{
    return m_iLatestTransTimeSlot;
}

bool SchedulingMessageDL::CanReTx(int _iTime) const{
    //    assert(_iTime <= (m_iLatestTransTimeSlot + m_ik1_slot + m_ik3_slot));
    return _iTime >= (m_iLatestTransTimeSlot + m_ik1_slot + Parameters::Instance().SIM.DL.Imink3_slot);
}

bool SchedulingMessageDL::CanHARQProcess(int _iTime) const{
    return _iTime >= (m_iLatestTransTimeSlot + m_ik1_slot);
}

void SchedulingMessageDL::Setk1_slot(int _iGap) {
    m_ik1_slot = _iGap;
}

int SchedulingMessageDL::Getk1_slot() const{
    return m_ik1_slot;
}

std::shared_ptr<Packet> SchedulingMessageDL::GetFirstPacket() {
    return m_vpPacketAndSizeKbits[0][0].first;
}

double SchedulingMessageDL::GetCarriedPacket_SizeKbits() {

    double dCarriedPacket_SizeKbits = 0.0;

    int iRank = m_vpPacketAndSizeKbits.size() - 1;
    for (int iRankID = 0; iRankID <= iRank; ++iRankID) {
        int iPacketNum = static_cast<int> (m_vpPacketAndSizeKbits[iRankID].size());
        for (int i = 0; i < iPacketNum; ++i) {
            dCarriedPacket_SizeKbits += m_vpPacketAndSizeKbits[iRankID][i].second;
        }
    }
    return dCarriedPacket_SizeKbits;
}

void SchedulingMessageDL::SetTBSizeKbit() {
    int iRank = m_iRank;
    imat mMCS = m_mMCS;
    std::unordered_map<int, int> mSBID2LayerNum = m_mSBID2LayerNum;
    int iRENum_AllSB = 0;
    for (auto it = m_vSBUsed.begin(); it != m_vSBUsed.end(); ++it) {
        //        //todo PDCCH
        //        int iSymbolNumPerSlot = 14;
        //        int iRENumPerPRB_DMRS = (it->second + 1) / 2 * 4;
        //
        //        int iRENum_temp = Parameters::Instance().BASIC.IRBSize * iSymbolNumPerSlot - iRENumPerPRB_DMRS;

        //        iRENum_AllSB += min(iRENum_temp, 156) * Parameters::Instance().BASIC.ISBSize;
        //cty-merge
//        iRENum_AllSB += LinkLevelInterface::Instance_DL().GetRENum(Parameters::Instance().BASIC.ISBSize, it->second);


//        int iRENumPerPRB;
//        if(Parameters::Instance().BASIC.RRC_Config_On == 1){
//            iRENumPerPRB = _mCalcRENum.CalculateRENum();
//        }else{
//            iRENumPerPRB = Parameters::Instance().BASIC.DDLRENumPerRBforData;
//        }
        int iRENumPerPRB = Parameters::Instance().BASIC.DDLRENumPerRBforData;
        iRENum_AllSB += iRENumPerPRB * Parameters::Instance().BASIC.ISBSize;
    }

    m_mTBSize = itpp::zeros(iRank + 1, 1);
    for (int iRankID = 0; iRankID <= iRank; ++iRankID) {
        m_mTBSize(iRankID, 0) = LinkLevelInterface::Instance_DL().MCS2TBSKBit(mMCS(iRankID, 0), iRENum_AllSB);
    }
}
void SchedulingMessageDL::SetTBSizeKbit_TDD() {
    int iRank = m_iRank;
    int MCS = iMCS;
    int SBNum = m_vSBUsed.size();
//    int iRENum_AllSB = LinkLevelInterface::Instance_DL().GetRENum(SBNum, 1); //1这个参数里面根本没用到
    int iRENum_AllSB = LinkLevelInterface::Instance_DL().GetRENumInRange(m_vSBUsed[0].ToInt(), m_vSBUsed[SBNum-1].ToInt());
    dTBSize = LinkLevelInterface::Instance_DL().MCS2TBSKBit2(MCS, iRENum_AllSB,iRank+1);
}
void SchedulingMessageDL::SetdTBSize(double _dTBSize) {
    dTBSize = _dTBSize;
}
void SchedulingMessageDL::SetUniversal_PacketAndSizeKbits(vector<vector<pair<std::shared_ptr<Universal_Packet>, double>>>& _vpPacketAndSizeKbits){
    m_vpUniversal_PacketAndSizeKbits = _vpPacketAndSizeKbits;
}
vector<vector<pair<std::shared_ptr<Universal_Packet>, double>>>& SchedulingMessageDL::GetUniversal_PacketAndSizeKbits(){
    return m_vpUniversal_PacketAndSizeKbits;
}
void SchedulingMessageDL::SetCCE(const int _ccelevel, vector<int>& _cceSB){
    CCELevel = _ccelevel;
    CCESB = _cceSB;
}
int SchedulingMessageDL::GetCCELevel(){
    return CCELevel;
}
vector<int>& SchedulingMessageDL::GetCCESB(){
    return CCESB;
}