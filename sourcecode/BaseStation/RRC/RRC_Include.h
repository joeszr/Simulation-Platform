////
//// Created by AAA on 2023/4/6.
////
//#ifndef CHTY_RRC_INCLUDE_H
//#define CHTY_RRC_INCLUDE_H
//
//#include"./BTSID.h"
//#include"../MobileStation/MSID.h"
//
//enum RRC_STATE{
//    RRC_IDLE,
//    RRC_INACTIVE,
//    RRC_CONNECTED
//};
//
//
//class RRC_MSG{
//protected:
//    int msg_type;
//    int BornTime;
//    MSID msid;
//    BTSID btsid;
//public:
//    int GetType(){
//        return msg_type;
//    }
//    int GetBornTime(){
//        return BornTime;
//    }
//    MSID& GetMSID(){
//        return msid;
//    }
//    BTSID& GetBTSID(){
//        return btsid;
//    }
//    virtual void Process() = 0;
//public:
//    RRC_MSG(int _type, int _time, const BTSID& _btsid, const MSID& _msid):msg_type(_type), BornTime(_time), btsid(_btsid), msid(_msid){}
//    virtual ~RRC_MSG() = default;
//};
//
////这4个消息目前是一样的，但是可能后面具体的内容会不一样
//class MSG1 : public RRC_MSG{
//public:
//    void Process() override;
//public:
//    MSG1(const BTSID& _btsid, const MSID& _msid, int _time): RRC_MSG(1, _time, btsid, msid){}
//    ~MSG1() = default;
//};
//
//class MSG2 : public RRC_MSG{
//public:
//    void Process() override;
//public:
//    MSG2(const BTSID& _btsid, const MSID& _msid, int _time): RRC_MSG(2, _time, btsid, msid){}
//    ~MSG2() = default;
//};
//
//class MSG3 : public RRC_MSG{
//public:
//    void Process() override;
//public:
//    MSG3(const BTSID& _btsid, const MSID& _msid, int _time): RRC_MSG(3, _time, btsid, msid){}
//    ~MSG3() = default;
//};
//
//class MSG4 : public RRC_MSG{
//public:
//    void Process() override;
//public:
//    MSG4(const BTSID& _btsid, const MSID& _msid, int _time): RRC_MSG(4, _time, btsid, msid){}
//    ~MSG4() = default;
//};
//
////4代表release消息
//class MSG5 : public RRC_MSG{
//public:
//    void Process() override;
//public:
//    MSG5(const BTSID& _btsid, const MSID& _msid, int _time): RRC_MSG(5, _time, btsid, msid){}
//    ~MSG5() = default;
//};
//
//
//#endif //CHTY_RRC_INCLUDE_H
