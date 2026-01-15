///@file ACKNAKMessageUL.cpp
///@brief  ACKNAKMessageUL类实现
///@author wangsen
#include "../Utility/IDType.h"
#include "../MobileStation/MSID.h"
#include "../Utility/RBID.h"
#include "../Utility/SBID.h"
#include "../Scheduler/SchedulingMessageUL.h"
#include "ACKNAKMessageUL.h"

/// ACKNAKMessageUL类的构造函数
ACKNAKMessageUL::ACKNAKMessageUL(const std::shared_ptr<SchedulingMessageUL>& _pSchM) {
    m_pSchM = _pSchM;
    m_bACKNAK = false;
}

void ACKNAKMessageUL::SetACKNAK(bool _bACKNAK) {
    m_bACKNAK = _bACKNAK;
}

bool ACKNAKMessageUL::GetACKNAK() const{
    return m_bACKNAK;
}

std::shared_ptr<SchedulingMessageUL> ACKNAKMessageUL::GetSchMessage() {
    return m_pSchM;
}