#include "../NetworkDrive/Clock.h"
#include "../Utility/IDType.h"
#include "../Utility/SBID.h"
#include "../MobileStation/MSID.h"
#include "../Utility/SCID.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "MSRxBufferBTS.h"
boost::mutex FTP_MSRxBufferBTS_mutex;
MSRxBufferBTS::MSRxBufferBTS(const MSID& _msid) {
    m_MSID = _msid;
    m_dSuccessRxKbit = 0.01;
    m_vSRSSINR.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace);
    m_vSINR.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace);
    //20170271
    m_vULGeometry.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, 0); // 计算上行geometry

    //    m_vSINR_test.resize(Parameters::Instance().SIM_UL.UL.ISCNum, 0);
    m_vIoTEstimate.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, -1);
    m_vInfEst.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, -1);
    m_vSRSIoT.resize(Parameters::Instance().BASIC.ISCNum / Parameters::Instance().LINK_CTRL.IFrequencySpace, -1);
    m_dOLLAOffsetDB = 0;
    //    m_dOLLAoffset2streamDB = 0;
    m_dOLLAOffset4MUDB = 0;
}

void MSRxBufferBTS::AccumulateSuccessRxKbit(double _dKbit) {
    auto dSchWindowLength = (double)Parameters::Instance().BTS_UL.UL.ISchedulerWindowLength;
    m_dSuccessRxKbit = (299.0 / dSchWindowLength) * m_dSuccessRxKbit + (1.0 / dSchWindowLength) * _dKbit;
    m_dSuccessRxKbit += _dKbit;
}

std::shared_ptr<SchedulingMessageUL> MSRxBufferBTS::GetSchedulingMessage() {
    std::shared_ptr<SchedulingMessageUL> pSchM;
    pSchM.reset();
//    while (!(m_qScheduleMessageQueue.empty())
//            && (Clock::Instance().GetTimeSlot() - m_qScheduleMessageQueue.front()->GetBornTime() >= Parameters::Instance().MSS_UL.ISRS_DELAY)) {
//        pSchM = m_qScheduleMessageQueue.front();
//        m_qScheduleMessageQueue.pop_front();
//    }
    if(!m_qScheduleMessageQueue.empty())
    {
        pSchM = m_qScheduleMessageQueue.front();
        if(pSchM->m_iK2Slot + pSchM->GetBornTime() == Clock::Instance().GetTimeSlot())
        {
            m_qScheduleMessageQueue.pop_front();
        }
        else
        {
            pSchM = nullptr;
        }
    }
    return pSchM;
}

void MSRxBufferBTS::OllA(bool _bACK, int _iSendNum) {
    //由于上行不考虑多流，故不用mat作为变量
    const double dStepSize = 0.5;
    //    const double dStepSize = 4;
    if (_iSendNum == 0) { //首次传输
        if (_bACK) {
            m_dOLLAOffsetDB += dStepSize * Parameters::Instance().SIM_UL.UL.DBlerTarget;
        } else {
            m_dOLLAOffsetDB -= dStepSize * (1 - Parameters::Instance().SIM_UL.UL.DBlerTarget);
        }
    } else { //增加了对重传次数的考虑
        if (_iSendNum == 1 && !_bACK) {
            m_dOLLAOffsetDB -= (1.0 / 2) * dStepSize * (1 - Parameters::Instance().SIM_UL.UL.DBlerTarget);
        } else if (_iSendNum == 2 && !_bACK) {
            m_dOLLAOffsetDB -= (1.0 / 4) * dStepSize * (1 - Parameters::Instance().SIM_UL.UL.DBlerTarget);
        } else if (_iSendNum == 3 && !_bACK) {
            m_dOLLAOffsetDB -= (1.0 / 8) * dStepSize * (1 - Parameters::Instance().SIM_UL.UL.DBlerTarget);
        }
    }
}

