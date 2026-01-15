////
//// Created by AAA on 2023/4/6.
////
//
//#ifndef CHTY_RRC_BTS_H
//#define CHTY_RRC_BTS_H
//
//#include"RRC_Include.h"
//#include"../MobileStation/MSID.h"
//#include<queue>
//#include<memory>
//enum RRC_TASK_BTS{
//    Release,
//    Keep,
//    Access_WaitMSG1,
//    Access_SendMSG2,
//    Access_WaitMSG3,
//    Access_SendMSG4
//};
//
//class RRC_BTS {
//private:
//    BTSID btsid;
//    unordered_map<int, RRC_STATE>MSID2RRC_state;
//    unordered_map<int, RRC_TASK_BTS>MSID2RRC_task;
//    std::deque<std::shared_ptr<RRC_MSG>> MessageQueue;
//private:
//    void SendMessage(std::shared_ptr<RRC_MSG> _pMSG);
//    void ReleaseConnection(MSID _msid);
//    void SetState(RRC_STATE _state, MSID _msid);
//public:
//    void ReceiveMessage(std::shared_ptr<RRC_MSG> _pMSG);
//    void ProcessMessage();
//    void WorkSlot();
//public:
//    RRC_BTS(BTSID _btsid, vector<MSID>& _vMSID);
//    ~RRC_BTS() = default;
//};
//
//
//#endif //CHTY_RRC_BTS_H
