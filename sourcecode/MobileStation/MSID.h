///@file MSID.h
///@brief MSID类声明
///@author wangfei
#pragma once
#include "../Utility/IDType.h"

class MS;

///移动台ID类
class MSID final: public IDType {
public:
    MS& GetMS() const;
    static MSID Begin();
    static MSID End();

public:
    bool operator==(const MSID& msid2) const{
//        int id1=this->ToInt(),id2=msid2.ToInt();
//        bool r=(id1==id2);
//        std::cout<<"msid1 "<<id1<<"  msid2 "<<id2<<"  "<<std::boolalpha<<r<<std::endl;
//        return r;
        return m_iID==msid2.m_iID;
    }
     explicit MSID(int _id = -1);
    ~MSID() override = default;
};