//void MSRxBufferBTS::OllA(bool _bACK, int _iSendNum, bool _bACKanother) {
//    //由于上行不考虑多流，故不用mat作为变量
//    if (_iSendNum == 0) { //首次传输
//        //        if (_bACK == true && _bACKanother == true) {
//        if (_bACK == true) {
//            //            m_dOLLAOffsetDB += 0.5 * P.SIM.UL.DBlerTarget;
//            m_dOLLAoffset2streamDB += 0.5 * Parameters::Instance().SIM_UL.UL.DBlerTarget;
//        } else if (_bACK == false) {
//            //            m_dOLLAOffsetDB -= 0.5 * (1 - P.SIM.UL.DBlerTarget);
//            m_dOLLAoffset2streamDB -= 0.5 * (1 - Parameters::Instance().SIM_UL.UL.DBlerTarget);
//        } else
//            assert(false);
//    } else { //增加了对重传次数的考虑
//
//    }
//}

void MSRxBufferBTS::OllA4MU(bool _bACK, int _iSendNum) {
    //由于上行不考虑多流，故不用mat作为变量
    const double dStepSize = 0.5;
    //    const double dStepSize = 4;
    if (_iSendNum == 0) { //首次传输
        if (_bACK) {
            m_dOLLAOffset4MUDB += dStepSize * Parameters::Instance().SIM_UL.UL.DBlerTarget;
        } else{
            m_dOLLAOffset4MUDB -= dStepSize * (1 - Parameters::Instance().SIM_UL.UL.DBlerTarget);
        }
    } else { //增加了对重传次数的考虑
        if (_iSendNum == 1 && !_bACK) {
            m_dOLLAOffset4MUDB -= (1.0 / 2) * dStepSize * (1 - Parameters::Instance().SIM_UL.UL.DBlerTarget);
        } else if (_iSendNum == 2 && !_bACK) {
            m_dOLLAOffset4MUDB -= (1.0 / 4) * dStepSize * (1 - Parameters::Instance().SIM_UL.UL.DBlerTarget);
        } else if (_iSendNum == 3 && !_bACK) {
            m_dOLLAOffset4MUDB -= (1.0 / 8) * dStepSize * (1 - Parameters::Instance().SIM_UL.UL.DBlerTarget);
        }
    }
}
vector<pair<std::shared_ptr<Packet>,double> > MSRxBufferBTS::ReducePacketBuf(double _dLen) {
    vector<pair<std::shared_ptr<Packet>,double> > vResult;
    auto it = m_qPacketQueueUL.begin();
    while( it!=m_qPacketQueueUL.end()){
        if ((*it)->GetRemainUntransmitted_SizeKbits() > _dLen){
            (*it)->ReduceUntransmittedSizeKbits(_dLen);
            vResult.emplace_back(make_pair(*it,_dLen));
            (*it)->RecordSuccessRecievedSizeKbits(_dLen);
            return vResult;
        }
        else {
            double dTxSize = (*it)->GetRemainUntransmitted_SizeKbits();
            _dLen -= (*it)->GetRemainUntransmitted_SizeKbits();
            (*it)->ReduceUntransmittedSizeKbits((*it)->GetRemainUntransmitted_SizeKbits());
            vResult.emplace_back(make_pair(*it,dTxSize));
            (*it)->RecordSuccessRecievedSizeKbits(dTxSize);
        }
        PopFinishedPacket();
        if((*it)->IsCompleted())
             m_qPacketQueueUL.erase(it);
        //cout<<"erase"<<endl;
        it = m_qPacketQueueUL.begin();
    }
//    cout<<"MSRxBufferBTS::ReducePacketBuf(...) Wrong!"<<endl;
   // assert(false);
}
void MSRxBufferBTS::PopFinishedPacket() {
    boost::mutex::scoped_lock lock(FTP_MSRxBufferBTS_mutex);
    m_qPacketQueueUL[0]->LogFinishedPacket();
}
void MSRxBufferBTS::PopTimeOutPacket(int _Time) {
    boost::mutex::scoped_lock lock(FTP_MSRxBufferBTS_mutex);

    auto it = m_qPacketQueueUL.begin();
    while (it != m_qPacketQueueUL.end()) {
        if ((*it)->IsTimeout(_Time)) {
            (*it)->DropPacket_DueToTimeout();
            (*it)-> LogFinishedPacket();
            it = m_qPacketQueueUL.erase(it);
        } else {
            it++;
        }
    }
}