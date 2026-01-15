///@file HARQTxStateMS.cpp
///@brief MS端HARQ发射状态管理类函数实现
///@author zhengyi

#include "../NetworkDrive/Clock.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "../BaseStation/ACKNAKMessageUL.h"
#include "HARQTxStateMS.h"
#include"../Scheduler/SchedulingMessageUL.h"

//void HARQTxStateMS::WorkSlot(void) {
//    m_iHARQThreadID = (++m_iHARQThreadID) % Parameters::Instance().SIM_UL.UL.IHARQProcessNum;
//}

//int HARQTxStateMS::GetHARQID() {
//    return m_iHARQThreadID;
//}

/// @brief 注册重发消息

void HARQTxStateMS::RegistReTxMessage(const std::shared_ptr<ACKNAKMessageUL>& _pACKNAKMesUL) {
    m_qACKNAKMesQueue.push_back(_pACKNAKMesUL);
    //    m_vACKNAKMes[m_iHARQThreadID] = _pACKNAKMesUL;
}

/// @brief 取重发消息

std::shared_ptr<ACKNAKMessageUL> HARQTxStateMS::GetReTxMessage() {
    //    if (!IsNeedReTx()) {
    //        m_vACKNAKMes[m_iHARQThreadID] = std::shared_ptr<ACKNAKMessageUL > ((ACKNAKMessageUL*) 0);
    //    }
    std::shared_ptr<ACKNAKMessageUL> pNAKMessage = m_qACKNAKMesQueue.front();
    m_qACKNAKMesQueue.pop_front();

    return pNAKMessage;
}

//void HARQTxStateMS::Reset() {
////    m_qACKNAKMesQueue.clear();
//    m_vACKNAKMes[m_iHARQThreadID].reset(); //这个是将该指针设为空，并不是将内容清空？
//}

//void HARQTxStateMS::Clear() { //将所有进程的内容清零，防止drop间有数据传递
//    for (int i = 0; i < Parameters::Instance().SIM_UL.UL.IHARQProcessNum; ++i) {
//        m_vACKNAKMes[i].reset();
//    }
//}

bool HARQTxStateMS::IsNeedReTx() {
	//    return m_vACKNAKMes[m_iHARQThreadID] != 0;
	if (m_qACKNAKMesQueue.empty()) {
		return false;
	}
	else {
		std::shared_ptr<ACKNAKMessageUL> pNAKMessage = m_qACKNAKMesQueue.front();
		if (Clock::Instance().GetTimeSlot() - pNAKMessage->GetSchMessage()->GetBornTime() >= 5) {
			return true;
		}
		else {
			return false;
		}
	}
}