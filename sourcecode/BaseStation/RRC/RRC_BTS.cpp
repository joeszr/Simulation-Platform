////
//// Created by AAA on 2023/4/6.
////
//#include<mutex>
//#include "RRC_BTS.h"
//#include"./BTS.h"
//#include"../DCI.h"
//#include"../NetworkDrive/Clock.h"
//RRC_BTS::RRC_BTS(BTSID _btsid, vector<MSID>& _vMSID){
//    btsid = _btsid;
//    for(auto msid:_vMSID){
//        MSID2RRC_state[msid.ToInt()] = RRC_IDLE;
//        MSID2RRC_task[msid.ToInt()] = Keep;
//    }
//}
//void RRC_BTS::SendMessage(std::shared_ptr<RRC_MSG> _pMSG){
//    btsid.GetBTS().GetDci()->CollectRRC_MSG(_pMSG);
//}
//void RRC_BTS::ReleaseConnection(MSID _msid){
//    int iTime = Clock::Instance().GetTimeSlot();
//    std::shared_ptr<RRC_MSG> newMSG(new MSG5(btsid, _msid, iTime));
//    SendMessage(newMSG);
//    SetState(RRC_IDLE, _msid);
//}
//void RRC_BTS::SetState(RRC_STATE _state, MSID _msid){
//    MSID2RRC_state[_msid.ToInt()] = _state;
//}
//std::mutex RRC_LOCK;
//void RRC_BTS::ReceiveMessage(std::shared_ptr<RRC_MSG> _pMSG){
//    RRC_LOCK.lock();
//    MessageQueue.push_back(_pMSG);
//    RRC_LOCK.unlock();
//}
//void RRC_BTS::ProcessMessage(){
//    //每个用户应该同一时刻只有一个消息
//    while(!MessageQueue.empty()){
//        auto pMSG = MessageQueue.front();
//        MessageQueue.pop_front();
//        MSID& msid = pMSG->GetMSID();
//
//    }
//}
//void RRC_BTS::WorkSlot(){
//
//}
//
