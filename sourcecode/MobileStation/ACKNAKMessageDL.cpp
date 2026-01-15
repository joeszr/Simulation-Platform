///@file ACKNAKMessageDL.cpp
///@brief  ACKNAKMessage类实现
///@author wangfei
#include "../Utility/IDType.h"
#include "../MobileStation/MSID.h"
#include "../Utility/RBID.h"
#include "../Utility/SBID.h"
#include "../Scheduler/SchedulingMessageDL.h"
#include "ACKNAKMessageDL.h"
/// ACKNAKMessage类的构造函数

ACKNAKMessageDL::ACKNAKMessageDL(std::shared_ptr<SchedulingMessageDL> _pSchM) {
    m_pScheduleMes = _pSchM;
    int iRank = m_pScheduleMes->GetRank();
    m_mACKNAK = itpp::ones_b(iRank + 1, 1);
}


void ACKNAKMessageDL::SetACKNAK(bool _bACKNAK) {
    bACKNAK = _bACKNAK;
}

bool ACKNAKMessageDL::GetbACKNAK() const{
    return bACKNAK;
}

std::shared_ptr<SchedulingMessageDL> ACKNAKMessageDL::GetScheduleMes() {
    return m_pScheduleMes;
}

