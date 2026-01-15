#include "../NetworkDrive/Clock.h"
#include "../Utility/IDType.h"
#include "../Utility/SBID.h"
#include "../MobileStation/MSID.h"
#include "../MobileStation/CQIMessage.h"
#include "../Utility/RBID.h"
#include "../BaseStation/BTSID.h"
#include "../Utility/SCID.h"
#include "../ChannelModel/LinkMatrix.h"
#include "../MobileStation/ACKNAKMessageDL.h"
#include "../BaseStation/HARQTxStateBTS.h"
#include "../BaseStation/HARQRxStateBTS.h"
#include "../BaseStation/MSRxBufferBTS.h"
#include "../BaseStation/MSTxBufferBTS.h"
#include "../BaseStation/BTS.h"
#include "../MobileStation/HARQRxStateMS.h"
#include "../MobileStation/HARQTxStateMS.h"
#include "../MobileStation/MS.h"
#include "CodeBookFactory.h"
#include "../ChannelModel/BasicChannelState.h"
#include "../LinklevelInterface/LinkLevelInterface.h"
#include "../Parameters/Parameters.h"
#include "../Statistician/Statistics.h"
boost::mutex FTP_MSTxBufferBTS_mutex;

MSTxBufferBTS::MSTxBufferBTS(const BTSID& _btsid, const MSID& _msid){
    m_BTSID = _btsid;
    m_MSID = _msid;
    m_dSuccessRxKbit = 0.01;
    m_dTotalTxKbit = 0.01;

    m_iPacketNum_Right = 0;

    m_iPacketNum_Wrong = 0;

    CQISINR.assign(Parameters::Instance().MIMO_CTRL.IMaxRankNum, -100.0);
    CQIFilterFactor = 0.2;
    if(Parameters::Instance().AMC.SelfConfigOlla){
        CQIFilterFactor = Parameters::Instance().AMC.CQIFilterFactor;
        OllaController.SetConfig(Parameters::Instance().AMC.IinitialSINR,
                                 Parameters::Instance().AMC.ACKSchedWindow,
                                 Parameters::Instance().AMC.ACKThred,
                                 Parameters::Instance().AMC.NACKSchedWindow,
                                 Parameters::Instance().AMC.NACKThred,
                                 Parameters::Instance().AMC.SINRDownStep,
                                 Parameters::Instance().AMC.SINRUpStep);
    }
    LCNum = 4;
    for(int i=0;i<LCNum;i++){
        m_LC2PacketQueueDL.insert(pair< int,deque<std::shared_ptr<Universal_Packet>> >(i, deque<std::shared_ptr<Universal_Packet>>()));
        m_LC2Buffer.insert(pair<int, double>(i, 0.00001));
        m_LC2TxKBits.insert(pair<int, double>(i, 0.00001));
        m_LC2TxKBits_ThisSlot.insert(pair<int, double>(i, 0.00001));
    }
}

void MSTxBufferBTS::PushCQIMessage(const std::shared_ptr<CQIMessage>& _pCQIM) {
    m_qCQIMesQueue.emplace_back(_pCQIM);
}

void MSTxBufferBTS::ReceiveCQIMessage() {

    while ((!m_qCQIMesQueue.empty()) && (Clock::Instance().GetTimeSlot() - m_qCQIMesQueue.front()->GetBornTime() >= Parameters::Instance().ERROR.ICQI_DELAY)) {
        m_pCurrentCQIM = m_qCQIMesQueue.front();
        int Rank = m_pCurrentCQIM->GetRank();
        itpp::imat m_MCS = m_pCurrentCQIM->GetWideBandMCS();
        int iMCS = m_MCS(0,0);
        double SINR = LinkLevelInterface::Instance_DL().MCS2SINR_simplified(iMCS);
        if(CQISINR[Rank] == -100.0){
            CQISINR[Rank] = SINR;
        }
        else{
            CQISINR[Rank] = (1-CQIFilterFactor)*CQISINR[Rank] + CQIFilterFactor*SINR;
        }
        m_qCQIMesQueue.pop_front();

        Statistics::Instance().m_BS_DL_CQI[m_BTSID.GetBTS().GetTxID()] += LinkLevelInterface::Instance_DL().MCS2CQI(iMCS);
        Statistics::Instance().m_BS_DL_CQI_Times[m_BTSID.GetBTS().GetTxID()] += 1;
    }
}

