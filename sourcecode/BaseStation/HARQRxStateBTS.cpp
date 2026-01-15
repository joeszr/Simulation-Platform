///@file HARQRxStateBTS.cpp
///@brief  HARQRxStateBTS类实现
///@author wangsen

#include "../MobileStation/MSID.h"
#include "../NetworkDrive/Clock.h"
#include "../NetworkDrive/NetWorkDrive.h"
#include "ACKNAKMessageUL.h"
#include "HARQRxStateBTS.h"
#include "../Scheduler/SchedulingMessageUL.h"
size_t pair_MSID_int_hash(const pair<MSID, int>& p1) {
    return std::hash<int>()(p1.first.ToInt()) ^ std::hash<int>()(p1.second);
}
HARQRxStateBTS::HARQRxStateBTS(): m_mMSIDHARQID2SINR(10,pair_MSID_int_hash){
}

///@brief 合并重传的SINR

//void HARQRxStateBTS::CombineSINR(MSID _msid, const vector<double>& _vSINR) {
//    m_vHARQThreadRxState[m_iHARQThreadID].CombineSINR(_msid, _vSINR);
//}

void HARQRxStateBTS::CombineSINR(const MSID& _msid, const int& _HARQID, const vector<mat>& _vSINR) {
    if (m_mMSIDHARQID2SINR.find(make_pair(_msid, _HARQID)) == m_mMSIDHARQID2SINR.end()) {
        m_mMSIDHARQID2SINR[make_pair(_msid, _HARQID)] = _vSINR;
    } else {
        assert(m_mMSIDHARQID2SINR[make_pair(_msid, _HARQID)].size() == _vSINR.size());
        for (int i = 0; i < static_cast<int>(_vSINR.size()); ++i) {
            m_mMSIDHARQID2SINR[make_pair(_msid, _HARQID)][i] += _vSINR[i];
        }
    }
}

///@brief 获取合并后的SINR

//const vector<double>& HARQRxStateBTS::GetCombinedSINR(MSID _msid) {
//    return m_vHARQThreadRxState[m_iHARQThreadID].GetCombinedSINR(_msid);
//}

const vector<mat>& HARQRxStateBTS::GetCombinedSINR(const MSID& _msid, const int& _HARQID) {
    return m_mMSIDHARQID2SINR[make_pair(_msid, _HARQID)];
}
///@brief 获取HARQID

//int HARQRxStateBTS::GetHARQID() {
//    return m_iHARQThreadID;
//}

///@brief 刷新

//void HARQRxStateBTS::Refresh(MSID _msid) {
//    m_vHARQThreadRxState[m_iHARQThreadID].Refresh(_msid);
//}

void HARQRxStateBTS::Refresh(const MSID& _MSid, const int& _HARQid) {
    m_mMSIDHARQID2SINR.erase(make_pair(_MSid, _HARQid));
}

///@brief reset

void HARQRxStateBTS::Reset() {
    m_mMSIDHARQID2SINR.clear();
    m_qACKNAKMesQueue.clear();

    //    for (int i = 0; i < Parameters::Instance().SIM_UL.UL.IHARQProcessNum; ++i) {
    //        m_vHARQThreadRxState[i].Clear();
    //    }
}

//判断当前时刻是否是重传

bool HARQRxStateBTS::IsReTxCurrentSF(const MSID& _msid) {
    deque<std::shared_ptr<ACKNAKMessageUL> > qTmpACKNACKMsgQue = m_qACKNAKMesQueue;
    while (!qTmpACKNACKMsgQue.empty()) {
        std::shared_ptr<ACKNAKMessageUL> pACKNAKMes = qTmpACKNACKMsgQue.front();
        std::shared_ptr<SchedulingMessageUL> pSchMes = pACKNAKMes->GetSchMessage();
		if (pSchMes->GetMSID() == _msid && Clock::Instance().GetTimeSlot() - pSchMes->GetBornTime() >= 5) {
			return true;
		}
		else {
			qTmpACKNACKMsgQue.pop_front();
		}
	}
    return false; //遍历调度信息，但是没有属于MSID的消息

    //    return m_vHARQThreadRxState[m_iHARQThreadID].IsNeedReTx(_msid);
}

