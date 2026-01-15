////
//// Created by AAA on 2023/4/6.
////
//#ifndef CHTY_RRC_MS_H
//#define CHTY_RRC_MS_H
//
//#include"../BaseStation/RRC/RRC_Include.h"
//#include<queue>
//#include<memory>
//enum RRC_TASK_MS{
//    Release,
//    Keep,
//    Access_SendMSG1,
//    Access_WaitMSG2,
//    Access_SendMSG3,
//    Access_WaitMSG4
//};
//
//class RRC_MS {
//private:
//    MSID msid;
//    RRC_STATE state;
//    RRC_TASK_MS task;
//    std::deque<std::shared_ptr<RRC_MSG>> MessageQueue;
//private:
//    void SendMessage(std::shared_ptr<RRC_MSG> _pMSG);
//    void StartRandomAccess();
//    void ReleaseConnection();
//    void SetState(RRC_STATE _state);
//public:
//    void ReceiveMessage(std::shared_ptr<RRC_MSG> _pMSG);
//    void WorkSlot();
//public:
//    RRC_MS(MSID _msid):msid(_msid), state(RRC_IDLE), task(Keep){}
//    ~RRC_MS() = default;
//};
//
//
//#endif //CHTY_RRC_MS_H