std::shared_ptr<CQIMessage> MSTxBufferBTS::GetCQIMessage() {
    return m_pCurrentCQIM;
}

double MSTxBufferBTS::GetRateKbit() const{
    if (m_dSuccessRxKbit < 0.01) {
//        cout << "MSTxBufferBTS: SuccessRxKbit is now 0" << endl;
        return 0.01;
    }
    return m_dSuccessRxKbit;
}

double MSTxBufferBTS::GetTotalTxKbit() const{
    return m_dTotalTxKbit;
}

void MSTxBufferBTS::AccumulateSuccessRxKbit(double _dKbit) {
//    m_dSuccessRxKbit += _dKbit;
    auto dSchWindowLength = (double)Parameters::Instance().SIM.DL.ISchedulerWindowLength;
    m_dSuccessRxKbit = (299.0 / dSchWindowLength) * m_dSuccessRxKbit + (1.0 / dSchWindowLength) * _dKbit;
}

void MSTxBufferBTS::AccumulateTotalTxKbit(double _dKbit) {
//    m_dTotalTxKbit += _dKbit;
    auto dSchWindowLength = (double)Parameters::Instance().SIM.DL.ISchedulerWindowLength;
    m_dTotalTxKbit = (299.0 / dSchWindowLength) * m_dTotalTxKbit + (1.0 / dSchWindowLength) * _dKbit;
}
void MSTxBufferBTS::UpdateHistoryThrouthput(){
    auto dSchWindowLength = (double)Parameters::Instance().SIM.DL.ISchedulerWindowLength;
    for(int i=0;i<LCNum;i++){
        m_LC2TxKBits[i] = (299.0 / dSchWindowLength) * m_LC2TxKBits[i] + (1.0 / dSchWindowLength) * m_LC2TxKBits_ThisSlot[i];
        m_LC2TxKBits_ThisSlot[i] = 0.00001;
    }
}
void MSTxBufferBTS::SetH(const RBID& _rbid, const cmat& _mH) {
    for(auto row=0;row<_mH.rows();row++){
        for(auto col=0;col<_mH.cols();col++){
            if(isnan(_mH(row,col).real())){
                cout<<"setH1"<<endl;
                //cout<<_mH<<endl;
                assert(false);
            }
        }
    }
    m_mRBID2H[_rbid.ToInt()] = _mH;
}

void MSTxBufferBTS::SetH(const RBID& _rbid, const int& _iAntID, const cmat& _mH) {
    for(auto row=0;row<_mH.rows();row++){
        for(auto col=0;col<_mH.cols();col++){
            if(isnan(_mH(row,col).real())){
                cout<<"setH2"<<endl;
                //cout<<_mH<<endl;
                assert(false);
            }
        }
    }
    int icol = m_mRBID2H[_rbid.ToInt()].cols();
    m_mRBID2H[_rbid.ToInt()].set_submatrix(_iAntID, _iAntID, 0, icol - 1, _mH);
}

cmat MSTxBufferBTS::GetH(const RBID& _rbid) {
    auto _mH=m_mRBID2H[_rbid.ToInt()];
    for(auto row=0;row<_mH.rows();row++){
        for(auto col=0;col<_mH.cols();col++){
            if(isnan(_mH(row,col).real())){
                cout<<"getH1"<<endl;
                //cout<<_mH<<endl;
                assert(false);
            }
        }
    }
    return m_mRBID2H[_rbid.ToInt()];
}

cmat MSTxBufferBTS::GetH(const SBID& _sbid) {
    int final_rbid = _sbid.GetFirstRBID().ToInt();
    int gap = floor(((_sbid.GetLastRBID().ToInt() - _sbid.GetFirstRBID().ToInt()) + 1) / 2);
    final_rbid += gap;
    auto _mH=m_mRBID2H[final_rbid];
    for(auto row=0;row<_mH.rows();row++){
        for(auto col=0;col<_mH.cols();col++){
            if(isnan(_mH(row,col).real())){
                cout<<"getH2"<<endl;
                //cout<<_mH<<endl;
                assert(false);
            }
        }
    }
    return m_mRBID2H[final_rbid];
}

