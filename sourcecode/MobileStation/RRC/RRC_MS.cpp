////
//// Created by AAA on 2023/4/6.
////
//
//#include "RRC_MS.h"
//#include "./MS.h"
//#include "../UCI.h"
//#include "../NetworkDrive/Clock.h"
//
//void RRC_MS::SendMessage(std::shared_ptr<RRC_MSG> _pMSG){
//    msid.GetMS().GetUci()->CollectRRC_MSG(_pMSG);
//}
//void RRC_MS::ReleaseConnection() {
//    SetState(RRC_IDLE);
//}
//void RRC_MS::SetState(RRC_STATE _state){
//    state = _state;
//}
//void RRC_MS::StartRandomAccess(){
//    task = Access_SendMSG1;
//}
//void RRC_MS::ReceiveMessage(std::shared_ptr<RRC_MSG> _pMSG){
//    switch(_pMSG->GetType()){
//        case 1:
//        {
//            assert(false);    //MS应该只能收到2，4,5
//            break;
//        }
//        case 2:
//        {
//            assert(state == RRC_IDLE);
//            assert(task == Access_WaitMSG2);
//            task = Access_SendMSG3;
//        }
//        case 3:
//        {
//            assert(false);
//            break;
//        }
//        case 4:
//        {
//            assert(state == RRC_IDLE);
//            assert(task == Access_WaitMSG4);
//            SetState(RRC_CONNECTED);
//            task = Keep;
//            break;
//        }
//        case 5:
//        {
//            assert(state == RRC_CONNECTED);
//            assert(task == Keep);
//            task = Release;
//        }
//        default:
//            assert(false);
//    }
//}
//
//void RRC_MS::WorkSlot(){
//
//    int iTime = Clock::Instance().GetTimeSlot();
//    BTSID btsid = msid.GetMS().GetMainServBTS();
//
//    if(state == RRC_IDLE){
//        switch(task){
//            case Keep:
//            {
//                break;
//            }
//            case Access_SendMSG1:
//            {
//                std::shared_ptr<RRC_MSG> newMSG(new MSG1(btsid, msid, iTime));
//                newMSG->Process();
//                SendMessage(newMSG);
//                task = Access_WaitMSG2;
//                break;
//            }
//            case Access_WaitMSG2:
//            {
//                //这里可能会有计时器之类的
//            }
//            case Access_SendMSG3:
//            {
//                std::shared_ptr<RRC_MSG> newMSG(new MSG3(btsid, msid, iTime));
//                newMSG->Process();
//                SendMessage(newMSG);
//                task = Access_WaitMSG4;
//                break;
//            }
//            case Access_WaitMSG4:
//            {
//                //这里可能会有计时器之类的
//            }
//            default:
//            {
//                assert(false);
//                break;
//            }
//        }
//    }
//    else if(state == RRC_CONNECTED){
//        switch(task){
//            case Keep:
//            {
//                break;
//            }
//            case Release:
//            {
//                ReleaseConnection(); // 释放的时候有没有多次信令交互？
//            }
//            default:
//            {
//                assert(false);
//                break;
//            }
//        }
//    }
//    else{
//        assert(false); // 暂时不考虑INACTIVE
//    }
//}