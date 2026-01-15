//
// Created by AAA on 2022/10/3.
//

#include "MSTxBufferMS.h"
#include "MS.h"
boost::mutex FTP_MSTxBufferMS_mutex;

MSTxBufferMS::MSTxBufferMS(MSID _msid) {
    m_MSID = _msid;
}
void MSTxBufferMS::ReceivePacketUL(std::shared_ptr<Packet> _pPacket){
    m_qPacketQueueUL.push_back(_pPacket);
}
void MSTxBufferMS::PopFinishedPacket(){
//    RecordFinishedPacket();
    double dSuccessRxKbit = 0.0;
    deque<std::shared_ptr<Packet> >::iterator it = m_qPacketQueueUL.begin();
    while (it != m_qPacketQueueUL.end()) {
        if ((*it)->IsFullySuccessRecieved()) {
            (*it)-> LogFinishedPacket();
            if (!((*it)->IsDrop_DueToTimeout())) {
                dSuccessRxKbit += 10.0;
            }
            it = m_qPacketQueueUL.erase(it);
        } else {
            it++;
        }
    }
    if (m_qPacketQueueUL.empty()) {
        m_MSID.GetMS().SetTrafficState(false);
    }
}
void MSTxBufferMS::PopTimeOutPacket(int _Time){
    boost::mutex::scoped_lock lock(FTP_MSTxBufferMS_mutex);
    deque<std::shared_ptr<Packet> >::iterator it = m_qPacketQueueUL.begin();
    while (it != m_qPacketQueueUL.end()) {
        if ((*it)->IsTimeout(_Time)) {
            (*it)->DropPacket_DueToTimeout();
            (*it)-> LogFinishedPacket();
            it = m_qPacketQueueUL.erase(it);
        } else {
            it++;
        }
    }
    if (m_qPacketQueueUL.empty()) {
        m_MSID.GetMS().SetTrafficState(false);
    }
}
/*std::vector<pair<std::shared_ptr<Packet>,double> > MSTxBufferMS::ReducePacketBuf(double _dLen){
    vector<pair<std::shared_ptr<Packet>,double> > vResult;
    deque<std::shared_ptr<Packet> >::iterator it = m_qPacketQueueUL.begin();
    while( it!=m_qPacketQueueUL.end()){
        if ((*it)->GetRemainUntransmitted_SizeKbits() > _dLen){
            (*it)->ReduceUntransmittedSizeKbits(_dLen);
            vResult.push_back(make_pair(*it,_dLen));
            //(*it)->RecordSuccessRecievedSizeKbits(_dLen);
            return vResult;
        }
        else {
            double dTxSize = (*it)->GetRemainUntransmitted_SizeKbits();
            _dLen -= (*it)->GetRemainUntransmitted_SizeKbits();
            (*it)->ReduceUntransmittedSizeKbits((*it)->GetRemainUntransmitted_SizeKbits());
            vResult.push_back(make_pair(*it,dTxSize));
            //(*it)->RecordSuccessRecievedSizeKbits(dTxSize);
        }
//        PopFinishedPacket();
//        if((*it)->IsCompleted())
//            m_qPacketQueueUL.erase(it);
        //cout<<"erase"<<endl;
        it = m_qPacketQueueUL.begin();
    }
}*/
void MSTxBufferMS::RecordFinishedPacket() {
    boost::mutex::scoped_lock lock(FTP_MSTxBufferMS_mutex);
    m_qPacketQueueUL[0]->LogFinishedPacket();
}
double MSTxBufferMS::GetULFirstAvailablePacket_RemainUntransmitted_SizeKbits() {

    double dFirstSizeKbits = 0.0;

    std::shared_ptr<Packet> pFirstAvailablePacket = GetULFirstAvailablePacket();
    if (pFirstAvailablePacket) {
        dFirstSizeKbits = pFirstAvailablePacket->GetRemainUntransmitted_SizeKbits();
    }

    return dFirstSizeKbits;
}

std::shared_ptr<Packet> MSTxBufferMS::GetULFirstAvailablePacket() {

    std::shared_ptr<Packet> pFirstAvailablePacket=std::shared_ptr<Packet>(nullptr);
    if (m_qPacketQueueUL.size() > 0) {
        deque<std::shared_ptr<Packet> >::iterator it = m_qPacketQueueUL.begin();
        while (it != m_qPacketQueueUL.end()) {
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