void MSTxBufferBTS::ComputeCovRInfo(){
    int icol = m_mRBID2H[SBID::Begin().GetFirstRBID().ToInt()].cols();
    for (SBID sbid = SBID::Begin(); sbid <= SBID::End(); ++sbid) {
        cmat mCovR = itpp::zeros_c(icol, icol);
        for (RBID rbid = sbid.GetFirstRBID(); rbid <= sbid.GetLastRBID(); ++rbid) {
            mCovR += (m_mRBID2H[rbid.ToInt()].H() * m_mRBID2H[rbid.ToInt()]);
        }
        mCovR *= 1.0 / Parameters::Instance().BASIC.ISBSize;
        m_mSBID2CovR[sbid.ToInt()] = mCovR;
    }
}

cmat MSTxBufferBTS::GetCovR(const SBID& _sbid){
    return m_mSBID2CovR[_sbid.ToInt()];
}
void MSTxBufferBTS::PushPacketDL(const std::shared_ptr<Packet>& _pPacket) {
    m_qPacketQueueDL.emplace_back(_pPacket);
}
void MSTxBufferBTS::PushPacketDL(const std::shared_ptr<Universal_Packet>& _pPacket){
    int lcid;
    int p = _pPacket->qos;
    if (p == 0) {
        lcid = 0;
    } else if (1 <= p && p <= 8) {
        lcid = 1;
    } else if (9 <= p && p <= 12) {
        lcid = 2;
    } else if (13 <= p && p <= 16) {
        lcid = 3;
    } else {
        cout << "invalid Packet QoS" << endl;
        assert(false);
    }
    m_LC2PacketQueueDL[lcid].emplace_back(_pPacket);
    m_LC2Buffer[lcid] += _pPacket->GetOriginSizeKbits();
}

int MSTxBufferBTS::GetDLPacketNum(){
    return static_cast<int>(m_qPacketQueueDL.size());
}

double MSTxBufferBTS::GetDLPacketSizeKbits() {
    double dTotalSizeKbits = 0;
    for (const auto& it : m_qPacketQueueDL) {
        dTotalSizeKbits += it->GetRemainUntransmitted_SizeKbits();
    }
    return dTotalSizeKbits;
}

double MSTxBufferBTS::GetDLFirstAvailablePacket_RemainUntransmitted_SizeKbits() {

    double dFirstSizeKbits = 0.0;

    std::shared_ptr<Packet> pFirstAvailablePacket = GetDLFirstAvailablePacket();
    if (pFirstAvailablePacket) {
        dFirstSizeKbits = pFirstAvailablePacket->GetRemainUntransmitted_SizeKbits();
    }

    return dFirstSizeKbits;
}

std::shared_ptr<Packet> MSTxBufferBTS::GetDLFirstAvailablePacket() {

    std::shared_ptr<Packet> pFirstAvailablePacket=std::shared_ptr<Packet>(nullptr);
    if (!m_qPacketQueueDL.empty()) {
        auto it = m_qPacketQueueDL.begin();
        while (it != m_qPacketQueueDL.end()) {
            double dFirstSizeKbits = (*it)->GetRemainUntransmitted_SizeKbits();
            if (dFirstSizeKbits < 0.001) {
                it++;
            } else {
                pFirstAvailablePacket = *it;
                break;
            }
        }
    }

    return pFirstAvailablePacket;
}

double MSTxBufferBTS::GetFirstPacketBorntimeSlot() {
    if (!m_qPacketQueueDL.empty()) {
        return m_qPacketQueueDL[0]->GetBornTimeSlot();
    } else {
        cout << "MSTxBufferBTS::GetFirstPacketBorntimeSlot() Error!" << endl;
    }
    assert(false);
}