///

void HARQRxStateBTS::RegistReTxMessage(const std::shared_ptr<ACKNAKMessageUL>& _pACKNAKMes) {
    //    m_vHARQThreadRxState[m_iHARQThreadID].RegistACKNAKMessage(_pACKNAKMes);
    m_qACKNAKMesQueue.push_back(_pACKNAKMes);
}

///

//std::shared_ptr<ACKNAKMessageUL> HARQRxStateBTS::GetReTxMessage() {
//    int iTmpHARQThreadID = (m_iHARQThreadID + Parameters::Instance().SIM_UL.UL.ISubframeNum) % Parameters::Instance().SIM_UL.UL.IHARQProcessNum; //提取即将调度的进程
//    return m_vHARQThreadRxState[iTmpHARQThreadID].GetReTxMessage();
//}

deque<std::shared_ptr<ACKNAKMessageUL> > HARQRxStateBTS::GetReTxMsgQue() {
    //    int iTmpHARQThreadID = (m_iHARQThreadID + Parameters::Instance().SIM_UL.UL.ISubframeNum) % Parameters::Instance().SIM_UL.UL.IHARQProcessNum; //提取即将调度的进程
    //    return m_vHARQThreadRxState[iTmpHARQThreadID].GetReTxMsgQue();
    return m_qACKNAKMesQueue;
}
deque<std::shared_ptr<ACKNAKMessageUL> >& HARQRxStateBTS::GetReTxMsgQueRef() {
    //    int iTmpHARQThreadID = (m_iHARQThreadID + Parameters::Instance().SIM_UL.UL.ISubframeNum) % Parameters::Instance().SIM_UL.UL.IHARQProcessNum; //提取即将调度的进程
    //    return m_vHARQThreadRxState[iTmpHARQThreadID].GetReTxMsgQue();
    return m_qACKNAKMesQueue;
}

std::shared_ptr<ACKNAKMessageUL> HARQRxStateBTS::GetReTxMessage(const MSID& _msid) {
    //对应上面那个，用来提取调度消息
    //    return m_vHARQThreadRxState[m_iHARQThreadID].GetReTxMessage(_msid);

    std::shared_ptr<ACKNAKMessageUL> pACKNAKMes;
    pACKNAKMes.reset();

    //提取消息，找到则消息返回，并从消息队列中将其删除，如果没有找到对应的消息，则返回空指针
    for (auto it = m_qACKNAKMesQueue.begin(); it != m_qACKNAKMesQueue.end(); ++it) {
        if ((*it)->GetSchMessage()->GetMSID() == _msid) {
            pACKNAKMes = (*it);
            m_qACKNAKMesQueue.erase(it);
            return pACKNAKMes;
        }
    }
    return pACKNAKMes;
}

std::shared_ptr<ACKNAKMessageUL> HARQRxStateBTS::GetReTxMsgwithoutDel(const MSID& _msid) {
    //对应上面那个，用来提取调度消息
    //    return m_vHARQThreadRxState[m_iHARQThreadID].GetReTxMsgwithoutDel(_msid);

    std::shared_ptr<ACKNAKMessageUL> pACKNAKMes;
    pACKNAKMes.reset();

    //提取消息，找到则消息返回，并从消息队列中将其删除，如果没有找到对应的消息，则返回空指针
    deque<std::shared_ptr<ACKNAKMessageUL> >::iterator it;
    for (it = m_qACKNAKMesQueue.begin(); it != m_qACKNAKMesQueue.end(); ++it) {
        if ((*it)->GetSchMessage()->GetMSID() == _msid) {
            pACKNAKMes = (*it);
            //            m_qACKNAKMesQueue.erase(it);
            return pACKNAKMes;
        }
    }
    return pACKNAKMes;
}

bool HARQRxStateBTS::IsNeedReTx() {
    //todo
    return !m_qACKNAKMesQueue.empty();
}