vector<vector<pair<std::shared_ptr<Packet>, double> > > MSTxBufferBTS::ReducePacketBuf(vector<double>& _vLen) {
    vector<vector<pair<std::shared_ptr<Packet>, double> > > vResult;
    double dTotalLen = 0;
    int iRankNum = static_cast<int> (_vLen.size());
    for (const auto& len : _vLen) {
        dTotalLen += len;
    }
    std::shared_ptr<Packet> pFirstAvailablePacket = GetDLFirstAvailablePacket();
    assert(pFirstAvailablePacket);

    if (pFirstAvailablePacket->GetRemainUntransmitted_SizeKbits() < dTotalLen) {
        for (auto& len : _vLen) {
            len = pFirstAvailablePacket->GetRemainUntransmitted_SizeKbits() / iRankNum;
        }
    }

    for (const auto& dLen : _vLen) {
        vector<pair<std::shared_ptr<Packet>, double> > vRanki;
        pFirstAvailablePacket->ReduceUntransmittedSizeKbits(dLen);
        vRanki.emplace_back(make_pair(pFirstAvailablePacket, dLen));
        vResult.emplace_back(vRanki);
    }
    return vResult;
}

void MSTxBufferBTS::PopFinishedPacket() {
        bool IsEmpty = true;
        for(int i=0;i<LCNum;i++){
            auto& PackedQue = m_LC2PacketQueueDL[i];
            auto it = PackedQue.begin();
            while (it != PackedQue.end()) {
                if ((*it)->IsFullySuccessRecieved()) {
                    if (!((*it)->IsDrop_DueToTimeout())){
                        m_iPacketNum_Right++;
                    }
                    (*it)-> LogFinishedPacket();
                    if (!((*it)->IsDrop_DueToTimeout())) {
                        //                dSuccessRxKbit += 10.0;
//                        m_dSuccessRxKbit += 10.0;
                    }
                    it = PackedQue.erase(it);
                } else {
                    it++;
                }
            }
            //    m_dSuccessRxKbit = dSuccessRxKbit;
            if (!PackedQue.empty()) {
                IsEmpty  =false;
            }
        }
        if(IsEmpty){
            m_MSID.GetMS().SetTrafficState(false);
        }

}

void MSTxBufferBTS::PopTimeOutPacket(int _Time) {
    if(Parameters::Instance().TRAFFIC.ITrafficModel != Parameters::MixedTraffic){
        auto it = m_qPacketQueueDL.begin();
        while (it != m_qPacketQueueDL.end()) {
            if ((*it)->IsTimeout(_Time)) {
                m_iPacketNum_Wrong++;
                (*it)->DropPacket_DueToTimeout();
                (*it)-> LogFinishedPacket();
                it = m_qPacketQueueDL.erase(it);
            } else {
                it++;
            }
        }
        if (m_qPacketQueueDL.empty()) {
            m_MSID.GetMS().SetTrafficState(false);
        }
    }
    else{
        bool IsEmpty = true;
        for(int i=0;i<LCNum;i++){
            auto& PacketQue = m_LC2PacketQueueDL[i];
            auto it = PacketQue.begin();
            while (it != PacketQue.end()) {
                if ((*it)->IsTimeout(_Time)) {
                    m_LC2Buffer[i] -= (*it)->GetRemainUntransmitted_SizeKbits();
                    assert(m_LC2Buffer[i] > -0.001);
                    m_iPacketNum_Wrong++;
                    (*it)->DropPacket_DueToTimeout();
                    (*it)-> LogFinishedPacket();
                    it = PacketQue.erase(it);
                } else {
                    it++;
                }
            }
            if (!PacketQue.empty()) {
                IsEmpty = false;
            }
        }
        if(!IsEmpty){
            m_MSID.GetMS().SetTrafficState(false);
        }

    }
}
int MSTxBufferBTS::GetSmallestLCIndexWithData(){
    for(int i=0;i<LCNum;i++){
        if(m_LC2Buffer[i] >= 0.001){
            return i;
        }
    }
    return -1;
}
double MSTxBufferBTS::GetSumOfBufferSize(){
    double res = 0.0;
    for(int i=0;i<LCNum;i++){
        if(m_LC2Buffer[i] >= 0.001){
            res += m_LC2Buffer[i];
        }
    }
    return res;
}
bool MSTxBufferBTS::HaveAnyData()
{
    return GetSumOfBufferSize() >= 0.001;
}
vector<vector<pair<std::shared_ptr<Universal_Packet>, double>>> MSTxBufferBTS::ReduceBuffer(double datasize){
    vector<pair<std::shared_ptr<Universal_Packet>, double>> res;
    for(int i=0;i<LCNum;i++){
        deque<std::shared_ptr<Universal_Packet>>& PacketQue = m_LC2PacketQueueDL[i];
        auto it = PacketQue.begin();
        while(it!=PacketQue.end() && datasize >= 0.001){
            double RemainSize = (*it)->GetRemainUntransmitted_SizeKbits();
            if(RemainSize >= 0.001){
                if(RemainSize > datasize){
                    res.push_back({(*it), datasize});
                    m_LC2Buffer[i] -= datasize;
                    m_LC2TxKBits_ThisSlot[i] += datasize;
                    assert(m_LC2Buffer[i] > -0.001);
                    (*it)->ReduceUntransmittedSizeKbits(datasize);
                    datasize = 0;
                }
                else{
                    res.push_back({(*it), RemainSize});
                    m_LC2Buffer[i] -= RemainSize;
                    m_LC2TxKBits_ThisSlot[i] += RemainSize;
                    assert(m_LC2Buffer[i] > -0.001);
                    (*it)->ReduceUntransmittedSizeKbits(RemainSize);
                    datasize -= RemainSize;
                }
            }
            it++;
        }
    }
    return vector<vector<pair<std::shared_ptr<Universal_Packet>, double>>>({res});
}

void MSTxBufferBTS::OllaProcess(bool acknack, int iSendNum){
    OllaController.OllaProcess(acknack, iSendNum);
}
double MSTxBufferBTS::GetOllaSINR(){
    return OllaController.GetOllaSINR();
}
double MSTxBufferBTS::GetCQISINR(int rank){
    return CQISINR[rank];
}
void MSTxBufferBTS::LogPacketErrorRate() {
    boost::mutex::scoped_lock lock(FTP_MSTxBufferBTS_mutex);
    double dPacketNum_Wrong = m_iPacketNum_Wrong;
    double dPacketNum = m_iPacketNum_Wrong + m_iPacketNum_Right;
    if (dPacketNum < 0.001) {
        return;
    }
    double dPacketErrorRate = dPacketNum_Wrong / dPacketNum;
    int iTime = Clock::Instance().GetTimeSlot();
    //hyl 冗余
//    Observer::Print("XRPacketErrorRate") << setw(20) << iTime
//            << setw(20) << m_MSID
//            << setw(20) << m_BTSID
//            << setw(20) << m_iPacketNum_Wrong
//            << setw(20) << m_iPacketNum_Right
//            << setw(20) << dPacketErrorRate << endl;
}
Olla::Olla(double sinr, int ASC, int AC, int NASC, int NAC, double DownStep, double UpStep) {
    OllaSINRdB = sinr;

    AckSchedCounter = 0;
    AckSchedWindow = ASC;
    AckCounter = 0;
    AckThred = AC;

    NackSchedCounter = 0;
    NackSchedWindow = NASC;
    NackCounter = 0;
    NackThred = NAC;

    SINRDownStep = DownStep;
    SINRUpStep = UpStep;
}

void Olla::SetConfig(double sinr, int ASC, int AC, int NASC, int NAC, double DownStep, double UpStep) {
    OllaSINRdB = sinr;

    AckSchedWindow = ASC;
    AckThred = AC;

    NackSchedWindow = NASC;
    NackThred = NAC;

    SINRDownStep = DownStep;
    SINRUpStep = UpStep;
}

void Olla::OllaProcess(bool ACKNACK, int iSendNum) {
    if(iSendNum == 1){
        AckSchedCounter++;
        NackSchedCounter++;
        if(ACKNACK){
            AckCounter++;
            if(AckCounter >= AckThred && AckSchedCounter >= AckSchedWindow){
                AckSchedCounter =0;
                AckCounter =0;
                NackSchedCounter = 0;
                NackCounter = 0;
                OllaSINRdB += SINRUpStep;
            }
        }
        else{
            NackCounter++;
            if(NackCounter >= NackThred && NackSchedCounter >= NackSchedWindow){
                AckSchedCounter =0;
                AckCounter =0;
                NackSchedCounter = 0;
                NackCounter = 0;
                OllaSINRdB -= SINRDownStep;
            }
        }
    }
}
double Olla::GetOllaSINR(){
    return OllaSINRdB;
}